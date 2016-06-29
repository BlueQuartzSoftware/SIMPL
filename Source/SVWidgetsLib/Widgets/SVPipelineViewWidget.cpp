/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SVPipelineViewWidget.h"

#include <iostream>


#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QTemporaryFile>
#include <QtCore/QMimeData>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtGui/QMouseEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDrag>
#include <QtGui/QClipboard>
#include <QtWidgets/QLabel>
#include <QtGui/QPixmap>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QMessageBox>




#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/IFilterFactory.hpp"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/CoreFilters/Breakpoint.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"

#include "SVWidgetsLib/QtSupport/QtSDroppableScrollArea.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Core/FilterWidgetManager.h"
#include "SVWidgetsLib/Core/PipelineViewPtrMimeData.h"
#include "SVWidgetsLib/Widgets/BreakpointFilterWidget.h"
#include "SVWidgetsLib/Widgets/util/AddFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/MoveFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/RemoveFilterCommand.h"
#include "SVWidgetsLib/Widgets/SIMPLViewMenuItems.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_SVPipelineViewWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineViewWidget::SVPipelineViewWidget(QWidget* parent) :
  QFrame(parent),
  PipelineView(parent),
  m_PipelineIsRunning(false),
  m_ShiftStart(NULL),
  m_FilterWidgetLayout(NULL),
  m_FilterOrigPos(-1),
  m_DropBox(NULL),
  m_DropIndex(-1),
  m_EmptyPipelineLabel(NULL),
  m_PipelineMessageObserver(NULL),
  m_ScrollArea(NULL),
  m_AutoScroll(true),
  m_AutoScrollMargin(10),
  m_autoScrollCount(0),
  m_InputParametersWidget(NULL),
  m_UndoStack(new QUndoStack(this))
{
  setupGui();
  m_autoScrollTimer.setParent(this);

  setContextMenuPolicy(Qt::CustomContextMenu);
  setFocusPolicy(Qt::StrongFocus);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(requestContextMenu(const QPoint&)));

  createPipelineViewWidgetMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineViewWidget::~SVPipelineViewWidget()
{
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::setupGui()
{
  newEmptyPipelineViewLayout();
  connect(&m_autoScrollTimer, SIGNAL(timeout()), this, SLOT(doAutoScroll()));

  //connect(this, SIGNAL(deleteKeyPressed(PipelineView*)), dream3dApp, SLOT(on_pipelineViewWidget_deleteKeyPressed(PipelineView*)));

  m_DropBox = new DropBoxWidget();

  m_ActionUndo = m_UndoStack->createUndoAction(NULL);
  m_ActionRedo = m_UndoStack->createRedoAction(NULL);
  m_ActionUndo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
  m_ActionRedo->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* SVPipelineViewWidget::createPipelineFilterWidgetMenu(SVPipelineFilterWidget *filterWidget)
{
  // Creating Pipeline Filter Widget Menu
  SIMPLViewMenuItems* menuItems = SIMPLViewMenuItems::Instance();

  QMenu* contextMenu = new QMenu(this);

  contextMenu->addAction(menuItems->getActionCut());
  contextMenu->addAction(menuItems->getActionCopy());

  contextMenu->addSeparator();

  QAction* actionLaunchHelp = new QAction("Filter Help", this);
  connect(actionLaunchHelp, SIGNAL(triggered()), filterWidget, SLOT(launchHelpForItem()));

  contextMenu->addAction(actionLaunchHelp);
  return contextMenu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::createPipelineViewWidgetMenu()
{
  // Creating Pipeline View Widget Menu
  SIMPLViewMenuItems* menuItems = SIMPLViewMenuItems::Instance();

  m_ContextMenu = new QMenu(this);

  m_ContextMenu->addAction(menuItems->getActionPaste());
  m_ContextMenu->addSeparator();
  m_ContextMenu->addAction(menuItems->getActionClearPipeline());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::setScrollArea(QScrollArea* sa)
{
  m_ScrollArea = sa;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::newEmptyPipelineViewLayout()
{
  if(m_EmptyPipelineLabel == NULL)
  {
    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    QSpacerItem* verticalSpacer = new QSpacerItem(20, 341, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer, 0, 1, 1, 1);

    QSpacerItem* horizontalSpacer_3 = new QSpacerItem(102, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_3, 1, 0, 1, 1);

    m_EmptyPipelineLabel = new QLabel(this);
    m_EmptyPipelineLabel->setObjectName(QString::fromUtf8("label"));
    m_EmptyPipelineLabel->setMinimumSize(QSize(325, 250));
    m_EmptyPipelineLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
                                                          "border-radius: 20px;\n"
                                                          "/*border: 1px solid rgb(120, 120, 120);*/\n"
                                                          "/* background-color: rgb(160, 160, 160); */\n"
                                                          "font-weight: bold;\n"
                                                          "color : rgb(150, 150, 150);\n"
                                                          "text-align: center;\n"
                                                          "margin: 5px;\n"
                                                          "padding: 10px;\n"
                                                          "}"));
    m_EmptyPipelineLabel->setAlignment(Qt::AlignCenter);
    QString text;
    QTextStream ss (&text);
    ss << "<h2>Creating a Pipeline</h2>";
    ss << "<hr>";
    ss << "File > Open <br />";
    ss << "File > New <br />";
    ss << "Drag and drop filters<br />";
    ss << "Double click a Bookmark<br />";
    ss << "Double click a Prebuilt Pipeline<br />";
    m_EmptyPipelineLabel->setText(text);


    gridLayout->addWidget(m_EmptyPipelineLabel, 1, 1, 1, 1);

    QSpacerItem* horizontalSpacer_4 = new QSpacerItem(102, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_4, 1, 2, 1, 1);

    QSpacerItem* verticalSpacer_2 = new QSpacerItem(20, 341, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer_2, 2, 1, 1, 1);
  }
  emit pipelineChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SVPipelineViewWidget::filterCount(QUuid startId)
{
  Q_UNUSED(startId)

  int count = 0;
  if (NULL != m_FilterWidgetLayout)
  {
    count = m_FilterWidgetLayout->count() - 1;
  }
  return count;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::focusInEvent(QFocusEvent* event)
{
  QList<PipelineFilterObject*> filterWidgets = getSelectedFilterObjects();
  for(int i=0; i<filterWidgets.size(); i++)
  {
    PipelineFilterObject* fw = filterWidgets[i];
    fw->setHasFocus(true);
    fw->changeStyle();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::focusOutEvent(QFocusEvent* event)
{
  QList<PipelineFilterObject*> filterWidgets = getSelectedFilterObjects();
  for(int i=0; i<filterWidgets.size(); i++)
  {
    PipelineFilterObject* fw = filterWidgets[i];
    fw->setHasFocus(false);
    fw->changeStyle();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject* SVPipelineViewWidget::filterObjectAt(QVariant value)
{
  bool ok;
  int index = value.toInt(&ok);
  if (ok == false) { return nullptr; }

  if (index < 0)
  {
    index = filterCount();
  }

  SVPipelineFilterWidget* fw = NULL;
  if (m_FilterWidgetLayout != NULL)
  {
    QWidget* w = m_FilterWidgetLayout->itemAt(index)->widget();
    fw = qobject_cast<SVPipelineFilterWidget*>(w);
  }
  return fw;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant SVPipelineViewWidget::valueOfFilterWidget(PipelineFilterObject* filterObject)
{
  SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(filterObject);
  Q_ASSERT(filterWidget != nullptr);

  return m_FilterWidgetLayout->indexOf(filterWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::resetLayout()
{
  // Check to see if we have removed all the filters
  if (filterCount() <= 0)
  {
    // Emit a signal to tell SIMPLView_UI to erase the Filter Input Widget.
    emit filterInputWidgetNeedsCleared();

    // Remove the current Layout
    QLayout* l = layout();
    if (NULL != l && l == m_FilterWidgetLayout)
    {
      qDeleteAll(l->children());
      delete l;
      m_FilterWidgetLayout = NULL;
    }

    // and add the empty pipeline layout instead
    newEmptyPipelineViewLayout();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::clearFilterWidgets(bool allowUndo)
{
  if (allowUndo == true)
  {
    int numFilters = filterCount();

    QUndoCommand* topCmd = new QUndoCommand();
    topCmd->setText(QObject::tr("\"%1 %2 Filter Widgets\"").arg("Clear").arg(numFilters));
    for (int i = numFilters - 1; i >= 0; i--)
    {
      SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      new RemoveFilterCommand(filterWidget, this, "Clear", QUuid(), QUuid(), topCmd);
    }
    addUndoCommand(topCmd);
  }
  else
  {
    emit pipelineIssuesCleared();
    emit pipelineChanged();

    qint32 count = filterCount();
    for(qint32 i = count - 1; i >= 0; --i)
    {
      QWidget* w = m_FilterWidgetLayout->itemAt(i)->widget();
      QSpacerItem* spacer = m_FilterWidgetLayout->itemAt(i)->spacerItem();
      if (NULL != w)
      {
        m_FilterWidgetLayout->removeWidget(w);
        SVPipelineFilterWidget* fw = qobject_cast<SVPipelineFilterWidget*>(w);
        if(fw)
        {
          fw->getFilter()->setPreviousFilter(AbstractFilter::NullPointer());
          fw->getFilter()->setNextFilter(AbstractFilter::NullPointer());
        }
        w->deleteLater();
      }
      else if (NULL != spacer)
      {
        m_FilterWidgetLayout->removeItem(spacer);
      }
    }
    resetLayout();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::reindexWidgetTitles()
{
  qint32 count = filterCount();
  for(qint32 i = 0; i < count; ++i)
  {
    PipelineFilterObject* fw = filterObjectAt(i);
    if (fw)
    {
      QString hl = fw->getFilter()->getHumanLabel();
      hl = QString("[") + QString::number(i + 1) + QString("] ") + hl;
      fw->setFilterTitle(hl);
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer SVPipelineViewWidget::getFilterPipeline(QUuid startId)
{
  Q_UNUSED(startId)

  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  qint32 count = filterCount();
  for(qint32 i = 0; i < count; ++i)
  {
    PipelineFilterObject* fw = filterObjectAt(i);
    if (fw)
    {
      AbstractFilter::Pointer filter = fw->getFilter();
      Breakpoint::Pointer breakpoint = std::dynamic_pointer_cast<Breakpoint>(filter);
      if (NULL != breakpoint)
      {
        connect(pipeline.get(), SIGNAL(pipelineCanceled()), breakpoint.get(), SLOT(resumePipeline()));
      }

      pipeline->pushBack(filter);
    }

  }
  pipeline->addMessageReceiver(m_PipelineMessageObserver);
  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer SVPipelineViewWidget::getCopyOfFilterPipeline()
{
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  qint32 count = filterCount();
  for(qint32 i = 0; i < count; ++i)
  {
    PipelineFilterObject* fw = filterObjectAt(i);
    if (fw)
    {
      AbstractFilter::Pointer filter = fw->getFilter();
      AbstractFilter::Pointer copy = filter->newFilterInstance(true);
      pipeline->pushBack(copy);
    }

  }
  pipeline->addMessageReceiver(m_PipelineMessageObserver);
  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SVPipelineViewWidget::openPipeline(const QString& filePath, QVariant value, const bool& setOpenedFilePath, const bool& changeTitle)
{
  QFileInfo fi(filePath);
  if (fi.exists() == false)
  {
    QMessageBox::warning(this, QString::fromLatin1("Pipeline Read Error"),
                         QString::fromLatin1("There was an error opening the specified pipeline file. The pipeline file does not exist."));
    return -1;
  }

  // Clear the pipeline Issues table first so we can collect all the error messages
  emit pipelineIssuesCleared();

  QString ext = fi.suffix();
  QString name = fi.fileName();

  // Read the pipeline from the file
  FilterPipeline::Pointer pipeline = readPipelineFromFile(filePath);

  // Check that a valid extension was read...
  if (pipeline == FilterPipeline::NullPointer())
  {
    emit statusMessage(tr("The pipeline was not read correctly from file '%1'. '%2' is an unsupported file extension.").arg(name).arg(ext));
    return -1;
  }

  // Populate the pipeline view
  populatePipelineView(pipeline, value);

  // Notify user of successful read
  emit statusMessage(tr("The pipeline has been read successfully from '%1'.").arg(name));

  QString file = filePath;
  emit pipelineOpened(file, setOpenedFilePath, changeTitle);

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::addFilter(const QString& filterClassName, QVariant value, bool allowUndo, QUuid previousNode, QUuid nextNode)
{  
  Q_UNUSED(previousNode)
  Q_UNUSED(nextNode)

  if (this->isEnabled() == false) { return; }
  FilterManager* fm = FilterManager::Instance();
  if(NULL == fm) { return; }
  IFilterFactory::Pointer wf = fm->getFactoryForFilter(filterClassName);
  if (NULL == wf.get()) { return; }

  // Create an instance of the filter. Since we are dealing with the AbstractFilter interface we can not
  // actually use the concrete filter class. We are going to have to rely on QProperties or Signals/Slots
  // to communicate changes back to the filter.
  AbstractFilter::Pointer filter = wf->create();

  addFilter(filter, value, allowUndo);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::addFilters(QList<AbstractFilter::Pointer> filters, QVariant value, bool allowUndo, QUuid previousNode, QUuid nextNode)
{
  Q_UNUSED(previousNode)
  Q_UNUSED(nextNode)

  if (filters.size() <= 0) { return; }

  bool ok;
  int index = value.toInt(&ok);
  if (ok == false) { return; }
  if (index < 0)
  {
    index = filterCount();
  }

  if (allowUndo == true)
  {
    QUndoCommand* topCmd = new QUndoCommand();
    topCmd->setText(QObject::tr("\"%1 %2 Filter Widgets\"").arg("Add").arg(filters.size()));
    for (int i=0; i<filters.size(); i++)
    {
      //SVPipelineFilterWidget* filterWidget = new SVPipelineFilterWidget(filters[i]->newFilterInstance(true), NULL, NULL);
      new AddFilterCommand(filters[i], this, "Add", index, QUuid(), QUuid(), topCmd);
      index++;
    }
    addUndoCommand(topCmd);
  }
  else
  {
    bool ok;
    int index = value.toInt(&ok);
    if (ok == false) { return; }

    for (int i=0; i<filters.size(); i++)
    {
      addFilter(filters[i], index, false);
      index++;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::addFilter(AbstractFilter::Pointer filter, QVariant value, bool allowUndo, QUuid previousNode, QUuid nextNode)
{
  Q_UNUSED(previousNode)
  Q_UNUSED(nextNode)

  if (value < 0) // If the programmer wants to add it to the end of the list
  {
    value = filterCount();
  }

  Breakpoint::Pointer breakpoint = std::dynamic_pointer_cast<Breakpoint>(filter);

  // Create a FilterWidget object
  if (NULL != breakpoint)
  {
    BreakpointFilterWidget* w = new BreakpointFilterWidget(filter, NULL, this);
    addFilterObject(w, value, allowUndo);
  }
  else
  {
    SVPipelineFilterWidget* w = new SVPipelineFilterWidget(filter, NULL, this);
    addFilterObject(w, value, allowUndo);
  }

  // Clear the pipeline Issues table first so we can collect all the error messages
  emit pipelineIssuesCleared();
  // Now preflight the pipeline for this filter.
  preflightPipeline();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::addFilterObjects(QList<PipelineFilterObject*> filterObjects, QVariant value, bool allowUndo)
{
  if (filterObjects.size() <= 0) { return; }

  bool ok;
  int index = value.toInt(&ok);
  if (ok == false) { return; }
  if (index < 0)
  {
    index = filterCount();
  }

  if (allowUndo == true)
  {
    QUndoCommand* topCmd = new QUndoCommand();
    topCmd->setText(QObject::tr("\"%1 %2 Filter Widgets\"").arg("Add").arg(filterObjects.size()));
    for (int i=0; i<filterObjects.size(); i++)
    {
      new AddFilterCommand(filterObjects[i]->getFilter(), this, "Add", index, QUuid(), QUuid(), topCmd);
      index++;
    }
    addUndoCommand(topCmd);
  }
  else
  {
    bool ok;
    int index = value.toInt(&ok);
    if (ok == false) { return; }
    if (index < 0)
    {
      index = filterCount();
    }

    for (int i=0; i<filterObjects.size(); i++)
    {
      addFilterObject(filterObjects[i], index, false);
      index++;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::addFilterObject(PipelineFilterObject* filterObject, QVariant value, bool allowUndo, QUuid previousNodeId, QUuid nextNodeId)
{
  Q_UNUSED(previousNodeId)
  Q_UNUSED(nextNodeId)

  SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(filterObject);
  Q_ASSERT(filterWidget != nullptr);

  if (allowUndo == true)
  {
    AddFilterCommand* cmd = new AddFilterCommand(filterWidget->getFilter(), this, "Add", value);
    addUndoCommand(cmd);
  }
  else
  {
    bool ok;
    int index = value.toInt(&ok);
    if (ok == false) { return; }
    if (index < 0)
    {
      index = filterCount();
    }

    bool addSpacer = false;
    if (filterCount() <= 0)
    {
      if (NULL != m_EmptyPipelineLabel)
      {
        m_EmptyPipelineLabel->hide();
        delete m_EmptyPipelineLabel;
        m_EmptyPipelineLabel = NULL;
      }
      QLayout* l = layout();
      if (NULL != l)
      {
        qDeleteAll(l->children());
        delete l;
      }

      m_FilterWidgetLayout = new QVBoxLayout(this);
      m_FilterWidgetLayout->setObjectName(QString::fromUtf8("m_FilterWidgetLayout"));
      m_FilterWidgetLayout->setContentsMargins(2, 2, 2, 2);
      m_FilterWidgetLayout->setSpacing(3);
      addSpacer = true;

      if(index < 0)
      {
        index = 0;
      }
    }

    if(index == -1)
    {
      index = filterCount();
    }

    // The layout will take control of the SVPipelineFilterWidget 'w' instance
    m_FilterWidgetLayout->insertWidget(index, filterWidget);
    // Set the Parent
    filterWidget->setParent(this);

    /// Now setup all the connections between the various widgets

    // When the filter is removed from this view
    connect(filterWidget, SIGNAL(filterWidgetRemoved(PipelineFilterObject*, bool)),
            this, SLOT(removeFilterObject(PipelineFilterObject*, bool)) );

    // When the FilterWidget is selected
    connect(filterWidget, SIGNAL(filterWidgetPressed(PipelineFilterObject*, Qt::KeyboardModifiers)),
            this, SLOT(setSelectedFilterObject(PipelineFilterObject*, Qt::KeyboardModifiers)));

    // When the filter widget is dragged
    connect(filterWidget, SIGNAL(dragStarted(QMouseEvent*, SVPipelineFilterWidget*)),
            this, SLOT(startDrag(QMouseEvent*, SVPipelineFilterWidget*)));

    connect(filterWidget, SIGNAL(parametersChanged(QUuid)),
            this, SLOT(preflightPipeline(QUuid)));

    connect(filterWidget, SIGNAL(parametersChanged(QUuid)),
            this, SLOT(handleFilterParameterChanged(QUuid)));

    connect(filterWidget, SIGNAL(focusInEventStarted(QFocusEvent*)), this, SLOT(on_focusInEventStarted(QFocusEvent*)));

    connect(filterWidget, SIGNAL(focusOutEventStarted(QFocusEvent*)), this, SLOT(on_focusOutEventStarted(QFocusEvent*)));

    filterWidget->installEventFilter(this);

    // Check to make sure at least the vertical spacer is in the Layout
    if (addSpacer)
    {
      QSpacerItem* verticalSpacer = new QSpacerItem(20, 361, QSizePolicy::Minimum, QSizePolicy::Expanding);
      m_FilterWidgetLayout->insertSpacerItem(-1, verticalSpacer);
    }

    // Make sure the widget titles are all correct
    reindexWidgetTitles();

    // Finally, set this new filter widget as selected
    setSelectedFilterObject(filterWidget, Qt::NoModifier);
    filterWidget->setSelected(true);

    // Get the filter to ignore Scroll Wheel Events
    filterWidget->installEventFilter( this);

    // Emit that the pipeline changed
    emit pipelineChanged();

    preflightPipeline();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::moveFilterWidget(PipelineFilterObject* fw, QVariant origin, QVariant destination, bool allowUndo)
{
  Q_UNUSED(allowUndo)

  if (NULL == fw)
  {
    return;
  }

  if (allowUndo == true)
  {
    MoveFilterCommand* cmd = new MoveFilterCommand(fw, origin, destination, this, true);
    addUndoCommand(cmd);
  }
  else
  {
    if (containsFilterWidget(fw) == true)
    {
      removeFilterObject(fw, false, false);
    }

    addFilterObject(fw, destination, false);

    setSelectedFilterObject(fw, Qt::NoModifier);

    preflightPipeline();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::pasteFilters(QList<AbstractFilter::Pointer> filters, QVariant value, bool allowUndo)
{
  if (filters.isEmpty()) { return; }

  bool ok;
  int index = value.toInt(&ok);
  if (ok == false) { return; }
  if (index < 0)
  {
    index = filterCount();
  }

  if (allowUndo == true)
  {
    QUndoCommand* topCmd = new QUndoCommand();
    topCmd->setText(QObject::tr("\"%1 %2 Filter Widgets\"").arg("Paste").arg(filters.size()));
    for (int i=0; i<filters.size(); i++)
    {
      //SVPipelineFilterWidget* filterWidget = new SVPipelineFilterWidget(filters[i]->newFilterInstance(true), NULL, NULL);
      new AddFilterCommand(filters[i], this, "Paste", index, QUuid(), QUuid(), topCmd);
      index++;
    }
    addUndoCommand(topCmd);
  }
  else
  {
    addFilters(filters, -1, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::pasteFilterWidgets(const QString &jsonString, QVariant value, bool allowUndo)
{
  if (allowUndo == true)
  {
    FilterPipeline::Pointer pipeline = JsonFilterParametersReader::ReadPipelineFromString(jsonString);
    FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();

    pasteFilters(container, value, allowUndo);
  }
  else
  {
    FilterPipeline::Pointer pipeline = JsonFilterParametersReader::ReadPipelineFromString(jsonString);
    FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();

    pasteFilters(container, value, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::startDrag(QMouseEvent* event, SVPipelineFilterWidget* fw)
{
  // The user is dragging the filter widget so we should set it as selected.
  if (fw->isSelected() == false)
  {
    setSelectedFilterObject(fw, Qt::NoModifier);
  }

  QList<PipelineFilterObject*> selectedObjects = getSelectedFilterObjects();
  setDraggedFilterObjects(selectedObjects);

  QPixmap pixmap = m_ShiftStart->grab();

  int pWidth = pixmap.size().width();
  int pHeight = pixmap.size().height() * selectedObjects.size() + (3 * (selectedObjects.size() - 1));

  // Create new picture for transparent
  QPixmap transparent(pWidth, pHeight);
  // Do transparency
  transparent.fill(Qt::transparent);

  QPainter p;
  p.begin(&transparent);
  p.setOpacity(0.70);
  int offset = 0;
  for (int i = 0; i < selectedObjects.size(); i++)
  {
    SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(selectedObjects[i]);
    Q_ASSERT(filterWidget != nullptr);

    QPixmap currentPixmap = filterWidget->grab();
    p.drawPixmap(0, offset, currentPixmap);
    offset = offset + pixmap.size().height() + 3;
  }
  p.end();

  PipelineViewPtrMimeData* mimeData = new PipelineViewPtrMimeData();
  mimeData->setPipelineViewPtr(this);

  QDrag* drag = new QDrag(this);
  drag->setMimeData(mimeData);
  drag->setPixmap(transparent);
  drag->setHotSpot(event->pos());

  drag->exec(Qt::CopyAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVPipelineViewWidget::eventFilter(QObject* o, QEvent* e)
{
  if ( e->type() == QEvent::Wheel && qobject_cast<SVPipelineFilterWidget*>(o) )
  {
    return false;
  }
  else if (dynamic_cast<SVPipelineFilterWidget*>(o) != nullptr && e->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(e);
    if (mouseEvent != nullptr && mouseEvent->button() == Qt::RightButton)
    {
      SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(o);

      if (filterWidget != nullptr && filterWidget->getFilter() != nullptr)
      {
        filterWidget->setHasRightClickTarget(true);

        // Show Pipeline Filter Widget context menu
        QMenu* pfwContextMenu = createPipelineFilterWidgetMenu(filterWidget);
        pfwContextMenu->exec(QCursor::pos());
        delete pfwContextMenu;

        filterWidget->setHasRightClickTarget(false);
      }
      return true;
    }
  }

  return QFrame::eventFilter( o, e );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::on_focusInEventStarted(QFocusEvent* event)
{
  Q_UNUSED(event)

  setFocus();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::on_focusOutEventStarted(QFocusEvent* event)
{
  focusOutEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::preflightPipeline(QUuid id)
{
  Q_UNUSED(id)

  emit pipelineIssuesCleared();
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = getFilterPipeline();

  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
  for(int i = 0; i < filters.size(); i++)
  {
    PipelineFilterObject* fw = filterObjectAt(i);
    if (fw)
    {
      fw->setHasPreflightErrors(false);
    }
    filters.at(i)->setErrorCondition(0);
    filters.at(i)->setCancel(false);
  }


  QProgressDialog progress("Preflight Pipeline", "", 0, 1, this);
  progress.setWindowModality(Qt::WindowModal);

  // Preflight the pipeline
  int err = pipeline->preflightPipeline();
  if (err < 0)
  {
    //FIXME: Implement error handling.
  }
  progress.setValue(1);

  int count = pipeline->getFilterContainer().size();
  //Now that the preflight has been executed loop through the filters and check their error condition and set the
  // outline on the filter widget if there were errors or warnings
  for(qint32 i = 0; i < count; ++i)
  {
    PipelineFilterObject* fw = filterObjectAt(i);
    if (fw)
    {
      AbstractFilter::Pointer filter = fw->getFilter();
      if(filter->getErrorCondition() < 0) {fw->setHasPreflightErrors(true);}
    }
  }
  emit preflightPipelineComplete();
  emit preflightFinished(err);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::removeFilterObject(PipelineFilterObject* filterObject, bool allowUndo, bool deleteWidget)
{
  SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(filterObject);
  Q_ASSERT(filterWidget != nullptr);

  if (filterWidget)
  {
    if (allowUndo)
    {
      RemoveFilterCommand* cmd = new RemoveFilterCommand(filterWidget, this, "Remove");
      addUndoCommand(cmd);
    }
    else
    {
      QWidget* w = qobject_cast<QWidget*>(filterWidget);
      m_FilterWidgetLayout->removeWidget(w);

      if (w)
      {
        filterWidget->getFilter()->setPreviousFilter(AbstractFilter::NullPointer());
        filterWidget->getFilter()->setNextFilter(AbstractFilter::NullPointer());

        if (deleteWidget == true)
        {
          w->deleteLater();
        }
        else
        {
          w->setParent(nullptr);

          // When the filter is removed from this view
          disconnect(filterWidget, SIGNAL(filterWidgetRemoved(PipelineFilterObject*, bool)),
                  this, SLOT(removeFilterObject(PipelineFilterObject*, bool)) );

          // When the FilterWidget is selected
          disconnect(filterWidget, SIGNAL(filterWidgetPressed(PipelineFilterObject*, Qt::KeyboardModifiers)),
                  this, SLOT(setSelectedFilterObject(PipelineFilterObject*, Qt::KeyboardModifiers)));

          // When the filter widget is dragged
          disconnect(filterWidget, SIGNAL(dragStarted(QMouseEvent*, SVPipelineFilterWidget*)),
                  this, SLOT(startDrag(QMouseEvent*, SVPipelineFilterWidget*)));

          disconnect(filterWidget, SIGNAL(parametersChanged(QUuid)),
                  this, SLOT(preflightPipeline(QUuid)));

          disconnect(filterWidget, SIGNAL(parametersChanged(QUuid)),
                  this, SLOT(handleFilterParameterChanged(QUuid)));

          disconnect(filterWidget, SIGNAL(focusInEventStarted(QFocusEvent*)), this, SLOT(on_focusInEventStarted(QFocusEvent*)));

          disconnect(filterWidget, SIGNAL(focusOutEventStarted(QFocusEvent*)), this, SLOT(on_focusOutEventStarted(QFocusEvent*)));
        }
      }

      if (getSelectedFilterObjects().isEmpty())
      {
        m_ShiftStart = NULL;
      }

      QSpacerItem* spacer = m_FilterWidgetLayout->itemAt(0)->spacerItem();
      if (NULL != spacer)
      {
        m_FilterWidgetLayout->removeItem(spacer);
      }

      reindexWidgetTitles();
      preflightPipeline();

      resetLayout();
      emit pipelineChanged();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::removeFilterObjects(QList<PipelineFilterObject*> filterWidgets, bool allowUndo, bool deleteWidgets)
{
  if (allowUndo == true)
  {
    QUndoCommand* topCmd = new QUndoCommand();
    topCmd->setText(QObject::tr("\"%1 %2 Filter Widgets\"").arg("Remove").arg(filterWidgets.size()));
    for (int i = filterWidgets.size() - 1; i >= 0; i--)
    {
      new RemoveFilterCommand(filterWidgets[i], this, "Remove", QUuid(), QUuid(), topCmd);
    }
    addUndoCommand(topCmd);
  }
  else
  {
    for (int i=0; i<filterWidgets.size(); i++)
    {
      removeFilterObject(filterWidgets[i], false, deleteWidgets);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::addUndoCommand(QUndoCommand* cmd)
{
  m_UndoStack->push(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::cutFilterWidgets(QList<PipelineFilterObject*> filterWidgets, bool allowUndo)
{
  if (filterWidgets.isEmpty())
  {
    return;
  }

  if (allowUndo == true)
  {
    QUndoCommand* topCmd = new QUndoCommand();
    topCmd->setText(QObject::tr("\"%1 %2 Filter Widgets\"").arg("Cut").arg(filterWidgets.size()));
    for (int i = filterWidgets.size() - 1; i >= 0; i--)
    {
      new RemoveFilterCommand(filterWidgets[i], this, "Cut", QUuid(), QUuid(), topCmd);
    }
    addUndoCommand(topCmd);
  }
  else
  {
    removeFilterObjects(filterWidgets, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::setSelectedFilterObject(PipelineFilterObject* w, Qt::KeyboardModifiers modifiers)
{
  SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(w);
  Q_ASSERT(filterWidget != nullptr);

  if (modifiers == Qt::ShiftModifier)
  {
    clearSelectedFilterObjects();

    if (NULL == m_ShiftStart)
    {
      m_ShiftStart = filterWidget;
    }

    int begin;
    int end;
    if (m_FilterWidgetLayout->indexOf(filterWidget) < m_FilterWidgetLayout->indexOf(m_ShiftStart))
    {
      // The filter widget that was just selected is before the "active" widget
      begin = m_FilterWidgetLayout->indexOf(filterWidget);
      end = m_FilterWidgetLayout->indexOf(m_ShiftStart);
    }
    else
    {
      // The filter widget that was just selected is after the "active" widget
      begin = m_FilterWidgetLayout->indexOf(m_ShiftStart);
      end = m_FilterWidgetLayout->indexOf(filterWidget);
    }

    for (int i = begin; i <= end; i++)
    {
      SVPipelineFilterWidget* fw = dynamic_cast<SVPipelineFilterWidget*>(filterObjectAt(i));
      if(fw) {
        fw->setSelected(true);
      }
    }
  }
  else if (modifiers == Qt::ControlModifier)
  {
    m_ShiftStart = filterWidget;

    if (filterWidget->isSelected())
    {
      filterWidget->setSelected(true);
      if (getSelectedFilterObjects().isEmpty())
      {
        m_ShiftStart = NULL;
      }
    }
    else
    {
      filterWidget->setSelected(true);
    }
  }
  else
  {
    clearSelectedFilterObjects();

    m_ShiftStart = filterWidget;
    filterWidget->setSelected(true);
  }

  QList<PipelineFilterObject*> selectedObjects = getSelectedFilterObjects();

  if (selectedObjects.size() == 1)
  {
    emit filterInputWidgetChanged(selectedObjects[0]->getFilterInputWidget());
  }
  else
  {
    emit filterInputWidgetNeedsCleared();
  }

  filterWidget->setFocus();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::clearSelectedFilterObjects()
{
  for (int i=0; i<filterCount(); i++)
  {
    if (NULL != dynamic_cast<DropBoxWidget*>(m_FilterWidgetLayout->itemAt(i)->widget()))
    {
      continue;
    }
    else
    {
      SVPipelineFilterWidget* fw = dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      fw->setSelected(false);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::addSIMPLViewReaderFilter(const QString& filePath, QVariant value)
{
  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(filePath);

  addFilter(reader, value, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::populatePipelineView(FilterPipeline::Pointer pipeline, QVariant value)
{
  if (NULL == pipeline.get()) { clearFilterWidgets(); return; }

  FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();

  pasteFilters(container, value);

  if (filterCount() > 0)
  {
    SVPipelineFilterWidget* fw = dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(0)->widget());
    if (fw)
    {
      setSelectedFilterObject(fw, Qt::NoModifier);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
  {
    bool isRunning = getPipelineIsRunning();
    if (isRunning == false)
    {
      emit deleteKeyPressed(this);
    }
  }
  else if (event->key() == Qt::Key_A && qApp->queryKeyboardModifiers() == Qt::ControlModifier)
  {
    clearSelectedFilterObjects();

    for (int i=0; i<filterCount(); i++)
    {
      SVPipelineFilterWidget* filterWidget = qobject_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      setSelectedFilterObject(filterWidget, Qt::ControlModifier);
    }
  }

  QFrame::keyPressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVPipelineViewWidget::containsFilterWidget(PipelineFilterObject* filterWidget)
{
  for (int i = 0; i < m_FilterWidgetLayout->count(); i++)
  {
    if (dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget()) == filterWidget)
    {
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::toRunningState()
{
  setPipelineIsRunning(true);
  setAcceptDrops(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::toIdleState()
{
  setPipelineIsRunning(false);
  setAcceptDrops(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer SVPipelineViewWidget::readPipelineFromFile(const QString& filePath)
{
  QFileInfo fi(filePath);
  QString ext = fi.suffix();

  FilterPipeline::Pointer pipeline;
  if (ext == "dream3d")
  {
    pipeline = H5FilterParametersReader::ReadPipelineFromFile(filePath);
  }
  else if (ext == "json")
  {
    pipeline = JsonFilterParametersReader::ReadPipelineFromFile(filePath);
  }
  else
  {
    pipeline = FilterPipeline::NullPointer();
  }

  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SVPipelineViewWidget::writePipeline(QString filePath)
{
  QFileInfo fi(filePath);
  QString ext = fi.completeSuffix();

  //If the filePath already exists - delete it so that we get a clean write to the file
  if (fi.exists() == true && (ext == "dream3d" || ext == "json"))
  {
    QFile f(filePath);
    if (f.remove() == false)
    {
      QMessageBox::warning(this, QString::fromLatin1("Pipeline Write Error"),
                           QString::fromLatin1("There was an error removing the existing pipeline file. The pipeline was NOT saved."));
      return -1;
    }
  }

  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = getFilterPipeline();

  int err = 0;
  if (ext == "dream3d")
  {
    err = H5FilterParametersWriter::WritePipelineToFile(pipeline, fi.absoluteFilePath(), fi.fileName(), reinterpret_cast<IObserver*>(m_PipelineMessageObserver));
  }
  else if (ext == "json")
  {
    JsonFilterParametersWriter::WritePipelineToFile(pipeline, fi.absoluteFilePath(), fi.fileName(), reinterpret_cast<IObserver*>(m_PipelineMessageObserver));
  }
  else
  {
    emit statusMessage(tr("The pipeline was not written to file '%1'. '%2' is an unsupported file extension.").arg(fi.fileName()).arg(ext));
    return -1;
  }

  if (err < 0)
  {
    emit statusMessage(tr("There was an error while saving the pipeline to file '%1'.").arg(fi.fileName()));
    return -1;
  }
  else
  {
    emit statusMessage(tr("The pipeline has been saved successfully to '%1'.").arg(fi.fileName()));
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::dragMoveEvent(QDragMoveEvent* event)
{
  // Remove the drop box, if it exists
  if (NULL != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_DropBox) != -1)
  {
    m_FilterWidgetLayout->removeWidget(m_DropBox);
    m_DropBox->setParent(NULL);
  }

  // If cursor is within margin boundaries, start scrolling
  if (shouldAutoScroll(event->pos()))
  {
    startAutoScroll();
  }
  // Otherwise, stop scrolling
  else
  {
    stopAutoScroll();
  }

  const QMimeData* mimedata = event->mimeData();
  if (NULL != dynamic_cast<const PipelineViewPtrMimeData*>(mimedata))
  {
    // The user is moving existing filter widgets, either within the same pipeline view or between pipeline views
    SVPipelineViewWidget* origin = dynamic_cast<const PipelineViewPtrMimeData*>(mimedata)->getPipelineViewPtr();

    if (origin == this)
    {
      if (qApp->queryKeyboardModifiers() != Qt::AltModifier)
      {
        QList<PipelineFilterObject*> draggedFilterObjects = getDraggedFilterObjects();
        if (draggedFilterObjects.size() > 1)
        {
          event->ignore();
          return;
        }
        else if (draggedFilterObjects.size() == 1)
        {
          // Remove the filter widget
          if (NULL != m_FilterWidgetLayout && origin->containsFilterWidget(draggedFilterObjects[0]))
          {
            PipelineFilterObject* draggedObject = draggedFilterObjects[0];
            SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(draggedObject);
            Q_ASSERT(filterWidget != nullptr);

            m_FilterOrigPos = origin->valueOfFilterWidget(filterWidget).toInt();
            m_FilterWidgetLayout->removeWidget(filterWidget);
            filterWidget->setParent(NULL);
          }
        }
      }
    }

    int count = filterCount();
    if (count == 0)
    {
      return;
    }

    for (int i = 0; i <= count; ++i)
    {
      SVPipelineFilterWidget* w;
      if (NULL == m_FilterWidgetLayout)
      {
        return;
      }
      else if (i == count)
      {
        w = qobject_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i-1)->widget());
      }
      else
      {
        w = qobject_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      }
      if (w != NULL)
      {
        if ((i == count && event->pos().y() >= w->geometry().y() + w->geometry().height() / 2) || (event->pos().y() <= w->geometry().y() + w->geometry().height() / 2))
        {
          QList<PipelineFilterObject*> draggedObjects = origin->getDraggedFilterObjects();
          if (draggedObjects.size() > 1)
          {
            m_DropBox->setLabel("Place " + QString::number(draggedObjects.size()) + " Filters Here");
          }
          else if (draggedObjects.size() == 1)
          {
            m_DropBox->setLabel("    [" + QString::number(i + 1) + "] " + draggedObjects[0]->getHumanLabel());
          }
          else
          {
            event->ignore();
            return;
          }
          m_FilterWidgetLayout->insertWidget(i, m_DropBox);
          reindexWidgetTitles();
          break;
        }
      }
    }

    event->accept();
  }
  else if (mimedata->hasUrls() || mimedata->hasFormat(SIMPLView::DragAndDrop::BookmarkItem) || mimedata->hasFormat(SIMPLView::DragAndDrop::FilterItem))
  {
    QString data;
    if (mimedata->hasUrls())
    {
      data = mimedata->text();
      QUrl url(data);
      data = url.toLocalFile();
    }
    else if (mimedata->hasText())
    {
      data = mimedata->text();
    }
    else if (mimedata->hasFormat(SIMPLView::DragAndDrop::BookmarkItem))
    {
      QByteArray jsonArray = mimedata->data(SIMPLView::DragAndDrop::BookmarkItem);
      QJsonDocument doc = QJsonDocument::fromJson(jsonArray);
      QJsonObject obj = doc.object();

      if (obj.size() > 1)
      {
        event->accept();
        return;
      }

      QJsonObject::iterator iter = obj.begin();
      data = iter.value().toString();
    }
    else
    {
      QByteArray jsonArray = mimedata->data(SIMPLView::DragAndDrop::FilterItem);
      QJsonDocument doc = QJsonDocument::fromJson(jsonArray);
      QJsonObject obj = doc.object();
      QJsonObject::iterator iter = obj.begin();
      data = iter.value().toString();
    }

    QFileInfo fi(data);
    QString ext = fi.completeSuffix();
    FilterManager* fm = FilterManager::Instance();
    if (NULL == fm) { return; }
    IFilterFactory::Pointer wf = fm->getFactoryForFilter(data);

    // If the dragged item is a filter item...
    if (NULL != wf)
    {
      QString humanName = wf->getFilterHumanLabel();

      bool didInsert = false;
      // This path is taken if a filter is dragged from the list of filters

      int count = filterCount();
      for (int i = 0; i < count; ++i)
      {
        SVPipelineFilterWidget* w = qobject_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
        if (NULL != w && event->pos().y() <= w->geometry().y() + w->geometry().height() / 2)
        {
          m_DropBox->setLabel("    [" + QString::number(i + 1) + "] " + humanName);
          m_FilterWidgetLayout->insertWidget(i, m_DropBox);
          reindexWidgetTitles();
          didInsert = true;
          break;
        }
      }
      // Check to see if we are trying to move it to the end
      if (false == didInsert && count > 0)
      {
        SVPipelineFilterWidget* w = qobject_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(count - 1)->widget());
        if (NULL != w && event->pos().y() >= w->geometry().y() + w->geometry().height() / 2)
        {
          m_DropBox->setLabel("    [" + QString::number(count) + "] " + humanName);
          m_FilterWidgetLayout->insertWidget(count, m_DropBox);
          reindexWidgetTitles();
        }
      }

      event->accept();
    }
    // If the dragged item is a pipeline file...
    else if (ext == "dream3d" || ext == "json" || ext == "ini" || ext == "txt")
    {
      QString pipelineName = fi.baseName();

      bool didInsert = false;
      // This path is taken if a filter is dragged from the list of filters

      int count = filterCount();
      for (int i = 0; i < count; ++i)
      {
        SVPipelineFilterWidget* w = qobject_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
        if (NULL != w && event->pos().y() <= w->geometry().y() + w->geometry().height() / 2)
        {
          m_DropBox->setLabel("Place '" + pipelineName + "' Here");
          m_FilterWidgetLayout->insertWidget(i, m_DropBox);
          reindexWidgetTitles();
          didInsert = true;
          break;
        }
      }
      // Check to see if we are trying to move it to the end
      if (false == didInsert && count > 0)
      {
        SVPipelineFilterWidget* w = qobject_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(count - 1)->widget());
        if (NULL != w && event->pos().y() >= w->geometry().y() + w->geometry().height() / 2)
        {
          m_DropBox->setLabel("Place '" + pipelineName + "' Here");
          m_FilterWidgetLayout->insertWidget(count, m_DropBox);
          reindexWidgetTitles();
        }
      }

      event->accept();
    }
    else
    {
      event->ignore();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::dropEvent(QDropEvent* event)
{
  const QMimeData* mimedata = event->mimeData();
  if (mimedata->hasUrls() || mimedata->hasText() || mimedata->hasFormat(SIMPLView::DragAndDrop::BookmarkItem) || mimedata->hasFormat(SIMPLView::DragAndDrop::FilterItem))
  {
    QString data;

    if (mimedata->hasUrls())
    {
      data = mimedata->text();
      QUrl url(data);
      data = url.toLocalFile();
    }
    else if (mimedata->hasText())
    {
      data = mimedata->text();
    }
    else if (mimedata->hasFormat(SIMPLView::DragAndDrop::FilterItem))
    {
      QByteArray jsonArray = mimedata->data(SIMPLView::DragAndDrop::FilterItem);
      QJsonDocument doc = QJsonDocument::fromJson(jsonArray);
      QJsonObject obj = doc.object();
      QJsonObject::iterator iter = obj.begin();
      data = iter.value().toString();
    }
    else
    {
      QByteArray jsonArray = mimedata->data(SIMPLView::DragAndDrop::BookmarkItem);
      QJsonDocument doc = QJsonDocument::fromJson(jsonArray);
      QJsonObject obj = doc.object();

      if (obj.size() > 1)
      {
        QMessageBox::warning(NULL, "SIMPLView Warning", "SIMPLView currently does not support dragging and dropping multiple bookmarks.", QMessageBox::Ok);
        event->ignore();
        return;
      }

      QJsonObject::iterator iter = obj.begin();
      data = iter.value().toString();
    }

    QFileInfo fi(data);
    QString ext = fi.completeSuffix();
    FilterManager* fm = FilterManager::Instance();
    if (NULL == fm) { return; }
    IFilterFactory::Pointer wf = fm->getFactoryForFilter(data);

    // If the dragged item is a filter item...
    if (NULL != wf)
    {
      int index;

      // We need to figure out where it was dropped relative to other filters
      if (NULL != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_DropBox) != -1)
      {
        index = m_FilterWidgetLayout->indexOf(m_DropBox);
      }
      else
      {
        index = -1;
      }

      AbstractFilter::Pointer filter = wf->create();
      addFilter(filter, index, true);

      emit pipelineChanged();
      event->accept();
    }
    // If the dragged item is a pipeline file...
    else if (ext == "dream3d" || ext == "json" || ext == "ini" || ext == "txt")
    {
      int index = 0;
      if (NULL != m_FilterWidgetLayout)
      {
        index = m_FilterWidgetLayout->indexOf(m_DropBox);
      }

      if (ext == "json" || ext == "ini" || ext == "txt")
      {
        openPipeline(data, index, false, false);

        emit pipelineChanged();
      }
      else if (ext == "dream3d")
      {
        QtSFileDragMessageBox* msgBox = new QtSFileDragMessageBox(this);
        msgBox->exec();
        msgBox->deleteLater();

        if (msgBox->didPressOkBtn() == true)
        {
          if (msgBox->isExtractPipelineBtnChecked() == true)
          {
            openPipeline(data, index, false, false);
          }
          else
          {
            addSIMPLViewReaderFilter(data, index);
            emit pipelineChanged();
          }
        }
      }
      event->accept();
    }
    else
    {
      event->ignore();
    }
  }
  else if (NULL != dynamic_cast<const PipelineViewPtrMimeData*>(mimedata))
  {
    SVPipelineViewWidget* origin = dynamic_cast<const PipelineViewPtrMimeData*>(mimedata)->getPipelineViewPtr();

    // The drop box, if it exists, marks the index where the filter should be dropped
    int index;
    if (NULL != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_DropBox) != -1)
    {
      index = m_FilterWidgetLayout->indexOf(m_DropBox);
      m_FilterWidgetLayout->removeWidget(m_DropBox);
      m_DropBox->setParent(NULL);
    }
    else
    {
      index = filterCount();
    }

    QList<PipelineFilterObject*> draggedFilterObjects = getDraggedFilterObjects();

    if (origin != this || (origin == this && qApp->queryKeyboardModifiers() == Qt::AltModifier))
    {
      QList<PipelineFilterObject*> filterObjects = origin->getDraggedFilterObjects();

      FilterPipeline::Pointer pipeline = FilterPipeline::New();
      for (int i=0; i<filterObjects.size(); i++)
      {
        pipeline->pushBack(filterObjects[i]->getFilter());
      }

      QString jsonString = JsonFilterParametersWriter::WritePipelineToString(pipeline, "Pipeline");
      pasteFilterWidgets(jsonString, index, true);

      if (qApp->queryKeyboardModifiers() != Qt::AltModifier)
      {
        origin->cutFilterWidgets(filterObjects);
      }

      origin->setDraggedFilterObjects(QList<PipelineFilterObject*>());
      event->accept();
    }
    else if (draggedFilterObjects.size() == 1)
    {
      moveFilterWidget(draggedFilterObjects[0], m_FilterOrigPos, index);
      setDraggedFilterObjects(QList<PipelineFilterObject*>());
      event->accept();
    }
    else
    {
      event->ignore();
    }
  }

  // Stop auto scrolling if widget is dropped
  stopAutoScroll();

  // Remove the drop line, if it exists
  if (NULL != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_DropBox) != -1)
  {
    m_FilterWidgetLayout->removeWidget(m_DropBox);
    m_DropBox->setParent(NULL);
    reindexWidgetTitles();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
  int index;
  if (NULL != m_FilterWidgetLayout)
  {
    index = m_FilterWidgetLayout->indexOf(m_DropBox);
  }

  // Remove the placeholder drop box
  if (NULL != m_FilterWidgetLayout && index != -1)
  {
    m_FilterWidgetLayout->removeWidget(m_DropBox);
    m_DropBox->setParent(NULL);
  }

  QList<PipelineFilterObject*> draggedFilterObjects = getDraggedFilterObjects();

  // Put filter widget back to original position
  if (draggedFilterObjects.size() == 1 && qApp->queryKeyboardModifiers() != Qt::AltModifier)
  {
    SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(draggedFilterObjects[0]);
    Q_ASSERT(filterWidget != nullptr);

    m_FilterWidgetLayout->insertWidget(m_FilterOrigPos, filterWidget);
    setSelectedFilterObject(filterWidget, Qt::NoModifier);
  }

  reindexWidgetTitles();

//  // Set the current filter as previous, and nullify the current
//  m_PreviousFilterBeingDragged = m_CurrentFilterBeingDragged;
//  m_CurrentFilterBeingDragged = NULL;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::dragEnterEvent(QDragEnterEvent* event)
{
  event->acceptProposedAction();

//  // If there is a previous filter, set it as current
//  if (NULL != m_PreviousFilterBeingDragged && event->dropAction() == Qt::MoveAction)
//  {
//    m_CurrentFilterBeingDragged = m_PreviousFilterBeingDragged;
//  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::stopAutoScroll()
{
  m_autoScrollTimer.stop();
  m_autoScrollCount = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::startAutoScroll()
{
  int scrollInterval = 50;
  m_autoScrollTimer.start(scrollInterval);
  m_autoScrollCount = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::doAutoScroll()
{
  // find how much we should scroll with
  int verticalStep = m_ScrollArea->verticalScrollBar()->pageStep();
  int horizontalStep = m_ScrollArea->horizontalScrollBar()->pageStep();
  if (m_autoScrollCount < qMax(verticalStep, horizontalStep))
  { m_autoScrollCount = m_autoScrollCount + 15; }

  int margin = m_AutoScrollMargin;
  int verticalValue = m_ScrollArea->verticalScrollBar()->value();
  int horizontalValue = m_ScrollArea->horizontalScrollBar()->value();

  QPoint pos = m_ScrollArea->viewport()->mapFromGlobal(QCursor::pos());
  QRect area = m_ScrollArea->geometry();

  // do the scrolling if we are in the scroll margins
  if (pos.y() - area.top() < margin)
  { m_ScrollArea->verticalScrollBar()->setValue(verticalValue - m_autoScrollCount); }
  else if (area.bottom() - pos.y() < margin)
  { m_ScrollArea-> verticalScrollBar()->setValue(verticalValue + m_autoScrollCount); }
  //  if (pos.x() - area.left() < margin)
  //    m_ScrollArea->horizontalScrollBar()->setValue(horizontalValue - d->m_autoScrollCount);
  //  else if (area.right() - pos.x() < margin)
  //    m_ScrollArea->horizontalScrollBar()->setValue(horizontalValue + d->m_autoScrollCount);
  // if nothing changed, stop scrolling
  bool verticalUnchanged = (verticalValue == m_ScrollArea->verticalScrollBar()->value());
  bool horizontalUnchanged = (horizontalValue == m_ScrollArea->horizontalScrollBar()->value());
  if (verticalUnchanged && horizontalUnchanged)
  {
    stopAutoScroll();
  }
  else
  {
    m_ScrollArea->viewport()->update();
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVPipelineViewWidget::shouldAutoScroll(const QPoint& pos)
{
  if (NULL == m_ScrollArea) { return false; }
  QPoint scpos = m_ScrollArea->viewport()->mapFromGlobal(QCursor::pos());
  QRect rect = m_ScrollArea->geometry();

  if (scpos.y() <= getAutoScrollMargin())
  {
    return true;
  }
  else if (pos.y() >= rect.height() - getAutoScrollMargin())
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    clearSelectedFilterObjects();
    m_ShiftStart = NULL;
    emit filterInputWidgetNeedsCleared();
  }

  QFrame::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::requestContextMenu(const QPoint& pos)
{
  activateWindow();

  m_ContextMenu->exec(mapToGlobal(pos));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<PipelineFilterObject*> SVPipelineViewWidget::getSelectedFilterObjects()
{ 
  QList<PipelineFilterObject*> filterObjects;
  for (int i=0; i<filterCount(); i++)
  {
    if (NULL != dynamic_cast<DropBoxWidget*>(m_FilterWidgetLayout->itemAt(i)->widget()))
    {
      continue;
    }
    else
    {
      SVPipelineFilterWidget* fw = dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      if (fw->isSelected() == true || fw->hasRightClickTarget() == true)
      {
        filterObjects.push_back(fw);
      }
    }
  }
  return filterObjects;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* SVPipelineViewWidget::getActionRedo()
{
  return m_ActionRedo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* SVPipelineViewWidget::getActionUndo()
{
  return m_ActionUndo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::setPipelineMessageObserver(QObject* pipelineMessageObserver)
{
  m_PipelineMessageObserver = pipelineMessageObserver;
  // setup our connection
  connect(this, SIGNAL(pipelineIssuesCleared()),
          m_PipelineMessageObserver, SLOT(clearIssues()) );
  connect(this, SIGNAL(preflightPipelineComplete()),
          m_PipelineMessageObserver, SLOT(displayCachedMessages()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::handleFilterParameterChanged(QUuid id)
{
  Q_UNUSED(id)

  emit filterInputWidgetEdited();
}

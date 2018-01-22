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

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMimeData>
#include <QtCore/QTemporaryFile>
#include <QtCore/QUrl>
#include <QtCore/QSharedPointer>

#include <QtGui/QClipboard>
#include <QtGui/QDrag>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QVBoxLayout>

#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/Breakpoint.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/SIMPLib.h"

#include "SVWidgetsLib/QtSupport/QtSDroppableScrollArea.h"

#include "SVWidgetsLib/Core/FilterWidgetManager.h"
#include "SVWidgetsLib/Core/PipelineViewPtrMimeData.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"
#include "SVWidgetsLib/Widgets/BreakpointFilterWidget.h"
#include "SVWidgetsLib/Widgets/SIMPLViewMenuItems.h"
#include "SVWidgetsLib/Widgets/util/AddFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/MoveFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/RemoveFilterCommand.h"
#include "SVWidgetsLib/Widgets/DataStructureWidget.h"
#include "SVWidgetsLib/Widgets/ProgressDialog.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineViewWidget::SVPipelineViewWidget(QWidget* parent)
: QFrame(parent)
, PipelineView(parent)
, m_PipelineIsRunning(false)
, m_FilterOrigPos(-1)
, m_AutoScroll(true)
, m_AutoScrollMargin(10)
, m_autoScrollCount(0)
, m_UndoStack(new QUndoStack(this))
, m_BlockPreflight(false)
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineViewWidget::~SVPipelineViewWidget()
{
  // These disconnections are needed so that the slots are not called when the undo stack is deconstructed.  Calling the slots during deconstruction causes a crash.
  disconnect(m_UndoStack.data(), SIGNAL(undoTextChanged(const QString &)), this, SLOT(updateCurrentUndoText(const QString &)));
  disconnect(m_UndoStack.data(), SIGNAL(redoTextChanged(const QString &)), this, SLOT(updateCurrentRedoText(const QString &)));

  if(m_ContextMenu)
  {
    delete m_ContextMenu;
  }
  if(m_FilterOutlineWidget && !m_FilterOutlineWidget->parent())
  {
    delete m_FilterOutlineWidget;
  }
  // Delete action if it exists
  if(m_ActionEnableFilter)
  {
    delete m_ActionEnableFilter;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::setupGui()
{
  newEmptyPipelineViewLayout();
  connect(&m_autoScrollTimer, SIGNAL(timeout()), this, SLOT(doAutoScroll()));

  // Delete action if it exists
  if(m_ActionEnableFilter)
  {
    delete m_ActionEnableFilter;
  }

  m_ActionEnableFilter = new QAction("Enable", this);
  m_ActionEnableFilter->setCheckable(true);
  m_ActionEnableFilter->setChecked(true);
  m_ActionEnableFilter->setEnabled(false);

  // connect(this, SIGNAL(deleteKeyPressed(PipelineView*)), dream3dApp, SLOT(on_pipelineViewWidget_deleteKeyPressed(PipelineView*)));
  m_UndoStack = QSharedPointer<QUndoStack>(new QUndoStack(this));
  m_UndoStack->setUndoLimit(10);

  m_FilterOutlineWidget = new SVPipelineFilterOutlineWidget(nullptr);
  m_FilterOutlineWidget->setObjectName("m_DropBox");

  m_ActionUndo = m_UndoStack->createUndoAction(nullptr);
  m_ActionRedo = m_UndoStack->createRedoAction(nullptr);
  m_ActionUndo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
  m_ActionRedo->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));

  connect(m_UndoStack.data(), SIGNAL(undoTextChanged(const QString &)), this, SLOT(updateCurrentUndoText(const QString &)));
  connect(m_UndoStack.data(), SIGNAL(redoTextChanged(const QString &)), this, SLOT(updateCurrentRedoText(const QString &)));

  connect(m_ActionUndo, SIGNAL(triggered()), this, SLOT(actionUndo_triggered()));
  connect(m_ActionRedo, SIGNAL(triggered()), this, SLOT(actionRedo_triggered()));

  m_autoScrollTimer.setParent(this);

  setContextMenuPolicy(Qt::CustomContextMenu);
  setFocusPolicy(Qt::StrongFocus);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(requestContextMenu(const QPoint&)));

  createPipelineViewWidgetMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject* SVPipelineViewWidget::createFilterObjectFromFilter(AbstractFilter::Pointer filter)
{
  Breakpoint::Pointer breakpoint = std::dynamic_pointer_cast<Breakpoint>(filter);

  // Create a FilterWidget object
  SVPipelineFilterWidget* fw;
  if(NULL != breakpoint)
  {
    fw = new BreakpointFilterWidget(filter, nullptr, this);
  }
  else
  {
    fw = new SVPipelineFilterWidget(filter, nullptr, this);
  }

  return fw;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* SVPipelineViewWidget::getActionEnableFilter()
{
  return m_ActionEnableFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::updateActionEnableFilter()
{
  QList<PipelineFilterObject*> selectedObjs = getSelectedFilterObjects();

  // Set Enabled / Disabled
  disconnect(m_ActionEnableFilter, &QAction::toggled, 0, 0);
  m_ActionEnableFilter->setEnabled(selectedObjs.size());

  // Set checked state
  int count = selectedObjs.size();
  bool widgetEnabled = true;
  for(int i = 0; i < count && widgetEnabled; i++)
  {
    widgetEnabled = selectedObjs[i]->getFilter()->getEnabled();
  }

  // Lambda connections don't allow Qt::UniqueConnection
  // Also, this needs to be disconnected before changing the checked state
  m_ActionEnableFilter->setChecked(widgetEnabled);

  connect(m_ActionEnableFilter, &QAction::toggled, [=] { setSelectedFiltersEnabled(m_ActionEnableFilter->isChecked()); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* SVPipelineViewWidget::createPipelineFilterWidgetMenu(SVPipelineFilterWidget* filterWidget)
{
  // Creating Pipeline Filter Widget Menu
  SIMPLViewMenuItems* menuItems = SIMPLViewMenuItems::Instance();

  QList<PipelineFilterObject*> selectedObjs = getSelectedFilterObjects();

  QMenu* contextMenu = new QMenu(this);

  contextMenu->addAction(menuItems->getActionCut());
  contextMenu->addAction(menuItems->getActionCopy());

  contextMenu->addSeparator();

  int count = selectedObjs.size();
  bool widgetEnabled = true;
  for(int i = 0; i < count && widgetEnabled; i++)
  {
    widgetEnabled = selectedObjs[i]->getFilter()->getEnabled();
  }

  if(selectedObjs.contains(filterWidget) == false)
  {
    // Only toggle the target filter widget if it is not in the selected objects
    QList<PipelineFilterObject*> toggledObjects = QList<PipelineFilterObject*>();
    toggledObjects.push_back(filterWidget);

    QAction* actionEnableFilter = new QAction("Enable", this);
    actionEnableFilter->setCheckable(true);

    widgetEnabled = filterWidget->getFilter()->getEnabled();
    actionEnableFilter->setChecked(widgetEnabled);

    connect(actionEnableFilter, &QAction::toggled, [=] { setFiltersEnabled(toggledObjects, actionEnableFilter->isChecked()); });
    contextMenu->addAction(actionEnableFilter);
  }
  else
  {
    disconnect(m_ActionEnableFilter, &QAction::toggled, 0, 0);
    m_ActionEnableFilter->setChecked(widgetEnabled);

    connect(m_ActionEnableFilter, &QAction::toggled, [=] { setSelectedFiltersEnabled(m_ActionEnableFilter->isChecked()); });
    contextMenu->addAction(m_ActionEnableFilter);
  }

  contextMenu->addSeparator();

  QAction* removeAction;
  QList<QKeySequence> shortcutList;
  shortcutList.push_back(QKeySequence(Qt::Key_Backspace));
  shortcutList.push_back(QKeySequence(Qt::Key_Delete));

  if (selectedObjs.contains(filterWidget) == false || selectedObjs.size() == 1)
  {
    removeAction = new QAction("Delete", contextMenu);
    connect(removeAction, &QAction::triggered, [=] { removeFilterObject(filterWidget); });
  }
  else
  {
    removeAction = new QAction(tr("Delete %1 Filters").arg(selectedObjs.size()), contextMenu);
    connect(removeAction, &QAction::triggered, [=] { removeFilterObjects(selectedObjs); });
  }
  removeAction->setShortcuts(shortcutList);
  if (getPipelineIsRunning() == true)
  {
    removeAction->setDisabled(true);
  }

  contextMenu->addAction(removeAction);

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
void SVPipelineViewWidget::updateCurrentUndoText(const QString &text)
{
  m_PreviousUndoText = m_CurrentUndoText;
  m_CurrentUndoText = text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::updateCurrentRedoText(const QString &text)
{
  m_PreviousRedoText = m_CurrentRedoText;
  m_CurrentRedoText = text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::actionUndo_triggered()
{
  emit stdOutMessage("Undo " + m_PreviousUndoText);
//  QString text = m_ActionUndo->text();
//  emit stdOutMessage(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::actionRedo_triggered()
{
  emit stdOutMessage("Redo " + m_PreviousRedoText);
//  QString text = m_ActionRedo->text();
//  emit stdOutMessage(text);
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
  if(m_EmptyPipelineLabel == nullptr)
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
    QTextStream ss(&text);
    ss << "<h2>Creating a Pipeline</h2>";
    ss << "<hr>";
    ss << "File -> Open <br />";
    ss << "File -> New <br />";
    ss << "Drag and drop filters here<br />";
    ss << "Double click a Bookmark<br />";
    ss << "Double click a Prebuilt Pipeline<br />";
    m_EmptyPipelineLabel->setText(text);

    gridLayout->addWidget(m_EmptyPipelineLabel, 1, 1, 1, 1);

    QSpacerItem* horizontalSpacer_4 = new QSpacerItem(102, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_4, 1, 2, 1, 1);

    QSpacerItem* verticalSpacer_2 = new QSpacerItem(20, 341, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer_2, 2, 1, 1, 1);
  }
  emit windowNeedsRecheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SVPipelineViewWidget::filterCount(QUuid startId)
{
  Q_UNUSED(startId)

  int count = 0;
  if(nullptr != m_FilterWidgetLayout)
  {
    count = m_FilterWidgetLayout->count() - 1;

    if(count < 0)
    {
      count = 0;
    }
  }
  
  return count;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::focusInEvent(QFocusEvent* event)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::focusOutEvent(QFocusEvent* event)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject* SVPipelineViewWidget::filterObjectAt(QVariant value)
{
  bool ok;
  int index = value.toInt(&ok);
  if(ok == false)
  {
    return nullptr;
  }

  if(index < 0)
  {
    index = filterCount();
  }

  SVPipelineFilterWidget* fw = nullptr;
  if(m_FilterWidgetLayout != nullptr)
  {
    QLayoutItem* layoutItem = m_FilterWidgetLayout->itemAt(index);
    QWidget* w = layoutItem->widget();
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
  if(filterWidget == nullptr)
  {
    return QVariant();
  }

  return m_FilterWidgetLayout->indexOf(filterWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::resetLayout()
{
  // Check to see if we have removed all the filters
  if(filterCount() <= 0)
  {
    // Emit a signal to tell SIMPLView_UI to erase the Filter Input Widget.
    emit filterInputWidgetNeedsCleared();

    // Remove the current Layout
    QLayout* l = layout();
    if(nullptr != l && l == m_FilterWidgetLayout)
    {
      qDeleteAll(l->children());
      delete l;
      m_FilterWidgetLayout = nullptr;
    }

    // and add the empty pipeline layout instead
    newEmptyPipelineViewLayout();

    if(m_DataStructureWidget)
    {
      m_DataStructureWidget->filterObjectActivated(nullptr);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::clearFilterWidgets()
{
  int numFilters = filterCount();

  QList<PipelineFilterObject*> filterObjects;
  for(int i = 0; i < numFilters; i++)
  {
#if 1
    PipelineFilterObject* object = dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
    if(nullptr != object)
    {
      filterObjects.push_back(object);
    }
#else
    emit pipelineIssuesCleared();
    emit pipelineChanged();

    qint32 count = filterCount();
    for(qint32 i = count - 1; i >= 0; --i)
    {
      QWidget* w = m_FilterWidgetLayout->itemAt(i)->widget();
      QSpacerItem* spacer = m_FilterWidgetLayout->itemAt(i)->spacerItem();
      if(nullptr != w)
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
      else if(nullptr != spacer)
      {
        m_FilterWidgetLayout->removeItem(spacer);
      }
    }
    resetLayout();
#endif
  }

  RemoveFilterCommand* removeCmd = new RemoveFilterCommand(filterObjects, this, "Clear");
  addUndoCommand(removeCmd);

  if(m_DataStructureWidget)
  {
    m_DataStructureWidget->filterObjectActivated(nullptr);
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
    if(fw)
    {
      AbstractFilter::Pointer filter = fw->getFilter();
      if(filter)
      {
        QString hl = filter->getHumanLabel();

        fw->setFilterTitle(hl);
        fw->setFilterIndex(i+1, count);
        filter->setPipelineIndex(i);
      }
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
    if(fw)
    {
      AbstractFilter::Pointer filter = fw->getFilter();
      Breakpoint::Pointer breakpoint = std::dynamic_pointer_cast<Breakpoint>(filter);
      if(nullptr != breakpoint)
      {
        connect(pipeline.get(), SIGNAL(pipelineCanceled()), breakpoint.get(), SLOT(resumePipeline()));
      }

      pipeline->pushBack(filter);
    }
  }
  for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
  {
    pipeline->addMessageReceiver(m_PipelineMessageObservers[i]);
  }
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
    if(fw)
    {
      AbstractFilter::Pointer filter = fw->getFilter();
      AbstractFilter::Pointer copy = filter->newFilterInstance(true);
      pipeline->pushBack(copy);
    }
  }
  for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
  {
    pipeline->addMessageReceiver(m_PipelineMessageObservers[i]);
  }

  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SVPipelineViewWidget::openPipeline(const QString& filePath, QVariant value, const bool& setOpenedFilePath, const bool& changeTitle)
{
  QFileInfo fi(filePath);
  if(fi.exists() == false)
  {
    QMessageBox::warning(this, QString::fromLatin1("Pipeline Read Error"), QString::fromLatin1("There was an error opening the specified pipeline file. The pipeline file does not exist."));
    return -1;
  }

  // Clear the pipeline Issues table first so we can collect all the error messages
  emit pipelineIssuesCleared();

  QString ext = fi.suffix();
  QString name = fi.fileName();
  QString baseName = fi.baseName();

  // Read the pipeline from the file
  QString jsonString = getJsonFromFile(filePath);

  // Check that a valid extension was read...
  if(jsonString.isEmpty())
  {
    emit statusMessage(tr("The pipeline was not read correctly from file '%1'. '%2' is an unsupported file extension.").arg(name).arg(ext));
    emit stdOutMessage(tr("The pipeline was not read correctly from file '%1'. '%2' is an unsupported file extension.").arg(name).arg(ext));
    return -1;
  }

  // Notify user of successful read
  emit statusMessage(tr("Opened \"%1\" Pipeline").arg(baseName));
  emit stdOutMessage(tr("Opened \"%1\" Pipeline").arg(baseName));

  // Populate the pipeline view
  populatePipelineView(jsonString, value);

  QString file = filePath;
  emit pipelineOpened(file, setOpenedFilePath, changeTitle);

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::addFilterObject(PipelineFilterObject* filterObject, QVariant value)
{
  SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(filterObject);
  if(filterWidget == nullptr)
  {
    return;
  }

  bool ok;
  int index = value.toInt(&ok);
  if(ok == false)
  {
    return;
  }
  if(index < 0 || index > filterCount())
  {
    index = filterCount();
  }

  bool addSpacer = false;
  if(filterCount() <= 0)
  {
    if(NULL != m_EmptyPipelineLabel)
    {
      m_EmptyPipelineLabel->hide();
      delete m_EmptyPipelineLabel;
      m_EmptyPipelineLabel = NULL;
    }
    QLayout* l = layout();
    if(NULL != l)
    {
      qDeleteAll(l->children());
      delete l;

      if(l == m_FilterWidgetLayout)
      {
        m_FilterWidgetLayout = nullptr;
      }
    }

    if(m_FilterWidgetLayout)
    {
      delete m_FilterWidgetLayout;
      m_FilterWidgetLayout = nullptr;
    }

    m_FilterWidgetLayout = new QVBoxLayout(this);
    m_FilterWidgetLayout->setObjectName(QString::fromUtf8("m_FilterWidgetLayout"));
    m_FilterWidgetLayout->setContentsMargins(2, 2, 2, 2);
    m_FilterWidgetLayout->setSpacing(4);
    addSpacer = true;
  }

  // The layout will take control of the SVPipelineFilterWidget 'w' instance
  m_FilterWidgetLayout->insertWidget(index, filterWidget);
  // Set the Parent
  filterWidget->setParent(this);

  /// Now setup all the connections between the various widgets
  // Clear any existing connections before recreating them

  // When the filter is removed from this view
  disconnect(filterWidget, SIGNAL(filterWidgetRemoved(PipelineFilterObject*)),
             this, SLOT(removeFilterObject(PipelineFilterObject*)));
  connect(filterWidget, SIGNAL(filterWidgetRemoved(PipelineFilterObject*)),
          this, SLOT(removeFilterObject(PipelineFilterObject*)));

  // When the FilterWidget is selected
  disconnect(filterWidget, SIGNAL(filterWidgetPressed(PipelineFilterObject*, Qt::KeyboardModifiers)),
             this, SLOT(setSelectedFilterObject(PipelineFilterObject*, Qt::KeyboardModifiers)));
  connect(filterWidget, SIGNAL(filterWidgetPressed(PipelineFilterObject*, Qt::KeyboardModifiers)),
          this, SLOT(setSelectedFilterObject(PipelineFilterObject*, Qt::KeyboardModifiers)));

  // When the filter widget is dragged
  disconnect(filterWidget, SIGNAL(dragStarted(QMouseEvent*, SVPipelineFilterWidget*)),
             this, SLOT(startDrag(QMouseEvent*, SVPipelineFilterWidget*)));
  connect(filterWidget, SIGNAL(dragStarted(QMouseEvent*, SVPipelineFilterWidget*)),
          this, SLOT(startDrag(QMouseEvent*, SVPipelineFilterWidget*)));

  disconnect(filterWidget, SIGNAL(parametersChanged(QUuid)),
             this, SLOT(preflightPipeline(QUuid)));
  connect(filterWidget, SIGNAL(parametersChanged(QUuid)),
          this, SLOT(preflightPipeline(QUuid)));

  disconnect(filterWidget, SIGNAL(parametersChanged(QUuid)),
             this, SLOT(handleFilterParameterChanged(QUuid)));
  connect(filterWidget, SIGNAL(parametersChanged(QUuid)),
          this, SLOT(handleFilterParameterChanged(QUuid)));

  disconnect(filterWidget, SIGNAL(focusInEventStarted(QFocusEvent*)),
             this, SLOT(on_focusInEventStarted(QFocusEvent*)));
  connect(filterWidget, SIGNAL(focusInEventStarted(QFocusEvent*)),
          this, SLOT(on_focusInEventStarted(QFocusEvent*)));

  disconnect(filterWidget, SIGNAL(focusOutEventStarted(QFocusEvent*)),
             this, SLOT(on_focusOutEventStarted(QFocusEvent*)));
  connect(filterWidget, SIGNAL(focusOutEventStarted(QFocusEvent*)),
          this, SLOT(on_focusOutEventStarted(QFocusEvent*)));

  if(m_DataStructureWidget)
  {
    connect(filterWidget, SIGNAL(parametersChanged1(PipelineFilterObject*)),
            m_DataStructureWidget, SLOT(handleFilterParameterChanged(PipelineFilterObject*)));
  }

  filterWidget->installEventFilter(this);

  // Check to make sure at least the vertical spacer is in the Layout
  if(addSpacer)
  {
    QSpacerItem* verticalSpacer = new QSpacerItem(20, 361, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_FilterWidgetLayout->insertSpacerItem(-1, verticalSpacer);
  }
  if(!m_LoadingJson)
  {
    // Finally, set this new filter widget as selected
    setSelectedFilterObject(filterWidget, Qt::NoModifier);
    filterWidget->setSelected(true);
  }
  // Get the filter to ignore Scroll Wheel Events
  filterWidget->installEventFilter(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::startDrag(QMouseEvent* event, SVPipelineFilterWidget* fw)
{
  // The user is dragging the filter widget so we should set it as selected.
  if(fw->isSelected() == false)
  {
    setSelectedFilterObject(fw, Qt::NoModifier);
  }

  QList<IndexedFilterObject> selectedObjects = getSelectedIndexedFilterObjects();
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
  for(int i = 0; i < selectedObjects.size(); i++)
  {
    SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(selectedObjects[i].second);
    if(filterWidget == nullptr)
    {
      continue;
    }

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
  if(e->type() == QEvent::Wheel && qobject_cast<SVPipelineFilterWidget*>(o))
  {
    return false;
  }
  else if(dynamic_cast<SVPipelineFilterWidget*>(o) != nullptr && e->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(e);
    if(mouseEvent != nullptr && mouseEvent->button() == Qt::RightButton)
    {
      SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(o);

      if(filterWidget != nullptr && filterWidget->getFilter() != nullptr)
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

  return QFrame::eventFilter(o, e);
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
void SVPipelineViewWidget::blockPreflightSignals(bool b)
{
  if(b)
  {
    m_BlockPreflightStack.push(b);
  }
  else
  {
    m_BlockPreflightStack.pop();
  }

  m_BlockPreflight = (m_BlockPreflightStack.size() > 0) ? true : false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::preflightPipeline(QUuid id)
{
  Q_UNUSED(id)
  if(m_BlockPreflight)
  {
    return;
  }

  emit pipelineIssuesCleared();
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = getFilterPipeline();

  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
  for(int i = 0; i < filters.size(); i++)
  {
    filters.at(i)->setErrorCondition(0);
    filters.at(i)->setCancel(false);

    PipelineFilterObject* fw = filterObjectAt(i);
    if(fw)
    {
      fw->setHasPreflightWarnings(false);
      fw->setHasPreflightErrors(false);
      if(PipelineFilterObject::WidgetState::Disabled != fw->getWidgetState())
      {
        fw->setWidgetState(PipelineFilterObject::WidgetState::Ready);
      }
      fw->changeStyle();
    }
  }

//  QSharedPointer<ProgressDialog> progressDialog(new ProgressDialog());
//  progressDialog->setWindowTitle("Pipeline Preflighting");
//  QString msg = QString("Please wait for %1 filters to preflight...").arg(pipeline->getFilterContainer().count());
//  progressDialog->setLabelText(msg);
//  progressDialog->show();
//  progressDialog->raise();
//  progressDialog->activateWindow();

  // Preflight the pipeline
  int err = pipeline->preflightPipeline();
  if(err < 0)
  {
    // FIXME: Implement error handling.
  }

  int count = pipeline->getFilterContainer().size();
  // Now that the preflight has been executed loop through the filters and check their error condition and set the
  // outline on the filter widget if there were errors or warnings
  for(qint32 i = 0; i < count; ++i)
  {
    PipelineFilterObject* fw = filterObjectAt(i);
    if(fw)
    {
      AbstractFilter::Pointer filter = fw->getFilter();
      if(filter->getWarningCondition() < 0)
      {
        fw->setHasPreflightWarnings(true);
      }
      if(filter->getErrorCondition() < 0)
      {
        fw->setHasPreflightErrors(true);
      }
    }
  }
  emit preflightPipelineComplete();
  emit preflightFinished(err);

  if(m_DataStructureWidget)
  {
    m_DataStructureWidget->refreshData();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::removeFilterObjects(QList<PipelineFilterObject*> filterObjects)
{
  if (filterObjects.size() <= 0) { return; }

  RemoveFilterCommand* removeCmd = new RemoveFilterCommand(filterObjects, this, "Remove");
  addUndoCommand(removeCmd);

  if (filterObjects.size() > 1)
  {
    emit statusMessage(tr("Removed %1 filters").arg(filterObjects.size()));
    emit stdOutMessage(tr("Removed %1 filters").arg(filterObjects.size()));
  }
  else
  {
    emit statusMessage(tr("Removed \"%1\" filter").arg(filterObjects[0]->getHumanLabel()));
    emit stdOutMessage(tr("Removed \"%1\" filter").arg(filterObjects[0]->getHumanLabel()));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::removeFilterObject(PipelineFilterObject* filterObject)
{
  int pipelineIndex = -1;
  if(filterObject)
  {
    pipelineIndex = filterObject->getFilter()->getPipelineIndex();
  }
  SVPipelineFilterWidget* w = getFilterWidgetAtIndex(pipelineIndex);
  bool wasSelected = false;
  if(w)
  {
    wasSelected = w->isSelected();
  }

  RemoveFilterCommand* removeCmd = new RemoveFilterCommand(filterObject, this, "Remove");
  addUndoCommand(removeCmd);

  emit statusMessage(tr("Removed \"%1\" filter").arg(filterObject->getHumanLabel()));
  emit stdOutMessage(tr("Removed \"%1\" filter").arg(filterObject->getHumanLabel()));

  if(m_DataStructureWidget)
  {
    m_DataStructureWidget->handleFilterRemoved(filterObject);
  }

  // This block figures out which filter widget should be selected.

  if(wasSelected) // If the deleted widget was selected, then select the next one down
  {               //
    int count = filterCount();
    if(pipelineIndex >= count) // unless the index is at the end (or past) of the pipeline count
    {
      pipelineIndex = count - 1; // The just select the last filter object in the pipeline
    }
    w = getFilterWidgetAtIndex(pipelineIndex);
    if(w)
    {
      w->setSelected(true);
      // PipelineFilterObject pfObj(w->getFilter());
      emit w->filterWidgetPressed(w, qApp->keyboardModifiers());
    }
  }
  else
  {
    int count = filterCount();
    for(auto i = 0; i < count; i++)
    {
      w = getFilterWidgetAtIndex(i);
      if(w->isSelected())
      {
        // PipelineFilterObject pfObj(w->getFilter());
        emit w->filterWidgetPressed(w, qApp->keyboardModifiers());
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::removeFilterObject(PipelineFilterObject* filterObject, bool deleteWidget)
{
  SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(filterObject);
  if(filterWidget == nullptr)
  {
    return;
  }

  if(filterWidget)
  {
    int index = filterWidget->getFilter()->getPipelineIndex() - 1;

    QWidget* w = qobject_cast<QWidget*>(filterWidget);
    m_FilterWidgetLayout->removeWidget(w);

    if(w)
    {

      SVPipelineFilterWidget* prevWidget = getFilterWidgetAtIndex(index);
      if(prevWidget)
      {
        emit prevWidget->filterWidgetPressed(prevWidget, qApp->keyboardModifiers());
      }

      filterWidget->getFilter()->setPreviousFilter(AbstractFilter::NullPointer());
      filterWidget->getFilter()->setNextFilter(AbstractFilter::NullPointer());

      if(deleteWidget == true)
      {
        w->deleteLater();
      }
      else
      {
        w->setParent(nullptr);

        // When the filter is removed from this view
        disconnect(filterWidget, SIGNAL(filterWidgetRemoved(PipelineFilterObject*)), this, SLOT(removeFilterObject(PipelineFilterObject*)));

        // When the FilterWidget is selected
        disconnect(filterWidget, SIGNAL(filterWidgetPressed(PipelineFilterObject*, Qt::KeyboardModifiers)), this, SLOT(setSelectedFilterObject(PipelineFilterObject*, Qt::KeyboardModifiers)));

        // When the filter widget is dragged
        disconnect(filterWidget, SIGNAL(dragStarted(QMouseEvent*, SVPipelineFilterWidget*)), this, SLOT(startDrag(QMouseEvent*, SVPipelineFilterWidget*)));

        disconnect(filterWidget, SIGNAL(parametersChanged(QUuid)), this, SLOT(preflightPipeline(QUuid)));

        disconnect(filterWidget, SIGNAL(parametersChanged(QUuid)), this, SLOT(handleFilterParameterChanged(QUuid)));

        disconnect(filterWidget, SIGNAL(focusInEventStarted(QFocusEvent*)), this, SLOT(on_focusInEventStarted(QFocusEvent*)));

        disconnect(filterWidget, SIGNAL(focusOutEventStarted(QFocusEvent*)), this, SLOT(on_focusOutEventStarted(QFocusEvent*)));
      }
    }

    if(getSelectedFilterObjects().isEmpty())
    {
      m_ShiftStart = NULL;
    }

    QSpacerItem* spacer = m_FilterWidgetLayout->itemAt(0)->spacerItem();
    if(NULL != spacer)
    {
      m_FilterWidgetLayout->removeItem(spacer);
      delete spacer;
    }

    QList<PipelineFilterObject*> selectedObjs = getSelectedFilterObjects();
    if(selectedObjs.size() <= 0 || selectedObjs.size() >= 2)
    {
      emit filterInputWidgetNeedsCleared();
    }
    else
    {
      emit filterInputWidgetChanged(selectedObjs[0]->getFilterInputWidget());
    }

    resetLayout();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::setFiltersEnabled(QList<PipelineFilterObject*> widgets, bool enabled)
{
  int count = widgets.size();
  for(int i = 0; i < count; i++)
  {
    widgets[i]->setIsEnabled(enabled);
  }

  preflightPipeline();
  emit filterEnabledStateChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::setSelectedFiltersEnabled(bool enabled)
{
  QList<PipelineFilterObject*> selectedObjs = getSelectedFilterObjects();
  setFiltersEnabled(selectedObjs, enabled);
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
void SVPipelineViewWidget::setSelectedFilterObject(PipelineFilterObject* w, Qt::KeyboardModifiers modifiers)
{
  SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(w);
  if(filterWidget == nullptr)
  {
    return;
  }

  if(modifiers == Qt::ShiftModifier)
  {
    clearSelectedFilterObjects();

    if(nullptr == m_ShiftStart)
    {
      m_ShiftStart = filterWidget;
    }

    int begin;
    int end;
    if(m_FilterWidgetLayout->indexOf(filterWidget) < m_FilterWidgetLayout->indexOf(m_ShiftStart))
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

    for(int i = begin; i <= end; i++)
    {
      SVPipelineFilterWidget* fw = dynamic_cast<SVPipelineFilterWidget*>(filterObjectAt(i));
      if(fw)
      {
        fw->setSelected(true);
      }
    }
  }
  else if(modifiers == Qt::ControlModifier)
  {
    m_ShiftStart = filterWidget;

    if(filterWidget->isSelected())
    {
      filterWidget->setSelected(true);
      if(getSelectedFilterObjects().isEmpty())
      {
        m_ShiftStart = nullptr;
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

  if(selectedObjects.size() == 1)
  {
    emit filterInputWidgetChanged(selectedObjects[0]->getFilterInputWidget());
    emit pipelineFilterObjectSelected(selectedObjects[0]);
    //    if(m_DataStructureWidget)
    //    {
    //      m_DataStructureWidget->filterObjectActivated(selectedObjects[0]);
    //    }
  }
  else
  {
    emit filterInputWidgetNeedsCleared();

    if(m_DataStructureWidget)
    {
      m_DataStructureWidget->filterObjectActivated(nullptr);
    }
  }

  filterWidget->setFocus();

  updateActionEnableFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::clearSelectedFilterObjects()
{
  for(int i = 0; i < filterCount(); i++)
  {
    if(nullptr != dynamic_cast<SVPipelineFilterOutlineWidget*>(m_FilterWidgetLayout->itemAt(i)->widget()))
    {
      continue;
    }
    else
    {
      SVPipelineFilterWidget* fw = dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      if(fw != nullptr)
      {
        fw->setSelected(false);
      }
    }
  }

  if(m_DataStructureWidget)
  {
    m_DataStructureWidget->filterObjectActivated(nullptr);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::addSIMPLViewReaderFilter(const QString& filePath, QVariant value)
{
  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(filePath);

  AddFilterCommand* addCmd = new AddFilterCommand(reader, this, "Add", value);
  addCmd->setText(QObject::tr("\"%1 '%2' Filter\"").arg("Add").arg(reader->getHumanLabel()));
  addUndoCommand(addCmd);

  emit statusMessage(tr("Added \"%1\" filter").arg(reader->getHumanLabel()));
  emit stdOutMessage(tr("Added \"%1\" filter").arg(reader->getHumanLabel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::populatePipelineView(QString jsonString, QVariant value)
{
  if(jsonString.isEmpty())
  {
    clearFilterWidgets();
    return;
  }

  bool ok;
  int index = value.toInt(&ok);
  if(ok == false)
  {
    return;
  }
  if(index < 0 || index > filterCount())
  {
    index = filterCount();
  }
  m_LoadingJson = true;

  AddFilterCommand* addCmd = new AddFilterCommand(jsonString, this, "Paste", index);
  addUndoCommand(addCmd);

  if(filterCount() > 0)
  {
    SVPipelineFilterWidget* fw = dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(0)->widget());
    if(fw)
    {
      setSelectedFilterObject(fw, Qt::NoModifier);
    }
  }
  m_LoadingJson = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::keyPressEvent(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
  {
    bool isRunning = getPipelineIsRunning();
    if(isRunning == false)
    {
      emit deleteKeyPressed(this);
    }
  }
  else if(event->key() == Qt::Key_A && qApp->queryKeyboardModifiers() == Qt::ControlModifier)
  {
    clearSelectedFilterObjects();

    for(int i = 0; i < filterCount(); i++)
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
  if(filterWidget == nullptr)
  {
    return false;
  }

  for(int i = 0; i < m_FilterWidgetLayout->count(); i++)
  {
    if(dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget()) == filterWidget)
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
  qint32 count = filterCount();
  for(qint32 i = 0; i < count; ++i)
  {
    PipelineFilterObject* fw = filterObjectAt(i);
    if(fw)
    {
      if(fw->isFilterEnabled())
      {
        fw->toReadyState();
      }
      else
      {
        fw->toDisabledState();
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer SVPipelineViewWidget::readPipelineFromFile(const QString& filePath)
{
  QFileInfo fi(filePath);
  QString ext = fi.suffix();

  FilterPipeline::Pointer pipeline;
  if(ext == "dream3d")
  {
    H5FilterParametersReader::Pointer dream3dReader = H5FilterParametersReader::New();
    pipeline = dream3dReader->readPipelineFromFile(filePath);
  }
  else if(ext == "json")
  {
    JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
    pipeline = jsonReader->readPipelineFromFile(filePath);
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
QString SVPipelineViewWidget::getJsonFromFile(const QString& filePath)
{
  QFileInfo fi(filePath);
  QString ext = fi.suffix();

  QString jsonString = "";
  if(ext == "dream3d")
  {
    H5FilterParametersReader::Pointer dream3dReader = H5FilterParametersReader::New();
    jsonString = dream3dReader->getJsonFromFile(filePath);
  }
  else if(ext == "json")
  {
    JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
    jsonString = jsonReader->getJsonFromFile(filePath);
  }

  return jsonString;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SVPipelineViewWidget::writePipeline(QString filePath)
{
  QFileInfo fi(filePath);
  QString ext = fi.completeSuffix();

  // If the filePath already exists - delete it so that we get a clean write to the file
  if(fi.exists() == true && (ext == "dream3d" || ext == "json"))
  {
    QFile f(filePath);
    if(f.remove() == false)
    {
      QMessageBox::warning(this, QString::fromLatin1("Pipeline Write Error"), QString::fromLatin1("There was an error removing the existing pipeline file. The pipeline was NOT saved."));
      return -1;
    }
  }

  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = getFilterPipeline();

  int err = 0;
  if(ext == "dream3d")
  {    
    QList<IObserver*> observers;
    for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
    {
      observers.push_back(reinterpret_cast<IObserver*>(m_PipelineMessageObservers[i]));
    }

    H5FilterParametersWriter::Pointer dream3dWriter = H5FilterParametersWriter::New();
    err = dream3dWriter->writePipelineToFile(pipeline, fi.absoluteFilePath(), fi.fileName(), observers);
  }
  else if(ext == "json")
  {
    QList<IObserver*> observers;
    for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
    {
      observers.push_back(reinterpret_cast<IObserver*>(m_PipelineMessageObservers[i]));
    }

    JsonFilterParametersWriter::Pointer jsonWriter = JsonFilterParametersWriter::New();
    jsonWriter->writePipelineToFile(pipeline, fi.absoluteFilePath(), fi.fileName(), observers);
  }
  else
  {
    emit statusMessage(tr("The pipeline was not written to file '%1'. '%2' is an unsupported file extension.").arg(fi.fileName()).arg(ext));
    return -1;
  }

  if(err < 0)
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
int SVPipelineViewWidget::getIndexAtPoint(const QPoint& point)
{
  if(nullptr == m_FilterWidgetLayout) 
  {
    return -1;
  }

  // Check if the point is within the filter widget layout
  if(rect().contains(point))
  {
    // Check each widget in the layout
    int index = 0;
    for (index = 0; index < m_FilterWidgetLayout->count(); index++)
    {
      SVPipelineFilterWidget* filterWidget = getFilterWidgetAtIndex(index);
      if (nullptr == filterWidget)
      {
        return index;
      }

      QPoint widgetPos = filterWidget->pos();
      if(point.y() < widgetPos.y() + filterWidget->height() / 2)
      {
        return index;
      }
    }
  }

  return m_FilterWidgetLayout->count() - 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineFilterWidget* SVPipelineViewWidget::getFilterWidgetAtIndex(int index)
{
  if(nullptr == m_FilterWidgetLayout || index < 0)
  {
    return nullptr;
  }

  // Using filter layout (does not account for other widgets mixed in)
  QLayoutItem* item = m_FilterWidgetLayout->itemAt(index);
  if(nullptr != item)
  {
    QWidget* widget = item->widget();
    if(nullptr != widget)
    {
      return dynamic_cast<SVPipelineFilterWidget*>(widget);
    }
  }
  return nullptr;

  // Using order of pipeline filter widgets in layout (accounts for other widgets mixed in)
#if 0
  int widgetsFound = 0;
  for(int i = 0; i < m_FilterWidgetLayout->children().count(); i++)
  {
    SVPipelineFilterWidget* widget = dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());

    if (nullptr != widget)
    {
      if(index == widgetsFound)
      {
        return widget;
      }
      widgetsFound++;
    }
  }

  return nullptr;
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::dragMoveEvent(QDragMoveEvent* event)
{
  // Remove the drop box, if it exists
  if(nullptr != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_FilterOutlineWidget) != -1)
  {
    m_FilterWidgetLayout->removeWidget(m_FilterOutlineWidget);
    m_FilterOutlineWidget->hide();
    m_FilterOutlineWidget->setParent(nullptr);
  }

  // If cursor is within margin boundaries, start scrolling
  if(shouldAutoScroll(event->pos()))
  {
    startAutoScroll();
  }
  // Otherwise, stop scrolling
  else
  {
    stopAutoScroll();
  }

  const QMimeData* mimedata = event->mimeData();
  if(nullptr != dynamic_cast<const PipelineViewPtrMimeData*>(mimedata))
  {
    // The user is moving existing filter widgets, either within the same pipeline view or between pipeline views
    SVPipelineViewWidget* origin = dynamic_cast<SVPipelineViewWidget*>(dynamic_cast<const PipelineViewPtrMimeData*>(mimedata)->getPipelineViewPtr());

    if(origin == this)
    {
      if(qApp->queryKeyboardModifiers() != Qt::AltModifier)
      {
        QList<IndexedFilterObject> draggedFilterObjects = getDraggedFilterObjects();
        
        for(int i = 0; i < draggedFilterObjects.size(); i++)
        {
          // Remove the filter widget
          if(nullptr != m_FilterWidgetLayout && origin->containsFilterWidget(draggedFilterObjects[i].second))
          {
            PipelineFilterObject* draggedObject = draggedFilterObjects[i].second;
            SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(draggedObject);
            if(filterWidget == nullptr)
            {
              return;
            }

            m_FilterOrigPos = origin->valueOfFilterWidget(filterWidget).toInt();
            m_FilterWidgetLayout->removeWidget(filterWidget);
            filterWidget->setParent(nullptr);
          }
        }
      }
    }

    int count = filterCount();
    if(count <= 0)
    {
      return;
    }

    // insert filter
    {
      int i = getIndexAtPoint(event->pos());

      if(i < 0)
      {
        i = filterCount();
      }

      SVPipelineFilterWidget* w = getFilterWidgetAtIndex(i);

      if (w != nullptr)
      {
        if ((i >= count && event->pos().y() >= w->geometry().y() + w->geometry().height() / 2) || (event->pos().y() <= w->geometry().y() + w->geometry().height() / 2))
        {
          QList<IndexedFilterObject> draggedObjects = origin->getDraggedFilterObjects();
          if (draggedObjects.size() > 1)
          {
           // m_FilterOutlineWidget->setFilter(nullptr);
            m_FilterOutlineWidget->setFilterIndex(i + 1, count);
            m_FilterOutlineWidget->setFilterTitle("Place " + QString::number(draggedObjects.size()) + " Filters Here");
          }
          else if (draggedObjects.size() == 1)
          {
            AbstractFilter::Pointer f = draggedObjects[0].second->getFilter();
            m_FilterOutlineWidget->setFilter(f.get());
            m_FilterOutlineWidget->setFilterIndex(i + 1, count);
            m_FilterOutlineWidget->setFilterTitle(draggedObjects[0].second->getHumanLabel());
          }
          else
          {
            event->ignore();
            return;
          }
          m_FilterWidgetLayout->insertWidget(i, m_FilterOutlineWidget);
          m_FilterOutlineWidget->show();
          reindexWidgetTitles();
        }
      }
      else if(i == count)
      {
        QList<IndexedFilterObject> draggedObjects = origin->getDraggedFilterObjects();
        if (draggedObjects.size() == 1)
        {          
          m_FilterOutlineWidget->setFilter(draggedObjects[0].second->getFilter().get());
          m_FilterOutlineWidget->setFilterIndex(i + 1, count);
          m_FilterOutlineWidget->setFilterTitle(draggedObjects[0].second->getHumanLabel());
        }

        m_FilterWidgetLayout->insertWidget(i, m_FilterOutlineWidget);
        m_FilterOutlineWidget->show();
        reindexWidgetTitles();
      }

      event->accept();
    }
  }
  else if(mimedata->hasUrls() || mimedata->hasFormat(SIMPLView::DragAndDrop::BookmarkItem) || mimedata->hasFormat(SIMPLView::DragAndDrop::FilterItem))
  {
    QString data;
    if(mimedata->hasUrls())
    {
      data = mimedata->text();
      QUrl url(data);
      data = url.toLocalFile();
    }
    else if(mimedata->hasText())
    {
      data = mimedata->text();
    }
    else if(mimedata->hasFormat(SIMPLView::DragAndDrop::BookmarkItem))
    {
      QByteArray jsonArray = mimedata->data(SIMPLView::DragAndDrop::BookmarkItem);
      QJsonDocument doc = QJsonDocument::fromJson(jsonArray);
      QJsonObject obj = doc.object();

      if(obj.size() > 1)
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
    if(nullptr == fm)
    {
      return;
    }
    IFilterFactory::Pointer wf = fm->getFactoryFromClassName(data);

    // If the dragged item is a filter item...
    if(nullptr != wf)
    {
      QString humanName = wf->getFilterHumanLabel();

      //bool didInsert = false;
      // This path is taken if a filter is dragged from the list of filters

      int i = getIndexAtPoint(event->pos());
      
      if(i < 0 || i > filterCount())
      {
        return;
      }

      m_FilterOutlineWidget->setFilterIndex(i + 1, filterCount());
      m_FilterOutlineWidget->setFilterTitle(humanName);
      m_FilterWidgetLayout->insertWidget(i, m_FilterOutlineWidget);
      m_FilterOutlineWidget->show();
      reindexWidgetTitles();

      event->accept();
    }
    // If the dragged item is a pipeline file...
    else if(ext == "dream3d" || ext == "json" )
    {
      if(nullptr == m_FilterOutlineWidget)
      {
        event->ignore();
        return;
      }

      QString pipelineName = fi.baseName();

      //bool didInsert = false;
      // This path is taken if a filter is dragged from the list of filters

      int i = getIndexAtPoint(event->pos());

      if (i < 0 || i > filterCount())
      {
        return;
      }

      m_FilterOutlineWidget->setFilterIndex(i + 1, filterCount());
      m_FilterOutlineWidget->setFilterTitle("Place '" + pipelineName + "' Here");
      m_FilterWidgetLayout->insertWidget(i, m_FilterOutlineWidget);
      m_FilterOutlineWidget->show();
      reindexWidgetTitles();
      
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
  if(mimedata->hasUrls() || mimedata->hasText() || mimedata->hasFormat(SIMPLView::DragAndDrop::BookmarkItem) || mimedata->hasFormat(SIMPLView::DragAndDrop::FilterItem))
  {
    QString data;

    if(mimedata->hasUrls())
    {
      data = mimedata->text();
      QUrl url(data);
      data = url.toLocalFile();
    }
    else if(mimedata->hasText())
    {
      data = mimedata->text();
    }
    else if(mimedata->hasFormat(SIMPLView::DragAndDrop::FilterItem))
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

      if(obj.size() > 1)
      {
        QMessageBox::warning(nullptr, "Warning", "This application currently does not support dragging and dropping multiple bookmarks.", QMessageBox::Ok);
        event->ignore();
        return;
      }

      QJsonObject::iterator iter = obj.begin();
      data = iter.value().toString();
    }

    QFileInfo fi(data);
    QString ext = fi.completeSuffix();
    FilterManager* fm = FilterManager::Instance();
    if(nullptr == fm)
    {
      return;
    }
    IFilterFactory::Pointer wf = fm->getFactoryFromClassName(data);

    // If the dragged item is a filter item...
    if(nullptr != wf)
    {
      int index;

      // We need to figure out where it was dropped relative to other filters
      if(nullptr != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_FilterOutlineWidget) != -1)
      {
        index = m_FilterWidgetLayout->indexOf(m_FilterOutlineWidget);
      }
      else
      {
        index = -1;
      }

      if(index < 0 || index > filterCount())
      {
        index = filterCount();
      }

      AbstractFilter::Pointer filter = wf->create();
      AddFilterCommand* addCmd = new AddFilterCommand(filter, this, "Add", index);
      addUndoCommand(addCmd);

      emit statusMessage(tr("Added \"%1\" filter").arg(filter->getHumanLabel()));
      emit stdOutMessage(tr("Added \"%1\" filter").arg(filter->getHumanLabel()));

      event->accept();
    }
    // If the dragged item is a pipeline file...
    else if(ext == "dream3d" || ext == "json" || ext == "ini" || ext == "txt")
    {
      int index = 0;
      if(nullptr != m_FilterWidgetLayout)
      {
        index = m_FilterWidgetLayout->indexOf(m_FilterOutlineWidget);
      }
      if (index < 0 || index > filterCount())
      {
        return;
      }

      if(ext == "json")
      {
        openPipeline(data, index, false, false);

        emit windowNeedsRecheck();
      }
      else if(ext == "dream3d")
      {
        QtSFileDragMessageBox* msgBox = new QtSFileDragMessageBox(this);
        msgBox->exec();
        msgBox->deleteLater();

        if(msgBox->didPressOkBtn() == true)
        {
          if(msgBox->isExtractPipelineBtnChecked() == true)
          {
            openPipeline(data, index, false, false);
          }
          else
          {
            addSIMPLViewReaderFilter(data, index);
            emit windowNeedsRecheck();
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
  else if(nullptr != dynamic_cast<const PipelineViewPtrMimeData*>(mimedata))
  {
    SVPipelineViewWidget* origin = dynamic_cast<SVPipelineViewWidget*>(dynamic_cast<const PipelineViewPtrMimeData*>(mimedata)->getPipelineViewPtr());

    // The drop box, if it exists, marks the index where the filter should be dropped
    int index;
    if(nullptr != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_FilterOutlineWidget) != -1)
    {
      //m_DropBox->hide();
      index = m_FilterWidgetLayout->indexOf(m_FilterOutlineWidget);
      m_FilterWidgetLayout->removeWidget(m_FilterOutlineWidget);
      m_FilterOutlineWidget->setParent(nullptr);
    }
    else
    {
      index = filterCount();
    }

    if(index < 0 || index > filterCount())
    {
      return;
    }

    QList<IndexedFilterObject> draggedFilterObjects = getDraggedFilterObjects();

    if(origin != this || (origin == this && qApp->queryKeyboardModifiers() == Qt::AltModifier))
    {
      QList<IndexedFilterObject> indexedFilterObjects = origin->getDraggedFilterObjects();
      QList<PipelineFilterObject*> filterObjects;
      QList<AbstractFilter::Pointer> filters;
      for(int i = 0; i < indexedFilterObjects.size(); i++)
      {
        filterObjects.push_back(indexedFilterObjects[i].second);
        filters.push_back(indexedFilterObjects[i].second->getFilter());
      }

      AddFilterCommand* addCmd = new AddFilterCommand(filters, this, "Paste", index);
      addUndoCommand(addCmd);

      for (int i = 0; i < filters.size(); i++)
      {
        emit statusMessage(tr("Added \"%1\" filter").arg(filters[i]->getHumanLabel()));
        emit stdOutMessage(tr("Added \"%1\" filter").arg(filters[i]->getHumanLabel()));
      }

      if(qApp->queryKeyboardModifiers() != Qt::AltModifier)
      {
        RemoveFilterCommand* removeCmd = new RemoveFilterCommand(filterObjects, origin, "Cut");
        origin->addUndoCommand(removeCmd);

        for (int i = 0; i < filters.size(); i++)
        {
          emit origin->statusMessage(tr("Removed \"%1\" filter").arg(filters[i]->getHumanLabel()));
          emit origin->stdOutMessage(tr("Removed \"%1\" filter").arg(filters[i]->getHumanLabel()));
        }
      }

      origin->setDraggedFilterObjects(QList<IndexedFilterObject>());
      event->accept();
    }
    else
    {
      MoveFilterCommand* cmd = new MoveFilterCommand(draggedFilterObjects, index, this);
      addUndoCommand(cmd);

      // Do not allow move with index equal to the number of filters.  It will add below the spacer and cause errors
      if (index == filterCount())
      {
        m_UndoStack->undo();

        m_FilterOutlineWidget->hide();
        m_FilterWidgetLayout->removeWidget(m_FilterOutlineWidget);
        m_FilterOutlineWidget->setParent(nullptr);
      }
      else
      {
        for(int i = 0; i < draggedFilterObjects.size(); i++)
        {
          emit statusMessage(tr("Moved \"%1\" filter").arg(draggedFilterObjects[i].second->getHumanLabel()));
          emit stdOutMessage(tr("Moved \"%1\" filter").arg(draggedFilterObjects[i].second->getHumanLabel()));
        }
      }

      setDraggedFilterObjects(QList<IndexedFilterObject>());
      event->accept();
    }
  }

  // Stop auto scrolling if widget is dropped
  stopAutoScroll();

  // Remove the drop line, if it exists
  if(nullptr != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_FilterOutlineWidget) != -1)
  {
    //m_DropBox->hide();
    m_FilterWidgetLayout->removeWidget(m_FilterOutlineWidget);
    m_FilterOutlineWidget->setParent(nullptr);
    reindexWidgetTitles();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
  int index;
  if(nullptr != m_FilterWidgetLayout)
  {
    index = m_FilterWidgetLayout->indexOf(m_FilterOutlineWidget);
  }

  // Remove the placeholder drop box
  if(nullptr != m_FilterWidgetLayout && index != -1)
  {
    m_FilterWidgetLayout->removeWidget(m_FilterOutlineWidget);
    m_FilterOutlineWidget->hide();
    m_FilterOutlineWidget->setParent(nullptr);
  }

  QList<IndexedFilterObject> draggedFilterObjects = getDraggedFilterObjects();

  // Put filter widget back to original position
  if(draggedFilterObjects.size() == 1 && qApp->queryKeyboardModifiers() != Qt::AltModifier)
  {
    SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(draggedFilterObjects[0].second);
    if(filterWidget == nullptr)
    {
      return;
    }

    m_FilterWidgetLayout->insertWidget(m_FilterOrigPos, filterWidget);
    setSelectedFilterObject(filterWidget, Qt::NoModifier);
  }

  reindexWidgetTitles();

  //  // Set the current filter as previous, and nullify the current
  //  m_PreviousFilterBeingDragged = m_CurrentFilterBeingDragged;
  //  m_CurrentFilterBeingDragged = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::dragEnterEvent(QDragEnterEvent* event)
{
  event->acceptProposedAction();

  //  // If there is a previous filter, set it as current
  //  if (nullptr != m_PreviousFilterBeingDragged && event->dropAction() == Qt::MoveAction)
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
  if(m_autoScrollCount < qMax(verticalStep, horizontalStep))
  {
    m_autoScrollCount = m_autoScrollCount + 15;
  }

  int margin = m_AutoScrollMargin;
  int verticalValue = m_ScrollArea->verticalScrollBar()->value();
  int horizontalValue = m_ScrollArea->horizontalScrollBar()->value();

  QPoint pos = m_ScrollArea->viewport()->mapFromGlobal(QCursor::pos());
  QRect area = m_ScrollArea->geometry();

  // do the scrolling if we are in the scroll margins
  if(pos.y() - area.top() < margin)
  {
    m_ScrollArea->verticalScrollBar()->setValue(verticalValue - m_autoScrollCount);
  }
  else if(area.bottom() - pos.y() < margin)
  {
    m_ScrollArea->verticalScrollBar()->setValue(verticalValue + m_autoScrollCount);
  }
  //  if (pos.x() - area.left() < margin)
  //    m_ScrollArea->horizontalScrollBar()->setValue(horizontalValue - d->m_autoScrollCount);
  //  else if (area.right() - pos.x() < margin)
  //    m_ScrollArea->horizontalScrollBar()->setValue(horizontalValue + d->m_autoScrollCount);
  // if nothing changed, stop scrolling
  bool verticalUnchanged = (verticalValue == m_ScrollArea->verticalScrollBar()->value());
  bool horizontalUnchanged = (horizontalValue == m_ScrollArea->horizontalScrollBar()->value());
  if(verticalUnchanged && horizontalUnchanged)
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
  if(nullptr == m_ScrollArea)
  {
    return false;
  }
  QPoint scpos = m_ScrollArea->viewport()->mapFromGlobal(QCursor::pos());
  QRect rect = m_ScrollArea->geometry();

  if(scpos.y() <= getAutoScrollMargin())
  {
    return true;
  }
  else if(pos.y() >= rect.height() - getAutoScrollMargin())
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::recheckWindowTitleAndModification()
{
  emit windowNeedsRecheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton)
  {
    clearSelectedFilterObjects();
    m_ShiftStart = nullptr;
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
  for(int i = 0; i < filterCount(); i++)
  {
    if(nullptr != dynamic_cast<SVPipelineFilterOutlineWidget*>(m_FilterWidgetLayout->itemAt(i)->widget()))
    {
      continue;
    }
    else
    {
      SVPipelineFilterWidget* fw = dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      if(fw != nullptr && fw->isSelected() == true)
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
QList<PipelineView::IndexedFilterObject> SVPipelineViewWidget::getSelectedIndexedFilterObjects()
{
  QList<IndexedFilterObject> indexedFilterObjects;
  for(int i = 0; i < filterCount(); i++)
  {
    if(nullptr != dynamic_cast<SVPipelineFilterOutlineWidget*>(m_FilterWidgetLayout->itemAt(i)->widget()))
    {
      continue;
    }
    else
    {
      SVPipelineFilterWidget* fw = dynamic_cast<SVPipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      if(fw != nullptr && fw->isSelected() == true)
      {
        indexedFilterObjects.push_back(std::make_pair(i, fw));
      }
    }
  }
  return indexedFilterObjects;
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
void SVPipelineViewWidget::addPipelineMessageObserver(QObject* pipelineMessageObserver)
{
  m_PipelineMessageObservers.push_back(pipelineMessageObserver);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::handleFilterParameterChanged(QUuid id)
{
  Q_UNUSED(id)

  emit filterInputWidgetEdited();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVPipelineViewWidget::setDataStructureWidget(DataStructureWidget* w)
{
  if(nullptr == w)
  {
    disconnect(this, SIGNAL(pipelineFilterObjectSelected(PipelineFilterObject*)),
               m_DataStructureWidget, SLOT(filterObjectActivated(PipelineFilterObject*)));
    m_DataStructureWidget = w;
  }
  else
  {
    m_DataStructureWidget = w;
    connect(this, SIGNAL(pipelineFilterObjectSelected(PipelineFilterObject*)),
            m_DataStructureWidget, SLOT(filterObjectActivated(PipelineFilterObject*)));
  }
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureWidget* SVPipelineViewWidget::getDataStructureWidget()
{
  return m_DataStructureWidget;
}

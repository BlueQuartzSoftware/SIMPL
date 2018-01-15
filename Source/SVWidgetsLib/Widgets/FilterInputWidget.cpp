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

#include "FilterInputWidget.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaProperty>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QSequentialAnimationGroup>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

#include "SIMPLib/Common/DocRequestManager.h"
#include "SIMPLib/FilterParameters/DataContainerReaderFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/InputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedDataContainerSelectionFilterParameter.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"

#include "SVWidgetsLib/QtSupport/QtSHelpUrlGenerator.h"
#include "SVWidgetsLib/QtSupport/QtSStyles.h"

#include "SVWidgetsLib/FilterParameterWidgets/ChoiceWidget.h"
#include "SVWidgetsLib/FilterParameterWidgets/LinkedBooleanWidget.h"
#include "SVWidgetsLib/FilterParameterWidgets/LinkedDataContainerSelectionWidget.h"

#include "SVWidgetsLib/Core/FilterWidgetManager.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/DataContainerArrayWidget.h"
#include "SVWidgetsLib/Widgets/SVPipelineFilterWidget.h"

#if 0
#include "ctkCollapsibleGroupBox.h"
#define QGroupBox ctkCollapsibleGroupBox
#endif

// Include the MOC generated CPP file which has all the QMetaObject methods/data

// Initialize private static member variable
QString FilterInputWidget::m_OpenDialogLastFilePath = "";

enum TabIndices
{
  BASIC_TAB,
  CURRENT_STRUCTURE_TAB,
  HELP_TAB
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFileInfo getFilterParameterPath(AbstractFilter* filter, FilterParameter* parameter, QString& fType, QString& ext, int& err)
{
  QString currentPath = "";
  fType.clear();
  ext.clear();
  if(nullptr != dynamic_cast<DataContainerReaderFilterParameter*>(parameter))
  {
    DataContainerReaderFilterParameter* rParam = dynamic_cast<DataContainerReaderFilterParameter*>(parameter);
    currentPath = filter->property(rParam->getInputFileProperty().toLatin1().constData()).toString();
    fType.append(rParam->getFileType());
    ext.append(rParam->getFileExtension());
  }
  else if(nullptr != dynamic_cast<InputFileFilterParameter*>(parameter))
  {
    InputFileFilterParameter* rParam = dynamic_cast<InputFileFilterParameter*>(parameter);
    currentPath = filter->property(rParam->getPropertyName().toLatin1().constData()).toString();
    fType.append(rParam->getFileType());
    ext.append(rParam->getFileExtension());
  }
  else if(nullptr != dynamic_cast<InputPathFilterParameter*>(parameter))
  {
    InputPathFilterParameter* rParam = dynamic_cast<InputPathFilterParameter*>(parameter);
    currentPath = filter->property(rParam->getPropertyName().toLatin1().constData()).toString();
    fType.append(rParam->getFileType());
    ext.append(rParam->getFileExtension());
  }
  else
  {
    err = -1;
  }

  QFileInfo fi;
  if(currentPath.isEmpty() == false)
  {
    fi.setFile(currentPath);
  }

  return fi;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterInputWidget::FilterInputWidget(QString filterClassName, PipelineFilterObject* filterObj, QWidget* parent)
: QWidget(parent)
, m_FilterClassName(filterClassName)
, m_AdvFadedOut(false)
{
  setupUi(this);
  setupGui();

  if(m_OpenDialogLastFilePath.isEmpty())
  {
    m_OpenDialogLastFilePath = QDir::homePath();
  }

  layoutWidgets(filterObj->getFilter());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterInputWidget::~FilterInputWidget()
{
  if(m_VariablesVerticalLayout != nullptr)
  {
    delete m_VariablesVerticalLayout;
  }
  if(m_VariablesWidget != nullptr)
  {
    delete m_VariablesWidget;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterInputWidget::eventFilter(QObject* o, QEvent* e)
{
  if(e->type() == QEvent::Resize && qobject_cast<QLabel*>(o) && brandingLabel == o)
  {
    QFontMetrics metrics(brandingLabel->font());
    QString elidedText = metrics.elidedText(m_BrandingLabel, Qt::ElideMiddle, brandingLabel->width());
    brandingLabel->setText(elidedText);
    return true;
  }
  return QWidget::eventFilter(o, e);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::setupGui()
{
  QFont humanLabelFont = QtSStyles::GetHumanLabelFont();
  QFont brandingFont = QtSStyles::GetBrandingLabelFont();

  filterHumanLabel->setFont(humanLabelFont);
  filterIndex->setFont(humanLabelFont);

  QString releaseType = QString::fromLatin1(SIMPLViewProj_RELEASE_TYPE);
  if(releaseType.compare("Official") == 0)
  {
    brandingLabel->hide();
  }
  else
  {
    brandingLabel->setFont(brandingFont);
    brandingLabel->installEventFilter(this);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::toRunningState()
{
  QMapIterator<QString, QWidget*> iter(m_PropertyToWidget);
  while(iter.hasNext())
  {
    iter.next();
    iter.value()->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::toIdleState()
{
  QMapIterator<QString, QWidget*> iter(m_PropertyToWidget);
  while(iter.hasNext())
  {
    iter.next();
    iter.value()->setDisabled(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::layoutWidgets(AbstractFilter::Pointer filter)
{
  // If the filter is valid then instantiate all the FilterParameterWidgets
  // Create the Widget that will be placed into the Variables Scroll Area
  m_VariablesWidget = new QWidget(this);
  m_VariablesWidget->setGeometry(QRect(0, 0, 250, 267));

  m_VariablesVerticalLayout = new QVBoxLayout(m_VariablesWidget);
  m_VariablesVerticalLayout->setSpacing(30);
  m_VariablesVerticalLayout->setMargin(0);

  QString groupBoxStyle;
  QTextStream ss(&groupBoxStyle);
  ss << "QGroupBox {";
  ss << "    font-weight: bold;";
  ss << "}";
  ss << "QGroupBox::title {";
  ss << "    subcontrol-origin: margin;";
  ss << "    subcontrol-position: top left;";
  ss << "    padding: 0 5px;";
  ss << "    font-weight: bold;";
  ss << "}";

  QGroupBox* parametersGroupBox = new QGroupBox("Parameters", this);
  QVBoxLayout* pLayout = new QVBoxLayout(parametersGroupBox);
  pLayout->setContentsMargins(5, 5, 5, 5);
  parametersGroupBox->setStyleSheet(groupBoxStyle);

  QGroupBox* requiredGroupBox = new QGroupBox("Required Objects", this);
  QVBoxLayout* rLayout = new QVBoxLayout(requiredGroupBox);
  rLayout->setContentsMargins(5, 5, 5, 5);
  requiredGroupBox->setStyleSheet(groupBoxStyle);

  QGroupBox* createdGroupBox = new QGroupBox("Created Objects", this);
  QVBoxLayout* cLayout = new QVBoxLayout(createdGroupBox);
  cLayout->setContentsMargins(5, 5, 5, 5);
  createdGroupBox->setStyleSheet(groupBoxStyle);

  QGroupBox* noCategoryGroupBox = new QGroupBox("Uncategorized", this);
  QVBoxLayout* nLayout = new QVBoxLayout(noCategoryGroupBox);
  nLayout->setContentsMargins(5, 5, 5, 5);
  noCategoryGroupBox->setStyleSheet(groupBoxStyle);

  // Get the FilterWidgetManagere instance so we can instantiate new FilterParameterWidgets
  FilterWidgetManager* fwm = FilterWidgetManager::Instance();
  // Get a list of all the filterParameters from the filter.
  QVector<FilterParameter::Pointer> filterParameters = filter->getFilterParameters();
  // Create all the FilterParameterWidget objects that can be displayed where ever the developer needs
  bool addSpacer = true;

  int pCount = 0, rCount = 0, cCount = 0;
  for(QVector<FilterParameter::Pointer>::iterator iter = filterParameters.begin(); iter != filterParameters.end(); ++iter)
  {
    FilterParameter* parameter = (*iter).get();

    // Check to make sure that this is in fact a file system filter parameter
    if(nullptr != dynamic_cast<InputFileFilterParameter*>(parameter) || nullptr != dynamic_cast<InputPathFilterParameter*>(parameter) ||
       nullptr != dynamic_cast<DataContainerReaderFilterParameter*>(parameter))
    {
      validateFileSystemFilterParameter(parameter, filter);
    }

    QWidget* filterParameterWidget = fwm->createWidget(parameter, filter.get());
    m_PropertyToWidget.insert(parameter->getPropertyName(), filterParameterWidget); // Update our Map of Filter Parameter Properties to the Widget

    if(nullptr == filterParameterWidget)
    {
      continue;
    }

    // Determine which group box to add the widget into
    if(parameter->getCategory() == FilterParameter::Parameter)
    {
      filterParameterWidget->setParent(m_VariablesWidget);
      pLayout->addWidget(filterParameterWidget);
      pCount++;
    }
    else if(parameter->getCategory() == FilterParameter::RequiredArray)
    {
      filterParameterWidget->setParent(m_VariablesWidget);
      rLayout->addWidget(filterParameterWidget);
      rCount++;
    }
    else if(parameter->getCategory() == FilterParameter::CreatedArray)
    {
      filterParameterWidget->setParent(m_VariablesWidget);
      cLayout->addWidget(filterParameterWidget);
      cCount++;
    }
    else
    {
      filterParameterWidget->setParent(m_VariablesWidget);
      nLayout->addWidget(filterParameterWidget);
    }

    FilterParameterWidget* fpwPtr = qobject_cast<FilterParameterWidget*>(filterParameterWidget);
    if(fpwPtr)
    {
      if(fpwPtr->getWidgetIsExpanding())
      {
        addSpacer = false;
      }
    }

    // Connect up some signals and slots
    connect(filterParameterWidget, SIGNAL(parametersChanged()), this, SIGNAL(filterParametersChanged()));

    connect(filterParameterWidget, SIGNAL(errorSettingFilterParameter(const QString&)), this, SIGNAL(errorSettingFilterParameter(const QString&)));
  }

  // Now link any boolean widgets to any conditional Widgets that they might control.
  linkConditionalWidgets(filterParameters);

  // If there are widgets in the parameters group box, add it to the overall layout.  If not, remove the group box.
  if(pLayout->isEmpty() == false || pCount > 0)
  {
    m_VariablesVerticalLayout->addWidget(parametersGroupBox);
  }
  else
  {
    delete parametersGroupBox;
  }

  // If there are widgets in the required arrays group box, add it to the overall layout.  If not, remove the group box.
  if(rLayout->isEmpty() == false || rCount > 0)
  {
    m_VariablesVerticalLayout->addWidget(requiredGroupBox);
  }
  else
  {
    delete requiredGroupBox;
  }

  // If there are widgets in the created arrays group box, add it to the overall layout.  If not, remove the group box.
  if(cLayout->isEmpty() == false || cCount > 0)
  {
    m_VariablesVerticalLayout->addWidget(createdGroupBox);
  }
  else
  {
    delete createdGroupBox;
  }

  // If there are widgets in the uncategorized group box, add it to the overall layout.  If not, remove the group box.
  if(nLayout->isEmpty() == false)
  {
    m_VariablesVerticalLayout->addWidget(noCategoryGroupBox);
  }
  else
  {
    delete noCategoryGroupBox;
  }

  if(!addSpacer)
  {
    scrollAreaVertSpacer->removeItem(inputVertSpacer);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::validateFileSystemFilterParameter(FilterParameter* parameter, AbstractFilter::Pointer filter)
{
  QString fType;
  QString ext;
  int errCode = 0;
  QFileInfo fi = getFilterParameterPath(filter.get(), parameter, fType, ext, errCode);

  if(errCode < 0)
  {
    // Throw an error, because we have the wrong filter parameter type
  }

  QString currentPath = fi.absoluteFilePath();

  if(currentPath.isEmpty() == false && fi.exists() == false)
  {

    QString s = fType + QString(" Files (*") + ext + QString(");;All Files (*.*)");
    QString defaultName = m_OpenDialogLastFilePath;

    if(nullptr != dynamic_cast<InputFileFilterParameter*>(parameter))
    {
      InputFileFilterParameter* fsParam = dynamic_cast<InputFileFilterParameter*>(parameter);

      QString title = QObject::tr("Select a replacement input file for parameter '%1' in filter '%2'").arg(fsParam->getHumanLabel()).arg(filter->getHumanLabel());

      QString file = QFileDialog::getOpenFileName(this, title, defaultName, s);
      if(true == file.isEmpty())
      {
        file = currentPath;
      }
      file = QDir::toNativeSeparators(file);
      // Store the last used directory into the private instance variable
      QFileInfo fi(file);
      m_OpenDialogLastFilePath = fi.filePath();
      filter->setProperty(fsParam->getPropertyName().toLatin1().constData(), file);
    }

    else if(nullptr != dynamic_cast<InputPathFilterParameter*>(parameter))
    {
      InputPathFilterParameter* fsParam = dynamic_cast<InputPathFilterParameter*>(parameter);

      QString title = QObject::tr("Select a replacement input folder for parameter '%1' in filter '%2'").arg(fsParam->getHumanLabel()).arg(filter->getHumanLabel());

      QString file = QFileDialog::getExistingDirectory(this, title, defaultName, QFileDialog::ShowDirsOnly);
      file = QDir::toNativeSeparators(file);
      if(true == file.isEmpty())
      {
        file = currentPath;
      }
      // Store the last used directory into the private instance variable
      QFileInfo fi(file);
      m_OpenDialogLastFilePath = fi.filePath();
      filter->setProperty(fsParam->getPropertyName().toLatin1().constData(), file);
    }

    else if(nullptr != dynamic_cast<DataContainerReaderFilterParameter*>(parameter))
    {
      DataContainerReaderFilterParameter* fsParam = dynamic_cast<DataContainerReaderFilterParameter*>(parameter);

      QString title = QObject::tr("Select a replacement input file for parameter '%1' in filter '%2'").arg(fsParam->getHumanLabel()).arg(filter->getHumanLabel());

      QString file = QFileDialog::getOpenFileName(this, title, defaultName, s);
      if(true == file.isEmpty())
      {
        file = currentPath;
      }
      file = QDir::toNativeSeparators(file);
      // Store the last used directory into the private instance variable
      QFileInfo fi(file);
      m_OpenDialogLastFilePath = fi.filePath();
      filter->setProperty(fsParam->getInputFileProperty().toLatin1().constData(), file);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::linkConditionalWidgets(QVector<FilterParameter::Pointer>& filterParameters)
{
  // Figure out if we have any "Linked Boolean Widgets" to hook up to other widgets
  for(QVector<FilterParameter::Pointer>::iterator iter = filterParameters.begin(); iter != filterParameters.end(); ++iter)
  {
    FilterParameter::Pointer filterParameter = (*iter);
    LinkedBooleanFilterParameter::Pointer filterParameterPtr = std::dynamic_pointer_cast<LinkedBooleanFilterParameter>(filterParameter);

    if(nullptr != filterParameterPtr.get())
    {
      QStringList linkedProps = filterParameterPtr->getConditionalProperties();

      QStringListIterator iter = QStringListIterator(linkedProps);
      QWidget* checkboxSource = m_PropertyToWidget[filterParameterPtr->getPropertyName()];
      while(iter.hasNext())
      {
        QString propName = iter.next();
        QWidget* w = m_PropertyToWidget[propName];
        if(w)
        {
          connect(checkboxSource, SIGNAL(conditionalPropertyChanged(int)), w, SLOT(setLinkedConditionalState(int)));
          LinkedBooleanWidget* lbw = qobject_cast<LinkedBooleanWidget*>(checkboxSource);
          if(lbw && lbw->getLinkedState() != Qt::Checked)
          {
            w->hide();
          }
        }
      }
      LinkedBooleanWidget* boolWidget = qobject_cast<LinkedBooleanWidget*>(checkboxSource);
      if(boolWidget)
      {
        boolWidget->updateLinkedWidgets();
      }
    }

    // Figure out if we have any Linked ComboBox Widgets to hook up to other widgets
    LinkedChoicesFilterParameter::Pointer optionPtr2 = std::dynamic_pointer_cast<LinkedChoicesFilterParameter>(filterParameter);

    if(nullptr != optionPtr2.get())
    {
      QStringList linkedProps = optionPtr2->getLinkedProperties();

      QStringListIterator iter = QStringListIterator(linkedProps);
      QWidget* checkboxSource = m_PropertyToWidget[optionPtr2->getPropertyName()];
      while(iter.hasNext())
      {
        QWidget* w = nullptr;
        QString propName = iter.next();
        if(m_PropertyToWidget.contains(propName))
        {
          w = m_PropertyToWidget[propName];
        }
         
        if(w)
        {
          // qDebug() << "Connecting: " << optionPtr2->getPropertyName() << " to " << propName;
          connect(checkboxSource, SIGNAL(conditionalPropertyChanged(int)), w, SLOT(setLinkedComboBoxState(int)));

          ChoiceWidget* choiceWidget = qobject_cast<ChoiceWidget*>(checkboxSource);
          if(choiceWidget)
          {
            choiceWidget->widgetChanged(choiceWidget->getCurrentIndex(), false);
          }
        }
      }
    }

    LinkedDataContainerSelectionFilterParameter::Pointer optionPtr3 = std::dynamic_pointer_cast<LinkedDataContainerSelectionFilterParameter>(filterParameter);

    if(nullptr != optionPtr3.get())
    {
      QStringList linkedProps = optionPtr3->getLinkedProperties();

      QStringListIterator iter = QStringListIterator(linkedProps);
      QWidget* checkboxSource = m_PropertyToWidget[optionPtr3->getPropertyName()];
      while(iter.hasNext())
      {
        QWidget* w = nullptr;
        QString propName = iter.next();
        if(m_PropertyToWidget.contains(propName))
        {
          w = m_PropertyToWidget[propName];
        }
        if(w)
        {
          // qDebug() << "Connecting: " << optionPtr2->getPropertyName() << " to " << propName;
          connect(checkboxSource, SIGNAL(conditionalPropertyChanged(int)), w, SLOT(setLinkedComboBoxState(int)));

          LinkedDataContainerSelectionWidget* linkedDataContainerSelectionWidget = qobject_cast<LinkedDataContainerSelectionWidget*>(checkboxSource);
          if(linkedDataContainerSelectionWidget)
          {
            linkedDataContainerSelectionWidget->widgetChanged();
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::on_filterHelpBtn_clicked()
{
  DocRequestManager* docRequester = DocRequestManager::Instance();
  docRequester->requestFilterDocs(m_FilterClassName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::clearInputWidgets()
{
  // Remove any existing input widgets
  QLayoutItem* item = variablesGrid->itemAt(0);
  if(item)
  {
    QWidget* w = item->widget();
    if(w)
    {
      w->setVisible(false);
      variablesGrid->removeWidget(w);
    }
  }

  filterHumanLabel->setText("No Filter Selected");
  brandingLabel->clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::removeWidgetInputs(SVPipelineFilterWidget* w)
{
  m_VariablesWidget->setParent(w);
  clearInputWidgets();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::displayFilterParameters(PipelineFilterObject* w)
{
  clearInputWidgets();

  if(m_VariablesWidget != nullptr)
  {
    variablesGrid->addWidget(m_VariablesWidget);
    m_VariablesWidget->setVisible(true);
  }

  AbstractFilter::Pointer f = w->getFilter();
  if(f.get())
  {
    ISIMPLibPlugin* plug = f->getPluginInstance();
    if(plug)
    {
      m_BrandingLabel = QString("Plugin: %1 (%2) Filter Name: %3").arg(plug->getPluginDisplayName()).arg(plug->getVersion()).arg(w->getFilterClassName());
    }
    else
    {
      m_BrandingLabel = QString("Plugin: Unknown Plugin. Filter Name: %1").arg(w->getFilterClassName());
    }
    brandingLabel->setText(m_BrandingLabel);
  }
  // Add a label at the top of the Inputs Tabs to show what filter we are working on
  filterHumanLabel->setText(w->getHumanLabel());
  filterIndex->clear();
  QString style;


  QString filterGroup;
  QTextStream groupStream(&filterGroup);
  groupStream << "Group: " << w->getFilterGroup() << "\n";
  groupStream << "Subgroup: " << w->getFilterSubGroup();
  filterHumanLabel->setToolTip(filterGroup);

  QColor bgColor =  w->getGroupColor();
  QColor borderColor = QColor::fromHsv(bgColor.hue(), 100, 120);

  QTextStream styleStream(&style);
  styleStream << "QFrame#" << labelFrame->objectName() << "{";
  styleStream << "border-bottom: 0px solid;";
  styleStream << "border-bottom-color: " << borderColor.name() << ";";
 // styleStream << "background-color: " << bgColor.name() << ";";
 // styleStream << "border-radius: 0 0 0 0px;";
  styleStream << "}";

#if 0
  int index = -1;
  if(f.get()) {
    index = f->getPipelineIndex() + 1;
  }
  filterIndex->setText(QString::number(index));


  styleStream << "QLabel#" << filterIndex->objectName() << "{";
  styleStream << "background-color: rgb(48, 48, 48);";
  styleStream << "color: rgb(242, 242, 242);"; // Always have a white'ish font
 // styleStream << "border-radius: 3px;";
  styleStream << "padding: 1 5 1 5px;";
  styleStream << "}";
#endif

  labelFrame->setStyleSheet(style);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::fadeInWidget(QWidget* widget)
{
  if(m_FaderWidget)
  {
    m_FaderWidget->close();
  }
  m_FaderWidget = new QtSFaderWidget(widget);
  m_FaderWidget->setStartColor(SIMPLView::Defaults::AdvancedColor);
  m_FaderWidget->start();
  m_AdvFadedOut = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::fadeOutWidget(QWidget* widget)
{

  if(m_FaderWidget)
  {
    m_FaderWidget->close();
  }
  m_FaderWidget = new QtSFaderWidget(widget);
  m_FaderWidget->setFadeOut();
  m_FaderWidget->setStartColor(SIMPLView::Defaults::AdvancedColor);
  connect(m_FaderWidget, SIGNAL(animationComplete()), widget, SLOT(hide()));
  m_FaderWidget->start();
  m_AdvFadedOut = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* FilterInputWidget::getVariablesTabContentsWidget()
{
  return variablesTabContents;
}

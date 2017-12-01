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

#include "ComparisonSelectionAdvancedWidget.h"

#include <QtWidgets/QMenu>

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSStyles.h"

#include "FilterParameterWidgetUtils.hpp"
#include "FilterParameterWidgetsDialogs.h"

#include "SIMPLib/Filtering/ComparisonValue.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionAdvancedWidget::ComparisonSelectionAdvancedWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
  : FilterParameterWidget(parameter, filter, parent)
  , m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<ComparisonSelectionAdvancedFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "ComparisonSelectionAdvancedWidget can ONLY be used with a ComparisonSelectionAdvancedFilterParameter object");

  QString filterName = getFilter()->getNameOfClass();

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionAdvancedWidget::~ComparisonSelectionAdvancedWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInputsAdvanced ComparisonSelectionAdvancedWidget::getComparisonInputs()
{
  ComparisonInputsAdvanced comps;
  if (comparisonSetWidget == nullptr)
  {
    return comps;
  }

  DataArrayPath amPath = DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter);
  comps.setDataContainerName(amPath.getDataContainerName());
  comps.setAttributeMatrixName(amPath.getAttributeMatrixName());

  QString dcName = comps.getDataContainerName();
  QString amName = comps.getAttributeMatrixName();

  comps.setInvert(comparisonSetWidget->getComparisonSet()->getInvertComparison());

  QVector<AbstractComparison::Pointer> comparisons = comparisonSetWidget->getComparisons();

  for (int i = 0; i < comparisons.size(); ++i)
  {
    comps.addInput(comparisons[i]);
  }
  return comps;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::setupGui()
{
  if (getFilter() == nullptr)
  {
    return;
  }
  if (getFilterParameter() == nullptr)
  {
    return;
  }

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // Create the Comparison Set
  comparisonSetWidget->setComparisonSet(ComparisonSet::New());
  connect(comparisonSetWidget, SIGNAL(comparisonChanged()), this, SIGNAL(parametersChanged()));

  // Copy the data into the Comparison Set
  ComparisonInputsAdvanced comps = dynamic_cast<ComparisonSelectionAdvancedFilterParameter*>(getFilterParameter())->getGetterCallback()();

  m_SelectedAttributeMatrixPath->setStyleSheet(QtSStyles::QToolSelectionButtonStyle(false));

  m_MenuMapper = new QSignalMapper(this);
  connect(m_MenuMapper, SIGNAL(mapped(QString)),
    this, SLOT(attributeMatrixSelected(QString)));

  DataArrayPath defaultPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();
  m_SelectedAttributeMatrixPath->setText(defaultPath.serialize(Detail::Delimiter));

  DataArrayPath currentPath;
  currentPath.setDataContainerName(comps.getDataContainerName());
  currentPath.setAttributeMatrixName(comps.getAttributeMatrixName());
  if (!currentPath.isEmpty())
  {
    QStringList names = generateAttributeArrayList(currentPath.getDataContainerName(), currentPath.getAttributeMatrixName());
    presetAttributeMatrix(currentPath);
  }
  comparisonSetWidget->setComparisons(comps.getInputs());

#if 0
  // is the filter parameter tied to a boolean property of the Filter Instance, if it is then we need to make the check box visible
  if (getFilterParameter()->isConditional() == true)
  {
    bool boolProp = getFilter()->property(getFilterParameter()->getConditionalProperty().toLatin1().constData()).toBool();
    conditionalCB->setChecked(boolProp);
    conditionalCB->setText(getFilterParameter()->getConditionalLabel());
    dataContainerList->setEnabled(boolProp);
    attributeMatrixList->setEnabled(boolProp);
    attributeArrayList->setEnabled(boolProp);
  }
  else
  {
    widgetLayout->removeWidget(conditionalCB);
    conditionalCB->deleteLater();
    widgetLayout->removeWidget(linkLeft);
    linkLeft->deleteLater();
    widgetLayout->removeWidget(linkRight);
    linkRight->deleteLater();
  }
#endif
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::on_conditionalCB_stateChanged(int state)
{
  bool boolProp = conditionalCB->isChecked();
  dataContainerList->setEnabled(boolProp);
  dataContainerList->setVisible(boolProp);
  attributeMatrixList->setEnabled(boolProp);
  attributeMatrixList->setVisible(boolProp);
  attributeArrayList->setEnabled(boolProp);
  attributeArrayList->setVisible(boolProp);

  label->setVisible(boolProp);
  linkLeft->setVisible(boolProp);
  linkRight->setVisible(boolProp);
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;

}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ComparisonSelectionAdvancedWidget::generateAttributeArrayList(const QString& currentDCName, const QString& currentAttrMatName)
{
  //  std::cout << "ComparisonSelectionAdvancedWidget::generateAttributeArrayList()" << std::endl;
  QStringList attributeArrayList;

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = m_DcaProxy.dataContainers.values();
  QListIterator<DataContainerProxy> containerIter(containers);
  while (containerIter.hasNext())
  {
    DataContainerProxy dc = containerIter.next();
    if (dc.name.compare(currentDCName) == 0)
    {
      // We found the proper Data Container, now populate the AttributeMatrix List
      QMap<QString, AttributeMatrixProxy> attrMats = dc.attributeMatricies;
      QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
      while (attrMatsIter.hasNext())
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        if (amName.compare(currentAttrMatName) == 0)
        {
          // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
          AttributeMatrixProxy amProxy = attrMatsIter.value();
          QMap<QString, DataArrayProxy> dataArrays = amProxy.dataArrays;
          QMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
          while (dataArraysIter.hasNext())
          {
            dataArraysIter.next();
            // DataArrayProxy daProxy = dataArraysIter.value();
            QString daName = dataArraysIter.key();
            attributeArrayList << daName;
          }
        }
      }
    }
  }

  return attributeArrayList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComparisonSelectionAdvancedWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  if (curDcName.isEmpty() == true && filtDcName.isEmpty() == false)
  {
    return filtDcName;
  }
  else if (curDcName.isEmpty() == false && filtDcName.isEmpty() == true)
  {
    return curDcName;
  }
  else if (curDcName.isEmpty() == false && filtDcName.isEmpty() == false && m_DidCausePreflight == true)
  {
    return curDcName;
  }

  return filtDcName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::widgetChanged(const QString& text)
{
  Q_UNUSED(text)
    m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::setComparisons(QVector<AbstractComparison::Pointer> comparisons)
{
  qint32 count = comparisons.size();

  for (int i = 0; i < count; ++i)
  {
    comparisonSetWidget->addComparison(comparisons[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QVariant var;
  ComparisonInputsAdvanced comps = getComparisonInputs();
  var.setValue(comps);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if (false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::beforePreflight()
{
  if (nullptr == getFilter())
  {
    return;
  }
  if (m_DidCausePreflight == true)
  {
    // std::cout << "***  ComparisonSelectionAdvancedWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  if (m_SelectedAttributeMatrixPath->text().isEmpty())
  {
    populateButtonText();
  }

  createSelectionMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::afterPreflight()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (NULL == dca.get()) { return; }

  if (dca->doesAttributeMatrixExist(DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter)))
  {
    AttributeMatrix::Pointer am = dca->getAttributeMatrix(DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter));
    if (nullptr != am.get()) {
      QString html = am->getInfoString(SIMPL::HtmlFormat);
      m_SelectedAttributeMatrixPath->setToolTip(html);
      m_SelectedAttributeMatrixPath->setStyleSheet(QtSStyles::QToolSelectionButtonStyle(true));

      DataArrayPath path = DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter);
      QStringList arrayNames = generateAttributeArrayList(path.getDataContainerName(), path.getAttributeMatrixName());
      
      comparisonSetWidget->setArrayNames(arrayNames);

      if (nullptr == comparisonSetWidget->getAttributeMatrix())
      {
        ComparisonInputsAdvanced comps = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<ComparisonInputsAdvanced>();
        comparisonSetWidget->setAttributeMatrix(am);
        comparisonSetWidget->setComparisons(comps.getInputs());
      }
    }
  }
  else
  {
    m_SelectedAttributeMatrixPath->setStyleSheet(QtSStyles::QToolSelectionButtonStyle(false));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::populateButtonText()
{
  //  std::cout << "ComparisonSelectionWidget::populateComboBoxes()" << std::endl;

  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (nullptr == dca.get())
  {
    return;
  }

  // Check to see if we have any DataContainers to actually populate drop downs with.
  if (dca->getDataContainers().size() == 0)
  {
    return;
  }

  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Grab what is currently selected
  QString curDcName = "";
  QString curAmName = "";

  // Get what is in the filter
  ComparisonInputsAdvanced comps = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<ComparisonInputsAdvanced>();

  QString filtDcName = comps.getDataContainerName();
  QString filtAmName = comps.getAttributeMatrixName();

  QString dcName = checkStringValues(curDcName, filtDcName);
  QString amName = checkStringValues(curAmName, filtAmName);

  m_SelectedAttributeMatrixPath->setText(dcName + Detail::Delimiter + amName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonSelectionAdvancedWidget::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == QEvent::Show && obj == m_SelectedAttributeMatrixPath->menu())
  {
    QPoint pos = adjustedMenuPosition(m_SelectedAttributeMatrixPath);
    m_SelectedAttributeMatrixPath->menu()->move(pos);
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::attributeMatrixSelected(QString path)
{
  setSelectedPath(path);

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::setSelectedPath(QString path)
{
  DataArrayPath amPath = DataArrayPath::Deserialize(path, Detail::Delimiter);
  setSelectedPath(amPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::setSelectedPath(DataArrayPath amPath)
{
  if (amPath.isEmpty()) { return; }

  m_SelectedAttributeMatrixPath->setText("");
  m_SelectedAttributeMatrixPath->setToolTip("");

  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (nullptr == dca.get())
  {
    return;
  }

  if (dca->doesAttributeMatrixExist(amPath))
  {
    AttributeMatrix::Pointer am = dca->getAttributeMatrix(amPath);
    QString html = am->getInfoString(SIMPL::HtmlFormat);
    m_SelectedAttributeMatrixPath->setToolTip(html);
    m_SelectedAttributeMatrixPath->setText(amPath.serialize(Detail::Delimiter));

    if (nullptr != comparisonSetWidget->getComparisonSet())
    {
      comparisonSetWidget->setAttributeMatrix(am);
      comparisonSetWidget->setComparisonSet(ComparisonSet::New());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::presetAttributeMatrix(DataArrayPath amPath)
{
  m_presetPath = amPath;
  m_SelectedAttributeMatrixPath->setText(amPath.serialize(Detail::Delimiter));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedWidget::createSelectionMenu()
{
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (nullptr == dca.get())
  {
    return;
  }

  // Get the menu and clear it out
  QMenu* menu = m_SelectedAttributeMatrixPath->menu();
  if (!menu)
  {
    menu = new QMenu();
    m_SelectedAttributeMatrixPath->setMenu(menu);
    menu->installEventFilter(this);
  }
  if (menu)
  {
    menu->clear();
  }

  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  QList<DataContainer::Pointer> containers = dca->getDataContainers();
  QVector<AttributeMatrix::Type> amTypes = m_FilterParameter->getDefaultAttributeMatrixTypes();
  IGeometry::Types geomTypes = m_FilterParameter->getDefaultGeometryTypes();

  QListIterator<DataContainer::Pointer> containerIter(containers);
  while (containerIter.hasNext())
  {
    DataContainer::Pointer dc = containerIter.next();

    IGeometry::Pointer geom = IGeometry::NullPointer();
    IGeometry::Type geomType = IGeometry::Type::Unknown;
    if (nullptr != dc.get())
    {
      geom = dc->getGeometry();
    }
    if (nullptr != geom.get())
    {
      geomType = geom->getGeometryType();
    }

    QMenu* dcMenu = new QMenu(dc->getName());
    dcMenu->setDisabled(false);
    menu->addMenu(dcMenu);
    if (geomTypes.isEmpty() == false && geomTypes.contains(geomType) == false)
    {
      dcMenu->setDisabled(true);
    }

    // We found the proper Data Container, now populate the AttributeMatrix List
    DataContainer::AttributeMatrixMap_t attrMats = dc->getAttributeMatrices();
    QMapIterator<QString, AttributeMatrix::Pointer> attrMatsIter(attrMats);
    while (attrMatsIter.hasNext())
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      AttributeMatrix::Pointer am = attrMatsIter.value();

      QAction* action = new QAction(amName, dcMenu);
      DataArrayPath daPath(dc->getName(), amName, "");
      QString path = daPath.serialize(Detail::Delimiter);
      action->setData(path);

      connect(action, SIGNAL(triggered(bool)), m_MenuMapper, SLOT(map()));
      m_MenuMapper->setMapping(action, path);
      dcMenu->addAction(action);

      bool amIsNotNull = (nullptr != am.get()) ? true : false;
      bool amValidType = (amTypes.isEmpty() == false && amTypes.contains(am->getType()) == false) ? true : false;

      if (amIsNotNull && amValidType)
      {
        action->setDisabled(true);
      }
    }
  }
}

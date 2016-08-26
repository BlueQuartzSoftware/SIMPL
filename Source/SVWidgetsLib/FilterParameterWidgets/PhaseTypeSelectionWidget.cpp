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

#include "PhaseTypeSelectionWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>
#include <QtCore/QSignalMapper>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>

#include "SIMPLib/Common/PhaseType.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/FilterParameters/PhaseTypeSelectionFilterParameter.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSStyles.h"

#include "FilterParameterWidgetsDialogs.h"

// Include the MOC generated file for this class
#include "moc_PhaseTypeSelectionWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::PhaseTypeSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<PhaseTypeSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "PhaseTypeSelectionWidget can ONLY be used with a PhaseTypeSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::PhaseTypeSelectionWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::~PhaseTypeSelectionWidget()
{}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::setupGui()
{
  if(getFilter() == NULL)
  {
    return;
  }

  label->setText(m_FilterParameter->getHumanLabel() );

  // Get the default path from the Filter instance to cache
  //UInt32Vector_t data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();
  //data.d =

  m_SelectedAttributeMatrixPath->setStyleSheet(QtSStyles::DAPSelectionButtonStyle(true));

  m_MenuMapper = new QSignalMapper(this);
  connect(m_MenuMapper, SIGNAL(mapped(QString)),
            this, SLOT(attributeMatrixSelected(QString)));

  attributeMatrixSelected(m_DefaultPath.serialize(Detail::Delimiter));
  createSelectionMenu();

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  updatePhaseComboBoxes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhaseTypeSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  if(curDcName.isEmpty() == true && filtDcName.isEmpty() == false)
  {return filtDcName;}
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == true)
  {return curDcName;}
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == false && m_DidCausePreflight == true)
  { return curDcName;}

  return filtDcName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::createSelectionMenu()
{
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  // Get the menu and clear it out
  QMenu* menu = m_SelectedAttributeMatrixPath->menu();
  if(!menu)
  {
    menu = new QMenu();
    m_SelectedAttributeMatrixPath->setMenu(menu);
    menu->installEventFilter(this);
  }
  if(menu) {
    menu->clear();
  }

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  QList<DataContainer::Pointer> containers = dca->getDataContainers();
  QVector<unsigned int> amTypes = m_FilterParameter->getDefaultAttributeMatrixTypes();
  QVector<unsigned int> geomTypes = m_FilterParameter->getDefaultGeometryTypes();

  QListIterator<DataContainer::Pointer> containerIter(containers);
  while(containerIter.hasNext())
  {
    DataContainer::Pointer dc = containerIter.next();

    IGeometry::Pointer geom = IGeometry::NullPointer();
    uint32_t geomType = 999;
    if (NULL != dc.get()) { geom = dc->getGeometry(); }
    if (NULL != geom.get()) { geomType = geom->getGeometryType(); }


    QMenu* dcMenu = new QMenu(dc->getName());
    dcMenu->setDisabled(false);
    menu->addMenu(dcMenu);
    if(geomTypes.isEmpty() == false && geomTypes.contains(geomType) == false )
    {
      dcMenu->setDisabled(true);
    }


    // We found the proper Data Container, now populate the AttributeMatrix List
    DataContainer::AttributeMatrixMap_t attrMats = dc->getAttributeMatrices();
    QMapIterator<QString, AttributeMatrix::Pointer> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext() )
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PhaseTypeSelectionWidget::eventFilter(QObject* obj, QEvent* event)
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
void PhaseTypeSelectionWidget::attributeMatrixSelected(QString path)
{
  m_SelectedAttributeMatrixPath->setText(path);

  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }


  IDataArray::Pointer attrArray = dca->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(getFilter(), DataArrayPath::Deserialize(path, Detail::Delimiter));
  if(nullptr != attrArray.get()) {
    QString html = attrArray->getInfoString(SIMPL::HtmlFormat);
    m_SelectedAttributeMatrixPath->setToolTip(html);
  }

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::updatePhaseComboBoxes()
{
  bool ok = false;
  // setup the list of choices for the widget
  PhaseTypeSelectionFilterParameter* phaseTypes = dynamic_cast<PhaseTypeSelectionFilterParameter*>(getFilterParameter());
  QString countProp = phaseTypes->getPhaseTypeCountProperty();
  int phaseCount = getFilter()->property(countProp.toLatin1().constData()).toInt(&ok);
  QString phaseDataProp = phaseTypes->getPhaseTypeDataProperty();

  UInt32Vector_t vectorWrapper = getFilter()->property(phaseDataProp.toLatin1().constData()).value<UInt32Vector_t>();
  QVector<quint32> dataFromFilter = vectorWrapper.d;
  if(phaseCount < 0 && dataFromFilter.size() < 10)   // there was an issue getting the phase Count from the Filter.
  {
    phaseCount = dataFromFilter.size(); // So lets just use the count from the actual phase data
  }

  // Get our list of predefined enumeration values
  QVector<unsigned int> phaseTypeEnums;
  PhaseType::getPhaseTypeEnums(phaseTypeEnums);

  phaseListWidget->clear();
  // Get our list of Phase Type Strings
  QStringList phaseListChoices = m_FilterParameter->getPhaseListChoices();

  for (int i = 1; i < phaseCount; i++)
  {
    QComboBox* cb = new QComboBox(NULL);
    for (int s = 0; s < phaseListChoices.size(); ++s)
    {
      cb->addItem((phaseListChoices[s]), phaseTypeEnums[s]);
      cb->setItemData(static_cast<int>(s), phaseTypeEnums[s], Qt::UserRole);
    }

    QListWidgetItem* item = new QListWidgetItem(phaseListWidget);
    phaseListWidget->addItem(item);
    phaseListWidget->setItemWidget(item, cb);

    if (i < dataFromFilter.size())
    {
      cb->setCurrentIndex(dataFromFilter[i]);
    }
    connect(cb, SIGNAL(currentIndexChanged(int)),
            this, SLOT(phaseTypeComboBoxChanged(int)) );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::phaseTypeComboBoxChanged(int index)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::resetPhaseComboBoxes()
{
  int count = phaseListWidget->count();

  for (int i = 0; i < count; ++i)
  {
    QComboBox* cb = qobject_cast<QComboBox*>(phaseListWidget->itemWidget(phaseListWidget->item(i)));
    if (cb) { cb->setCurrentIndex(-1); }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::beforePreflight()
{
  if (NULL == getFilter()) { return; }
  if(m_DidCausePreflight == true)
  {
    return;
  }

  createSelectionMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::afterPreflight()
{
  updatePhaseComboBoxes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  int count = phaseListWidget->count();
//  {
//    std::cout << "PhaseTypeSelectionWidget::filterNeedsInputParameters Count = " << count << std::endl;
//  }

  PhaseTypeSelectionFilterParameter* p = dynamic_cast<PhaseTypeSelectionFilterParameter*>(getFilterParameter());
  QVariant var;

  // QVector<uint32_t> phaseTypes(count, SIMPL::PhaseType::UnknownPhaseType);
  QVector<uint32_t> phaseTypes(count + 1, SIMPL::PhaseType::UnknownPhaseType);
  bool ok = false;
  phaseTypes[0] = SIMPL::PhaseType::UnknownPhaseType;
  for (int i = 0; i < count; ++i)
  {
    QComboBox* cb = qobject_cast<QComboBox*>(phaseListWidget->itemWidget(phaseListWidget->item(i)));
    unsigned int sType = static_cast<unsigned int>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
    //phaseTypes[i+1] = sType;
    phaseTypes[i + 1] = sType;
  }

  UInt32Vector_t data;
  data.d = phaseTypes;
  var.setValue(data);
  ok = false;

  // Set the value into the Filter
  ok = filter->setProperty(p->getPhaseTypeDataProperty().toLatin1().constData(), var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

  DataArrayPath path = DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter);

  var.setValue(path);
  ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(p->getAttributeMatrixPathProperty().toLatin1().constData(), var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }
}

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
#include "MultiDataArraySelectionWidget.h"

#include <QtCore/QSignalMapper>
#include <QtCore/QMetaProperty>
#include <QtCore/QList>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QMenu>
#include <QtWidgets/QListWidgetItem>

#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSStyles.h"

#include "FilterParameterWidgetsDialogs.h"
#include "FilterParameterWidgetUtils.hpp"

// Include the MOC generated file for this class
#include "moc_MultiDataArraySelectionWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionWidget::MultiDataArraySelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<MultiDataArraySelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "nullptr Pointer", "MultiDataArraySelectionWidget can ONLY be used with a MultiDataArraySelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionWidget::MultiDataArraySelectionWidget(QWidget* parent) :
  FilterParameterWidget(nullptr, nullptr, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionWidget::~MultiDataArraySelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  //qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " MultiDataArraySelectionWidget::initializeWidget";
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::setupGui()
{
  // Sanity Check the filter and the filter parameter
  if (getFilter() == nullptr)
  {
    return;
  }
  if (getFilterParameter() == nullptr)
  {
    return;
  }

  // Generate the text for the QLabel
  label->setText(getFilterParameter()->getHumanLabel());

  m_SelectedAttributeMatrixPath->setStyleSheet(QtSStyles::DAPSelectionButtonStyle(true));

  m_MenuMapper = new QSignalMapper(this);
  connect(m_MenuMapper, SIGNAL(mapped(QString)),
            this, SLOT(attributeMatrixSelected(QString)));

  // Lastly, hook up the filter's signals and slots to our own signals and slots
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiDataArraySelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  ////qDebug() << "    checkStringValues(...)" << curDcName << "  " << filtDcName;
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
void MultiDataArraySelectionWidget::createSelectionMenu()
{
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get()) { return; }

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
    if (nullptr != dc.get()) { geom = dc->getGeometry(); }
    if (nullptr != geom.get()) { geomType = geom->getGeometryType(); }


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
bool MultiDataArraySelectionWidget::eventFilter(QObject* obj, QEvent* event)
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
void MultiDataArraySelectionWidget::attributeMatrixSelected(QString path)
{
  setSelectedPath(path);

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::setSelectedPath(QString path)
{
  DataArrayPath amPath = DataArrayPath::Deserialize(path, Detail::Delimiter);
  if (amPath.isEmpty()) { return; }

  m_SelectedAttributeMatrixPath->setText("");
  m_SelectedAttributeMatrixPath->setToolTip("");
  attributeArraysSelectWidget->clear();
  attributeArraysOrderWidget->clear();

  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get()) { return; }

  int err = 0;
  AttributeMatrix::Pointer attrMat = dca->getPrereqAttributeMatrixFromPath(getFilter(), amPath, err);
  if(nullptr != attrMat.get()) {
    QString html = attrMat->getInfoString(SIMPL::HtmlFormat);
    m_SelectedAttributeMatrixPath->setToolTip(html);
    m_SelectedAttributeMatrixPath->setText(path);
    QList<QString> arrayNames = attrMat->getAttributeArrayNames();
    QVector<DataArrayPath> selectedPaths = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<QVector<DataArrayPath> >();
    for (int i=0; i<arrayNames.size(); i++)
    {
      QString daName = arrayNames[i];
      QListWidgetItem* daItem = new QListWidgetItem(daName);
      daItem->setCheckState(Qt::Unchecked);

      for (int i = 0; i < selectedPaths.size(); i++)
      {
        if (selectedPaths.at(i).getDataArrayName() == daName)
        {
          daItem->setCheckState(Qt::Checked);
        }
      }
      attributeArraysSelectWidget->addItem(daItem);
    }

    for (int i = 0; i < selectedPaths.size(); i++)
    {
      attributeArraysOrderWidget->addItem(selectedPaths[i].getDataArrayName());
    }
  }

  updateSelectAllCheckbox();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_attributeArraysSelectWidget_itemChanged(QListWidgetItem* item)
{
  if (item->checkState() == Qt::Checked)
  {
    QList<QListWidgetItem*> list = attributeArraysOrderWidget->findItems(item->text(), Qt::MatchExactly);
    if (list.size() <= 0)
    {
      QListWidgetItem* itemCopy = new QListWidgetItem(item->text());
      attributeArraysOrderWidget->addItem(itemCopy);
    }
  }
  else
  {
    QList<QListWidgetItem*> list = attributeArraysOrderWidget->findItems(item->text(), Qt::MatchExactly);
    for (int i = 0; i < list.size(); i++)
    {
      attributeArraysOrderWidget->removeItemWidget(list[i]);
      delete list[i];
    }
  }

  updateSelectAllCheckbox();

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_upBtn_pressed()
{
  int currentIndex = attributeArraysOrderWidget->currentRow();

  if (currentIndex > 0)
  {
    QListWidgetItem* item = attributeArraysOrderWidget->takeItem(currentIndex);
    attributeArraysOrderWidget->insertItem(currentIndex - 1, item);
    attributeArraysOrderWidget->setCurrentRow(currentIndex - 1);

    m_DidCausePreflight = true;
    emit parametersChanged();
    m_DidCausePreflight = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_downBtn_pressed()
{
  int currentIndex = attributeArraysOrderWidget->currentRow();

  if (currentIndex < attributeArraysOrderWidget->count() - 1)
  {
    QListWidgetItem* item = attributeArraysOrderWidget->takeItem(currentIndex);
    attributeArraysOrderWidget->insertItem(currentIndex + 1, item);
    attributeArraysOrderWidget->setCurrentRow(currentIndex + 1);

    m_DidCausePreflight = true;
    emit parametersChanged();
    m_DidCausePreflight = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::removeNonexistantPaths(QVector<DataArrayPath> &paths)
{
	AbstractFilter* filter = getFilter();
	if (nullptr == filter) { return; }

	bool reloadPath = false;
	DataArrayPath amPath = DataArrayPath::GetAttributeMatrixPath(paths);

	for (int i = 0; i < paths.size(); i++)
	{
		bool valid = true;

		if (nullptr == filter->getDataContainerArray()->getAttributeMatrix(paths[i])->getAttributeArray(paths[i].getDataArrayName()))
			valid = false;
		
		if (false == paths[i].isValid())
			valid = false;
		
		if (!valid)
		{
			QString& pathName = paths[i].getDataArrayName();
			QList<QListWidgetItem*> invalidDataArrayWidgets = attributeArraysOrderWidget->findItems(pathName, Qt::MatchExactly);
			for (int j = 0; j < invalidDataArrayWidgets.size(); j++)
			{
				invalidDataArrayWidgets[j]->setCheckState(Qt::Unchecked);
				on_attributeArraysSelectWidget_itemChanged(invalidDataArrayWidgets[j]);
			}
			
			paths.removeAt(i);
			i--;

			reloadPath = true;
		}
	}

	if (reloadPath && !amPath.isEmpty())
	{
		setSelectedPath(amPath.serialize(Detail::Delimiter));
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::beforePreflight()
{
  if (nullptr == getFilter()) { return; }
  if (m_DidCausePreflight == true)
  {
   // std::cout << "***  MultiDataArraySelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  QVector<DataArrayPath> paths = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<QVector<DataArrayPath> >();

  removeNonexistantPaths(paths);
  getFilter()->property(PROPERTY_NAME_AS_CHAR).setValue(paths);

  DataArrayPath amPath = DataArrayPath::GetAttributeMatrixPath(paths);

  setSelectedPath(amPath.serialize(Detail::Delimiter));
  createSelectionMenu();
  updateSelectAllCheckbox();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::afterPreflight()
{
  // std::cout << "After Preflight" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  DataArrayPath amPath = DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter);

  QVector<DataArrayPath> selectedPaths;
  for (int i = 0; i < attributeArraysOrderWidget->count(); i++)
  {
    DataArrayPath path = amPath;
    path.setDataArrayName(attributeArraysOrderWidget->item(i)->text());
    selectedPaths.push_back(path);
  }

  // Generate the path to the AttributeArray
  QVariant var;
  var.setValue(selectedPaths);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if (false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_selectCheckBox_stateChanged(int state)
{
  attributeArraysSelectWidget->blockSignals(true);

  for (int i = 0; i < attributeArraysSelectWidget->count(); i++)
  {
    if (attributeArraysSelectWidget->item(i)->flags().testFlag(Qt::ItemIsEnabled) == true)
    {
      if (state == Qt::Checked)
      {
        attributeArraysSelectWidget->item(i)->setCheckState(Qt::Checked);
      }
      else if (state == Qt::Unchecked)
      {
        attributeArraysSelectWidget->item(i)->setCheckState(Qt::Unchecked);
      }
      else
      {
        // Skip the "Partially Checked" check state when merely toggling the "Select All/Deselect All" checkbox
        selectCheckBox->setCheckState(Qt::Checked);
      }
    }

    on_attributeArraysSelectWidget_itemChanged(attributeArraysSelectWidget->item(i));
  }

  attributeArraysSelectWidget->blockSignals(false);

  // This is needed to enable/disable the Data Container and Attribute Matrix combo boxes
  updateSelectAllCheckbox();

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::updateSelectAllCheckbox()
{
  bool checkedStateExists = false;
  int checkedStateCount = 0;
  bool uncheckedStateExists = false;
  bool oneItemEnabled = false;

  for (int i = 0; i < attributeArraysSelectWidget->count(); i++)
  {
    if (attributeArraysSelectWidget->item(i)->flags().testFlag(Qt::ItemIsEnabled) == true)
    {
      oneItemEnabled = true;
      if (attributeArraysSelectWidget->item(i)->checkState() == Qt::Checked)
      {
        checkedStateExists = true;
        checkedStateCount++;
      }
      else
      {
        uncheckedStateExists = true;
      }
    }
  }

  if (oneItemEnabled == false)
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setDisabled(true);
    selectCheckBox->blockSignals(false);
  }
  else if (oneItemEnabled == true)
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setDisabled(false);
    selectCheckBox->blockSignals(false);
  }

  if (checkedStateExists == true && uncheckedStateExists == true)
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setCheckState(Qt::PartiallyChecked);
    selectCheckBox->blockSignals(false);
  }
  else if (checkedStateCount == attributeArraysSelectWidget->count())
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setCheckState(Qt::Checked);
    selectCheckBox->blockSignals(false);
  }
  else
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setCheckState(Qt::Unchecked);
    selectCheckBox->blockSignals(false);
  }
}







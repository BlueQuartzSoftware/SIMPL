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
#include "MultiAttributeMatrixSelectionWidget.h"

#include <QtCore/QList>
#include <QtCore/QMetaProperty>
#include <QtCore/QSignalMapper>
#include <QtCore/QModelIndex>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStyledItemDelegate>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/FilterParameters/MultiAttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetUtils.hpp"
#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiAttributeMatrixSelectionWidget::MultiAttributeMatrixSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<MultiAttributeMatrixSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "MultiAttributeMatrixSelectionWidget can ONLY be used with a MultiAttributeMatrixSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiAttributeMatrixSelectionWidget::MultiAttributeMatrixSelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiAttributeMatrixSelectionWidget::~MultiAttributeMatrixSelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  // qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " MultiAttributeMatrixSelectionWidget::initializeWidget";
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::setupGui()
{
  // Sanity Check the filter and the filter parameter
  if(getFilter() == nullptr)
  {
    return;
  }
  if(getFilterParameter() == nullptr)
  {
    return;
  }

  attributeMatricesOrderWidget->installEventFilter(this);
  attributeMatricesSelectWidget->installEventFilter(this);

  // Generate the text for the QLabel
  label->setText(getFilterParameter()->getHumanLabel());

  DataContainerSelectionFilterParameter::RequirementType reqs;
  m_SelectedDataContainerPath->setDataContainerRequirements(reqs);
  m_SelectedDataContainerPath->setFilter(getFilter());

  // Lastly, hook up the filter's signals and slots to our own signals and slots
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)),
    this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  connect(this, SIGNAL(filterPathInput(DataArrayPath)), m_SelectedDataContainerPath, SLOT(checkDragPath(DataArrayPath)));
  connect(this, SIGNAL(endViewPathRequirements()), m_SelectedDataContainerPath, SLOT(clearPathFiltering()));
  connect(this, SIGNAL(endDataStructureRequirements()), m_SelectedDataContainerPath, SLOT(endExternalFiltering()));
  connect(this, SIGNAL(applyPathToFilteringParameter(DataArrayPath)), m_SelectedDataContainerPath, SLOT(setFilteredDataArrayPath(DataArrayPath)));

  connect(m_SelectedDataContainerPath, SIGNAL(viewPathsMatchingReqs(DataContainerSelectionFilterParameter::RequirementType)), this, SIGNAL(viewPathsMatchingReqs(DataContainerSelectionFilterParameter::RequirementType)));
  connect(m_SelectedDataContainerPath, SIGNAL(endViewPaths()), this, SIGNAL(endViewPaths()));
  connect(m_SelectedDataContainerPath, SIGNAL(pathChanged()), this, SIGNAL(parametersChanged()));
  connect(m_SelectedDataContainerPath, SIGNAL(filterPath(DataArrayPath)), this, SIGNAL(filterPath(DataArrayPath)));

  connect(m_SelectedDataContainerPath, SIGNAL(dataArrayPathSelectionLocked(QToolButton*)), this, SIGNAL(dataArrayPathSelectionLocked(QToolButton*)));
  connect(this, SIGNAL(lockDataArrayPathSelection(QToolButton*)), m_SelectedDataContainerPath, SLOT(selectionWidgetLocked(QToolButton*)));
  connect(m_SelectedDataContainerPath, SIGNAL(dataArrayPathSelectionUnlocked(QToolButton*)), this, SIGNAL(dataArrayPathSelectionUnlocked(QToolButton*)));
  connect(this, SIGNAL(unlockDataArrayPathSelection(QToolButton*)), m_SelectedDataContainerPath, SLOT(selectionWidgetUnlocked(QToolButton*)));

  QVector<DataArrayPath> selectedPaths = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<QVector<DataArrayPath>>();
  DataArrayPath amPath = DataArrayPath::GetAttributeMatrixPath(selectedPaths);
  m_SelectedDataContainerPath->setText(amPath.getDataContainerName());
  m_SelectedDataContainerPath->setPropertyName(getFilterParameter()->getHumanLabel());
  for (int i=0; i<selectedPaths.size(); i++)
  {
    DataArrayPath selectedPath = selectedPaths[i];
    attributeMatricesOrderWidget->addItem(selectedPath.getAttributeMatrixName());
  }

  selectBtn->setDisabled(true);
  deselectBtn->setDisabled(true);
  removeBtn->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiAttributeMatrixSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  ////qDebug() << "    checkStringValues(...)" << curDcName << "  " << filtDcName;
  if(curDcName.isEmpty() == true && filtDcName.isEmpty() == false)
  {
    return filtDcName;
  }
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == true)
  {
    return curDcName;
  }
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == false && m_DidCausePreflight == true)
  {
    return curDcName;
  }

  return filtDcName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MultiAttributeMatrixSelectionWidget::eventFilter(QObject* obj, QEvent* event)
{
  if(event->type() == QEvent::Show && obj == m_SelectedDataContainerPath->menu())
  {
    QPoint pos = adjustedMenuPosition(m_SelectedDataContainerPath);
    m_SelectedDataContainerPath->menu()->move(pos);
    return true;
  }
  else if ( event->type() == QEvent::FocusIn && obj == attributeMatricesOrderWidget )
  {
    on_attributeMatricesOrderWidget_itemSelectionChanged();
  }
  else if ( event->type() == QEvent::FocusIn && obj == attributeMatricesSelectWidget )
  {
    on_attributeMatricesSelectWidget_itemSelectionChanged();
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::dataContainerSelected(QString path)
{
  setSelectedPath(path);

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::setSelectedPath(QString path)
{
  DataArrayPath dcPath = DataArrayPath::Deserialize(path, Detail::Delimiter);
  setSelectedPath(dcPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::setSelectedPath(DataArrayPath dcPath)
{
  if(dcPath.isEmpty())
  {
    return;
  }

  m_SelectedDataContainerPath->setDataArrayPath(dcPath);
  attributeMatricesSelectWidget->clear();
  attributeMatricesOrderWidget->clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::on_selectBtn_clicked()
{
  QModelIndexList indexList = attributeMatricesSelectWidget->selectionModel()->selectedRows();
  if (indexList.size() > 0)
  {
    int offset = 0;
    for (int i=0; i<indexList.size(); i++)
    {
      int row = indexList[i].row() - offset;
      QListWidgetItem* item = attributeMatricesSelectWidget->takeItem(row);
      offset++;
      if (item)
      {
        attributeMatricesOrderWidget->addItem(item);
      }
    }

    m_DidCausePreflight = true;
    emit parametersChanged();
    m_DidCausePreflight = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::on_deselectBtn_clicked()
{
  QModelIndexList indexList = attributeMatricesOrderWidget->selectionModel()->selectedRows();
  if (indexList.size() > 0)
  {
    int offset = 0;
    for (int i=0; i<indexList.size(); i++)
    {
      int row = indexList[i].row() - offset;
      QListWidgetItem* item = attributeMatricesOrderWidget->takeItem(row);
      offset++;
      if (item)
      {
        attributeMatricesSelectWidget->addItem(item);
      }
    }

    m_DidCausePreflight = true;
    emit parametersChanged();
    m_DidCausePreflight = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::on_upBtn_clicked()
{ 
  int currentIndex = attributeMatricesOrderWidget->currentRow();

  if(currentIndex > 0)
  {
    QListWidgetItem* item = attributeMatricesOrderWidget->takeItem(currentIndex);
    attributeMatricesOrderWidget->insertItem(currentIndex - 1, item);
    attributeMatricesOrderWidget->setCurrentRow(currentIndex - 1);

    m_DidCausePreflight = true;
    emit parametersChanged();
    m_DidCausePreflight = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::on_downBtn_clicked()
{
  int currentIndex = attributeMatricesOrderWidget->currentRow();

  if(currentIndex < attributeMatricesOrderWidget->count() - 1)
  {
    QListWidgetItem* item = attributeMatricesOrderWidget->takeItem(currentIndex);
    attributeMatricesOrderWidget->insertItem(currentIndex + 1, item);
    attributeMatricesOrderWidget->setCurrentRow(currentIndex + 1);

    m_DidCausePreflight = true;
    emit parametersChanged();
    m_DidCausePreflight = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::on_removeBtn_clicked()
{
  QModelIndexList indexList = attributeMatricesOrderWidget->selectionModel()->selectedRows();
  if (indexList.size() > 0)
  {
    int offset = 0;
    for (int i=0; i<indexList.size(); i++)
    {
      int row = indexList[i].row() - offset;
      QListWidgetItem* item = attributeMatricesOrderWidget->item(row);
      attributeMatricesOrderWidget->removeItemWidget(item);
      delete item;
      offset++;
    }

    m_DidCausePreflight = true;
    emit parametersChanged();
    m_DidCausePreflight = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::removeNonexistantPaths(QVector<DataArrayPath>& paths)
{
  AbstractFilter* filter = getFilter();
  if(nullptr == filter)
  {
    return;
  }

  bool reloadPath = false;
  DataArrayPath dcPath;
  if(paths.size() > 0)
  {
    dcPath = DataArrayPath(paths[0].getDataContainerName(), "", "");
  }

  for(int i = 0; i < paths.size(); i++)
  {
    bool valid = true;

    if(nullptr == filter->getDataContainerArray()->getAttributeMatrix(paths[i])->getAttributeArray(paths[i].getDataArrayName()))
      valid = false;

    if(false == paths[i].isValid())
      valid = false;

    if(!valid)
    {
      const QString& pathName = paths[i].getDataArrayName();
      QList<QListWidgetItem*> invalidDataArrayWidgets = attributeMatricesOrderWidget->findItems(pathName, Qt::MatchExactly);
      for(int j = 0; j < invalidDataArrayWidgets.size(); j++)
      {
        invalidDataArrayWidgets[j]->setCheckState(Qt::Unchecked);
        attributeMatricesOrderWidget->removeItemWidget(invalidDataArrayWidgets[j]);
      }

      paths.removeAt(i);
      i--;

      reloadPath = true;
    }
  }

  if(reloadPath && !dcPath.isEmpty())
  {
    setSelectedPath(dcPath.serialize(Detail::Delimiter));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::selectionChanged()
{
  upBtn->setDisabled(true);
  downBtn->setDisabled(true);
  selectBtn->setDisabled(true);
  deselectBtn->setDisabled(true);
  removeBtn->hide();

  int selectSize = attributeMatricesSelectWidget->selectionModel()->selectedRows().size();
  int orderSize = attributeMatricesOrderWidget->selectionModel()->selectedRows().size();

  if (selectSize > 0)
  {
    selectBtn->setEnabled(true);
  }

  if (orderSize > 0)
  {
    deselectBtn->setEnabled(true);

    if (orderSize == 1)
    {
      upBtn->setEnabled(true);
      downBtn->setEnabled(true);
    }

    bool allErrorRows = true;
    for (int i=0; i<orderSize; i++)
    {
      int row = attributeMatricesOrderWidget->selectionModel()->selectedRows()[i].row();
      if (attributeMatricesOrderWidget->item(row)->backgroundColor() != QColor(235, 110, 110))
      {
        allErrorRows = false;
      }
    }

    if (allErrorRows == true)
    {
      removeBtn->show();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::on_attributeMatricesSelectWidget_itemSelectionChanged()
{
  selectionChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::on_attributeMatricesOrderWidget_itemSelectionChanged()
{
  selectionChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }

  // Previously in afterPreflight()
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  if(dca->doesDataContainerExist(m_SelectedDataContainerPath->getDataArrayPath().getDataContainerName()))
  {
    DataContainer::Pointer dc = dca->getDataContainer(m_SelectedDataContainerPath->getDataArrayPath());
    if(nullptr != dc.get()) 
    {
      QList<QString> matrixNames = dc->getAttributeMatrixNames();

      QList<QString> selectListNames;
      for(int i = 0; i<attributeMatricesSelectWidget->count(); i++)
      {
        selectListNames.append(attributeMatricesSelectWidget->item(i)->text());
      }

      QList<QString> orderListNames;
      for(int i = 0; i<attributeMatricesOrderWidget->count(); i++)
      {
        QListWidgetItem* item = attributeMatricesOrderWidget->item(i);
        QString name = item->text();
        orderListNames.append(name);
        if(matrixNames.contains(name) == false)
        {
          //item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
          item->setBackgroundColor(QColor(235, 110, 110));
        }
        else
        {
          item->setBackgroundColor(QColor(255, 255, 255));
        }
      }

      for(int i = 0; i<matrixNames.size(); i++)
      {
        if(selectListNames.contains(matrixNames[i]) == false && orderListNames.contains(matrixNames[i]) == false)
        {
          attributeMatricesSelectWidget->addItem(matrixNames[i]);
        }
      }
    }
  }

  m_SelectedDataContainerPath->beforePreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::afterPreflight()
{
  m_SelectedDataContainerPath->afterPreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  DataArrayPath amPath = DataArrayPath::Deserialize(m_SelectedDataContainerPath->text(), Detail::Delimiter);

  QVector<DataArrayPath> selectedPaths;
  for(int i = 0; i < attributeMatricesOrderWidget->count(); i++)
  {
    DataArrayPath path = amPath;
    path.setAttributeMatrixName(attributeMatricesOrderWidget->item(i)->text());
    selectedPaths.push_back(path);
  }

  // Generate the path to the AttributeArray
  QVariant var;
  var.setValue(selectedPaths);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiAttributeMatrixSelectionWidget::updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  if(propertyName.compare(getFilterParameter()->getPropertyName()) == 0)
  {
    QVariant var = getFilter()->property(PROPERTY_NAME_AS_CHAR);
    DataArrayPath updatedPath = var.value<DataArrayPath>();
    QString dataArrayName = updatedPath.getDataArrayName();
    updatedPath.setDataArrayName("");

    blockSignals(true);
    DataArrayPath currentPath = DataArrayPath::Deserialize(m_SelectedDataContainerPath->text(), Detail::Delimiter);
    if(currentPath.getDataContainerName() == oldPath.getDataContainerName())
    {
      if(oldPath.getDataContainerName() != newPath.getDataContainerName())
      {
        DataArrayPath updatedPath(newPath.getDataContainerName(), "", "");
        m_SelectedDataContainerPath->setText(updatedPath.serialize(Detail::Delimiter));
      }
      else if(oldPath.getAttributeMatrixName() != newPath.getAttributeMatrixName())
      {
        // Unselected list widget
        {
          QList<QListWidgetItem*> renamedItems = attributeMatricesSelectWidget->findItems(oldPath.getAttributeMatrixName(), Qt::MatchFlag::MatchCaseSensitive);

          int renamedCount = renamedItems.size();
          for(int i = 0; i < renamedCount; i++)
          {
            QListWidgetItem* item = renamedItems[i];
            item->setText(newPath.getAttributeMatrixName());
          }
        }

        // Selected list widget
        {
          QList<QListWidgetItem*> renamedItems = attributeMatricesOrderWidget->findItems(oldPath.getAttributeMatrixName(), Qt::MatchFlag::MatchCaseSensitive);

          int renamedCount = renamedItems.size();
          for(int i = 0; i < renamedCount; i++)
          {
            QListWidgetItem* item = renamedItems[i];
            if(item->text() == oldPath.getAttributeMatrixName())
            {
              item->setText(newPath.getAttributeMatrixName());
            }
          }
        }
        // End AttributeMatrix section
      }
    }

    blockSignals(false);
  }
}

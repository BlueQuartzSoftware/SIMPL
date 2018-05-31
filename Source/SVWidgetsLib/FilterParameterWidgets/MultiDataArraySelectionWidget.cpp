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
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetUtils.hpp"
#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionWidget::MultiDataArraySelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<MultiDataArraySelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "MultiDataArraySelectionWidget can ONLY be used with a MultiDataArraySelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionWidget::MultiDataArraySelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionWidget::~MultiDataArraySelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
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
  if(getFilter() == nullptr)
  {
    return;
  }
  if(getFilterParameter() == nullptr)
  {
    return;
  }

  selectedArraysListWidget->installEventFilter(this);
  availableArraysListWidget->installEventFilter(this);

  // Generate the text for the QLabel
  label->setText(getFilterParameter()->getHumanLabel());

  AttributeMatrixSelectionFilterParameter::RequirementType reqs;
  m_SelectedAttributeMatrixPath->setAttrMatrixRequirements(reqs);
  m_SelectedAttributeMatrixPath->setFilter(getFilter());

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

  connect(this, SIGNAL(filterPathInput(DataArrayPath)), m_SelectedAttributeMatrixPath, SLOT(checkDragPath(DataArrayPath)));
  connect(this, SIGNAL(endViewPathRequirements()), m_SelectedAttributeMatrixPath, SLOT(clearPathFiltering()));
  connect(this, SIGNAL(endDataStructureRequirements()), m_SelectedAttributeMatrixPath, SLOT(endExternalFiltering()));
  connect(this, SIGNAL(applyPathToFilteringParameter(DataArrayPath)), m_SelectedAttributeMatrixPath, SLOT(setFilteredDataArrayPath(DataArrayPath)));

  connect(m_SelectedAttributeMatrixPath, SIGNAL(viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType)), this, SIGNAL(viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType)));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(endViewPaths()), this, SIGNAL(endViewPaths()));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(pathChanged()), this, SIGNAL(parametersChanged()));
  connect(m_SelectedAttributeMatrixPath, SIGNAL(filterPath(DataArrayPath)), this, SIGNAL(filterPath(DataArrayPath)));

  QVector<DataArrayPath> selectedPaths = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<QVector<DataArrayPath>>();
  DataArrayPath amPath = DataArrayPath::GetAttributeMatrixPath(selectedPaths);
  m_SelectedAttributeMatrixPath->setText(amPath.serialize(Detail::Delimiter));
  m_SelectedAttributeMatrixPath->setPropertyName(getFilterParameter()->getHumanLabel());
  for (int i=0; i<selectedPaths.size(); i++)
  {
    DataArrayPath selectedPath = selectedPaths[i];
    QListWidgetItem* item = new QListWidgetItem(QIcon(":/SIMPL/icons/images/bullet_ball_green.png"), selectedPath.getDataArrayName());
    selectedArraysListWidget->addItem(item);
  }
  selectBtn->setDisabled(true);
  deselectBtn->setDisabled(true);
  removeBtn->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiDataArraySelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
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
bool MultiDataArraySelectionWidget::eventFilter(QObject* obj, QEvent* event)
{
  if(event->type() == QEvent::Show && obj == m_SelectedAttributeMatrixPath->menu())
  {
    QPoint pos = adjustedMenuPosition(m_SelectedAttributeMatrixPath);
    m_SelectedAttributeMatrixPath->menu()->move(pos);
    return true;
  }
  else if(event->type() == QEvent::FocusIn && obj == selectedArraysListWidget)
  {
    on_selectedArraysListWidget_itemSelectionChanged();
  }
  else if(event->type() == QEvent::FocusIn && obj == availableArraysListWidget)
  {
    on_availableArraysListWidget_itemSelectionChanged();
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
  setSelectedPath(amPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::setSelectedPath(DataArrayPath amPath)
{
  if(amPath.isEmpty())
  {
    return;
  }

  m_SelectedAttributeMatrixPath->setDataArrayPath(amPath);
  availableArraysListWidget->clear();
  selectedArraysListWidget->clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_availableArraysListWidget_itemDoubleClicked(QListWidgetItem* item)
{
  on_selectBtn_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_selectedArraysListWidget_itemDoubleClicked(QListWidgetItem* item)
{
  on_deselectBtn_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_selectBtn_clicked()
{
  QModelIndexList indexList = availableArraysListWidget->selectionModel()->selectedRows();
  if (indexList.size() > 0)
  {
    int offset = 0;
    for (int i=0; i<indexList.size(); i++)
    {
      int row = indexList[i].row() - offset;
      QListWidgetItem* item = availableArraysListWidget->takeItem(row);
      offset++;
      if (item)
      {
        selectedArraysListWidget->addItem(item);
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
void MultiDataArraySelectionWidget::on_deselectBtn_clicked()
{
  // QModelIndexList indexList = selectedArraysListWidget->selectionModel()->selectedRows();
  QList<QListWidgetItem*> items = selectedArraysListWidget->selectedItems();
  foreach(QListWidgetItem* item, items)
  {
    int row = selectedArraysListWidget->row(item);
    selectedArraysListWidget->takeItem(row);
    availableArraysListWidget->addItem(item);
  }
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_upBtn_clicked()
{
  int currentIndex = selectedArraysListWidget->currentRow();

  if(currentIndex > 0)
  {
    QListWidgetItem* item = selectedArraysListWidget->takeItem(currentIndex);
    selectedArraysListWidget->insertItem(currentIndex - 1, item);
    selectedArraysListWidget->setCurrentRow(currentIndex - 1);

    m_DidCausePreflight = true;
    emit parametersChanged();
    m_DidCausePreflight = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_downBtn_clicked()
{
  int currentIndex = selectedArraysListWidget->currentRow();

  if(currentIndex < selectedArraysListWidget->count() - 1)
  {
    QListWidgetItem* item = selectedArraysListWidget->takeItem(currentIndex);
    selectedArraysListWidget->insertItem(currentIndex + 1, item);
    selectedArraysListWidget->setCurrentRow(currentIndex + 1);

    m_DidCausePreflight = true;
    emit parametersChanged();
    m_DidCausePreflight = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_removeBtn_clicked()
{
  QModelIndexList indexList = selectedArraysListWidget->selectionModel()->selectedRows();
  if (indexList.size() > 0)
  {
    int offset = 0;
    for (int i=0; i<indexList.size(); i++)
    {
      int row = indexList[i].row() - offset;
      QListWidgetItem* item = selectedArraysListWidget->item(row);
      selectedArraysListWidget->removeItemWidget(item);
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
void MultiDataArraySelectionWidget::removeNonexistantPaths(QVector<DataArrayPath>& paths)
{
  AbstractFilter* filter = getFilter();
  if(nullptr == filter)
  {
    return;
  }

  bool reloadPath = false;
  DataArrayPath amPath = DataArrayPath::GetAttributeMatrixPath(paths);

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
      QList<QListWidgetItem*> invalidDataArrayWidgets = selectedArraysListWidget->findItems(pathName, Qt::MatchExactly);
      for(int j = 0; j < invalidDataArrayWidgets.size(); j++)
      {
        invalidDataArrayWidgets[j]->setCheckState(Qt::Unchecked);
        selectedArraysListWidget->removeItemWidget(invalidDataArrayWidgets[j]);
      }

      paths.removeAt(i);
      i--;

      reloadPath = true;
    }
  }

  if(reloadPath && !amPath.isEmpty())
  {
    setSelectedPath(amPath.serialize(Detail::Delimiter));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::selectionChanged()
{
  upBtn->setDisabled(true);
  downBtn->setDisabled(true);
  selectBtn->setDisabled(true);
  deselectBtn->setDisabled(true);
  removeBtn->hide();

  int selectSize = availableArraysListWidget->selectionModel()->selectedRows().size();
  int orderSize = selectedArraysListWidget->selectionModel()->selectedRows().size();

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
      int row = selectedArraysListWidget->selectionModel()->selectedRows()[i].row();
      if(selectedArraysListWidget->item(row)->backgroundColor() != QColor(235, 110, 110))
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
void MultiDataArraySelectionWidget::on_availableArraysListWidget_itemSelectionChanged()
{
  selectionChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_selectedArraysListWidget_itemSelectionChanged()
{
  selectionChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }

  // Previously in afterPreflight()
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  if(dca->doesAttributeMatrixExist(m_SelectedAttributeMatrixPath->getDataArrayPath()))
  {
    AttributeMatrix::Pointer am = dca->getAttributeMatrix(m_SelectedAttributeMatrixPath->getDataArrayPath());
    if(nullptr != am.get()) 
    {
      QList<QString> arrayNames = am->getAttributeArrayNames();

      QList<QString> selectListNames;
      for(int i = 0; i < availableArraysListWidget->count(); i++)
      {
        selectListNames.append(availableArraysListWidget->item(i)->text());
      }

      QList<QString> orderListNames;
      for(int i = 0; i < selectedArraysListWidget->count(); i++)
      {
        QListWidgetItem* item = selectedArraysListWidget->item(i);
        QString name = item->text();
        orderListNames.append(name);
        if(arrayNames.contains(name) == false)
        {
          //item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
          // item->setBackgroundColor(QColor(235, 110, 110));
          item->setIcon(QIcon(":/SIMPL/icons/images/bullet_ball_red.png"));
        }
        else
        {
          // item->setBackgroundColor(QColor(255, 255, 255));
          item->setIcon(QIcon(":/SIMPL/icons/images/bullet_ball_green.png"));
        }
      }

      for(int i = 0; i<arrayNames.size(); i++)
      {
        if(selectListNames.contains(arrayNames[i]) == false && orderListNames.contains(arrayNames[i]) == false)
        {
          QListWidgetItem* item = new QListWidgetItem(QIcon(":/SIMPL/icons/images/bullet_ball_green.png"), arrayNames[i]);
          availableArraysListWidget->addItem(item);
        }
      }
    }
  }

  m_SelectedAttributeMatrixPath->beforePreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::afterPreflight()
{
  m_SelectedAttributeMatrixPath->afterPreflight();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  DataArrayPath amPath = m_SelectedAttributeMatrixPath->getDataArrayPath();

  QVector<DataArrayPath> selectedPaths;
  for(int i = 0; i < selectedArraysListWidget->count(); i++)
  {
    DataArrayPath path = amPath;
    path.setDataArrayName(selectedArraysListWidget->item(i)->text());
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
void MultiDataArraySelectionWidget::updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath)
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
    DataArrayPath currentPath = DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter);
    if(currentPath.hasSameDataContainer(oldPath))
    {
      bool hasAM = false == newPath.getAttributeMatrixName().isEmpty();
      bool hasDA = false == newPath.getDataArrayName().isEmpty();

      // Update the DataArray options
      if(hasDA && currentPath.hasSameAttributeMatrix(oldPath))
      {
        // Unselected list widget
        {
          QList<QListWidgetItem*> renamedItems = availableArraysListWidget->findItems(oldPath.getDataArrayName(), Qt::MatchFlag::MatchCaseSensitive);

          int renamedCount = renamedItems.size();
          for(int i = 0; i < renamedCount; i++)
          {
            QListWidgetItem* item = renamedItems[i];
            //availableArraysListWidget->removeItemWidget(item);
            item->setText(newPath.getDataArrayName());
          }
        }

        // Selected list widget
        {
          QList<QListWidgetItem*> renamedItems = selectedArraysListWidget->findItems(oldPath.getDataArrayName(), Qt::MatchFlag::MatchCaseSensitive);

          int renamedCount = renamedItems.size();
          for(int i = 0; i < renamedCount; i++)
          {
            QListWidgetItem* item = renamedItems[i];
            if(item->text() == oldPath.getDataArrayName())
            {
              item->setText(newPath.getDataArrayName());
            }
          }
        }
      }// End DataArray section

      // Update the AttributeMatrix Selection widget
      if(false == hasDA)
      {
        if(false == hasAM)
        {
          DataArrayPath updatedPath(newPath.getDataContainerName(), currentPath.getAttributeMatrixName(), "");
          m_SelectedAttributeMatrixPath->setText(updatedPath.serialize(Detail::Delimiter));
        }
        else
        {
          if(currentPath.hasSameAttributeMatrix(oldPath))
          {
            DataArrayPath updatedPath(newPath.getDataContainerName(), newPath.getAttributeMatrixName(), "");
            m_SelectedAttributeMatrixPath->setText(updatedPath.serialize(Detail::Delimiter));
          }
        }
      }
    }

    blockSignals(false);
  }
}

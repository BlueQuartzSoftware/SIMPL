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
#include "MultiDataContainerSelectionWidget.h"

#include <QtCore/QList>
#include <QtCore/QModelIndex>

#include <QtWidgets/QListWidgetItem>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataContainerSelectionFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetUtils.hpp"
#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataContainerSelectionWidget::MultiDataContainerSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<MultiDataContainerSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "MultiDataContainerSelectionWidget can ONLY be used with a MultiDataContainerSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataContainerSelectionWidget::MultiDataContainerSelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataContainerSelectionWidget::~MultiDataContainerSelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::setupGui()
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

  selectedDataContainersListWidget->installEventFilter(this);
  availableDataContainersListWidget->installEventFilter(this);

  // Generate the text for the QLabel
  // label->setText(getFilterParameter()->getHumanLabel());

  // Lastly, hook up the filter's signals and slots to our own signals and slots
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &MultiDataContainerSelectionWidget::beforePreflight);

  // Catch when the filter is finished running the preflight
  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &MultiDataContainerSelectionWidget::afterPreflight);

  // Catch when the filter wants its values updated
  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &MultiDataContainerSelectionWidget::filterNeedsInputParameters);

  // If the DataArrayPath is updated in the filter, update the widget
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)), this, SLOT(updateDataArrayPath(QString, DataArrayPath::RenameType)));

  QStringList dcNames = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<QStringList>();
  for(int i = 0; i < dcNames.size(); i++)
  {
    QString dcName = dcNames[i];
    QListWidgetItem* item = new QListWidgetItem(QIcon(":/SIMPL/icons/images/bullet_ball_green.png"), dcName);
    selectedDataContainersListWidget->addItem(item);
  }
  selectBtn->setDisabled(true);
  deselectBtn->setDisabled(true);
  removeBtn->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiDataContainerSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  if(curDcName.isEmpty() && !filtDcName.isEmpty())
  {
    return filtDcName;
  }
  if(!curDcName.isEmpty() && filtDcName.isEmpty())
  {
    return curDcName;
  }
  if(!curDcName.isEmpty() && !filtDcName.isEmpty() && m_DidCausePreflight)
  {
    return curDcName;
  }

  return filtDcName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MultiDataContainerSelectionWidget::eventFilter(QObject* obj, QEvent* event)
{
  if(event->type() == QEvent::FocusIn && obj == selectedDataContainersListWidget)
  {
    on_selectedDataContainersListWidget_itemSelectionChanged();
  }
  else if(event->type() == QEvent::FocusIn && obj == availableDataContainersListWidget)
  {
    on_availableDataContainersListWidget_itemSelectionChanged();
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::on_availableDataContainersListWidget_itemDoubleClicked(QListWidgetItem* item)
{
  on_selectBtn_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::on_selectedDataContainersListWidget_itemDoubleClicked(QListWidgetItem* item)
{
  on_deselectBtn_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::on_selectBtn_clicked()
{
  QModelIndexList indexList = availableDataContainersListWidget->selectionModel()->selectedRows();
  if(!indexList.empty())
  {
    int offset = 0;
    for(int i = 0; i < indexList.size(); i++)
    {
      int row = indexList[i].row() - offset;
      QListWidgetItem* item = availableDataContainersListWidget->takeItem(row);
      offset++;
      if(item != nullptr)
      {
        selectedDataContainersListWidget->addItem(item);
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
void MultiDataContainerSelectionWidget::on_deselectBtn_clicked()
{
  // QModelIndexList indexList = selectedDataContainersListWidget->selectionModel()->selectedRows();
  QList<QListWidgetItem*> items = selectedDataContainersListWidget->selectedItems();
  foreach(QListWidgetItem* item, items)
  {
    int row = selectedDataContainersListWidget->row(item);
    selectedDataContainersListWidget->takeItem(row);
    availableDataContainersListWidget->addItem(item);
  }
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::on_upBtn_clicked()
{
  int currentIndex = selectedDataContainersListWidget->currentRow();

  if(currentIndex > 0)
  {
    QListWidgetItem* item = selectedDataContainersListWidget->takeItem(currentIndex);
    selectedDataContainersListWidget->insertItem(currentIndex - 1, item);
    selectedDataContainersListWidget->setCurrentRow(currentIndex - 1);

    m_DidCausePreflight = true;
    emit parametersChanged();
    m_DidCausePreflight = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::on_downBtn_clicked()
{
  int currentIndex = selectedDataContainersListWidget->currentRow();

  if(currentIndex < selectedDataContainersListWidget->count() - 1)
  {
    QListWidgetItem* item = selectedDataContainersListWidget->takeItem(currentIndex);
    selectedDataContainersListWidget->insertItem(currentIndex + 1, item);
    selectedDataContainersListWidget->setCurrentRow(currentIndex + 1);

    m_DidCausePreflight = true;
    emit parametersChanged();
    m_DidCausePreflight = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::on_removeBtn_clicked()
{
  QModelIndexList indexList = selectedDataContainersListWidget->selectionModel()->selectedRows();
  if(!indexList.empty())
  {
    int offset = 0;
    for(int i = 0; i < indexList.size(); i++)
    {
      int row = indexList[i].row() - offset;
      QListWidgetItem* item = selectedDataContainersListWidget->item(row);
      selectedDataContainersListWidget->removeItemWidget(item);
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
void MultiDataContainerSelectionWidget::selectionChanged()
{
  upBtn->setDisabled(true);
  downBtn->setDisabled(true);
  selectBtn->setDisabled(true);
  deselectBtn->setDisabled(true);
  removeBtn->hide();

  int selectSize = availableDataContainersListWidget->selectionModel()->selectedRows().size();
  int orderSize = selectedDataContainersListWidget->selectionModel()->selectedRows().size();

  if(selectSize > 0)
  {
    selectBtn->setEnabled(true);
  }

  if(orderSize > 0)
  {
    deselectBtn->setEnabled(true);

    if(orderSize == 1)
    {
      upBtn->setEnabled(true);
      downBtn->setEnabled(true);
    }

    bool allErrorRows = true;
    for(int i = 0; i < orderSize; i++)
    {
      int row = selectedDataContainersListWidget->selectionModel()->selectedRows()[i].row();
      if(selectedDataContainersListWidget->item(row)->background() != QColor(235, 110, 110))
      {
        allErrorRows = false;
      }
    }

    if(allErrorRows)
    {
      removeBtn->show();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::on_availableDataContainersListWidget_itemSelectionChanged()
{
  selectionChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::on_selectedDataContainersListWidget_itemSelectionChanged()
{
  selectionChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::clearNonexistantDataContainers()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  QList<QString> dcNames = dca->getDataContainerNames();

  for(int i = 0; i < availableDataContainersListWidget->count(); i++)
  {
    if(!dcNames.contains(availableDataContainersListWidget->item(i)->text()))
    {
      QListWidgetItem* item = availableDataContainersListWidget->takeItem(i);
      delete item;
      i--;
    }
  }

  for(int i = 0; i < selectedDataContainersListWidget->count(); i++)
  {
    if(!dcNames.contains(selectedDataContainersListWidget->item(i)->text()))
    {
      QListWidgetItem* item = selectedDataContainersListWidget->takeItem(i);
      delete item;
      i--;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }

  clearNonexistantDataContainers();

  // Previously in afterPreflight()
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  QList<QString> dcNames = dca->getDataContainerNames();

  QList<QString> selectListNames;
  for(int i = 0; i < availableDataContainersListWidget->count(); i++)
  {
    selectListNames.append(availableDataContainersListWidget->item(i)->text());
  }

  QList<QString> orderListNames;
  for(int i = 0; i < selectedDataContainersListWidget->count(); i++)
  {
    QListWidgetItem* item = selectedDataContainersListWidget->item(i);
    QString name = item->text();
    orderListNames.append(name);
    if(!dcNames.contains(name))
    {
      // item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
      // item->setBackgroundColor(QColor(235, 110, 110));
      item->setIcon(QIcon(":/SIMPL/icons/images/bullet_ball_red.png"));
    }
    else
    {
      // item->setBackgroundColor(QColor(255, 255, 255));
      item->setIcon(QIcon(":/SIMPL/icons/images/bullet_ball_green.png"));
    }
  }

  for(int i = 0; i < dcNames.size(); i++)
  {
    if(!selectListNames.contains(dcNames[i]) && !orderListNames.contains(dcNames[i]))
    {
      QListWidgetItem* item = new QListWidgetItem(QIcon(":/SIMPL/icons/images/bullet_ball_green.png"), dcNames[i]);
      availableDataContainersListWidget->addItem(item);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::afterPreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QStringList dcNames;
  for(int i = 0; i < selectedDataContainersListWidget->count(); i++)
  {
    dcNames.push_back(selectedDataContainersListWidget->item(i)->text());
  }
  Q_UNUSED(filter)
  MultiDataContainerSelectionFilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
  if(setter)
  {
    setter(dcNames);
  }
  else
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;
}

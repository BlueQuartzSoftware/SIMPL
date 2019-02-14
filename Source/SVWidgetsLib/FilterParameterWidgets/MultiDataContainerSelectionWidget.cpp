#include <utility>

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
#include <QtCore/QMetaProperty>
#include <QtCore/QModelIndex>
#include <QtCore/QSignalMapper>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStyledItemDelegate>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataContainerSelectionFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetUtils.hpp"
#include "FilterParameterWidgetsDialogs.h"

MultiDataContainerSelectionWidget::MultiDataContainerSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<MultiDataContainerSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "MultiDataContainerSelectionWidget can ONLY be used with a MultiDataContainerFilterParameter object");

  setupUi(this);
  setupGui();
}

MultiDataContainerSelectionWidget::~MultiDataContainerSelectionWidget() = default;

void MultiDataContainerSelectionWidget::setupGui()
{
  // Sanity Check the filter and the filter parameter
  if(getFilter() == nullptr) // Trivial
  {
    return;
  }
  if(getFilterParameter() == nullptr) // Trivial
  {
    return;
  }

  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));
  connect(getFilter(), SIGNAL(dataArrayPathUpdated(QString, DataArrayPath::RenameType)), this, SLOT(updateDataContainerName(QString, DataArrayPath::RenameType)));

  dataContainersSelectWidget->addItems(getFilterParameter()->getDefaultValue().toStringList());
}

// -----------------------------------------------------------------------------
// Slot callbacks
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Validate containers in each list are in the filter data container array
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }

  if(nullptr == getFilter()->getDataContainerArray().get())
  {
    return;
  }

  QStringList dcNames{getFilter()->getDataContainerArray()->getDataContainerNames()};
  for(const auto& eachDC : getFilter()->getDataContainerArray()->getDataContainerNames())
  {
    dcNames.push_back(eachDC);
  }

  validateDataContainerNames(dataContainersSelectWidget, dcNames);
  validateDataContainerNames(dataContainersOrderWidget, dcNames);
}

// -----------------------------------------------------------------------------
// Populate and syncronize the lists
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::afterPreflight()
{
  QStringList dcNames{getFilter()->getDataContainerArray()->getDataContainerNames()};
  QStringList newItems{dcNames};

  syncItems(dataContainersSelectWidget, dcNames, newItems);
  syncItems(dataContainersOrderWidget, dcNames, newItems);

  dataContainersSelectWidget->addItems(newItems);
}

// -----------------------------------------------------------------------------
// Set the filter's active data containers or notify that parameter is missing
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QStringList selectedDCs{nullptr};
  for(int itemIndex = 0; itemIndex < dataContainersOrderWidget->count(); itemIndex++)
  {
    selectedDCs.push_back(dataContainersOrderWidget->item(itemIndex)->text());
  }
  QVariant var{0};
  var.setValue(selectedDCs);
  if(!filter->setProperty(PROPERTY_NAME_AS_CHAR, var))
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
// Update the name of a data container in the appropriate list widget
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::updateDataContainerName(const QString& propertyName, const DataArrayPath::RenameType& renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  if(propertyName.compare(getFilterParameter()->getPropertyName()) == 0)
  {
    if(oldPath.getDataContainerName() != newPath.getDataContainerName())
    {
      blockSignals(true);
      for(const auto& eachOrderDC : dataContainersOrderWidget->findItems(oldPath.getDataContainerName(), Qt::MatchFlag::MatchCaseSensitive))
      {
        eachOrderDC->setText(newPath.getDataContainerName());
      }
      for(const auto& eachOrderDC : dataContainersSelectWidget->findItems(oldPath.getDataContainerName(), Qt::MatchFlag::MatchCaseSensitive))
      {
        eachOrderDC->setText(newPath.getDataContainerName());
      }
      blockSignals(false);
    }
  }
}

// -----------------------------------------------------------------------------
// Helper Methods
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Sync list widgets with updated data container array
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::syncItems(QListWidget* listWidget, const QStringList& list, QStringList& newItems)
{
  for(const auto& eachItem : listWidget->findItems("*", Qt::MatchFlag::MatchWildcard))
  {
    QString itemText = eachItem->text();
    if(list.contains(itemText))
    {
      newItems.removeOne(itemText);
      continue;
    }
    delete listWidget->takeItem(listWidget->row(eachItem));
  }
}

// -----------------------------------------------------------------------------
// Validate ListWidget item exists in data container array
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionWidget::validateDataContainerNames(const SVListWidget* list, const QList<QString>& compareList)
{
  for(int i = 0; i < list->count(); i++)
  {
    QListWidgetItem* item = list->item(i);
    if(!compareList.contains(item->text()))
    {
      item->setBackgroundColor(QColor(235, 110, 110));
    }
    else
    {
      item->setBackgroundColor(QColor(255, 255, 255));
    }
  }
}

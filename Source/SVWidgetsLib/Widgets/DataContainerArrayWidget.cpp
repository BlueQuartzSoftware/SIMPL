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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "DataContainerArrayWidget.h"

#include <QtCore/QDebug>
#include <QtCore/QTextStream>

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayWidget::DataContainerArrayWidget(QWidget* parent)
: QWidget(parent)
, m_Filter(nullptr)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayWidget::DataContainerArrayWidget(AbstractFilter* filter, QWidget* parent)
: QWidget(parent)
, m_Filter(filter)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayWidget::~DataContainerArrayWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::initializeWithFilter(AbstractFilter* filter)
{
  m_Filter = filter;
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::clearStandardItemModel()
{
  // Put in a QStandardItemModel
  // qDebug() << "clearStandardItemModel()";
  QAbstractItemModel* oldModel = dcaProxyView->model();
  QStandardItemModel* model = new QStandardItemModel;
  dcaProxyView->setModel(model);
  model->setParent(dcaProxyView); // Set the parent so it gets cleaned up
  delete oldModel;                // Clean up the old model now.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::setupGui()
{
  dataContainerInfoLabel->setText("");
  clearStandardItemModel();

  if(nullptr != m_Filter)
  {
    connect(m_Filter, &AbstractFilter::preflightExecuted, this, &DataContainerArrayWidget::updateView);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::updateView()
{

  // Sanity check our filter object
  if(nullptr == m_Filter)
  {
    return;
  }

  // Get an check out DataContainerArray object
  DataContainerArray::Pointer dca = m_Filter->getDataContainerArray();
  if(dca.get() == nullptr)
  {
    return;
  }

  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
  QVector<QString> path;
  {
    QModelIndex currIndex = dcaProxyView->currentIndex();
    QStandardItem* item = model->itemFromIndex(currIndex);
    // Get what is selected and save it

    while(nullptr != item)
    {
      path.push_front(item->text());
      item = item->parent();
    }
  }
  // Clear out the view first
  clearStandardItemModel();
  model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());

  QItemSelectionModel* selectionModel = dcaProxyView->selectionModel();
  if(selectionModel != nullptr)
  {
    connect(selectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(dcaProxyView_indexChanged(const QModelIndex&, const QModelIndex&)));
  }

  // Sanity check model
  if(model == nullptr)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }

  QStandardItem* rootItem = model->invisibleRootItem();

  // Loop over the data containers until we find the proper data container
  DataContainerArray::Container containers = dca->getDataContainers();

  // QStringList dcList;
  int row0 = 0;
  for(DataContainer::Pointer dc : containers)
  {
    QStandardItem* dcItem = new QStandardItem(dc->getName());
    rootItem->appendRow(dcItem);

    dcItem->setData(dc->getInfoString(SIMPL::HtmlFormat), Qt::UserRole + 1);

    if(!path.empty() && dc->getName().compare(path[0]) == 0)
    {
      dcaProxyView->setCurrentIndex(model->indexFromItem(dcItem));
    }

    // We found the proper Data Container, now populate the AttributeMatrix List
    int row1 = 0;
    for(const auto& am : dc->getChildren())
    {
      QString amName = am->getName();

      QStandardItem* amItem = new QStandardItem(am->getName());
      dcItem->appendRow(amItem);
      amItem->setData(am->getInfoString(SIMPL::HtmlFormat), Qt::UserRole + 1);

      if(path.size() > 1 && am->getName().compare(path[1]) == 0)
      {
        dcaProxyView->setCurrentIndex(model->indexFromItem(amItem));
      }

      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QList<QString> attrArrayNames = am->getAttributeArrayNames();
      QListIterator<QString> dataArraysIter(attrArrayNames);
      int row2 = 0;
      while(dataArraysIter.hasNext())
      {
        QString attrArrayName = dataArraysIter.next();
        IDataArray::Pointer attrArray = am->getAttributeArray(attrArrayName);

        QStandardItem* aaItem = new QStandardItem(attrArrayName);
        amItem->appendRow(aaItem);
        aaItem->setData(attrArray->getInfoString(SIMPL::HtmlFormat), Qt::UserRole + 1);

        if(path.size() > 2 && attrArrayName.compare(path[2]) == 0)
        {
          QModelIndex idx = model->indexFromItem(aaItem);
          dcaProxyView->setCurrentIndex(idx);
        }
        row2++;
      }
      row1++;
    }
    row0++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::preflightCompleted()
{
  //  if(m_Filter)
  //  {
  //  qDebug() << "Filter: " << m_Filter->getHumanLabel() << " Filter DataContaiers: ";
  //  DataContainerArray::Pointer dcArray = m_Filter->getDataContainerArray();

  //  QList<DataContainer::Pointer> containers = dcArray->getDataContainers();

  //  for(int i = 0; i < containers.count(); i++)
  //  {
  //    DataContainer::Pointer container = containers.at(i);
  //    QString msg;
  //    QTextStream ss(&msg);
  //    ss << container->getName() << " <b>Geometry</b><br/>" << container->getGeometry()->getGeometryTypeAsString() << "\n";
  //    m_DataContainerInfos.insert(container->getName(), msg);
  //  }

  //  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::dcaProxyView_indexChanged(const QModelIndex& current, const QModelIndex& previous)
{
  QMap<int, QVariant> values = dcaProxyView->model()->itemData(current);
  QString infoString = values.value(Qt::UserRole + 1).toString();
  dataContainerInfoLabel->setText(infoString);
  //  QLabel* label = new QLabel("Foo");
  //  dcaProxyView->setPreviewWidget(label);
  // label->show();
  // dcaProxyView->update();
}

void DataContainerArrayWidget::on_dcaProxyView_updatePreviewWidget(const QModelIndex& index)
{
  //  QMap<int, QVariant> values = dcaProxyView->model()->itemData(index);
  //  QString dcName = values.value(Qt::DisplayRole).toString();
  //  qDebug() << "Update PreviewWidget for " << dcName;
  //  QLabel* label = new QLabel(dcName);
  //  dcaProxyView->setPreviewWidget(label);
}

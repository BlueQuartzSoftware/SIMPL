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

#include "DataBrowserWidget.h"


#include <QtCore/QMetaProperty>
#include <QtCore/QItemSelectionModel>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QHeaderView>


#include "SIMPLib/Common/AbstractFilter.h"

#include "SVWidgetsLib/QtSupport/QtSSettings.h"
#include "SVWidgetsLib/Widgets/PipelineFilterObject.h"


// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_DataBrowserWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataBrowserWidget::DataBrowserWidget(QWidget* parent)
: QWidget(parent)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataBrowserWidget::~DataBrowserWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBrowserWidget::setupGui()
{
  QStandardItemModel* model = new QStandardItemModel();
  dataBrowserTreeView->setModel(model);
  model->setColumnCount(1);
  model->setParent(dataBrowserTreeView); // Set the parent so it gets cleaned up
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void DataBrowserWidget::clearStandardItemModel()
//{
//  QAbstractItemModel* oldModel = dataBrowserTreeView->model();
//  QStandardItemModel* model = new QStandardItemModel;
//  model->setColumnCount(1);
//  dataBrowserTreeView->setModel(model);
//  model->setParent(dataBrowserTreeView); // Set the parent so it gets cleaned up
//  delete oldModel;                // Clean up the old model now.
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBrowserWidget::filterObjectActivated(PipelineFilterObject* object)
{
  //qDebug() << "DataBrowser: filterObjectActivated";

  AbstractFilter::Pointer filter = object->getFilter();

  // Sanity check our filter object
  if(nullptr == filter)
  {
    return;
  }

  // Get the DataContainerArray object
  DataContainerArray::Pointer dca = filter->getDataContainerArray();
  if(dca.get() == nullptr)
  {
    return;
  }

  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dataBrowserTreeView->model());
  QVector<QString> path;
  {
    QModelIndex currIndex = dataBrowserTreeView->currentIndex();
    QStandardItem* item = model->itemFromIndex(currIndex);
    // Get what is selected and save it
    while(nullptr != item)
    {
      path.push_front(item->text());
      item = item->parent();
    }

  }

  model = qobject_cast<QStandardItemModel*>(dataBrowserTreeView->model());

  QItemSelectionModel* selectionModel = dataBrowserTreeView->selectionModel();
  if(selectionModel)
  {
    connect(selectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(dataBrowserTreeView_indexChanged(const QModelIndex&, const QModelIndex&)));
  }

  // Sanity check model
  if(!model)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }

  // Loop over the data containers
  QList<DataContainer::Pointer> containers = dca->getDataContainers();


  QStandardItem* rootItem = model->invisibleRootItem();

  QListIterator<DataContainer::Pointer> containerIter(containers);


  while(containerIter.hasNext())
  {
    DataContainer::Pointer dc = containerIter.next();

    QStandardItem* dcItem = findChildByName(rootItem, dc->getName(), 0);
    if(!dcItem)
    {
      dcItem = new QStandardItem(dc->getName());
      dcItem->setData(dc->getInfoString(SIMPL::HtmlFormat), Qt::UserRole + 1);
      dcItem->setBackground(QBrush(QColor(154, 136, 255)));
      dcItem->setToolTip(dc->getInfoString(SIMPL::HtmlFormat));
      rootItem->appendRow(dcItem);
    }

    if(path.size() > 0 && dc->getName().compare(path[0]) == 0)
    {
      dataBrowserTreeView->setCurrentIndex(model->indexFromItem(dcItem));
    }

    // We found the proper Data Container, now populate the AttributeMatrix List
    DataContainer::AttributeMatrixMap_t attrMats = dc->getAttributeMatrices();
    QMapIterator<QString, AttributeMatrix::Pointer> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext())
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      AttributeMatrix::Pointer am = attrMatsIter.value();

      QStandardItem* amItem = findChildByName(dcItem, am->getName(), 0);
      if(!amItem)
      {
          amItem = new QStandardItem(am->getName());
          amItem->setBackground(QColor(128, 224, 138));
          amItem->setData(am->getInfoString(SIMPL::HtmlFormat), Qt::UserRole + 1);
          amItem->setToolTip(am->getInfoString(SIMPL::HtmlFormat));
          dcItem->appendRow(amItem);
      }

      if(path.size() > 1 && am->getName().compare(path[1]) == 0)
      {
        dataBrowserTreeView->setCurrentIndex(model->indexFromItem(amItem));
      }

      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QList<QString> attrArrayNames = am->getAttributeArrayNames();
      QListIterator<QString> dataArraysIter(attrArrayNames);
      while(dataArraysIter.hasNext())
      {
        QString attrArrayName = dataArraysIter.next();
        IDataArray::Pointer attrArray = am->getAttributeArray(attrArrayName);

        QStandardItem* aaItem = findChildByName(amItem, attrArray->getName(), 0);
        if(!aaItem)
        {
          aaItem = new QStandardItem(attrArrayName);
          aaItem->setData(attrArray->getInfoString(SIMPL::HtmlFormat), Qt::UserRole + 1);
          aaItem->setBackground(QColor(255, 210, 173));
          aaItem->setToolTip(attrArray->getInfoString(SIMPL::HtmlFormat));
          amItem->appendRow(aaItem);
        }
        if(path.size() > 2 && attrArrayName.compare(path[2]) == 0)
        {
          QModelIndex idx = model->indexFromItem(aaItem);
          dataBrowserTreeView->setCurrentIndex(idx);
        }
      }
      removeNonexistingEntries(amItem, attrArrayNames, 0);
    }
    removeNonexistingEntries(dcItem, dc->getAttributeMatrixNames(), 0);
  }
  removeNonexistingEntries(rootItem, dca->getDataContainerNames(), 0);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBrowserWidget::dataBrowserTreeView_indexChanged(const QModelIndex& current, const QModelIndex& previous)
{
//  QMap<int, QVariant> values = dataBrowserTreeView->model()->itemData(current);
//  QString infoString = values.value(Qt::UserRole + 1).toString();
//  dataContainerInfoLabel->setText(infoString);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBrowserWidget::handleFilterParameterChanged(PipelineFilterObject* obj)
{
  filterObjectActivated(obj);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStandardItem* DataBrowserWidget::findChildByName(QStandardItem* rootItem, const QString &name, int column)
{
  QStandardItem* item = nullptr;
  int rowCount = rootItem->rowCount();
  for (int row = 0; row < rowCount; ++row)
  {
    QStandardItem* anItem = rootItem->child(row, column);
    if(anItem->text().compare(name) == 0)
    {
      item = anItem;
      break;
    }
  }
  return item;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBrowserWidget::removeNonexistingEntries(QStandardItem* rootItem, QList<QString> existing, int column)
{
  int rowCount = rootItem->rowCount();
  for (int row = rowCount-1; row >= 0; row--)
  {
    QStandardItem* anItem = rootItem->child(row, column);
    if(!existing.contains(anItem->text()))
    {
      rootItem->removeRow(row);
    }
  }
}

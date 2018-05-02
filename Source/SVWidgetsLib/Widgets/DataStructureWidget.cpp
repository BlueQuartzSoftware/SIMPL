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

#include "DataStructureWidget.h"


#include <QtCore/QMetaProperty>
#include <QtCore/QItemSelectionModel>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QHeaderView>

#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/QtSupport/QtSSettings.h"
#include "SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h"
#include "SVWidgetsLib/Widgets/PipelineFilterObject.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureWidget::DataStructureWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::DataStructureWidget)

{
  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureWidget::~DataStructureWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setupGui()
{
  connect(m_Ui->dataBrowserTreeView, SIGNAL(filterPath(DataArrayPath)), this, SIGNAL(filterPath(DataArrayPath)));
  connect(m_Ui->dataBrowserTreeView, SIGNAL(endPathFiltering()), this, SIGNAL(endPathFiltering()));

  QStandardItemModel* model = new QStandardItemModel();
  m_Ui->dataBrowserTreeView->setModel(model);
  model->setColumnCount(1);
  model->setParent(m_Ui->dataBrowserTreeView); // Set the parent so it gets cleaned up

  m_DcColor = QColor(DataArrayPathSelectionWidget::GetActiveColor(DataArrayPath::DataType::DataContainer));
  m_AmColor = QColor(DataArrayPathSelectionWidget::GetActiveColor(DataArrayPath::DataType::AttributeMatrix));
  m_DaColor = QColor(DataArrayPathSelectionWidget::GetActiveColor(DataArrayPath::DataType::DataArray));

  // Forground brushes
  QColor filterColor(255, 255, 255);
  QColor invalidColor(100, 100, 100);
  m_CompliantBrush.setColor(filterColor);
  m_NoncompliantBrush.setColor(invalidColor);

  // Background brushes
  m_CompliantBgBrush.setColor(filterColor);
  m_CompliantBgBrush.setStyle(Qt::BrushStyle::SolidPattern);
  m_NoncompliantBgBrush.setColor(QColor(255, 255, 255, 0));
  m_NoncompliantBgBrush.setStyle(Qt::BrushStyle::SolidPattern);

  QString css(" QToolTip {\
              border: 2px solid #434343;\
              padding: 2px;\
              border-radius: 3px;\
              opacity: 255;\
              background-color: #FFFCEA;\
              color: #000000;\
              }");
  this->setStyleSheet(css);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::updateDataContainerArray(DataContainerArray::Pointer dca)
{
  m_Dca = dca->deepCopy(true);
  refreshData();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::refreshData()
{
  // Get the DataContainerArray object
  if(m_Dca.get() == nullptr)
  {
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_Ui->dataBrowserTreeView->model());
    QStandardItem* rootItem = model->invisibleRootItem();
    removeNonexistingEntries(rootItem, QStringList(), 0);
    return;
  }

  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_Ui->dataBrowserTreeView->model());
  QVector<QString> path;
  {
    QModelIndex currIndex = m_Ui->dataBrowserTreeView->currentIndex();
    QStandardItem* item = model->itemFromIndex(currIndex);
    // Get what is selected and save it
    while(nullptr != item)
    {
      path.push_front(item->text());
      item = item->parent();
    }

  }

  model = qobject_cast<QStandardItemModel*>(m_Ui->dataBrowserTreeView->model());

  //  QItemSelectionModel* selectionModel = m_Ui->dataBrowserTreeView->selectionModel();
  //  if(selectionModel)
  //  {
  //    connect(selectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
  //      this, SLOT(dataBrowserTreeView_indexChanged(const QModelIndex&, const QModelIndex&)));
  //  }

  // Sanity check model
  if(!model)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }

  // Loop over the data containers
  QList<DataContainer::Pointer> containers = m_Dca->getDataContainers();
  QStandardItem* rootItem = model->invisibleRootItem();
  QListIterator<DataContainer::Pointer> containerIter(containers);

  while(containerIter.hasNext())
  {
    DataContainer::Pointer dc = containerIter.next();

    QStandardItem* dcItem = findChildByName(rootItem, dc->getName(), 0);
    if(!dcItem)
    {
      dcItem = new QStandardItem(dc->getName());
      rootItem->appendRow(dcItem);
      //      dcItem->setBackground(QBrush(QColor(154, 136, 255)));
      m_Ui->dataBrowserTreeView->expand(dcItem->index());
    }
    dcItem->setData(dc->getInfoString(SIMPL::HtmlFormat), Qt::UserRole + 1);
    dcItem->setToolTip(dc->getInfoString(SIMPL::HtmlFormat));

    if(path.size() > 0 && dc->getName().compare(path[0]) == 0)
    {
      m_Ui->dataBrowserTreeView->setCurrentIndex(model->indexFromItem(dcItem));
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
        //          amItem->setBackground(QColor(128, 224, 138));
        dcItem->appendRow(amItem);
        m_Ui->dataBrowserTreeView->expand(amItem->index());
      }
      amItem->setData(am->getInfoString(SIMPL::HtmlFormat), Qt::UserRole + 1);
      amItem->setToolTip(am->getInfoString(SIMPL::HtmlFormat));

      if(path.size() > 1 && am->getName().compare(path[1]) == 0)
      {
        m_Ui->dataBrowserTreeView->setCurrentIndex(model->indexFromItem(amItem));
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
          // aaItem->setBackground(QColor(255, 210, 173));
          amItem->appendRow(aaItem);
        }
        //        else
        //        {
        //          aaItem->setBackground(QColor(255, 255, 255));
        //        }
        aaItem->setData(attrArray->getInfoString(SIMPL::HtmlFormat), Qt::UserRole + 1);
        aaItem->setToolTip(attrArray->getInfoString(SIMPL::HtmlFormat));

        if(path.size() > 2 && attrArrayName.compare(path[2]) == 0)
        {
          QModelIndex idx = model->indexFromItem(aaItem);
          m_Ui->dataBrowserTreeView->setCurrentIndex(idx);
        }
      }
      removeNonexistingEntries(amItem, attrArrayNames, 0);
    }
    removeNonexistingEntries(dcItem, dc->getAttributeMatrixNames(), 0);
  }
  removeNonexistingEntries(rootItem, m_Dca->getDataContainerNames(), 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::markNewItems()
{
  for(DataArrayPath path : m_CreatedPaths)
  {
    QStandardItem* newItem = findItemByPath(path);
    if(newItem)
    {
      switch(path.getDataType())
      {
      case DataArrayPath::DataType::DataContainer:
        m_NewItemBrush.setColor(m_DcColor);
        break;
      case DataArrayPath::DataType::AttributeMatrix:
        m_NewItemBrush.setColor(m_AmColor);
        break;
      case DataArrayPath::DataType::DataArray:
        m_NewItemBrush.setColor(m_DaColor);
        break;
      }
      newItem->setForeground(m_NewItemBrush);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::filterObjectActivated(PipelineFilterObject* object)
{
  m_Dca = DataContainerArray::NullPointer();
  m_CreatedPaths.clear();
  if(object)
  {
    AbstractFilter::Pointer filter = object->getFilter();
    if(filter.get())
    {
      DataContainerArray::Pointer dca = filter->getDataContainerArray();
      if(dca.get())
      {
        m_Dca = dca->deepCopy(true);
        m_CreatedPaths = filter->getCreatedPaths();
      }
    }
  }
  refreshData();
  update();
  markNewItems();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::handleFilterRemoved(PipelineFilterObject* object)
{
  Q_UNUSED(object);
  m_Dca = DataContainerArray::NullPointer();
  refreshData();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::handleFilterParameterChanged(PipelineFilterObject* object)
{
  filterObjectActivated(object);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStandardItem* DataStructureWidget::findChildByName(QStandardItem* rootItem, const QString &name, int column)
{
  if(nullptr == rootItem)
  {
    return nullptr;
  }

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
QStandardItem* DataStructureWidget::findItemByPath(DataArrayPath path)
{
  DataArrayPath::DataType dataType = path.getDataType();
  if(dataType == DataArrayPath::DataType::None)
  {
    return nullptr;
  }

  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_Ui->dataBrowserTreeView->model());
  if(!model)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return nullptr;
  }

  QStandardItem* rootItem = model->invisibleRootItem();
  QStandardItem* targetItem = nullptr;
  QStandardItem* dcItem = findChildByName(rootItem, path.getDataContainerName(), 0);
  if(dataType == DataArrayPath::DataType::DataContainer)
  {
    targetItem = dcItem;
  }
  else
  {
    QStandardItem* amItem = findChildByName(dcItem, path.getAttributeMatrixName(), 0);
    if(dataType == DataArrayPath::DataType::AttributeMatrix)
    {
      targetItem = amItem;
    }
    else
    {
      targetItem = findChildByName(amItem, path.getDataArrayName(), 0);
    }
  }

  return targetItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::removeNonexistingEntries(QStandardItem* rootItem, QList<QString> existing, int column)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setViewReqs(DataContainerSelectionFilterParameter::RequirementType dcReqs)
{
  // Filter DataArrayPaths
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_Ui->dataBrowserTreeView->model());
  QStandardItem* rootItem = model->invisibleRootItem();

  m_CompliantBgBrush.setColor(m_DcColor);

  int dcCount = rootItem->rowCount();
  for(int i = 0; i < dcCount; i++)
  {
    QStandardItem* dcItem = rootItem->child(i);

    DataArrayPath path(dcItem->text(), "", "");
    DataContainer::Pointer dc = m_Dca->getDataContainer(path);
    IGeometry::Pointer geom = dc->getGeometry();

    bool geomType = dcReqs.dcGeometryTypes.size() == 0 || dcReqs.dcGeometryTypes.contains(geom->getGeometryType());

    if(geomType)
    {
      dcItem->setForeground(m_CompliantBrush);
      dcItem->setBackground(m_CompliantBgBrush);
    }
    else
    {
      dcItem->setForeground(m_NoncompliantBrush);
      dcItem->setForeground(m_NoncompliantBgBrush);
    }

    int amCount = dcItem->rowCount();
    for(int j = 0; j < amCount; j++)
    {
      QStandardItem* amItem = dcItem->child(j);
      amItem->setForeground(m_NoncompliantBrush);
      amItem->setBackground(m_NoncompliantBgBrush);
      
      int daCount = amItem->rowCount();
      for(int k = 0; k < daCount; k++)
      {
        QStandardItem* daItem = amItem->child(k);
        daItem->setForeground(m_NoncompliantBrush);
        daItem->setBackground(m_NoncompliantBgBrush);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setViewReqs(AttributeMatrixSelectionFilterParameter::RequirementType amReqs)
{
  // Filter DataArrayPaths
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_Ui->dataBrowserTreeView->model());
  QStandardItem* rootItem = model->invisibleRootItem();

  m_CompliantBgBrush.setColor(m_AmColor);

  int dcCount = rootItem->rowCount();
  for(int i = 0; i < dcCount; i++)
  {
    QStandardItem* dcItem = rootItem->child(i);
    dcItem->setForeground(m_NoncompliantBrush);
    dcItem->setBackground(m_NoncompliantBgBrush);

    int amCount = dcItem->rowCount();
    for(int j = 0; j < amCount; j++)
    {
      QStandardItem* amItem = dcItem->child(j);

      DataArrayPath path(dcItem->text(), amItem->text(), "");
      DataContainer::Pointer dc = m_Dca->getDataContainer(path);
      IGeometry::Pointer geom = dc->getGeometry();
      AttributeMatrix::Pointer am = m_Dca->getAttributeMatrix(path);

      bool amType = amReqs.amTypes.size() == 0 || amReqs.amTypes.contains(am->getType());
      bool geomType = amReqs.dcGeometryTypes.size() == 0 || amReqs.dcGeometryTypes.contains(geom->getGeometryType());

      if(amType && geomType)
      {
        amItem->setForeground(m_CompliantBrush);
        amItem->setBackground(m_CompliantBgBrush);
      }
      else
      {
        amItem->setForeground(m_NoncompliantBrush);
        amItem->setBackground(m_NoncompliantBgBrush);
      }

      int daCount = amItem->rowCount();
      for(int k = 0; k < daCount; k++)
      {
        QStandardItem* daItem = amItem->child(k);
        daItem->setForeground(m_NoncompliantBrush);
        daItem->setBackground(m_NoncompliantBgBrush);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setViewReqs(DataArraySelectionFilterParameter::RequirementType daReqs)
{
  // Filter DataArrayPaths
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_Ui->dataBrowserTreeView->model());
  QStandardItem* rootItem = model->invisibleRootItem();

  m_CompliantBgBrush.setColor(m_DaColor);

  int dcCount = rootItem->rowCount();
  for(int i = 0; i < dcCount; i++)
  {
    QStandardItem* dcItem = rootItem->child(i);
    dcItem->setForeground(m_NoncompliantBrush);
    dcItem->setBackground(m_NoncompliantBgBrush);

    int amCount = dcItem->rowCount();
    for(int j = 0; j < amCount; j++)
    {
      QStandardItem* amItem = dcItem->child(j);
      amItem->setForeground(m_NoncompliantBrush);
      amItem->setBackground(m_NoncompliantBgBrush);

      int daCount = amItem->rowCount();
      for(int k = 0; k < daCount; k++)
      {
        QStandardItem* daItem = amItem->child(k);
        DataArrayPath path(dcItem->text(), amItem->text(), daItem->text());
        DataContainer::Pointer dc = m_Dca->getDataContainer(path);
        IGeometry::Pointer geom = dc->getGeometry();
        AttributeMatrix::Pointer am = m_Dca->getAttributeMatrix(path);
        IDataArray::Pointer da = am->getAttributeArray(path.getDataArrayName());
        
        bool amType = daReqs.amTypes.size() == 0 || daReqs.amTypes.contains(am->getType());
        bool compDims = daReqs.componentDimensions.size() == 0 || daReqs.componentDimensions.contains(da->getComponentDimensions());
        bool daType = daReqs.daTypes.size() == 0 || daReqs.daTypes.contains(da->getTypeAsString());
        bool geomType = daReqs.dcGeometryTypes.size() == 0 || (geom && daReqs.dcGeometryTypes.contains(geom->getGeometryType()));

        if(amType && compDims && daType && geomType)
        {
          daItem->setForeground(m_CompliantBrush);
          daItem->setBackground(m_CompliantBgBrush);
        }
        else
        {
          daItem->setForeground(m_NoncompliantBrush);
          daItem->setBackground(m_NoncompliantBgBrush);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::clearViewRequirements()
{
  // Clear requirements
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_Ui->dataBrowserTreeView->model());
  clearFilter(model->invisibleRootItem());
  
  markNewItems();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::clearFilter(QStandardItem* item)
{
  QBrush defaultBrush;
  item->setForeground(defaultBrush);
  item->setBackground(defaultBrush);

  int rows = item->rowCount();
  for(int i = 0; i < rows; i++)
  {
    clearFilter(item->child(i));
  }
}

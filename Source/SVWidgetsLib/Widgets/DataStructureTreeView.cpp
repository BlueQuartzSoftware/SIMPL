/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5280
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DataStructureTreeView.h"

#include <iostream>

#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QApplication>

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Montages/AbstractTileIndex.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h"
#include "SVWidgetsLib/Widgets/DataStructureItemDelegate.h"
#include "SVWidgetsLib/Widgets/DataStructureProxyModel.h"

namespace
{
const int InfoStringRole = Qt::UserRole + 1;
const int MontageRole = Qt::UserRole + 2;
} // namespace

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureTreeView::DataStructureTreeView(QWidget* parent)
: QTreeView(parent)
{
  setAcceptDrops(true);
  setMouseTracking(true);
  setAttribute(Qt::WA_MacShowFocusRect, false);
  // model
  QStandardItemModel* model = new QStandardItemModel(this);
  model->setColumnCount(1);
  DataStructureProxyModel* proxyModel = new DataStructureProxyModel(this);
  proxyModel->setSourceModel(model);
  setModel(proxyModel);

  m_Delegate = new DataStructureItemDelegate(this);
  setItemDelegate(m_Delegate);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureTreeView::~DataStructureTreeView() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStandardItemModel* DataStructureTreeView::getStandardModel()
{
  return dynamic_cast<QStandardItemModel*>(getProxyModel()->sourceModel());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataStructureProxyModel* DataStructureTreeView::getProxyModel()
{
  return dynamic_cast<DataStructureProxyModel*>(model());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setActiveFilter(AbstractFilter::Pointer filter)
{
  m_Filter = filter;
  m_Delegate->setActiveFilter(filter);
  viewport()->repaint();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setViewRequirements(DataContainerSelectionFilterParameter::RequirementType reqs)
{
  m_Delegate->setViewRequirements(reqs);
  viewport()->repaint();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setViewRequirements(AttributeMatrixSelectionFilterParameter::RequirementType reqs)
{
  m_Delegate->setViewRequirements(reqs);
  viewport()->repaint();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setViewRequirements(DataArraySelectionFilterParameter::RequirementType reqs)
{
  m_Delegate->setViewRequirements(reqs);
  viewport()->repaint();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::clearViewRequirements()
{
  m_Delegate->clearRequirements();
  viewport()->repaint();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataStructureTreeView::getDataArrayPath(const QModelIndex& index)
{
  DataArrayPath path;
  DataStructureProxyModel* proxyModel = dynamic_cast<DataStructureProxyModel*>(getProxyModel());
  QStandardItemModel* stdModel = dynamic_cast<QStandardItemModel*>(getStandardModel());

  if(nullptr == proxyModel || nullptr == stdModel)
  {
    return path;
  }

  if(index.isValid())
  {
    QStandardItem* item = stdModel->itemFromIndex(proxyModel->mapToSource(index));
    if(item->parent() != nullptr)
    {
      QStandardItem* parentItem = item->parent();
      if(parentItem->parent() != nullptr)
      {
        path.setDataContainerName(parentItem->parent()->text());
        path.setAttributeMatrixName(parentItem->text());
        path.setDataArrayName(item->text());
      }
      else
      {
        path.setDataContainerName(parentItem->text());
        path.setAttributeMatrixName(item->text());
      }
    }
    else
    {
      path.setDataContainerName(item->text());
    }
  }

  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::emitFilterPath(QModelIndex& index)
{
  // Do not filter the FilterInputWidget while the view is being filtered
  if(m_Delegate->isFiltered())
  {
    return;
  }

  if(!index.isValid())
  {
    Q_EMIT endDataStructureFiltering();
    return;
  }

  DataArrayPath path = getDataArrayPath(index);
  Q_EMIT filterPath(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton)
  {
    m_StartPos = event->pos();
  }

  QTreeView::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::mouseMoveEvent(QMouseEvent* event)
{
  QTreeView::mouseMoveEvent(event);

  if(event->buttons() & Qt::MouseButton::LeftButton)
  {
    QModelIndex index = indexAt(m_StartPos);
    if(m_Delegate->isFiltered() && !m_Delegate->indexMatchesReqs(index))
    {
      return;
    }

    int distance = (event->pos() - m_StartPos).manhattanLength();
    if(distance >= QApplication::startDragDistance() && index.isValid())
    {
      performDrag();
    }
  }
  else
  {
    QModelIndex index = indexAt(event->pos());
    if(m_Delegate->isFiltered() && !m_Delegate->indexMatchesReqs(index))
    {
      return;
    }

    emitFilterPath(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::leaveEvent(QEvent* event)
{
  QTreeView::leaveEvent(event);

  if(m_Dragging)
  {
    return;
  }

  Q_EMIT endDataStructureFiltering();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::performDrag()
{
  QModelIndex index = indexAt(m_StartPos);
  if(!index.isValid())
  {
    return;
  }

  DataArrayPath path = getDataArrayPath(index);
  if(nullptr == m_Filter)
  {
    return;
  }

  // Do not allow dragging paths created by the current filter
  std::list<DataArrayPath> createdPaths = m_Filter->getCreatedPaths();
  if(std::find(createdPaths.begin(), createdPaths.end(), path) != createdPaths.end())
  {
    return;
  }

  QMimeData* mimeData = new QMimeData;
  mimeData->setData(SIMPLView::DragAndDrop::DataArrayPath, path.serialize().toUtf8());
  QPixmap dragIcon = DataArrayPathSelectionWidget::CreateDragIcon(path);

  m_Dragging = true;
  QDrag* drag = new QDrag(this);
  drag->setMimeData(mimeData);
  drag->setPixmap(dragIcon);
  drag->exec(Qt::CopyAction);

  // drag->exec is a blocking method
  dragComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::dragComplete()
{
  Q_EMIT endDataStructureFiltering();
  m_Dragging = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::dragEnterEvent(QDragEnterEvent* event)
{
  if(event->mimeData()->hasFormat(SIMPLView::DragAndDrop::SelectionWidget))
  {
    DataArrayPathSelectionWidget* selectionWidget = dynamic_cast<DataArrayPathSelectionWidget*>(event->source());
    if(selectionWidget != nullptr)
    {
      event->accept();
      return;
    }
  }

  event->ignore();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::dragMoveEvent(QDragMoveEvent* event)
{
  // If Source is DataArrayPathSelectionWidget
  if(event->mimeData()->hasFormat(SIMPLView::DragAndDrop::SelectionWidget))
  {
    DataArrayPathSelectionWidget* selectionWidget = dynamic_cast<DataArrayPathSelectionWidget*>(event->source());
    if(selectionWidget != nullptr)
    {
      // Get the DataArrayPath under the cursor
      QModelIndex index = indexAt(event->pos());
      if(!index.isValid())
      {
        return;
      }
      DataArrayPath path = getDataArrayPath(index);

      // Check path requirements
      if(selectionWidget->checkPathReqs(path))
      {
        event->accept();
        return;
      }
    }
    // End SelectionWidget
  }

  event->ignore();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::dropEvent(QDropEvent* event)
{
  // If Source is DataArrayPathSelectionWidget
  if(event->mimeData()->hasFormat(SIMPLView::DragAndDrop::SelectionWidget))
  {
    DataArrayPathSelectionWidget* selectionWidget = dynamic_cast<DataArrayPathSelectionWidget*>(event->source());
    if(selectionWidget != nullptr)
    {
      QModelIndex index = indexAt(event->pos());
      if(!index.isValid())
      {
        return;
      }
      DataArrayPath path = getDataArrayPath(index);

      // Check path requirements
      if(selectionWidget->checkPathReqs(path))
      {
        selectionWidget->setDataArrayPath(path);
        event->accept();
        return;
      }
    }
  }

  event->ignore();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::collapseAllBut(const QModelIndex& index)
{
  QVector<QModelIndex> expandedChildren = getExpandedChildren(index);
  if(isExpanded(index))
  {
    expandedChildren.push_front(index);
  }
  collapseAll();

  // Disable Animation
  bool prevAnimationState = isAnimated();
  setAnimated(false);
  for(const QModelIndex& child : expandedChildren)
  {
    expand(child);
  }
  QModelIndex parentIndex = index.parent();
  while(parentIndex.isValid())
  {
    expand(parentIndex);
    parentIndex = parentIndex.parent();
  }
  // Reset animation
  setAnimated(prevAnimationState);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::findExpandedChildren(QAbstractItemModel* model, const QModelIndex& index, QVector<QModelIndex>& expandedVector)
{
  if(isExpanded(index))
  {
    expandedVector.push_back(index);
  }
  const int childCount = model->rowCount(index);
  for(int i = 0; i < childCount; i++)
  {
    QModelIndex childIndex = model->index(i, 0, index);
    if(isExpanded(childIndex))
    {
      expandedVector.push_back(childIndex);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QModelIndex> DataStructureTreeView::getExpandedChildren(const QModelIndex& index)
{
  QVector<QModelIndex> expandedChildren;
  QAbstractItemModel* abstrModel = model();
  const int childCount = abstrModel->rowCount(index);
  for(int i = 0; i < childCount; i++)
  {
    QModelIndex childIndex = abstrModel->index(i, 0, index);
    findExpandedChildren(abstrModel, childIndex, expandedChildren);
  }

  return expandedChildren;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::mouseDoubleClickEvent(QMouseEvent* event)
{
  QModelIndex index = indexAt(event->pos());
  if(!index.isValid())
  {
    return;
  }

  DataArrayPath path = getDataArrayPath(index);
  if(m_Delegate->pathMatchesReqs(path))
  {
    Q_EMIT applyPathToFilteringParameter(path);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::contextMenuEvent(QContextMenuEvent* event)
{
  const QPoint pos = event->pos();
  const QModelIndex index = indexAt(pos);
  DataStructureProxyModel* proxyModel = getProxyModel();
  const bool isValid = index.isValid();
  const int numChildren = proxyModel->rowCount(index);

  QMenu menu;
  QAction* expandAllAction = menu.addAction("Expand All");
  QAction* collapseAllAction = menu.addAction("Collapse All");

  connect(expandAllAction, &QAction::triggered, this, &DataStructureTreeView::expandAll);
  connect(collapseAllAction, &QAction::triggered, this, &DataStructureTreeView::collapseAll);

  if(isValid && numChildren > 0)
  {
    menu.addSeparator();
    QAction* collapseAllButAction = menu.addAction("Collapse All But This");
    connect(collapseAllButAction, &QAction::triggered, [=] { collapseAllBut(index); });
  }

  menu.exec(event->globalPos());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::search(const QString& name)
{
  if(name.isEmpty())
  {
    getProxyModel()->setFilterRegExp(QRegExp());
  }
  else
  {
    getProxyModel()->setFilterRegExp(QRegExp(name, Qt::CaseInsensitive));
  }
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStandardItem* DataStructureTreeView::findChildByName(QStandardItem* rootItem, const QString& name, int column)
{
  if(nullptr == rootItem)
  {
    return nullptr;
  }

  QStandardItem* item = nullptr;
  int rowCount = rootItem->rowCount();
  for(int row = 0; row < rowCount; ++row)
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
QStandardItem* DataStructureTreeView::findItemByPath(const DataArrayPath& path)
{
  DataArrayPathHelper::DataType dataType = path.getDataType();
  if(dataType == DataArrayPathHelper::DataType::None)
  {
    return nullptr;
  }

  QStandardItemModel* model = getStandardModel();
  if(model == nullptr)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return nullptr;
  }

  QStandardItem* rootItem = model->invisibleRootItem();
  QStandardItem* targetItem = nullptr;
  QStandardItem* dcItem = findChildByName(rootItem, path.getDataContainerName(), 0);
  if(dataType == DataArrayPathHelper::DataType::DataContainer)
  {
    targetItem = dcItem;
  }
  else
  {
    QStandardItem* amItem = findChildByName(dcItem, path.getAttributeMatrixName(), 0);
    if(dataType == DataArrayPathHelper::DataType::AttributeMatrix)
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
void DataStructureTreeView::removeNonexistingEntries(QStandardItem* rootItem, const QStringList& existingItems, int column)
{
  int rowCount = rootItem->rowCount();
  for(int row = rowCount - 1; row >= 0; row--)
  {
    QStandardItem* anItem = rootItem->child(row, column);
    if(!existingItems.contains(anItem->text()))
    {
      rootItem->removeRow(row);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::displayDataContainers(const DataContainerArray::Pointer& dca)
{
  if(nullptr == dca)
  {
    getStandardModel()->clear();
    return;
  }

  QStandardItemModel* model = getStandardModel();
  QVector<QString> path;
  {
    QModelIndex currIndex = currentIndex();
    QStandardItem* item = model->itemFromIndex(currIndex);
    // Get what is selected and save it
    while(nullptr != item)
    {
      path.push_front(item->text());
      item = item->parent();
    }
  }

  // Sanity check model
  if(model == nullptr)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }

  // Loop over the data containers
  DataContainerArray::Container containers = dca->getDataContainers();
  QStandardItem* rootItem = model->invisibleRootItem();

  for(DataContainer::Pointer dc : containers)
  {
    QStandardItem* dcItem = generateDataContainerItem(rootItem, dc);

    if(!path.empty() && dc->getName().compare(path[0]) == 0)
    {
      setCurrentIndex(model->indexFromItem(dcItem));
    }

    // We found the proper Data Container, now populate the AttributeMatrix List
    for(const auto& am : dc->getChildren())
    {
      QStandardItem* amItem = generateAttrMatrixItem(dcItem, am);

      if(path.size() > 1 && am->getName().compare(path[1]) == 0)
      {
        setCurrentIndex(model->indexFromItem(amItem));
      }

      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QList<QString> attrArrayNames = am->getAttributeArrayNames();
      for(const QString& attrArrayName : attrArrayNames)
      {
        QStandardItem* aaItem = generateDataArrayItem(amItem, am, attrArrayName);

        if(path.size() > 2 && attrArrayName.compare(path[2]) == 0)
        {
          QModelIndex idx = model->indexFromItem(aaItem);
          setCurrentIndex(idx);
        }
      }
      removeNonexistingEntries(amItem, attrArrayNames, 0);
    }
    removeNonexistingEntries(dcItem, dc->getAttributeMatrixNames(), 0);
  }
  removeNonexistingEntries(rootItem, dca->getDataContainerNames(), 0);

  // repaint the DataStructureTreeView
  repaint();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::displayMontages(const DataContainerArray::Pointer& dca)
{
  if(dca.get() == nullptr)
  {
    QStandardItemModel* model = getStandardModel();
    QStandardItem* rootItem = model->invisibleRootItem();
    removeNonexistingEntries(rootItem, QStringList(), 0);
    return;
  }

  QStandardItemModel* model = getStandardModel();
  QVector<QString> path;
  {
    QModelIndex currIndex = currentIndex();
    QStandardItem* item = model->itemFromIndex(currIndex);
    // Get what is selected and save it
    while(nullptr != item)
    {
      path.push_front(item->text());
      item = item->parent();
    }
  }

  // Sanity check model
  if(model == nullptr)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }

  QStandardItem* rootItem = model->invisibleRootItem();
  DataContainerArray::MontageCollection montageCollection = dca->getMontageCollection();

  for(const auto& montage : montageCollection)
  {
    QStandardItem* montageItem = generateMontageItem(rootItem, montage);
    montageItem->setData(true, ::MontageRole);

    // Loop over the data containers
    DataContainerArray::Container containers = montage->getDataContainers();
    for(DataContainer::Pointer dc : containers)
    {
      QStandardItem* dcItem;
      if(nullptr == dc)
      {
        dcItem = new QStandardItem("[Missing Data Container]");
        montageItem->appendRow(dcItem);
      }
      else
      {
        dcItem = generateDataContainerItem(montageItem, dc);
        dcItem->setData(false, ::MontageRole);

        AbstractTileIndexShPtr dcIndex = montage->getTileIndexFor(dc);
        ToolTipGenerator dcToolTip = dcIndex->getToolTipGenerator();
        dcToolTip.append(dc->getToolTipGenerator());
        dcItem->setToolTip(dcToolTip.generateHTML());
      }

      if(!path.empty() && dc->getName().compare(path[0]) == 0)
      {
        setCurrentIndex(model->indexFromItem(dcItem));
      }
    }
    removeNonexistingEntries(montageItem, dca->getDataContainerNames(), 0);
  }
  removeNonexistingEntries(rootItem, dca->getMontageNames(), 0);

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStandardItem* DataStructureTreeView::generateMontageItem(QStandardItem* rootItem, const AbstractMontage::Pointer& montage)
{
  QStandardItem* montageItem = getOrCreateItem(rootItem, montage->getName());
  montageItem->setData(montage->getInfoString(), ::InfoStringRole);
  montageItem->setToolTip(montage->getInfoString());
  if(montage->isValid())
  {
    montageItem->setIcon(QIcon());
  }
  else
  {
    montageItem->setIcon(QIcon(":SIMPL/icons/images/warning.png"));
  }

  return montageItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStandardItem* DataStructureTreeView::generateDataContainerItem(QStandardItem* rootItem, const DataContainer::Pointer& dc)
{
  QStandardItem* dcItem = getOrCreateItem(rootItem, dc->getName());
  dcItem->setData(dc->getInfoString(SIMPL::HtmlFormat), ::InfoStringRole);
  dcItem->setToolTip(dc->getInfoString(SIMPL::HtmlFormat));
  if(dc->getGeometry())
  {
    IGeometry::Type geomType = dc->getGeometry()->getGeometryType();
    dcItem->setIcon(getDataContainerIcon(geomType));
  }
  else
  {
    dcItem->setIcon(QIcon());
  }

  return dcItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStandardItem* DataStructureTreeView::generateAttrMatrixItem(QStandardItem* dcItem, const AttributeMatrix::Pointer& am)
{
  QStandardItem* amItem = getOrCreateItem(dcItem, am->getName());
  amItem->setData(am->getInfoString(SIMPL::HtmlFormat), ::InfoStringRole);
  amItem->setToolTip(am->getInfoString(SIMPL::HtmlFormat));
  amItem->setIcon(QIcon());

  return amItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStandardItem* DataStructureTreeView::generateDataArrayItem(QStandardItem* amItem, const AttributeMatrix::Pointer& am, const QString& name)
{
  IDataArray::Pointer attrArray = am->getAttributeArray(name);

  QStandardItem* aaItem = getOrCreateItem(amItem, attrArray->getName());
  aaItem->setData(attrArray->getInfoString(SIMPL::HtmlFormat), ::InfoStringRole);
  aaItem->setToolTip(attrArray->getInfoString(SIMPL::HtmlFormat));
  aaItem->setIcon(QIcon());

  return aaItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStandardItem* DataStructureTreeView::getOrCreateItem(QStandardItem* parentItem, const QString& name)
{
  QStandardItem* targetItem = findChildByName(parentItem, name, 0);
  if(targetItem == nullptr)
  {
    targetItem = new QStandardItem(name);
    parentItem->appendRow(targetItem);
  }
  return targetItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::rowsInserted(const QModelIndex& parent, int start, int end)
{
  QTreeView::rowsInserted(parent, start, end);
  // Expand if the specified parent did not have children previously
  if(this->model())
  {
    if(0 == start && !model()->index(end + 1, 0, parent).isValid())
      expand(parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setImageGeomIcon(const QIcon& path)
{
  m_ImageGeomIcon = path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setVertexGeomIcon(const QIcon& path)
{
  m_VertexGeomIcon = path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setEdgeGeomIcon(const QIcon& path)
{
  m_EdgeGeomIcon = path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setTriangleGeomIcon(const QIcon& path)
{
  m_TriangleGeomIcon = path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setQuadGeomIcon(const QIcon& path)
{
  m_QuadGeomIcon = path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setTetrahedralGeomIcon(const QIcon& path)
{
  m_TetrahedralGeomIcon = path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setHexahedralGeomIcon(const QIcon& path)
{
  m_HexahedralGeomIcon = path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureTreeView::setRectilinearGeomIcon(const QIcon& path)
{
  m_RectilinearGeomIcon = path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureTreeView::getImageGeomIcon() const
{
  return m_ImageGeomIcon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureTreeView::getVertexGeomIcon() const
{
  return m_VertexGeomIcon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureTreeView::getEdgeGeomIcon() const
{
  return m_EdgeGeomIcon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureTreeView::getTriangleGeomIcon() const
{
  return m_TriangleGeomIcon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureTreeView::getQuadGeomIcon() const
{
  return m_QuadGeomIcon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureTreeView::getTetrahedralGeomIcon() const
{
  return m_TetrahedralGeomIcon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureTreeView::getHexahedralGeomIcon() const
{
  return m_HexahedralGeomIcon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureTreeView::getRectilinearGeomIcon() const
{
  return m_RectilinearGeomIcon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureTreeView::getDataContainerIcon(IGeometry::Type type) const
{
  switch(type)
  {
  case IGeometry::Type::Image:
    return m_ImageGeomIcon;
    break;
  case IGeometry::Type::Vertex:
    return m_VertexGeomIcon;
    break;
  case IGeometry::Type::Edge:
    return m_EdgeGeomIcon;
    break;
  case IGeometry::Type::Triangle:
    return m_TriangleGeomIcon;
    break;
  case IGeometry::Type::Quad:
    return m_QuadGeomIcon;
    break;
  case IGeometry::Type::Tetrahedral:
    return m_TetrahedralGeomIcon;
    break;
  case IGeometry::Type::Hexahedral:
    return m_HexahedralGeomIcon;
    break;
  case IGeometry::Type::RectGrid:
    return m_RectilinearGeomIcon;
    break;
  default:
    return QIcon();
    break;
  }
}

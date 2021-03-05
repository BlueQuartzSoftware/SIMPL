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

#pragma once

#include <memory>

#include <QtCore/QModelIndex>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QTreeView>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Montages/AbstractMontage.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/DataStructureItemDelegate.h"

class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;
class DataContainerArray;
class DataStructureProxyModel;

using AttributeMatrixShPtr = std::shared_ptr<AttributeMatrix>;
using DataContainerShPtr = std::shared_ptr<DataContainer>;
using DataContainerArrayShPtr = std::shared_ptr<DataContainerArray>;

class SVWidgetsLib_EXPORT DataStructureTreeView : public QTreeView
{
  Q_OBJECT

public:
  //-------- Setup some QProperties that we can use from the CSS theme files to set the proper icon
  Q_PROPERTY(QIcon ImageGeomIcon READ getImageGeomIcon WRITE setImageGeomIcon)
  Q_PROPERTY(QIcon VertexGeomIcon READ getVertexGeomIcon WRITE setVertexGeomIcon)
  Q_PROPERTY(QIcon EdgeGeomIcon READ getEdgeGeomIcon WRITE setEdgeGeomIcon)
  Q_PROPERTY(QIcon TriangleGeomIcon READ getTriangleGeomIcon WRITE setTriangleGeomIcon)
  Q_PROPERTY(QIcon QuadGeomIcon READ getQuadGeomIcon WRITE setQuadGeomIcon)
  Q_PROPERTY(QIcon TetrahedralGeomIcon READ getTetrahedralGeomIcon WRITE setTetrahedralGeomIcon)
  Q_PROPERTY(QIcon HexahedralGeomIcon READ getHexahedralGeomIcon WRITE setHexahedralGeomIcon)
  Q_PROPERTY(QIcon RectilinearGeomIcon READ getRectilinearGeomIcon WRITE setRectilinearGeomIcon)

  /**
   * @brief DataStructureTreeView
   * @param parent
   */
  DataStructureTreeView(QWidget* parent = nullptr);

  /**
   * @brief ~DataStructureTreeView()
   */
  ~DataStructureTreeView() override;

  /**
   * @brief Returns the QStandardItemModel backing the tree view.
   * @return
   */
  QStandardItemModel* getStandardModel();

  /**
   * @brief Returns the DataStructureProxyModel used for searching the tree view
   * and accessing the QStandardItemModel.
   * @return
   */
  DataStructureProxyModel* getProxyModel();

  /**
   * @brief Returns the DataArrayPath for the given QModelIndex
   * @param index
   * @return
   */
  DataArrayPath getDataArrayPath(const QModelIndex& index);

  /**
   * @brief Updates the active filter for use in preventing created
   * items from being used as filter parameter values.
   * @param filter
   */
  void setActiveFilter(AbstractFilter::Pointer filter);

  /**
   * @brief Sets the filter requirements and forces a repaint
   * @param reqs
   */
  void setViewRequirements(DataContainerSelectionFilterParameter::RequirementType reqs);

  /**
   * @brief Sets the filter requirements and forces a repaint
   * @param reqs
   */
  void setViewRequirements(AttributeMatrixSelectionFilterParameter::RequirementType reqs);

  /**
   * @brief Sets the filter requirements and forces a repaint
   * @param reqs
   */
  void setViewRequirements(DataArraySelectionFilterParameter::RequirementType reqs);

  /**
   * @brief Clears the filter requirements and forces a repaint
   * @param reqs
   */
  void clearViewRequirements();

  QIcon getDataContainerIcon(IGeometry::Type type) const;

  /**
   * @brief Returns a QVector of QModelIndex for all expanded children
   * @param index
   * @return
   */
  QVector<QModelIndex> getExpandedChildren(const QModelIndex& index);

  /**
   * @brief Collapses all items but the given index and its children.
   * @param index
   */
  void collapseAllBut(const QModelIndex& index);

  /**
   * @brief Searches for items using the given name as part of their text.  All other items are hidden.
   */
  void search(const QString& name);

public Q_SLOTS:
  void displayDataContainers(const DataContainerArrayShPtr& dca);
  void displayMontages(const DataContainerArrayShPtr& dca);

  void setImageGeomIcon(const QIcon& path);
  void setVertexGeomIcon(const QIcon& path);
  void setEdgeGeomIcon(const QIcon& path);
  void setTriangleGeomIcon(const QIcon& path);
  void setQuadGeomIcon(const QIcon& path);
  void setTetrahedralGeomIcon(const QIcon& path);
  void setHexahedralGeomIcon(const QIcon& path);
  void setRectilinearGeomIcon(const QIcon& path);
  QIcon getImageGeomIcon() const;
  QIcon getVertexGeomIcon() const;
  QIcon getEdgeGeomIcon() const;
  QIcon getTriangleGeomIcon() const;
  QIcon getQuadGeomIcon() const;
  QIcon getTetrahedralGeomIcon() const;
  QIcon getHexahedralGeomIcon() const;
  QIcon getRectilinearGeomIcon() const;

Q_SIGNALS:
  void filterPath(DataArrayPath path);
  void endDataStructureFiltering();
  void applyPathToFilteringParameter(DataArrayPath path);

protected:
  /**
   * @brief mouseMoveEvent
   * @param event
   */
  void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief dragEnterEvent
   * @param event
   */
  void dragEnterEvent(QDragEnterEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief dragMoveEvent
   * @param event
   */
  void dragMoveEvent(QDragMoveEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief dropEvent
   * @param event
   */
  void dropEvent(QDropEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief leaveEvent
   * @param event
   */
  void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief mouseDoubleClickEvent
   * @param event
   */
  void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief Emits the filter path for the given model index
   * @param index
   */
  void emitFilterPath(QModelIndex& index);

  /**
   * @brief End the drag process
   */
  void dragComplete();

  /**
   * @brief findExpandedChildren
   * @param model
   * @param index
   * @param expandedVector
   */
  void findExpandedChildren(QAbstractItemModel* model, const QModelIndex& index, QVector<QModelIndex>& expandedVector);

  /**
   * @brief findChildByName
   * @param rootItem
   * @param name
   * @param column
   */
  QStandardItem* findChildByName(QStandardItem* rootItem, const QString& name, int column);

  /**
   * @brief findItemByPath
   * @param path
   * @return
   */
  QStandardItem* findItemByPath(const DataArrayPath& path);

  /**
   * @brief getOrCreateItem
   * @param parentItem
   * @param name
   * @return
   */
  QStandardItem* getOrCreateItem(QStandardItem* parentItem, const QString& name);

  /**
   * @brief generateMontageItem
   * @param rootItem
   * @param montage
   * @return
   */
  QStandardItem* generateMontageItem(QStandardItem* rootItem, const AbstractMontage::Pointer& montage);

  /**
   * @brief generateDataContainerItem
   * @param rootItem
   * @param dc
   */
  QStandardItem* generateDataContainerItem(QStandardItem* rootItem, const DataContainerShPtr& dc);

  /**
   * @brief generateAttrMatrixItem
   * @param dcItem
   * @param am
   * @return
   */
  QStandardItem* generateAttrMatrixItem(QStandardItem* dcItem, const AttributeMatrixShPtr& am);

  /**
   * @brief generateDataArrayItem
   * @param amItem
   * @param am
   * @param name
   * @return
   */
  QStandardItem* generateDataArrayItem(QStandardItem* amItem, const AttributeMatrixShPtr& am, const QString& name);

  /**
   * @brief removeNonexistingEntries
   * @param rootItem
   * @param existingItems
   * @param column
   */
  void removeNonexistingEntries(QStandardItem* rootItem, const QStringList& existingItems, int column);

  /**
   * @brief rowsInserted
   * @param parent
   * @param start
   * @param end
   */
  void rowsInserted(const QModelIndex& parent, int start, int end) override;

  /**
   * @brief Create custom context menu
   * @param event
   */
  void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;

private Q_SLOTS:
  /**
   * @brief mousePressEvent
   * @param event
   */
  void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
  QPoint m_StartPos;
  bool m_Dragging = false;
  AbstractFilter::Pointer m_Filter = nullptr;
  DataStructureItemDelegate* m_Delegate = nullptr;
  QIcon m_ImageGeomIcon = QIcon(SIMPLView::GeometryIcons::Image);
  QIcon m_VertexGeomIcon = QIcon(SIMPLView::GeometryIcons::Vertex);
  QIcon m_EdgeGeomIcon = QIcon(SIMPLView::GeometryIcons::Edge);
  QIcon m_TriangleGeomIcon = QIcon(SIMPLView::GeometryIcons::Triangle);
  QIcon m_QuadGeomIcon = QIcon(SIMPLView::GeometryIcons::Quad);
  QIcon m_TetrahedralGeomIcon = QIcon(SIMPLView::GeometryIcons::Tetetrahedral);
  QIcon m_HexahedralGeomIcon = QIcon(SIMPLView::GeometryIcons::Hexahedral);
  QIcon m_RectilinearGeomIcon = QIcon(SIMPLView::GeometryIcons::Rectilinear);

  /**
   * @brief performDrag
   */
  void performDrag();
};

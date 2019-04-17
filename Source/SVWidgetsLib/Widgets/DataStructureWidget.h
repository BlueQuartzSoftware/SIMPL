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

#pragma once

#include <QtCore/QUuid>
#include <QtGui/QBrush>
#include <QtGui/QIcon>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/SIMPLib.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/SVWidgetsLib.h"

#include "ui_DataStructureWidget.h"

class PipelineFilterObject;
class QStandardItem;

namespace Ui
{
class DataStructureWidget;
}

class SVWidgetsLib_EXPORT DataStructureWidget : public QWidget, public IObserver
{
  Q_OBJECT
public:
  DataStructureWidget(QWidget* parent = nullptr);
  ~DataStructureWidget() override;

  //-------- Setup some QProperties that we can use from the CSS theme files to set the proper icon
  Q_PROPERTY(QIcon ImageGeomIcon READ getImageGeomIcon WRITE setImageGeomIcon)
  Q_PROPERTY(QIcon VertexGeomIcon READ getVertexGeomIcon WRITE setVertexGeomIcon)
  Q_PROPERTY(QIcon EdgeGeomIcon READ getEdgeGeomIcon WRITE setEdgeGeomIcon)
  Q_PROPERTY(QIcon TriangleGeomIcon READ getTriangleGeomIcon WRITE setTriangleGeomIcon)
  Q_PROPERTY(QIcon QuadGeomIcon READ getQuadGeomIcon WRITE setQuadGeomIcon)
  Q_PROPERTY(QIcon TetrahedralGeomIcon READ getTetrahedralGeomIcon WRITE setTetrahedralGeomIcon)
  Q_PROPERTY(QIcon HexahedralGeomIcon READ getHexahedralGeomIcon WRITE setHexahedralGeomIcon)
  Q_PROPERTY(QIcon RectilinearGeomIcon READ getRectilinearGeomIcon WRITE setRectilinearGeomIcon)

  void setImageGeomIcon(const QIcon& path);
  void setVertexGeomIcon(const QIcon& path);
  void setEdgeGeomIcon(const QIcon& path);
  void setTriangleGeomIcon(const QIcon& path);
  void setQuadGeomIcon(const QIcon& path);
  void setTetrahedralGeomIcon(const QIcon& path);
  void setHexahedralGeomIcon(const QIcon& path);
  void setRectilinearGeomIcon(const QIcon& path);

  QIcon getImageGeomIcon();
  QIcon getVertexGeomIcon();
  QIcon getEdgeGeomIcon();
  QIcon getTriangleGeomIcon();
  QIcon getQuadGeomIcon();
  QIcon getTetrahedralGeomIcon();
  QIcon getHexahedralGeomIcon();
  QIcon getRectilinearGeomIcon();

public slots:
  /**
   * @brief Updates the internal DataContainerArray copy from the input DataContainerArray
   * @param dca
   */
  void updateDataContainerArray(DataContainerArray::Pointer dca);

  /**
   * @brief Updates the DataStructureWidget with the latest DataContainerArray from
   * the filter
   * @param filter
   */
  void filterActivated(AbstractFilter::Pointer filter);

  /**
   * @brief Forces a refresh of the TreeView from the internal copy of the DataContainerArray
   */
  void refreshData();

  /**
   * @brief Slot to handle when a PipelineFilterObject is removed from a pipeline view.
   * Currently this will clear the QTreeView.
   * @param object
   */
  void handleFilterRemoved(PipelineFilterObject* object);

  /**
  * @brief Sets the DataContainer requirements
  * @param dcReqs
  */
  void setViewReqs(DataContainerSelectionFilterParameter::RequirementType dcReqs);

  /**
  * @brief Sets the AttributeMatrix requirements
  * @param amReqs
  */
  void setViewReqs(AttributeMatrixSelectionFilterParameter::RequirementType amReqs);

  /**
  * @brief Sets the DataArray requirements
  * @param daReqs
  */
  void setViewReqs(DataArraySelectionFilterParameter::RequirementType daReqs);

  /**
  * @brief Clears the requirements
  */
  void clearViewRequirements();

signals:
  void filterPath(DataArrayPath path);
  void endDataStructureFiltering();
  void applyPathToFilteringParameter(DataArrayPath path);

protected:
  /**
   * @brief Basic GUI customization code.
   */
  void setupGui();

  /**
   * @brief findChildByName
   * @param rootItem
   * @param name
   * @param column
   * @return
   */
  QStandardItem* findChildByName(QStandardItem* rootItem, const QString& name, int column);

  /**
   * @brief Syncs up the QTreeView with the heirarchy from the DataContainerArray
   * @param rootItem RootItem to start the sync
   * @param existing List of existing objects
   * @param column Column to update
   */
  void removeNonexistingEntries(QStandardItem* rootItem, QList<QString> existing, int column);

  /**
   * @brief Returns a QStandardItem from the given DataArrayPath
   * @param path
   * @return
   */
  QStandardItem* findItemByPath(DataArrayPath path);

private:
  DataContainerArray::Pointer  m_Dca = nullptr;
  QSharedPointer<Ui::DataStructureWidget>       m_Ui;
  QIcon m_ImageGeomIcon = QIcon(SIMPLView::GeometryIcons::Image);
  QIcon m_VertexGeomIcon = QIcon(SIMPLView::GeometryIcons::Vertex);
  QIcon m_EdgeGeomIcon = QIcon(SIMPLView::GeometryIcons::Edge);
  QIcon m_TriangleGeomIcon = QIcon(SIMPLView::GeometryIcons::Triangle);
  QIcon m_QuadGeomIcon = QIcon(SIMPLView::GeometryIcons::Quad);
  QIcon m_TetrahedralGeomIcon = QIcon(SIMPLView::GeometryIcons::Tetetrahedral);
  QIcon m_HexahedralGeomIcon = QIcon(SIMPLView::GeometryIcons::Hexahedral);
  QIcon m_RectilinearGeomIcon = QIcon(SIMPLView::GeometryIcons::Rectilinear);
public:
  DataStructureWidget(const DataStructureWidget&) = delete; // Copy Constructor Not Implemented
  DataStructureWidget(DataStructureWidget&&) = delete;      // Move Constructor Not Implemented
  DataStructureWidget& operator=(const DataStructureWidget&) = delete; // Copy Assignment Not Implemented
  DataStructureWidget& operator=(DataStructureWidget&&) = delete;      // Move Assignment Not Implemented
};


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

#include "DataStructureWidget.h"

#include <QtGui/QStandardItemModel>

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "SVWidgetsLib/QtSupport/QtSSettings.h"
#include "SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h"

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
  connect(m_Ui->dataBrowserTreeView, SIGNAL(endDataStructureFiltering()), this, SIGNAL(endDataStructureFiltering()));
  connect(m_Ui->dataBrowserTreeView, SIGNAL(applyPathToFilteringParameter(DataArrayPath)), this, SIGNAL(applyPathToFilteringParameter(DataArrayPath)));

  connect(m_Ui->collapseAllBtn, &QPushButton::clicked, m_Ui->dataBrowserTreeView, &DataStructureTreeView::collapseAll);
  connect(m_Ui->expandAllBtn, &QPushButton::clicked, m_Ui->dataBrowserTreeView, &DataStructureTreeView::expandAll);

  // Search
  connect(m_Ui->dataStructureSearch, &QLineEdit::textChanged, m_Ui->dataBrowserTreeView, &DataStructureTreeView::search);
  connect(m_Ui->dataStructureSearch, &QLineEdit::textChanged, m_Ui->montageTreeView, &DataStructureTreeView::search);
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
  m_Ui->dataBrowserTreeView->displayDataContainers(m_Dca);
  m_Ui->montageTreeView->displayMontages(m_Dca);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::filterActivated(AbstractFilter::Pointer filter)
{
  m_Dca = DataContainerArray::NullPointer();
  m_Ui->dataBrowserTreeView->setActiveFilter(filter);
  m_Ui->montageTreeView->setActiveFilter(filter);
  if(filter.get() != nullptr)
  {
    DataContainerArray::Pointer dca = filter->getDataContainerArray();
    if(dca.get() != nullptr)
    {
      m_Dca = dca->deepCopy(true);
    }
  }

  refreshData();
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
void DataStructureWidget::setViewReqs(DataContainerSelectionFilterParameter::RequirementType dcReqs)
{
  m_Ui->dataBrowserTreeView->setViewRequirements(dcReqs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setViewReqs(AttributeMatrixSelectionFilterParameter::RequirementType amReqs)
{
  m_Ui->dataBrowserTreeView->setViewRequirements(amReqs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setViewReqs(DataArraySelectionFilterParameter::RequirementType daReqs)
{
  m_Ui->dataBrowserTreeView->setViewRequirements(daReqs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::clearViewRequirements()
{
  m_Ui->dataBrowserTreeView->clearViewRequirements();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setImageGeomIcon(const QIcon& path)
{
  m_Ui->dataBrowserTreeView->setImageGeomIcon(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setVertexGeomIcon(const QIcon& path)
{
  m_Ui->dataBrowserTreeView->setVertexGeomIcon(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setEdgeGeomIcon(const QIcon& path)
{
  m_Ui->dataBrowserTreeView->setEdgeGeomIcon(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setTriangleGeomIcon(const QIcon& path)
{
  m_Ui->dataBrowserTreeView->setTriangleGeomIcon(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setQuadGeomIcon(const QIcon& path)
{
  m_Ui->dataBrowserTreeView->setQuadGeomIcon(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setTetrahedralGeomIcon(const QIcon& path)
{
  m_Ui->dataBrowserTreeView->setTetrahedralGeomIcon(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setHexahedralGeomIcon(const QIcon& path)
{
  m_Ui->dataBrowserTreeView->setHexahedralGeomIcon(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataStructureWidget::setRectilinearGeomIcon(const QIcon& path)
{
  m_Ui->dataBrowserTreeView->setRectilinearGeomIcon(path);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureWidget::getImageGeomIcon()
{
  return m_Ui->dataBrowserTreeView->getImageGeomIcon();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureWidget::getVertexGeomIcon()
{
  return m_Ui->dataBrowserTreeView->getVertexGeomIcon();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureWidget::getEdgeGeomIcon()
{
  return m_Ui->dataBrowserTreeView->getEdgeGeomIcon();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureWidget::getTriangleGeomIcon()
{
  return m_Ui->dataBrowserTreeView->getTriangleGeomIcon();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureWidget::getQuadGeomIcon()
{
  return m_Ui->dataBrowserTreeView->getQuadGeomIcon();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureWidget::getTetrahedralGeomIcon()
{
  return m_Ui->dataBrowserTreeView->getTetrahedralGeomIcon();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureWidget::getHexahedralGeomIcon()
{
  return m_Ui->dataBrowserTreeView->getHexahedralGeomIcon();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon DataStructureWidget::getRectilinearGeomIcon()
{
  return m_Ui->dataBrowserTreeView->getRectilinearGeomIcon();
}

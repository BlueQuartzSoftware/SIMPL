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

#include "MoveFilterCommand.h"

#include "SVWidgetsLib/Widgets/FilterInputWidget.h"
#include "SVWidgetsLib/Widgets/PipelineModel.h"
#include "SVWidgetsLib/Widgets/SVPipelineView.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveFilterCommand::MoveFilterCommand(AbstractFilter::Pointer filter, SVPipelineView* view, int insertIndex, QUndoCommand* parent)
: QUndoCommand(parent)
, m_PipelineView(view)
, m_InsertIndex(insertIndex)
{
  m_Filters.push_back(filter);

  setText(QObject::tr("\"Move '%1'\"").arg(filter->getHumanLabel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveFilterCommand::MoveFilterCommand(std::vector<AbstractFilter::Pointer> filters, SVPipelineView* view, int insertIndex, QUndoCommand* parent)
: QUndoCommand(parent)
, m_Filters(filters)
, m_PipelineView(view)
, m_InsertIndex(insertIndex)
{
  setText(QObject::tr("\"Move '%1' Filters\"").arg(m_Filters.size()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveFilterCommand::~MoveFilterCommand() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveFilterCommand::undo()
{
//  if(m_OriginView == nullptr || m_DestinationView == nullptr)
//  {
//    return;
//  }

//  PipelineModel* originModel = m_DestinationView->getPipelineModel();
//  PipelineModel* destinationModel = m_OriginView->getPipelineModel();

//  bool itemsMoved = false;

//  originModel->blockSignals(true);
//  destinationModel->blockSignals(true);
//  size_t destinationIndex = m_DestinationIndex;
//  for (size_t i = 0; i < m_Filters.size(); i++)
//  {
//    AbstractFilter::Pointer filter = m_Filters[i];
//    QModelIndex originModelIndex = originModel->indexOfFilter(filter);
//    size_t originIndex = originModelIndex.row();

//    removeFilter(originIndex, originModel);

//    addFilter(filter, destinationModel, originIndexes[i]);

//    itemsMoved = true;
//    destinationIndex++;
//  }
//  originModel->blockSignals(false);
//  destinationModel->blockSignals(false);

//  if (itemsMoved)
//  {
//    m_OriginView->preflightPipeline();
//    m_DestinationView->preflightPipeline();

//    if (m_Filters.size() == 1)
//    {
//      emit m_DestinationView->statusMessage(QObject::tr("Moved \"%1\" filter").arg(m_Filters[0]->getHumanLabel()));
//      emit m_DestinationView->stdOutMessage(QObject::tr("Moved \"%1\" filter").arg(m_Filters[0]->getHumanLabel()));
//    }
//    else
//    {
//      emit m_DestinationView->statusMessage(QObject::tr("Moved %1 filters").arg(m_Filters.size()));
//      emit m_DestinationView->stdOutMessage(QObject::tr("Moved %1 filters").arg(m_Filters.size()));
//    }
//  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveFilterCommand::redo()
{
  if(m_Filters.empty())
  {
    return;
  }

  PipelineModel* model = m_PipelineView->getPipelineModel();

  // Remove the filters from their original positions
  if(m_FirstRun)
  {
    int filterOffset = 0;
    for(size_t i = 0; i < m_Filters.size(); i++)
    {
      QModelIndex filterIndex = model->indexOfFilter(m_Filters[i].get());

  //    m_RemovalIndexes.push_back(filterIndex.row() + filterOffset);

      removeFilter(filterIndex.row());

      filterOffset++;
    }
  }

  // Add the filters to their new positions
  int index = m_InsertIndex;
  for(int i = 0; i < m_Filters.size(); i++)
  {
    addFilter(m_Filters[i], index);
    index++;
  }

  QString statusMessage;
  if (m_Filters.size() > 1)
  {
    statusMessage = QObject::tr("Moved %1 filters").arg(m_Filters.size());
  }
  else
  {
    statusMessage = QObject::tr("Moved '%1' filter").arg(m_Filters[0]->getHumanLabel());
  }

  if(!m_FirstRun)
  {
    statusMessage.prepend("Redo \"");
    statusMessage.append('\"');
  }
  else
  {
    m_FirstRun = false;
  }

  emit m_PipelineView->preflightPipeline();
  emit model->pipelineDataChanged(QModelIndex());

  emit model->statusMessageGenerated(statusMessage);
  emit model->standardOutputMessageGenerated(statusMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveFilterCommand::addFilter(AbstractFilter::Pointer filter, int insertionIndex)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();

  model->insertRow(insertionIndex);

  QModelIndex filterIndex = model->index(insertionIndex, PipelineItem::Contents);
//  model->setData(filterIndex, filter->getHumanLabel(), Qt::DisplayRole);
  model->setData(filterIndex, static_cast<int>(PipelineItem::ItemType::Filter), PipelineModel::ItemTypeRole);
  model->setFilter(filterIndex, filter);

  if(!filter->getEnabled())
  {
    model->setData(filterIndex, static_cast<int>(PipelineItem::WidgetState::Disabled), PipelineModel::WidgetStateRole);
  }

  FilterInputWidget* fiw = model->filterInputWidget(filterIndex);

  QObject::connect(fiw, &FilterInputWidget::filterParametersChanged, m_PipelineView, &SVPipelineView::preflightPipeline);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveFilterCommand::removeFilter(int filterIndex)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();
  QModelIndex index = model->index(filterIndex, PipelineItem::Contents);

  FilterInputWidget* fiw = model->filterInputWidget(index);

  QObject::disconnect(fiw, &FilterInputWidget::filterParametersChanged, m_PipelineView, &SVPipelineView::preflightPipeline);

  model->removeRow(filterIndex);
}

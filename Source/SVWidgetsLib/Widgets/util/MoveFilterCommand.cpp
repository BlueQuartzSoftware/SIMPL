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

#include "SVWidgetsLib/Widgets/PipelineModel.h"
#include "SVWidgetsLib/Widgets/SVPipelineView.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveFilterCommand::MoveFilterCommand(AbstractFilter::Pointer filter, SVPipelineView* originView, SVPipelineView* destinationView, int destinationIndex, QUndoCommand* parent)
: QUndoCommand(parent)
, m_OriginView(originView)
, m_DestinationView(destinationView)
{
  m_Filters.push_back(filter);

  if (destinationIndex < 0)
  {
    PipelineModel* destinationModel = destinationView->getPipelineModel();
    destinationIndex = destinationModel->rowCount();
  }
  m_DestinationIndex = destinationIndex;

  setText(QObject::tr("\"Move '%1'\"").arg(filter->getHumanLabel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveFilterCommand::MoveFilterCommand(std::vector<AbstractFilter::Pointer> filters, SVPipelineView* originView, SVPipelineView* destinationView, int destinationIndex, QUndoCommand* parent)
: QUndoCommand(parent)
, m_Filters(filters)
, m_OriginView(originView)
, m_DestinationView(destinationView)
{
  if (destinationIndex < 0)
  {
    PipelineModel* destinationModel = destinationView->getPipelineModel();
    destinationIndex = destinationModel->rowCount();
  }
  m_DestinationIndex = destinationIndex;

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
  if(m_OriginView == nullptr || m_DestinationView == nullptr)
  {
    return;
  }

  PipelineModel* originModel = m_DestinationView->getPipelineModel();
  PipelineModel* destinationModel = m_OriginView->getPipelineModel();

  bool itemsMoved = false;

  originModel->blockSignals(true);
  destinationModel->blockSignals(true);
  size_t destinationIndex = m_DestinationIndex;
  for (size_t i = 0; i < m_Filters.size(); i++)
  {
    AbstractFilter::Pointer filter = m_Filters[i];
    QModelIndex originModelIndex = originModel->indexOfFilter(filter);
    size_t originIndex = originModelIndex.row();

    removeFilter(originIndex, originModel);

    addFilter(filter, destinationModel, originIndexes[i]);

    itemsMoved = true;
    destinationIndex++;
  }
  originModel->blockSignals(false);
  destinationModel->blockSignals(false);

  if (itemsMoved)
  {
    m_OriginView->preflightPipeline();
    m_DestinationView->preflightPipeline();

    if (m_Filters.size() == 1)
    {
      emit m_DestinationView->statusMessage(QObject::tr("Moved \"%1\" filter").arg(m_Filters[0]->getHumanLabel()));
      emit m_DestinationView->stdOutMessage(QObject::tr("Moved \"%1\" filter").arg(m_Filters[0]->getHumanLabel()));
    }
    else
    {
      emit m_DestinationView->statusMessage(QObject::tr("Moved %1 filters").arg(m_Filters.size()));
      emit m_DestinationView->stdOutMessage(QObject::tr("Moved %1 filters").arg(m_Filters.size()));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveFilterCommand::redo()
{
  if(m_OriginView == nullptr || m_DestinationView == nullptr)
  {
    return;
  }

  PipelineModel* originModel = m_OriginView->getPipelineModel();
  PipelineModel* destinationModel = m_DestinationView->getPipelineModel();

  bool itemsMoved = false;

  m_OriginView->blockSignals(true);
  m_DestinationView->blockSignals(true);
  for (size_t i = 0; i < m_Filters.size(); i++)
  {
    AbstractFilter::Pointer filter = m_Filters[i];
    QModelIndex originModelIndex = originModel->indexOfFilter(filter);
    size_t originIndex = originModelIndex.row();
    originIndexes.push_back(originIndex);

    removeFilter(originIndex, originModel);

    addFilter(filter, destinationModel, m_DestinationIndex);

    itemsMoved = true;
  }
  m_OriginView->blockSignals(false);
  m_DestinationView->blockSignals(false);

  if (itemsMoved)
  {
    m_OriginView->preflightPipeline();
    m_DestinationView->preflightPipeline();

    if (m_Filters.size() == 1)
    {
      emit m_DestinationView->statusMessage(QObject::tr("Moved \"%1\" filter").arg(m_Filters[0]->getHumanLabel()));
      emit m_DestinationView->stdOutMessage(QObject::tr("Moved \"%1\" filter").arg(m_Filters[0]->getHumanLabel()));
    }
    else
    {
      emit m_DestinationView->statusMessage(QObject::tr("Moved %1 filters").arg(m_Filters.size()));
      emit m_DestinationView->stdOutMessage(QObject::tr("Moved %1 filters").arg(m_Filters.size()));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveFilterCommand::addFilter(AbstractFilter::Pointer filter, PipelineModel* model, size_t insertionIndex)
{
  model->insertRow(insertionIndex);
  QModelIndex filterIndex = model->index(insertionIndex, PipelineItem::Contents);
  model->setData(filterIndex, filter->getHumanLabel(), Qt::DisplayRole);
  model->setItemType(filterIndex, PipelineItem::ItemType::Filter);
  model->setFilter(filterIndex, filter);

  QModelIndexList list;
  list.push_back(filterIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveFilterCommand::removeFilter(int filterIndex, PipelineModel* model)
{
  model->removeRow(filterIndex);
}

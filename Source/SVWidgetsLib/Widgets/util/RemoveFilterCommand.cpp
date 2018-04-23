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

#include "RemoveFilterCommand.h"

#include "SVWidgetsLib/Widgets/PipelineModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFilterCommand::RemoveFilterCommand(AbstractFilter::Pointer filter, PipelineModel* pipelineModel, QString actionText, QUndoCommand* parent)
  : QUndoCommand(parent)
  , m_PipelineModel(pipelineModel)
{
  if(nullptr == filter || nullptr == pipelineModel)
  {
    return;
  }

  setText(QObject::tr("\"%1 '%2'\"").arg(actionText).arg(filter->getHumanLabel()));

  m_Filters.push_back(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFilterCommand::RemoveFilterCommand(std::vector<AbstractFilter::Pointer> filters, PipelineModel* pipelineModel, QString actionText, QUndoCommand* parent)
  : QUndoCommand(parent)
  , m_PipelineModel(pipelineModel)
  , m_Filters(filters)
{
  if(nullptr == pipelineModel)
  {
    return;
  }

  setText(QObject::tr("\"%1 %2 Filters\"").arg(actionText).arg(filters.size()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFilterCommand::~RemoveFilterCommand() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFilterCommand::undo()
{
  for(size_t i = 0; i < m_RemovalIndexes.size(); i++)
  {
    int insertIndex = m_RemovalIndexes[i];
    AbstractFilter::Pointer filter = m_Filters[i];

    addFilter(filter, insertIndex);
  }

  m_RemovalIndexes.clear();

  emit m_PipelineModel->pipelineDataChanged(QModelIndex());
  emit m_PipelineModel->preflightTriggered(QModelIndex(), m_PipelineModel);

  QString statusMessage;
  if (m_Filters.size() > 1)
  {
    statusMessage = QObject::tr("Undo \"Removed %1 filters\"").arg(m_Filters.size());
  }
  else
  {
    statusMessage = QObject::tr("Undo \"Removed '%1' filter\"").arg(m_Filters[0]->getHumanLabel());
  }

  emit m_PipelineModel->statusMessageGenerated(statusMessage);
  emit m_PipelineModel->standardOutputMessageGenerated(statusMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool variantCompare(const QVariant &v1, const QVariant &v2)
{
  return v1.toInt() > v2.toInt();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFilterCommand::redo()
{
  int filterOffset = 0;
  for(size_t i = 0; i < m_Filters.size(); i++)
  {
    QModelIndex filterIndex = m_PipelineModel->indexOfFilter(m_Filters[i]);

    m_RemovalIndexes.push_back(filterIndex.row() + filterOffset);

    removeFilter(filterIndex.row());

    filterOffset++;
  }

  QString statusMessage;
  if (m_Filters.size() > 1)
  {
    statusMessage = QObject::tr("Removed %1 filters").arg(m_Filters.size());
  }
  else
  {
    statusMessage = QObject::tr("Removed '%1' filter").arg(m_Filters[0]->getHumanLabel());
  }

  if (m_FirstRun == false)
  {
    statusMessage.prepend("Redo \"");
    statusMessage.append('\"');
  }
  else
  {
    m_FirstRun = false;
  }

  emit m_PipelineModel->statusMessageGenerated(statusMessage);
  emit m_PipelineModel->standardOutputMessageGenerated(statusMessage);

  emit m_PipelineModel->pipelineDataChanged(QModelIndex());
  emit m_PipelineModel->preflightTriggered(QModelIndex(), m_PipelineModel);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFilterCommand::addFilter(AbstractFilter::Pointer filter, int insertionIndex)
{
  m_PipelineModel->insertRow(insertionIndex);
  QModelIndex filterIndex = m_PipelineModel->index(insertionIndex, PipelineItem::Contents);
  m_PipelineModel->setData(filterIndex, filter->getHumanLabel(), Qt::DisplayRole);
  m_PipelineModel->setItemType(filterIndex, PipelineItem::ItemType::Filter);
  m_PipelineModel->setFilter(filterIndex, filter);

  QModelIndexList list;
  list.push_back(filterIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFilterCommand::removeFilter(int filterIndex)
{
  m_PipelineModel->removeRow(filterIndex);
}

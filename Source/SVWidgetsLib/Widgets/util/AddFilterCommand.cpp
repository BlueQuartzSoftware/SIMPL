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

#include "AddFilterCommand.h"

#include <QtCore/QObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>

#include "SIMPLib/CoreFilters/Breakpoint.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"

#include "SVWidgetsLib/Widgets/BreakpointFilterWidget.h"
#include "SVWidgetsLib/Widgets/PipelineModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(AbstractFilter::Pointer filter, PipelineModel* model, int insertIndex, QString actionText, QUndoCommand* parent)
: QUndoCommand(parent)
, m_FilterCount(1)
, m_ActionText(actionText)
, m_PipelineModel(model)
{
  if(insertIndex < 0)
  {
    insertIndex = model->rowCount();
  }
  m_InsertIndex = insertIndex;

  setText(QObject::tr("\"%1 '%2'\"").arg(actionText).arg(filter->getHumanLabel()));

  m_Filters.push_back(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(std::vector<AbstractFilter::Pointer> filters, PipelineModel* model, int insertIndex, QString actionText, QUndoCommand* parent)
: QUndoCommand(parent)
, m_Filters(filters)
, m_FilterCount(filters.size())
, m_ActionText(actionText)
, m_PipelineModel(model)
{
  if(insertIndex < 0)
  {
    insertIndex = model->rowCount();
  }
  m_InsertIndex = insertIndex;

  setText(QObject::tr("\"%1 %2 Filters\"").arg(actionText).arg(filters.size()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::~AddFilterCommand() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::undo()
{
  m_PipelineModel->blockSignals(true);
  for(int i = 0; i < m_Filters.size(); i++)
  {
    QModelIndex filterIndex = m_PipelineModel->indexOfFilter(m_Filters[i]);

    removeFilter(filterIndex.row());
  }
  m_PipelineModel->blockSignals(false);

  //  m_PipelineView->reindexWidgetTitles();
  emit m_PipelineModel->pipelineDataChanged(QModelIndex());
  emit m_PipelineModel->preflightTriggered(QModelIndex(), m_PipelineModel);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::redo()
{
  int index = m_InsertIndex;
  m_PipelineModel->blockSignals(true);
  for(int i = 0; i < m_Filters.size(); i++)
  {
    addFilter(m_Filters[i], index);
    index++;
  }
  m_PipelineModel->blockSignals(false);

  //  m_PipelineView->reindexWidgetTitles();
  emit m_PipelineModel->pipelineDataChanged(QModelIndex());
  emit m_PipelineModel->preflightTriggered(QModelIndex(), m_PipelineModel);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::addFilter(AbstractFilter::Pointer filter, int insertionIndex)
{
  m_PipelineModel->insertRow(insertionIndex);
  QModelIndex filterIndex = m_PipelineModel->index(insertionIndex, PipelineItem::Name);
  m_PipelineModel->setData(filterIndex, filter->getHumanLabel(), Qt::DisplayRole);
  m_PipelineModel->setItemType(filterIndex, PipelineItem::ItemType::Filter);
  m_PipelineModel->setFilter(filterIndex, filter);

  QModelIndexList list;
  list.push_back(filterIndex);

  emit m_PipelineModel->preflightTriggered(QModelIndex(), m_PipelineModel);
  emit m_PipelineModel->pipelineDataChanged(QModelIndex());

  emit m_PipelineModel->statusMessageGenerated(QObject::tr("Added \"%1\" filter").arg(filter->getHumanLabel()));
  emit m_PipelineModel->standardOutputMessageGenerated(QObject::tr("Added \"%1\" filter").arg(filter->getHumanLabel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::removeFilter(int filterIndex)
{
  m_PipelineModel->removeRow(filterIndex);

  emit m_PipelineModel->preflightTriggered(QModelIndex(), m_PipelineModel);
}

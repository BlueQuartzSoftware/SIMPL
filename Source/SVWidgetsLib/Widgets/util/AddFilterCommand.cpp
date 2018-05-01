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
#include "SVWidgetsLib/Widgets/SVPipelineView.h"
#include "SVWidgetsLib/Widgets/PipelineModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(AbstractFilter::Pointer filter, SVPipelineView* view, int insertIndex, QString actionText, QUndoCommand* parent)
: QUndoCommand(parent)
, m_FilterCount(1)
, m_ActionText(actionText)
, m_PipelineView(view)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();

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
AddFilterCommand::AddFilterCommand(std::vector<AbstractFilter::Pointer> filters, SVPipelineView* view, int insertIndex, QString actionText, QUndoCommand* parent)
: QUndoCommand(parent)
, m_Filters(filters)
, m_FilterCount(filters.size())
, m_ActionText(actionText)
, m_PipelineView(view)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();

  if(insertIndex < 0)
  {
    insertIndex = model->rowCount();
  }
  m_InsertIndex = insertIndex;

  if (filters.size() == 1)
  {
    setText(QObject::tr("\"%1 '%2'\"").arg(actionText).arg(filters[0]->getHumanLabel()));
  }
  else
  {
    setText(QObject::tr("\"%1 %2 Filters\"").arg(actionText).arg(filters.size()));
  }
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
  if (m_Filters.size() <= 0)
  {
    return;
  }

  PipelineModel* model = m_PipelineView->getPipelineModel();

  for(int i = 0; i < m_Filters.size(); i++)
  {
    QModelIndex filterIndex = model->indexOfFilter(m_Filters[i]);

    removeFilter(filterIndex.row());
  }

  emit model->pipelineDataChanged(QModelIndex());
  emit m_PipelineView->preflightPipeline();

  QString statusMessage;
  if (m_Filters.size() > 1)
  {
    statusMessage = QObject::tr("Undo \"Added %1 filters\"").arg(m_Filters.size());
  }
  else
  {
    statusMessage = QObject::tr("Undo \"Added '%1' filter\"").arg(m_Filters[0]->getHumanLabel());
  }

  emit model->statusMessageGenerated(statusMessage);
  emit model->standardOutputMessageGenerated(statusMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::redo()
{
  if (m_Filters.size() <= 0)
  {
    return;
  }

  PipelineModel* model = m_PipelineView->getPipelineModel();

  int index = m_InsertIndex;
  for(int i = 0; i < m_Filters.size(); i++)
  {
    addFilter(m_Filters[i], index);
    index++;
  }

  emit m_PipelineView->preflightPipeline();
  emit m_PipelineView->pipelineChanged();

  QString statusMessage;
  if (m_Filters.size() > 1)
  {
    statusMessage = QObject::tr("Added %1 filters").arg(m_Filters.size());
  }
  else
  {
    statusMessage = QObject::tr("Added '%1' filter").arg(m_Filters[0]->getHumanLabel());
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

  emit model->statusMessageGenerated(statusMessage);
  emit model->standardOutputMessageGenerated(statusMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::addFilter(AbstractFilter::Pointer filter, int insertionIndex)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();

  model->insertRow(insertionIndex);

  QModelIndex filterIndex = model->index(insertionIndex, PipelineItem::Contents);
//  model->setData(filterIndex, filter->getHumanLabel(), Qt::DisplayRole);
  model->setItemType(filterIndex, PipelineItem::ItemType::Filter);
  model->setFilter(filterIndex, filter);

  if (filter->getEnabled() == false)
  {
    model->setWidgetState(filterIndex, PipelineItem::WidgetState::Disabled);
  }

  FilterInputWidget* fiw = model->filterInputWidget(filterIndex);

  QObject::connect(fiw, &FilterInputWidget::filterParametersChanged, [=] {
    m_PipelineView->preflightPipeline();
    emit m_PipelineView->filterParametersChanged(filter);
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::removeFilter(int filterIndex)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();
  QModelIndex index = model->index(filterIndex, PipelineItem::Contents);

  FilterInputWidget* fiw = model->filterInputWidget(index);

  QObject::disconnect(fiw, &FilterInputWidget::filterParametersChanged, 0, 0);

  model->removeRow(filterIndex);
}

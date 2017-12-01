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

#include <QtCore/QObject>

#include "SVWidgetsLib/Widgets/PipelineFilterObject.h"
#include "SVWidgetsLib/Widgets/PipelineView.h"
#include "SVWidgetsLib/Widgets/SIMPLViewMenuItems.h"

#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFilterCommand::RemoveFilterCommand(PipelineFilterObject* fw, PipelineView* pipelineView, QString actionText, QUuid prevNodeId, QUuid nextNodeId, QUndoCommand* parent)
: QUndoCommand(parent)
, m_PipelineView(pipelineView)
, m_PrevNodeId(prevNodeId)
, m_NextNodeId(nextNodeId)
{
  if(nullptr == fw || nullptr == pipelineView)
  {
    return;
  }

  m_FilterPositions.push_back(pipelineView->valueOfFilterWidget(fw));

  setText(QObject::tr("\"%1 '%2'\"").arg(actionText).arg(fw->getHumanLabel()));

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  pipeline->pushBack(fw->getFilter());
  JsonFilterParametersWriter::Pointer jsonWriter = JsonFilterParametersWriter::New();
  m_JsonString = jsonWriter->writePipelineToString(pipeline, "");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFilterCommand::RemoveFilterCommand(QList<PipelineFilterObject*> filterObjects, PipelineView* pipelineView, QString actionText, QUuid prevNodeId, QUuid nextNodeId, QUndoCommand* parent)
: QUndoCommand(parent)
, m_PipelineView(pipelineView)
, m_PrevNodeId(prevNodeId)
, m_NextNodeId(nextNodeId)
{
  if(NULL == pipelineView)
  {
    return;
  }

  setText(QObject::tr("\"%1 %2 Filters\"").arg(actionText).arg(filterObjects.size()));

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  for(int i = 0; i < filterObjects.size(); i++)
  {
    m_FilterPositions.push_back(m_PipelineView->valueOfFilterWidget(filterObjects[i]));
    pipeline->pushBack(filterObjects[i]->getFilter());
  }

  JsonFilterParametersWriter::Pointer jsonWriter = JsonFilterParametersWriter::New();
  m_JsonString = jsonWriter->writePipelineToString(pipeline, "");
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
  QList<QVariant> ascendingList = m_FilterPositions;
  std::sort(ascendingList.begin(), ascendingList.end());

  JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
  jsonReader->setMaxFilterIndex(ascendingList.size());
  FilterPipeline::Pointer pipeline = jsonReader->readPipelineFromString(m_JsonString);
  FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();

  for(int i = 0; i < container.size(); i++)
  {
    PipelineFilterObject* filterObject = m_PipelineView->createFilterObjectFromFilter(container[i]);
    m_PipelineView->addFilterObject(filterObject, ascendingList[i]);
  }

  m_PipelineView->reindexWidgetTitles();
  m_PipelineView->recheckWindowTitleAndModification();
  m_PipelineView->preflightPipeline();

  SIMPLViewMenuItems* menuItems = SIMPLViewMenuItems::Instance();
  menuItems->getActionClearPipeline()->setEnabled(true);
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
  QList<QVariant> descendingList = m_FilterPositions;
  qSort(descendingList.begin(), descendingList.end(), variantCompare);

  for(int i = 0; i < descendingList.size(); i++)
  {
    m_PipelineView->removeFilterObject(m_PipelineView->filterObjectAt(descendingList[i]));
  }

  m_PipelineView->reindexWidgetTitles();
  m_PipelineView->recheckWindowTitleAndModification();
  m_PipelineView->preflightPipeline();

  if (m_PipelineView->getFilterPipeline()->getFilterContainer().size() <= 0)
  {
    SIMPLViewMenuItems* menuItems = SIMPLViewMenuItems::Instance();
    menuItems->getActionClearPipeline()->setDisabled(true);
  }
}

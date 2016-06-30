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

#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/CoreFilters/Breakpoint.h"


#include "SVWidgetsLib/Widgets/PipelineFilterObject.h"
#include "SVWidgetsLib/Widgets/SVPipelineViewWidget.h"
#include "SVWidgetsLib/Widgets/BreakpointFilterWidget.h"

// WORKSPACE: FilterWidgetGeometry & BreakpointFilter

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(AbstractFilter::Pointer filter, PipelineView* destination, QString actionText, QVariant value, QUuid previousNode, QUuid nextNode, QUndoCommand* parent) :
  QUndoCommand(parent),
  m_ActionText(actionText),
  m_Destination(destination),
  m_Value(value),
  m_FilterWidgetGeometry(QRect()),
  m_PreviousNodeId(previousNode),
  m_NextNodeId(nextNode)
{
  if (m_Value.canConvert<int>())
  {
    int index = value.toInt();
    if (index < 0)
    {
      m_Value.setValue(destination->filterCount());
    }
  }

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  pipeline->pushBack(filter);
  JsonFilterParametersWriter::Pointer jsonWriter = JsonFilterParametersWriter::New();
  m_JsonString = jsonWriter->writePipelineToString(pipeline, "");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::~AddFilterCommand()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::undo()
{
  m_Destination->removeFilterObject(m_Destination->filterObjectAt(m_Value), false);

  m_Destination->preflightPipeline();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::redo()
{
  JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
  FilterPipeline::Pointer pipeline = jsonReader->readPipelineFromString(m_JsonString);
  AbstractFilter::Pointer filter = pipeline->getFilterContainer()[0];

  setText(QObject::tr("\"%1 '%2'\"").arg(m_ActionText).arg(filter->getHumanLabel()));

  m_Destination->addFilter(filter, m_Value, false, m_PreviousNodeId, m_NextNodeId);
}




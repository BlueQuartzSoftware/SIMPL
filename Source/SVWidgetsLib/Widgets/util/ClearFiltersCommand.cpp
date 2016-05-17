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

#include "ClearFiltersCommand.h"

#include <QtCore/QObject>

#include "SVPipelineViewWidget.h"

#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ClearFiltersCommand::ClearFiltersCommand(SVPipelineViewWidget* pipelineView, QUndoCommand* parent) :
QUndoCommand(parent),
m_PipelineView(pipelineView)
{
  setText(QObject::tr("\"Clear All Filters\""));

  m_JsonString = JsonFilterParametersWriter::WritePipelineToString(m_PipelineView->getFilterPipeline(), "Pipeline");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ClearFiltersCommand::~ClearFiltersCommand()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ClearFiltersCommand::undo()
{
  FilterPipeline::Pointer pipeline = JsonFilterParametersReader::ReadPipelineFromString(m_JsonString);
  FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();
  for (int i = 0; i < container.size(); i++)
  {
    SVPipelineFilterWidget* filterWidget = new SVPipelineFilterWidget(container[i], NULL, m_PipelineView);
    m_PipelineView->addFilterWidget(filterWidget, false);
  }

  m_PipelineView->clearSelectedFilterObjects();

  if (m_PipelineView->filterCount() > 0)
  {
    m_PipelineView->setSelectedFilterObject(m_PipelineView->filterObjectAt(0), Qt::NoModifier);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ClearFiltersCommand::redo()
{
  // Clear the filter input widget
  emit m_PipelineView->filterInputWidgetNeedsCleared();

  m_PipelineView->clearWidgets();
}




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

#include <QtCore/QObject>

#include "SVWidgetsLib/Widgets/PipelineFilterObject.h"
#include "SVWidgetsLib/Widgets/PipelineView.h"

#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveFilterCommand::MoveFilterCommand(PipelineFilterObject* filterWidget, QVariant origin, QVariant destination, PipelineView* pipelineView, QUndoCommand* parent)
: QUndoCommand(parent)
, m_OriginView(pipelineView)
, m_DestinationView(pipelineView)
, m_Destination(destination)
, m_FirstRun(true)
{
  m_FilterWidgets.push_back(std::make_pair(origin.toInt(), filterWidget));

  setText(QObject::tr("\"Move '%1'\"").arg(filterWidget->getFilter()->getHumanLabel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveFilterCommand::MoveFilterCommand(QList<std::pair<int, PipelineFilterObject*>> filterWidgets, QVariant destination, PipelineView* pipelineView, QUndoCommand* parent)
: QUndoCommand(parent)
, m_FilterWidgets(filterWidgets)
, m_OriginView(pipelineView)
, m_DestinationView(pipelineView)
, m_Destination(destination)
, m_FirstRun(true)
{
  setText(QObject::tr("\"Move '%1' Filters\"").arg(filterWidgets.size()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveFilterCommand::MoveFilterCommand(QList<std::pair<int, PipelineFilterObject*>> filterWidgets, QVariant destination, PipelineView* originView, PipelineView* destinationView, QUndoCommand* parent)
: QUndoCommand(parent)
, m_FilterWidgets(filterWidgets)
, m_OriginView(originView)
, m_DestinationView(destinationView)
, m_Destination(destination)
, m_FirstRun(true)
{
  setText(QObject::tr("\"Move '%1' Filters\"").arg(filterWidgets.size()));
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
  // If the destination is not valid, do not do anything
  bool ok;
  m_Destination.toInt(&ok);
  if(ok == false)
  {
    return;
  }

  // Remove all moved items first to correct any potential ordering mistakes
  for(int i = 0; i < m_FilterWidgets.size(); i++)
  {
    m_DestinationView->removeFilterObject(m_FilterWidgets[i].second, false);
  }

  // After removing all moved items, items may now be reinserted at the correct index
  for(int i = 0; i < m_FilterWidgets.size(); i++)
  {
    PipelineFilterObject* filterObject = m_FilterWidgets[i].second;

    m_OriginView->addFilterObject(filterObject, m_FilterWidgets[i].first);
  }

  // After adding all moved items, items may now be selected
  for(int i = 0; i < m_FilterWidgets.size(); i++)
  {
    PipelineFilterObject* filterObject = m_FilterWidgets[i].second;

    m_OriginView->setSelectedFilterObject(filterObject, Qt::ControlModifier);
  }

  m_OriginView->reindexWidgetTitles();
  m_OriginView->recheckWindowTitleAndModification();
  m_OriginView->preflightPipeline();

  // index the destination view as well if it is not the same as the origin view
  if(m_OriginView != m_DestinationView)
  {
    m_DestinationView->reindexWidgetTitles();
    m_DestinationView->recheckWindowTitleAndModification();
    m_DestinationView->preflightPipeline();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveFilterCommand::redo()
{
  // If the destination is not valid, do not do anything
  bool ok;
  int destinationIndex = m_Destination.toInt(&ok);
  if(ok == false)
  {
    return;
  }

  // Check if this is the first occurance of the QUndoCommand,
  // If it is not, remove all moved items first
  if(m_FirstRun == true)
  {
    m_FirstRun = false;
  }
  else
  {
    // Remove all moved items first to correct any potential ordering mistakes
    for(int i = 0; i < m_FilterWidgets.size(); i++)
    {
      PipelineFilterObject* filterObject = m_FilterWidgets[i].second;

      m_OriginView->removeFilterObject(filterObject, false);
    }
  }

  // After removing all moved items, items may now be reinserted at the correct index
  for(int i = 0; i < m_FilterWidgets.size(); i++)
  {
    PipelineFilterObject* filterObject = m_FilterWidgets[i].second;

    m_DestinationView->addFilterObject(filterObject, destinationIndex + i);
  }

  // After adding all moved items, items may now be selected
  for(int i = 0; i < m_FilterWidgets.size(); i++)
  {
    PipelineFilterObject* filterObject = m_FilterWidgets[i].second;
    m_DestinationView->setSelectedFilterObject(filterObject, Qt::ControlModifier);
  }

  m_OriginView->reindexWidgetTitles();
  m_OriginView->recheckWindowTitleAndModification();
  m_OriginView->preflightPipeline();

  // index the destination view as well if it is not the same as the origin view
  if(m_OriginView != m_DestinationView)
  {
    m_DestinationView->reindexWidgetTitles();
    m_DestinationView->recheckWindowTitleAndModification();
    m_DestinationView->preflightPipeline();
  }
}

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
AddFilterCommand::AddFilterCommand(AbstractFilter::Pointer filter, PipelineView* destination, QString actionText, QVariant value, bool connectToStart, QUndoCommand* parent) :
  QUndoCommand(parent),
  m_ActionText(actionText),
  m_Destination(destination),
  m_Value(value),
  m_FilterWidgetGeometry(QRect()),
  m_ConnectToStart(connectToStart)
{
  if (m_Value.canConvert<int>())
  {
    int index = value.toInt();
    if (index < 0)
    {
      m_Value.setValue(destination->filterCount());
    }
  }

  if (destination->filterCount() > 0)
  {
    //m_FilterWidgetGeometry = destination->filterObjectAt(m_Value)->geometry();
  }

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  pipeline->pushBack(filter);

  m_JsonString = JsonFilterParametersWriter::WritePipelineToString(pipeline, "Pipeline");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(PipelineFilterObject* filterWidget, PipelineView* destination, QString actionText, QVariant value, bool connectToStart, QUndoCommand* parent) :
  QUndoCommand(parent),
  m_ActionText(actionText),
  m_Destination(destination),
  m_Value(value),
  m_FilterWidgetGeometry(QRect()),
  m_ConnectToStart(connectToStart)
{
  if (m_Value.canConvert<int>())
  {
    int index = value.toInt();
    if (index < 0)
    {
      m_Value.setValue(destination->filterCount());
    }
  }

  if (destination->filterCount() > 0)
  {
    //m_FilterWidgetGeometry = destination->filterObjectAt(m_Value)->geometry();
  }

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  pipeline->pushBack(filterWidget->getFilter());
  delete filterWidget;

  m_JsonString = JsonFilterParametersWriter::WritePipelineToString(pipeline, "Pipeline");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(const QString &jsonString, PipelineView* destination, QString actionText, QVariant value, bool connectToStart, QUndoCommand* parent) :
  QUndoCommand(parent),
  m_JsonString(jsonString),
  m_ActionText(actionText),
  m_Destination(destination),
  m_Value(value),
  m_FilterWidgetGeometry(QRect()),
  m_ConnectToStart(connectToStart)
{
  if (m_Value.canConvert<int>())
  {
    int index = value.toInt();
    if (index < 0)
    {
      m_Value.setValue(destination->filterCount());
    }
  }

  if (destination->filterCount() > 0)
  {
    //m_FilterWidgetGeometry = destination->filterObjectAt(m_Value)->geometry();
  }
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
  FilterPipeline::Pointer pipeline = JsonFilterParametersReader::ReadPipelineFromString(m_JsonString);
  if (pipeline == nullptr) { return; }

  FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();

  AbstractFilter::Pointer filter = container[0];

  setText(QObject::tr("\"%1 '%2'\"").arg(m_ActionText).arg(filter->getHumanLabel()));

  m_Destination->addFilter(filter, m_Value, false, m_ConnectToStart);

//  for (int i = 0; i < container.size(); i++)
//  {
////    if (std::dynamic_pointer_cast<Breakpoint>(container[i]))
////    {
////      BreakpointFilterWidget* filterWidget = new BreakpointFilterWidget(container[i], NULL, NULL);
////      m_Destination->addFilterWidget(filterWidget, insertValue, false);
////    }
////    else
////    {
//      //SVPipelineFilterWidget* filterWidget = new SVPipelineFilterWidget(container[i], NULL, NULL);
//      m_Destination->addFilters(container, insertValue, false);
////    }

//    m_AddValues.push_back(insertValue);

//    if (insertValue.canConvert<int>())
//    {
//      int integer = insertValue.toInt();
//      integer++;
//      insertValue.setValue(integer);
//    }
//    else if (insertValue.canConvert<QPointF>())
//    {
//      QPointF pos = insertValue.toPointF();
//      //pos.setX(pos.x() + m_FilterWidgetGeometry.width());
//      //pos.setY(pos.y() + m_FilterWidgetGeometry.height());
//      //pos.setX(pos.x() + 20);
//      pos.setY(pos.y() + 30);
//      insertValue.setValue(pos);
//    }
//  }

  m_Destination->setSelectedFilterObject(m_Destination->filterObjectAt(m_Value), Qt::NoModifier);
}




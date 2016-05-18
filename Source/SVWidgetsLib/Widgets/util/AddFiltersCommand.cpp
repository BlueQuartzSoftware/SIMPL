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

#include "AddFiltersCommand.h"

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
AddFiltersCommand::AddFiltersCommand(AbstractFilter::Pointer filter, PipelineView* destination, QString actionText, QVariant value, bool connectToStart, QUndoCommand* parent) :
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
AddFiltersCommand::AddFiltersCommand(QList<AbstractFilter::Pointer> filters, PipelineView* destination, QString actionText, QVariant value, bool connectToStart, QUndoCommand* parent) :
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
  for (int i=0; i<filters.size(); i++)
  {
    pipeline->pushBack(filters[i]);
  }

  m_JsonString = JsonFilterParametersWriter::WritePipelineToString(pipeline, "Pipeline");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFiltersCommand::AddFiltersCommand(PipelineFilterObject* filterWidget, PipelineView* destination, QString actionText, QVariant value, bool connectToStart, QUndoCommand* parent) :
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
AddFiltersCommand::AddFiltersCommand(QList<PipelineFilterObject*> filterWidgets, PipelineView* destination, QString actionText, QVariant value, bool connectToStart, QUndoCommand* parent) :
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
  for (int i=0; i<filterWidgets.size(); i++)
  {
    pipeline->pushBack(filterWidgets[i]->getFilter());
    delete filterWidgets[i];
  }

  m_JsonString = JsonFilterParametersWriter::WritePipelineToString(pipeline, "Pipeline");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFiltersCommand::AddFiltersCommand(const QString &jsonString, PipelineView* destination, QString actionText, QVariant value, bool connectToStart, QUndoCommand* parent) :
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
AddFiltersCommand::~AddFiltersCommand()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFiltersCommand::undo()
{
  for (int i = 0; i < m_AddedFilters.size(); i++)
  {
    m_Destination->removeFilterObject(m_AddedFilters[i], false);
  }

  m_AddedFilters.clear();

  m_Destination->preflightPipeline();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFiltersCommand::redo()
{
  FilterPipeline::Pointer pipeline = JsonFilterParametersReader::ReadPipelineFromString(m_JsonString);
  if (pipeline == nullptr) { return; }

  FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();

  if (container.size() == 1)
  {
    setText(QObject::tr("\"%1 '%2'\"").arg(m_ActionText).arg(container[0]->getHumanLabel()));
  }
  else
  {
    setText(QObject::tr("\"%1 %2 Filter Widgets\"").arg(m_ActionText).arg(container.size()));
  }

  m_AddedFilters = m_Destination->addFilters(container, m_Value, false, m_ConnectToStart);

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<PipelineFilterObject*> AddFiltersCommand::getAddedFilters()
{
  return m_AddedFilters;
}




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
#include "SVWidgetsLib/Widgets/PipelineFilterObject.h"
#include "SVWidgetsLib/Widgets/SVPipelineViewWidget.h"
#include "SVWidgetsLib/Widgets/SIMPLViewMenuItems.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(AbstractFilter::Pointer filter, PipelineView* destination, QString actionText, QVariant value, QUuid previousNode, QUuid nextNode, QUndoCommand* parent)
: QUndoCommand(parent)
, m_FilterCount(1)
, m_ActionText(actionText)
, m_Destination(destination)
, m_Value(value)
, m_PreviousNodeId(previousNode)
, m_NextNodeId(nextNode)
{
  if(m_Value.canConvert<int>())
  {
    int index = value.toInt();
    if(index < 0)
    {
      m_Value.setValue(destination->filterCount());
    }
  }

  setText(QObject::tr("\"%1 '%2'\"").arg(actionText).arg(filter->getHumanLabel()));

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  pipeline->pushBack(filter);
  JsonFilterParametersWriter::Pointer jsonWriter = JsonFilterParametersWriter::New();
  m_JsonString = jsonWriter->writePipelineToString(pipeline, "");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(QList<AbstractFilter::Pointer> filters, PipelineView* destination, QString actionText, QVariant value, QUuid previousNode, QUuid nextNode, QUndoCommand* parent)
: QUndoCommand(parent)
, m_FilterCount(filters.size())
, m_ActionText(actionText)
, m_Destination(destination)
, m_Value(value)
, m_PreviousNodeId(previousNode)
, m_NextNodeId(nextNode)
{
  if(m_Value.canConvert<int>())
  {
    int index = value.toInt();
    if(index < 0)
    {
      m_Value.setValue(destination->filterCount());
    }
  }

  setText(QObject::tr("\"%1 %2 Filters\"").arg(actionText).arg(filters.size()));

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  for(int i = 0; i < filters.size(); i++)
  {
    pipeline->pushBack(filters[i]);
  }
  JsonFilterParametersWriter::Pointer jsonWriter = JsonFilterParametersWriter::New();
  m_JsonString = jsonWriter->writePipelineToString(pipeline, "");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(QString jsonString, PipelineView* destination, QString actionText, QVariant value, QUuid previousNode, QUuid nextNode, QUndoCommand* parent)
: QUndoCommand(parent)
, m_ActionText(actionText)
, m_Destination(destination)
, m_Value(value)
, m_PreviousNodeId(previousNode)
, m_NextNodeId(nextNode)
{
  if(m_Value.canConvert<int>())
  {
    int index = value.toInt();
    if(index < 0)
    {
      m_Value.setValue(destination->filterCount());
    }
  }

  QJsonParseError parseError;
  QByteArray byteArray = QByteArray::fromStdString(jsonString.toStdString());
  QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
  if(parseError.error == QJsonParseError::NoError)
  {
    QJsonObject rootObject = doc.object();
    
    FilterPipeline::Pointer pipeline = FilterPipeline::FromJson(rootObject);
    
    FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();
    
    m_FilterCount = container.size();
    
    setText(QObject::tr("\"%1 %2 Filters\"").arg(actionText).arg(m_FilterCount));
    
    m_JsonString = jsonString;
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
  if(m_Value.canConvert<int>())
  {
    int index = m_Value.toInt();

    for(int i = 0; i < m_FilterCount; i++)
    {
      m_Destination->removeFilterObject(m_Destination->filterObjectAt(index));
      // No need to increment index, because after removing the filter above, the next filter
      // will move up to this index
    }
  }
  else if(m_Value.canConvert<QPointF>())
  {
    QPointF pointF = m_Value.toPointF();

    for(int i = 0; i < m_FilterCount; i++)
    {
      m_Destination->removeFilterObject(m_Destination->filterObjectAt(pointF));
      pointF.setX(pointF.x() + 50);
      pointF.setY(pointF.y() + 50);
    }
  }

  m_Destination->reindexWidgetTitles();
  m_Destination->recheckWindowTitleAndModification();
  m_Destination->preflightPipeline();

  if (m_Destination->getFilterPipeline()->getFilterContainer().size() <= 0)
  {
    SIMPLViewMenuItems* menuItems = SIMPLViewMenuItems::Instance();
    menuItems->getActionClearPipeline()->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::redo()
{
  QJsonParseError parseError;
  QByteArray byteArray = QByteArray::fromStdString(m_JsonString.toStdString());
  QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
  if(parseError.error != QJsonParseError::NoError)
  {
    return;
  }
  QJsonObject rootObject = doc.object();
  
  FilterPipeline::Pointer pipeline = FilterPipeline::FromJson(rootObject);
  
  FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();
  
  PipelineFilterObject* filterObject = nullptr;
  if(m_Value.canConvert<int>())
  {
    int index = m_Value.toInt();

    for(int i = 0; i < container.size(); i++)
    {
      filterObject = m_Destination->createFilterObjectFromFilter(container[i]);
      m_Destination->addFilterObject(filterObject, index);
      index++;
    }
  }
  else if(m_Value.canConvert<QPointF>())
  {
    QPointF pointF = m_Value.toPointF();

    for(int i = 0; i < container.size(); i++)
    {
      filterObject = m_Destination->createFilterObjectFromFilter(container[i]);
      m_Destination->addFilterObject(filterObject, pointF);
      pointF.setX(pointF.x() + 50);
      pointF.setY(pointF.y() + 50);
    }
  }

  m_Destination->reindexWidgetTitles();
  m_Destination->recheckWindowTitleAndModification();
  m_Destination->preflightPipeline();
  SVPipelineFilterWidget* filterWidget = dynamic_cast<SVPipelineFilterWidget*>(filterObject);
  if(filterWidget)
  {
    emit filterWidget->filterWidgetPressed(filterObject, qApp->queryKeyboardModifiers());
  }

  SIMPLViewMenuItems* menuItems = SIMPLViewMenuItems::Instance();
  menuItems->getActionClearPipeline()->setEnabled(true);
}

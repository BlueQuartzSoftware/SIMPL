/* ============================================================================
* Copyright (c) 2017 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software nor the names of its
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
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PipelineTreeController.h"

#include "SVWidgetsLib/Widgets/BreakpointFilterWidget.h"
#include "SVWidgetsLib/Widgets/PipelineTreeModel.h"
#include "SVWidgetsLib/Widgets/PipelineTreeItem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeController::PipelineTreeController(QObject* parent)
: QObject(parent)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeController::~PipelineTreeController()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::preflightPipeline(const QModelIndex &pipelineIndex)
{
  if(m_BlockPreflight || pipelineIndex.isValid() == false)
  {
    return;
  }

  emit pipelineIssuesCleared();
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = getFilterPipeline(pipelineIndex);

  PipelineTreeModel* model = PipelineTreeModel::Instance();

  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
  for(int i = 0; i < filters.size(); i++)
  {
    filters.at(i)->setErrorCondition(0);
    filters.at(i)->setCancel(false);

    QModelIndex childIndex = model->index(i, PipelineTreeItem::Name, pipelineIndex);
    if(childIndex.isValid())
    {
      model->setErrorState(childIndex, PipelineTreeItem::ErrorState::Ok);
      if(PipelineTreeItem::WidgetState::Disabled != model->widgetState(childIndex))
      {
        model->setWidgetState(childIndex, PipelineTreeItem::WidgetState::Ready);
      }
    }
  }

//  QSharedPointer<ProgressDialog> progressDialog(new ProgressDialog());
//  progressDialog->setWindowTitle("Pipeline Preflighting");
//  QString msg = QString("Please wait for %1 filters to preflight...").arg(pipeline->getFilterContainer().count());
//  progressDialog->setLabelText(msg);
//  progressDialog->show();
//  progressDialog->raise();
//  progressDialog->activateWindow();

  // Preflight the pipeline
  int err = pipeline->preflightPipeline();
  if(err < 0)
  {
    // FIXME: Implement error handling.
  }

  int count = pipeline->getFilterContainer().size();
  // Now that the preflight has been executed loop through the filters and check their error condition and set the
  // outline on the filter widget if there were errors or warnings
  for(qint32 i = 0; i < count; ++i)
  {
    QModelIndex childIndex = model->index(i, PipelineTreeItem::Name, pipelineIndex);
    if(childIndex.isValid())
    {
      AbstractFilter::Pointer filter = model->filter(childIndex);
      if(filter->getWarningCondition() < 0)
      {
        model->setErrorState(childIndex, PipelineTreeItem::ErrorState::Warning);
      }
      if(filter->getErrorCondition() < 0)
      {
        model->setErrorState(childIndex, PipelineTreeItem::ErrorState::Error);
      }
    }
  }
  emit preflightFinished(err);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer PipelineTreeController::getFilterPipeline(const QModelIndex &pipelineIndex)
{
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  PipelineTreeModel* model = PipelineTreeModel::Instance();

  qint32 count = model->rowCount(pipelineIndex);
  for(qint32 i = 0; i < count; ++i)
  {
    QModelIndex childIndex = model->index(i, PipelineTreeItem::Name, pipelineIndex);
    if(childIndex.isValid())
    {
      AbstractFilter::Pointer filter = model->filter(childIndex);
      Breakpoint::Pointer breakpoint = std::dynamic_pointer_cast<Breakpoint>(filter);
      if(nullptr != breakpoint)
      {
        connect(pipeline.get(), SIGNAL(pipelineCanceled()), breakpoint.get(), SLOT(resumePipeline()));
      }

      pipeline->pushBack(filter);
    }
  }
  for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
  {
    pipeline->addMessageReceiver(m_PipelineMessageObservers[i]);
  }
  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::addFilter(AbstractFilter::Pointer filter)
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();
  if (m_ActivePipelineIndex.isValid() == false)
  {
    addPipeline("Untitled Pipeline", true);
  }

  int row = model->rowCount(m_ActivePipelineIndex);
  model->insertRow(row, m_ActivePipelineIndex);
  QModelIndex filterIndex = model->index(row, PipelineTreeItem::Name, m_ActivePipelineIndex);
  model->setData(filterIndex, filter->getHumanLabel(), Qt::DisplayRole);
  model->setItemType(filterIndex, PipelineTreeItem::ItemType::Filter);
  model->setFilter(filterIndex, filter);

  preflightPipeline(m_ActivePipelineIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::addPipeline(const QString &pipelineName, bool setAsActive)
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();

  int row = model->rowCount();
  model->insertRow(row);
  QModelIndex pipelineIndex = model->index(row, PipelineTreeItem::Name);
  model->setData(pipelineIndex, pipelineName, Qt::DisplayRole);
  model->setItemType(pipelineIndex, PipelineTreeItem::ItemType::Pipeline);

  if (setAsActive == true)
  {
    updateActivePipeline(pipelineIndex);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::updateActivePipeline(const QModelIndex &pipelineIdx)
{
  PipelineTreeModel* model = PipelineTreeModel::Instance();

  model->setActivePipeline(m_ActivePipelineIndex, false);
  model->setActivePipeline(pipelineIdx, true);

  m_ActivePipelineIndex = pipelineIdx;

  if (m_ActivePipelineIndex.isValid() == true)
  {
    preflightPipeline(m_ActivePipelineIndex);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::blockPreflightSignals(bool b)
{
  if(b)
  {
    m_BlockPreflightStack.push(b);
  }
  else
  {
    m_BlockPreflightStack.pop();
  }

  m_BlockPreflight = (m_BlockPreflightStack.size() > 0) ? true : false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::addPipelineMessageObserver(QObject* pipelineMessageObserver)
{
  m_PipelineMessageObservers.push_back(pipelineMessageObserver);
}



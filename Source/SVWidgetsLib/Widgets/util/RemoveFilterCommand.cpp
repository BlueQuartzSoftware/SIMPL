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

#include "SVWidgetsLib/Animations/PipelineItemSlideAnimation.h"
#include "SVWidgetsLib/Widgets/FilterInputWidget.h"
#include "SVWidgetsLib/Widgets/PipelineModel.h"
#include "SVWidgetsLib/Widgets/SVPipelineView.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFilterCommand::RemoveFilterCommand(AbstractFilter::Pointer filter, SVPipelineView *view, QString actionText, bool useAnimationOnFirstRun, QUndoCommand* parent)
  : QUndoCommand(parent)
  , m_PipelineView(view)
  , m_UseAnimationOnFirstRun(useAnimationOnFirstRun)
{
  if(nullptr == filter || nullptr == view)
  {
    return;
  }

  setText(QObject::tr("\"%1 '%2'\"").arg(actionText).arg(filter->getHumanLabel()));

  m_Filters.push_back(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFilterCommand::RemoveFilterCommand(std::vector<AbstractFilter::Pointer> filters, SVPipelineView *view, QString actionText, bool useAnimationOnFirstRun, QUndoCommand* parent)
  : QUndoCommand(parent)
  , m_PipelineView(view)
  , m_Filters(filters)
  , m_UseAnimationOnFirstRun(useAnimationOnFirstRun)
{
  if(nullptr == view)
  {
    return;
  }

  setText(QObject::tr("\"%1 %2 Filters\"").arg(actionText).arg(filters.size()));
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
  for(size_t i = 0; i < m_RemovalIndexes.size(); i++)
  {
    int insertIndex = m_RemovalIndexes[i];
    AbstractFilter::Pointer filter = m_Filters[i];

    addFilter(filter, insertIndex);
  }

  PipelineModel* model = m_PipelineView->getPipelineModel();
  QModelIndex firstAddedIndex = model->index(m_RemovalIndexes.front(), PipelineItem::Contents);
  m_PipelineView->scrollTo(firstAddedIndex, QAbstractItemView::PositionAtTop);

  m_RemovalIndexes.clear();

  m_PipelineView->preflightPipeline();

  emit m_PipelineView->pipelineChanged();

  QString statusMessage;
  if (m_Filters.size() > 1)
  {
    statusMessage = QObject::tr("Undo \"Removed %1 filters\"").arg(m_Filters.size());
  }
  else
  {
    statusMessage = QObject::tr("Undo \"Removed '%1' filter\"").arg(m_Filters[0]->getHumanLabel());
  }

  emit m_PipelineView->statusMessage(statusMessage);
  emit m_PipelineView->stdOutMessage(statusMessage);
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
  for(size_t i = 0; i < m_Filters.size(); i++)
  {
    removeFilter(m_Filters[i]);
  }

//  std::sort(m_RemovalIndexes.front(), m_RemovalIndexes.back());

  QString statusMessage;
  if (m_Filters.size() > 1)
  {
    statusMessage = QObject::tr("Removed %1 filters").arg(m_Filters.size());
  }
  else
  {
    statusMessage = QObject::tr("Removed '%1' filter").arg(m_Filters[0]->getHumanLabel());
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

  m_PipelineView->preflightPipeline();

  emit m_PipelineView->pipelineChanged();

  emit m_PipelineView->statusMessage(statusMessage);
  emit m_PipelineView->stdOutMessage(statusMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFilterCommand::addFilter(AbstractFilter::Pointer filter, int insertionIndex)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();

  model->insertRow(insertionIndex);
  QModelIndex filterIndex = model->index(insertionIndex, PipelineItem::Contents);
  model->setData(filterIndex, static_cast<int>(PipelineItem::ItemType::Filter), PipelineModel::ItemTypeRole);
  model->setFilter(filterIndex, filter);

  connectFilterSignalsSlots(filter);

  if (filter->getEnabled() == false)
  {
    model->setData(filterIndex, static_cast<int>(PipelineItem::WidgetState::Disabled), PipelineModel::WidgetStateRole);
  }

  QRect filterRect = m_PipelineView->visualRect(filterIndex);

  PipelineItemSlideAnimation* animation = new PipelineItemSlideAnimation(model, QPersistentModelIndex(filterIndex), filterRect.width(), PipelineItemSlideAnimation::AnimationDirection::Right);
  animation->start(QAbstractAnimation::DeleteWhenStopped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFilterCommand::removeFilter(AbstractFilter::Pointer filter)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();

  disconnectFilterSignalsSlots(filter);

  QModelIndex index = model->indexOfFilter(filter.get());
  m_RemovalIndexes.push_back(index.row());

  QPersistentModelIndex persistentIndex = index;

  QRect filterRect = m_PipelineView->visualRect(index);

  if (m_UseAnimationOnFirstRun == false && m_FirstRun == true)
  {
    model->removeRow(persistentIndex.row());
  }
  else
  {
    PipelineItemSlideAnimation* animation = new PipelineItemSlideAnimation(model, persistentIndex, filterRect.width(), PipelineItemSlideAnimation::AnimationDirection::Left);
    QObject::connect(animation, &PipelineItemSlideAnimation::finished, [=] {
      model->removeRow(persistentIndex.row());
    });
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFilterCommand::connectFilterSignalsSlots(AbstractFilter::Pointer filter)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();
  QModelIndex index = model->indexOfFilter(filter.get());

  QObject::connect(filter.get(), SIGNAL(filterCompleted(AbstractFilter*)), m_PipelineView, SLOT(listenFilterCompleted(AbstractFilter*)));

  QObject::connect(filter.get(), SIGNAL(filterInProgress(AbstractFilter*)), m_PipelineView, SLOT(listenFilterInProgress(AbstractFilter*)));

  FilterInputWidget* fiw = model->filterInputWidget(index);

  QObject::connect(fiw, &FilterInputWidget::filterParametersChanged, [=] {
    m_PipelineView->preflightPipeline();
    emit m_PipelineView->filterParametersChanged(filter);
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFilterCommand::disconnectFilterSignalsSlots(AbstractFilter::Pointer filter)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();
  QModelIndex index = model->indexOfFilter(filter.get());

  QObject::disconnect(filter.get(), &AbstractFilter::filterCompleted, 0, 0);

  QObject::disconnect(filter.get(), &AbstractFilter::filterInProgress, 0, 0);

  FilterInputWidget* fiw = model->filterInputWidget(index);

  QObject::disconnect(fiw, &FilterInputWidget::filterParametersChanged, 0, 0);
}

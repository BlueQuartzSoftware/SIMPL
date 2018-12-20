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
#include "SVWidgetsLib/Widgets/SVStyle.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFilterCommand::RemoveFilterCommand(AbstractFilter::Pointer filter, SVPipelineView* view, QString actionText, bool useAnimationOnFirstRun, QUndoCommand* parent)
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

  PipelineModel* model = m_PipelineView->getPipelineModel();

  QModelIndex index = model->indexOfFilter(filter.get());
  m_FilterRows.push_back(index.row());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFilterCommand::RemoveFilterCommand(std::vector<AbstractFilter::Pointer> filters, SVPipelineView* view, QString actionText, bool useAnimationOnFirstRun, QUndoCommand* parent)
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

  PipelineModel* model = m_PipelineView->getPipelineModel();

  for(size_t i = 0; i < m_Filters.size(); i++)
  {
    QModelIndex index = model->indexOfFilter(m_Filters[i].get());
    m_FilterRows.push_back(index.row());
  }
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
  for(size_t i = 0; i < m_FilterRows.size(); i++)
  {
    int insertIndex = m_FilterRows[i];
    AbstractFilter::Pointer filter = m_Filters[i];

    addFilter(filter, insertIndex);
  }

  m_PipelineView->preflightPipeline();

  emit m_PipelineView->pipelineChanged();

  QString statusMessage;
  if(m_Filters.size() > 1)
  {
    QString indexesString = QObject::tr("%1").arg(m_FilterRows[0] + 1);
    for(size_t i = 1; i < m_FilterRows.size(); i++)
    {
      indexesString.append(", ");
      indexesString.append(QObject::tr("%1").arg(m_FilterRows[i] + 1));
    }
    statusMessage = QObject::tr("Undo \"Removed %1 filters at indexes %2\"").arg(m_Filters.size()).arg(indexesString);
  }
  else
  {
    statusMessage = QObject::tr("Undo \"Removed '%1' filter at index %2\"").arg(m_Filters[0]->getHumanLabel()).arg(m_FilterRows[0] + 1);
  }

  emit m_PipelineView->statusMessage(statusMessage);
  emit m_PipelineView->stdOutMessage(SVStyle::Instance()->WrapTextWithHtmlStyle(statusMessage,false));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool variantCompare(const QVariant& v1, const QVariant& v2)
{
  return v1.toInt() > v2.toInt();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFilterCommand::redo()
{
  for(const auto& filter : m_Filters)
  {
    removeFilter(filter);
  }

  QString statusMessage;
  if(m_Filters.size() > 1)
  {
    QString indexesString = QObject::tr("%1").arg(m_FilterRows[0] + 1);
    for(size_t i = 1; i < m_FilterRows.size(); i++)
    {
      indexesString.append(", ");
      indexesString.append(QObject::tr("%1").arg(m_FilterRows[i] + 1));
    }
    statusMessage = QObject::tr("Removed %1 filters at indexes %2").arg(m_Filters.size()).arg(indexesString);
  }
  else
  {
    statusMessage = QObject::tr("Removed '%1' filter at index %2").arg(m_Filters[0]->getHumanLabel()).arg(m_FilterRows[0] + 1);
  }

  if(!m_FirstRun)
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
  emit m_PipelineView->stdOutMessage(SVStyle::Instance()->WrapTextWithHtmlStyle(statusMessage,false));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFilterCommand::addFilter(AbstractFilter::Pointer filter, int insertionIndex)
{
  filter->setRemoving(false);

  PipelineModel* model = m_PipelineView->getPipelineModel();

  model->insertRow(insertionIndex);
  QModelIndex filterIndex = model->index(insertionIndex, PipelineItem::Contents);
  model->setData(filterIndex, static_cast<int>(PipelineItem::ItemType::Filter), PipelineModel::ItemTypeRole);
  model->setFilter(filterIndex, filter);

  connectFilterSignalsSlots(filter);

  if(!filter->getEnabled())
  {
    model->setData(filterIndex, static_cast<int>(PipelineItem::WidgetState::Disabled), PipelineModel::WidgetStateRole);
  }

  QRect filterRect = m_PipelineView->visualRect(filterIndex);

  PipelineItemSlideAnimation* animation = new PipelineItemSlideAnimation(model, QPersistentModelIndex(filterIndex), filterRect.width(), PipelineItemSlideAnimation::AnimationDirection::EnterRight);
  model->setData(QPersistentModelIndex(filterIndex), PipelineItem::AnimationType::Add, PipelineModel::Roles::AnimationTypeRole);

  QObject::connect(animation, &PipelineItemSlideAnimation::finished,
                   [=] { model->setData(QPersistentModelIndex(filterIndex), PipelineItem::AnimationType::None, PipelineModel::Roles::AnimationTypeRole); });
  animation->start(QAbstractAnimation::DeleteWhenStopped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFilterCommand::removeFilter(AbstractFilter::Pointer filter)
{
  // Check if the given filter is already being removed before removing it again
  // Multiple calls to remove the same object causes crashes.
  if(filter->getRemoving())
  {
    return;
  }
  filter->setRemoving(true);

  PipelineModel* model = m_PipelineView->getPipelineModel();

  disconnectFilterSignalsSlots(filter);

  QModelIndex index = model->indexOfFilter(filter.get());

  QPersistentModelIndex persistentIndex = index;

  QRect filterRect = m_PipelineView->visualRect(index);

  if(!m_UseAnimationOnFirstRun && m_FirstRun)
  {
    model->removeRow(persistentIndex.row());
  }
  else
  {
    PipelineItemSlideAnimation* animation = new PipelineItemSlideAnimation(model, persistentIndex, filterRect.width(), PipelineItemSlideAnimation::AnimationDirection::ExitRight);
    model->setData(persistentIndex, PipelineItem::AnimationType::Remove, PipelineModel::Roles::AnimationTypeRole);

    QObject::connect(animation, &PipelineItemSlideAnimation::finished, [=] { model->removeRow(persistentIndex.row()); });
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

  QObject::connect(fiw, &FilterInputWidget::filterParametersChanged, [=] (bool preflight) {
    if (preflight)
    {
      m_PipelineView->preflightPipeline();
    }
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

  QObject::disconnect(filter.get(), &AbstractFilter::filterCompleted, nullptr, nullptr);

  QObject::disconnect(filter.get(), &AbstractFilter::filterInProgress, nullptr, nullptr);

  FilterInputWidget* fiw = model->filterInputWidget(index);

  QObject::disconnect(fiw, &FilterInputWidget::filterParametersChanged, nullptr, nullptr);
}

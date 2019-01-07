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

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QObject>

#include "SIMPLib/CoreFilters/Breakpoint.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"

#include "SVWidgetsLib/Animations/PipelineItemHeightAnimation.h"
#include "SVWidgetsLib/Animations/PipelineItemSlideAnimation.h"
#include "SVWidgetsLib/Widgets/FilterInputWidget.h"
#include "SVWidgetsLib/Widgets/PipelineModel.h"
#include "SVWidgetsLib/Widgets/SVPipelineView.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(AbstractFilter::Pointer filter, SVPipelineView* view, int insertIndex, QString actionText, bool useAnimationOnFirstRun, QUndoCommand* parent)
: QUndoCommand(parent)
, m_ActionText(actionText)
, m_PipelineView(view)
, m_UseAnimationOnFirstRun(useAnimationOnFirstRun)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();

  if(insertIndex < 0)
  {
    insertIndex = model->rowCount();
  }

  setText(QObject::tr("\"%1 '%2'\"").arg(actionText).arg(filter->getHumanLabel()));

  m_Filters.push_back(filter);

  m_FilterRows.push_back(insertIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(std::vector<AbstractFilter::Pointer> filters, SVPipelineView* view, int insertIndex, QString actionText, bool useAnimationOnFirstRun, QUndoCommand* parent)
: QUndoCommand(parent)
, m_Filters(filters)
, m_ActionText(actionText)
, m_PipelineView(view)
, m_UseAnimationOnFirstRun(useAnimationOnFirstRun)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();

  if(insertIndex < 0)
  {
    insertIndex = model->rowCount();
  }

  if(filters.size() == 1)
  {
    setText(QObject::tr("\"%1 '%2'\"").arg(actionText).arg(filters[0]->getHumanLabel()));
  }
  else
  {
    setText(QObject::tr("\"%1 %2 Filters\"").arg(actionText).arg(filters.size()));
  }

  for(size_t i = 0; i < m_Filters.size(); i++)
  {
    m_FilterRows.push_back(insertIndex + i);
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
  if(m_Filters.empty())
  {
    return;
  }

  PipelineModel* model = m_PipelineView->getPipelineModel();

  for(int i = 0; i < m_Filters.size(); i++)
  {
    QPersistentModelIndex filterIndex = model->indexOfFilter(m_Filters[i].get());

    removeFilter(filterIndex);
  }

  QString statusMessage;
  if(m_Filters.size() > 1)
  {
    statusMessage = QObject::tr("Undo \"Added %1 filters starting at index %2\"").arg(m_Filters.size()).arg(m_FilterRows[0] + 1);
  }
  else
  {
    statusMessage = QObject::tr("Undo \"Added '%1' filter at index %2\"").arg(m_Filters[0]->getHumanLabel()).arg(m_FilterRows[0] + 1);
  }

  m_PipelineView->preflightPipeline();

  emit m_PipelineView->pipelineChanged();

  emit m_PipelineView->statusMessage(statusMessage);
  emit m_PipelineView->stdOutMessage(SVStyle::Instance()->WrapTextWithHtmlStyle(statusMessage,false));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::redo()
{
  if(m_Filters.empty())
  {
    return;
  }

  PipelineModel* model = m_PipelineView->getPipelineModel();

  for(size_t i = 0; i < m_Filters.size(); i++)
  {
    addFilter(m_Filters[i], m_FilterRows[i]);
  }

  emit m_PipelineView->preflightPipeline();
  emit m_PipelineView->pipelineChanged();

  QString statusMessage;
  if(m_Filters.size() > 1)
  {
    statusMessage = QObject::tr("Added %1 filters starting at index %2").arg(m_Filters.size()).arg(m_FilterRows[0] + 1);
  }
  else
  {
    statusMessage = QObject::tr("Added '%1' filter at index %2").arg(m_Filters[0]->getHumanLabel()).arg(m_FilterRows[0] + 1);
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

  emit model->statusMessageGenerated(statusMessage);
  emit model->standardOutputMessageGenerated(SVStyle::Instance()->WrapTextWithHtmlStyle(statusMessage,false));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::addFilter(AbstractFilter::Pointer filter, int insertionIndex)
{
  // Reset the filter's Removing property
  filter->setRemoving(false);

  PipelineModel* model = m_PipelineView->getPipelineModel();

  model->insertRow(insertionIndex);

  QModelIndex filterIndex = model->index(insertionIndex, PipelineItem::Contents);
  //  model->setData(filterIndex, filter->getHumanLabel(), Qt::DisplayRole);
  model->setData(filterIndex, static_cast<int>(PipelineItem::ItemType::Filter), PipelineModel::ItemTypeRole);
  model->setFilter(filterIndex, filter);

  connectFilterSignalsSlots(filter);

  if(!filter->getEnabled())
  {
    model->setData(filterIndex, static_cast<int>(PipelineItem::WidgetState::Disabled), PipelineModel::WidgetStateRole);
  }

  if(!m_UseAnimationOnFirstRun && m_FirstRun)
  {
    QSize size = model->data(filterIndex, Qt::SizeHintRole).toSize();
    model->setData(filterIndex, size.height(), PipelineModel::Roles::HeightRole);
  }
  else
  {
    QRect filterRect = m_PipelineView->visualRect(filterIndex);

    PipelineItemSlideAnimation* slideAnimation =
        new PipelineItemSlideAnimation(model, QPersistentModelIndex(filterIndex), filterRect.width(), PipelineItemSlideAnimation::AnimationDirection::EnterRight);
    model->setData(QPersistentModelIndex(filterIndex), PipelineItem::AnimationType::Add, PipelineModel::Roles::AnimationTypeRole);

    QObject::connect(slideAnimation, &PipelineItemSlideAnimation::finished,
                     [=] { model->setData(QPersistentModelIndex(filterIndex), PipelineItem::AnimationType::None, PipelineModel::Roles::AnimationTypeRole); });
    slideAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::removeFilter(const QPersistentModelIndex& index)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();
  AbstractFilter::Pointer filter = model->filter(index);

  // Do not remove a filter that is already being removed
  if(filter->getRemoving())
  {
    return;
  }

  disconnectFilterSignalsSlots(filter);

  QRect filterRect = m_PipelineView->visualRect(index);

  PipelineItemSlideAnimation* animation = new PipelineItemSlideAnimation(model, QPersistentModelIndex(index), filterRect.width(), PipelineItemSlideAnimation::AnimationDirection::ExitRight);
  model->setData(QPersistentModelIndex(index), PipelineItem::AnimationType::Remove, PipelineModel::Roles::AnimationTypeRole);

  QObject::connect(animation, &PipelineItemSlideAnimation::finished, [=]() mutable { model->removeRow(index.row()); });
  animation->start(QAbstractAnimation::DeleteWhenStopped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::connectFilterSignalsSlots(AbstractFilter::Pointer filter)
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
void AddFilterCommand::disconnectFilterSignalsSlots(AbstractFilter::Pointer filter)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();
  QModelIndex index = model->indexOfFilter(filter.get());

  QObject::disconnect(filter.get(), &AbstractFilter::filterCompleted, nullptr, nullptr);

  QObject::disconnect(filter.get(), &AbstractFilter::filterInProgress, nullptr, nullptr);

  FilterInputWidget* fiw = model->filterInputWidget(index);

  QObject::disconnect(fiw, &FilterInputWidget::filterParametersChanged, nullptr, nullptr);
}

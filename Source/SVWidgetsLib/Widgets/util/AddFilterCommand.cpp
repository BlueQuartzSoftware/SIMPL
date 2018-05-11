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

#include "SVWidgetsLib/Animations/PipelineItemSlideAnimation.h"
#include "SVWidgetsLib/Animations/PipelineItemHeightAnimation.h"
#include "SVWidgetsLib/Widgets/FilterInputWidget.h"
#include "SVWidgetsLib/Widgets/SVPipelineView.h"
#include "SVWidgetsLib/Widgets/PipelineModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(AbstractFilter::Pointer filter, SVPipelineView* view, int insertIndex, QString actionText, bool useAnimationOnFirstRun, QUndoCommand* parent)
: QUndoCommand(parent)
, m_FilterCount(1)
, m_ActionText(actionText)
, m_PipelineView(view)
, m_UseAnimationOnFirstRun(useAnimationOnFirstRun)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();

  if(insertIndex < 0)
  {
    insertIndex = model->rowCount();
  }
  m_InsertIndex = insertIndex;

  setText(QObject::tr("\"%1 '%2'\"").arg(actionText).arg(filter->getHumanLabel()));

  m_Filters.push_back(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterCommand::AddFilterCommand(std::vector<AbstractFilter::Pointer> filters, SVPipelineView* view, int insertIndex, QString actionText, bool useAnimationOnFirstRun, QUndoCommand* parent)
: QUndoCommand(parent)
, m_Filters(filters)
, m_FilterCount(filters.size())
, m_ActionText(actionText)
, m_PipelineView(view)
, m_UseAnimationOnFirstRun(useAnimationOnFirstRun)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();

  if(insertIndex < 0)
  {
    insertIndex = model->rowCount();
  }
  m_InsertIndex = insertIndex;

  if (filters.size() == 1)
  {
    setText(QObject::tr("\"%1 '%2'\"").arg(actionText).arg(filters[0]->getHumanLabel()));
  }
  else
  {
    setText(QObject::tr("\"%1 %2 Filters\"").arg(actionText).arg(filters.size()));
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
  if (m_Filters.size() <= 0)
  {
    return;
  }

  PipelineModel* model = m_PipelineView->getPipelineModel();

  for(int i = 0; i < m_Filters.size(); i++)
  {
    QPersistentModelIndex filterIndex = model->indexOfFilter(m_Filters[i].get());

    removeFilter(filterIndex);
  }

  emit m_PipelineView->pipelineChanged();
  emit m_PipelineView->preflightPipeline();

  QString statusMessage;
  if (m_Filters.size() > 1)
  {
    statusMessage = QObject::tr("Undo \"Added %1 filters\"").arg(m_Filters.size());
  }
  else
  {
    statusMessage = QObject::tr("Undo \"Added '%1' filter\"").arg(m_Filters[0]->getHumanLabel());
  }

  emit model->statusMessageGenerated(statusMessage);
  emit model->standardOutputMessageGenerated(statusMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::redo()
{
  if (m_Filters.size() <= 0)
  {
    return;
  }

  PipelineModel* model = m_PipelineView->getPipelineModel();

  int row = m_InsertIndex;
  for(size_t i = 0; i < m_Filters.size(); i++)
  {
    addFilter(m_Filters[i], row);
    row++;
  }

  emit m_PipelineView->preflightPipeline();
  emit m_PipelineView->pipelineChanged();

  QString statusMessage;
  if (m_Filters.size() > 1)
  {
    statusMessage = QObject::tr("Added %1 filters").arg(m_Filters.size());
  }
  else
  {
    statusMessage = QObject::tr("Added '%1' filter").arg(m_Filters[0]->getHumanLabel());
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

  emit model->statusMessageGenerated(statusMessage);
  emit model->standardOutputMessageGenerated(statusMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::addFilter(AbstractFilter::Pointer filter, int insertionIndex)
{  
  PipelineModel* model = m_PipelineView->getPipelineModel();

  model->insertRow(insertionIndex);

  QModelIndex filterIndex = model->index(insertionIndex, PipelineItem::Contents);
//  model->setData(filterIndex, filter->getHumanLabel(), Qt::DisplayRole);
  model->setData(filterIndex, static_cast<int>(PipelineItem::ItemType::Filter), PipelineModel::ItemTypeRole);
  model->setFilter(filterIndex, filter);

  connectFilterSignalsSlots(filter);

  if (filter->getEnabled() == false)
  {
    model->setData(filterIndex, static_cast<int>(PipelineItem::WidgetState::Disabled), PipelineModel::WidgetStateRole);
  }

  if (m_UseAnimationOnFirstRun == false && m_FirstRun == true)
  {
    QSize size = model->data(filterIndex, Qt::SizeHintRole).toSize();
    model->setData(filterIndex, size.height(), PipelineModel::Roles::HeightRole);
  }
  else
  {
    QRect filterRect = m_PipelineView->visualRect(filterIndex);

    PipelineItemSlideAnimation* slideAnimation = new PipelineItemSlideAnimation(model, QPersistentModelIndex(filterIndex), filterRect.width(), PipelineItemSlideAnimation::AnimationDirection::Right);
    slideAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }

//  PipelineItemHeightAnimation* heightAnimation = new PipelineItemHeightAnimation(model, QPersistentModelIndex(filterIndex), PipelineItemHeightAnimation::AnimationDirection::Open);

//  QObject::connect(heightAnimation, &PipelineItemHeightAnimation::finished, [=]{
//    PipelineItemSlideAnimation* slideAnimation = new PipelineItemSlideAnimation(model, QPersistentModelIndex(filterIndex), filterRect.width(), PipelineItemSlideAnimation::AnimationDirection::Right);
//    slideAnimation->start(QAbstractAnimation::DeleteWhenStopped);
//  });

//  heightAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterCommand::removeFilter(const QPersistentModelIndex &index)
{
  PipelineModel* model = m_PipelineView->getPipelineModel();
  AbstractFilter::Pointer filter = model->filter(index);

  disconnectFilterSignalsSlots(filter);

  QRect filterRect = m_PipelineView->visualRect(index);

  PipelineItemSlideAnimation* animation = new PipelineItemSlideAnimation(model, QPersistentModelIndex(index), filterRect.width(), PipelineItemSlideAnimation::AnimationDirection::Left);

  QObject::connect(animation, &PipelineItemSlideAnimation::finished, [=] {
//    PipelineItemHeightAnimation* heightAnimation = new PipelineItemHeightAnimation(model, QPersistentModelIndex(index), PipelineItemHeightAnimation::AnimationDirection::Close);
//    heightAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    model->removeRow(index.row());
  });
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

  QObject::connect(fiw, &FilterInputWidget::filterParametersChanged, [=] {
    m_PipelineView->preflightPipeline();
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

  QObject::disconnect(filter.get(), &AbstractFilter::filterCompleted, 0, 0);

  QObject::disconnect(filter.get(), &AbstractFilter::filterInProgress, 0, 0);

  FilterInputWidget* fiw = model->filterInputWidget(index);

  QObject::disconnect(fiw, &FilterInputWidget::filterParametersChanged, 0, 0);
}

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

#include "PipelineItemHeightAnimation.h"

#include "SVWidgetsLib/Widgets/PipelineModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineItemHeightAnimation::PipelineItemHeightAnimation(PipelineModel* model, QPersistentModelIndex index, AnimationDirection direction, QObject* parent)
  : QVariantAnimation(parent),
    m_Index(index),
    m_PipelineModel(model),
    m_Direction(direction)
{
  setStartValue(0);
  setEndValue(10);

  int duration = 150;
  setDuration(duration);

//  QSize size = m_PipelineModel->data(m_Index, Qt::SizeHintRole).toSize();
//  m_PipelineModel->setData(m_Index, size.height(), PipelineModel::Roles::HeightRole);

  connect(this, &PipelineItemHeightAnimation::valueChanged, this, &PipelineItemHeightAnimation::listenValueChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineItemHeightAnimation::listenValueChanged(const QVariant & value)
{
  if(m_PipelineModel != nullptr)
  {
    int width = m_PipelineModel->data(m_Index, PipelineModel::Roles::WidthRole).toInt();
    if (m_Direction == AnimationDirection::Open)
    {
      int height = PipelineItem::MaxHeight * currentValue().toInt() * 0.1;
      QSize size(width, height);
      m_PipelineModel->setData(m_Index, size, Qt::SizeHintRole);
    }
    else if (m_Direction == AnimationDirection::Close)
    {
      int height = PipelineItem::MaxHeight - (PipelineItem::MaxHeight * currentValue().toInt() * 0.1);
      QSize size(width, height);
      m_PipelineModel->setData(m_Index, size, Qt::SizeHintRole);
    }
  }
  else
  {
    stop();
  }
}

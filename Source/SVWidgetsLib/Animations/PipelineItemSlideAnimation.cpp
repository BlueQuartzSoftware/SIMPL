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

#include "PipelineItemSlideAnimation.h"

#include "SVWidgetsLib/Widgets/PipelineModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineItemSlideAnimation::PipelineItemSlideAnimation(PipelineModel* model, QPersistentModelIndex index, int numberOfPixels, AnimationDirection direction)
  : QVariantAnimation(model),
    m_Index(index),
    m_PipelineModel(model),
    m_Direction(direction),
    m_NumberOfPixels(numberOfPixels)
{
  setStartValue(0);
  setEndValue(200);

  setDuration(150);

  if (m_Direction == AnimationDirection::EnterLeft)
  {
    m_PipelineModel->setData(m_Index, -m_NumberOfPixels, PipelineModel::Roles::XOffsetRole);
  }
  else if (m_Direction == AnimationDirection::EnterRight)
  {
    m_PipelineModel->setData(m_Index, m_NumberOfPixels, PipelineModel::Roles::XOffsetRole);
  }
  else if (m_Direction == AnimationDirection::ExitLeft || m_Direction == AnimationDirection::ExitRight)
  {
    m_PipelineModel->setData(m_Index, 0, PipelineModel::Roles::XOffsetRole);
  }

  QSize size = m_PipelineModel->data(m_Index, Qt::SizeHintRole).toSize();
  m_PipelineModel->setData(m_Index, size.height(), PipelineModel::Roles::HeightRole);

  connect(this, &PipelineItemSlideAnimation::valueChanged, this, &PipelineItemSlideAnimation::listenValueChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineItemSlideAnimation::listenValueChanged(const QVariant& value)
{
  if(m_PipelineModel != nullptr)
  {
    if (m_Direction == AnimationDirection::EnterLeft)
    {
      m_PipelineModel->setData(m_Index, -m_NumberOfPixels + (m_NumberOfPixels * currentValue().toInt() * 0.005), PipelineModel::Roles::XOffsetRole);
    }
    else if (m_Direction == AnimationDirection::EnterRight)
    {
      m_PipelineModel->setData(m_Index, m_NumberOfPixels - (m_NumberOfPixels * currentValue().toInt() * 0.005), PipelineModel::Roles::XOffsetRole);
    }
    if (m_Direction == AnimationDirection::ExitLeft)
    {
      m_PipelineModel->setData(m_Index, 0 - (m_NumberOfPixels * currentValue().toInt() * 0.005), PipelineModel::Roles::XOffsetRole);
    }
    else if (m_Direction == AnimationDirection::ExitRight)
    {
      m_PipelineModel->setData(m_Index, 0 + (m_NumberOfPixels * currentValue().toInt() * 0.005), PipelineModel::Roles::XOffsetRole);
    }
  }
  else
  {
    stop();
  }
}

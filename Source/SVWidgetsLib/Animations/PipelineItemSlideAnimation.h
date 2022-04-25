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

#pragma once

#include <QtCore/QPersistentModelIndex>
#include <QtCore/QVariantAnimation>

#include "SVWidgetsLib/SVWidgetsLib.h"

class PipelineModel;

class SVWidgetsLib_EXPORT PipelineItemSlideAnimation : public QVariantAnimation
{
    Q_OBJECT

  public:
    enum class AnimationDirection : unsigned int
    {
      EnterLeft,
      EnterRight,
      ExitLeft,
      ExitRight
    };

    PipelineItemSlideAnimation(PipelineModel* model, QPersistentModelIndex index, int numberOfPixels, AnimationDirection direction);
    ~PipelineItemSlideAnimation() override = default;

  private Q_SLOTS:
    void listenValueChanged(const QVariant & value);

  private:
    QPersistentModelIndex m_Index;
    PipelineModel* m_PipelineModel;
    AnimationDirection m_Direction;
    int m_NumberOfPixels = 0;
    int m_StartX = 0;

  public:
    PipelineItemSlideAnimation(const PipelineItemSlideAnimation&) = delete; // Copy Constructor Not Implemented
    PipelineItemSlideAnimation(PipelineItemSlideAnimation&&) = delete;      // Move Constructor Not Implemented
    PipelineItemSlideAnimation& operator=(const PipelineItemSlideAnimation&) = delete; // Copy Assignment Not Implemented
    PipelineItemSlideAnimation& operator=(PipelineItemSlideAnimation&&) = delete;      // Move Assignment Not Implemented
};

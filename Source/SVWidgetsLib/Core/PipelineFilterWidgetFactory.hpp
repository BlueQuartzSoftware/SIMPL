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

#pragma once

#include "SVWidgetsLib/Core/IFilterWidgetFactory.h"

template <class Widget>
class PipelineFilterWidgetFactory : public IFilterWidgetFactory
{
public:
  using Self = PipelineFilterWidgetFactory<Widget>;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  QString getNameOfClass() const override
  {
    return QString("PipelineFilterWidgetFactory<Widget>");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("PipelineFilterWidgetFactory<Widget>");
  }

  /**
   * @brief New
   * @return
   */
  static Pointer New()
  {
    Pointer sharedPtr(new(Self));
    return sharedPtr;
  }

  ~PipelineFilterWidgetFactory() override = default;

  /**
   * @brief Creates a new widget for this filter. The Calling method MUST set
   * a parent Widget OR take responsibility for deleting this object.
   * @return
   */
  QWidget* createWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr) override
  {
    auto* w = new Widget(parameter, filter, parent);
    QString str = QString("%1_FilterParameter").arg(parameter->getPropertyName());
    w->setObjectName(str);
    w->loadData();
    return w;
  }

protected:
  PipelineFilterWidgetFactory() = default;

public:
  PipelineFilterWidgetFactory(const PipelineFilterWidgetFactory&) = delete;            // Copy Constructor Not Implemented
  PipelineFilterWidgetFactory(PipelineFilterWidgetFactory&&) = delete;                 // Move Constructor
  PipelineFilterWidgetFactory& operator=(const PipelineFilterWidgetFactory&) = delete; // Copy Assignment Not Implemented
  PipelineFilterWidgetFactory& operator=(PipelineFilterWidgetFactory&&) = delete;      // Move Assignment Not Implemented
};

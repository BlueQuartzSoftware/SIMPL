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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class FilterParameter;
class QWidget;

/**
 * @brief This class serves as a base class to create Factory classes that can
 * create PipelineFilterWidgets for a GUI based on Qt.
 */
class SVWidgetsLib_EXPORT IFilterWidgetFactory
{
public:
  using Self = IFilterWidgetFactory;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for IFilterWidgetFactory
   */
  virtual QString getNameOfClass() const;
  /**
   * @brief Returns the name of the class for IFilterWidgetFactory
   */
  static QString ClassName();

  virtual ~IFilterWidgetFactory();

  /**
   * @brief This function should NEVER get called. The subclass should ALWAYS implement
   * this method so we are going to crash the program.t
   * @param parameter The FilterParameter that this widget will represent
   * @param filter The instance of the filter that the FilterParameter is a part of
   * @param parent The Parent QWidget for the created widget
   * @return
   */
  virtual QWidget* createWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

protected:
  IFilterWidgetFactory();

public:
  IFilterWidgetFactory(const IFilterWidgetFactory&) = delete;            // Copy Constructor Not Implemented
  IFilterWidgetFactory(IFilterWidgetFactory&&) = delete;                 // Move Constructor Not Implemented
  IFilterWidgetFactory& operator=(const IFilterWidgetFactory&) = delete; // Copy Assignment Not Implemented
  IFilterWidgetFactory& operator=(IFilterWidgetFactory&&) = delete;      // Move Assignment Not Implemented

private:
};

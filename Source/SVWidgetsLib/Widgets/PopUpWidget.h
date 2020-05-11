/* ============================================================================
 * Copyright (c) 2017-2019 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#include <QtWidgets/QWidget>

#include "SVWidgetsLib/SVWidgetsLib.h"

#include "SVWidgetsLib/ui_PopUpWidget.h"

/**
 * @brief The PopUpWidget class
 */
class SVWidgetsLib_EXPORT PopUpWidget : public QWidget, private Ui::PopUpWidget
{
  Q_OBJECT

public:
  PopUpWidget(QWidget* parent = Q_NULLPTR);
  ~PopUpWidget() override;

  using EnumType = unsigned int;

  enum class ArrowOrientation : EnumType
  {
    Top,
    Bottom,
    Left,
    Right,
    None
  };

  /**
   * @brief Setter property for ArrowHeight
   */
  void setArrowHeight(int value);
  /**
   * @brief Getter property for ArrowHeight
   * @return Value of ArrowHeight
   */
  int getArrowHeight() const;

  /**
   * @brief Setter property for ArrowWidth
   */
  void setArrowWidth(int value);
  /**
   * @brief Getter property for ArrowWidth
   * @return Value of ArrowWidth
   */
  int getArrowWidth() const;

  /**
   * @brief Setter property for ArrowOffset
   */
  void setArrowOffset(int value);
  /**
   * @brief Getter property for ArrowOffset
   * @return Value of ArrowOffset
   */
  int getArrowOffset() const;

  /**
   * @brief setupGui
   */
  void setupGui();

  /**
   * @brief setWidget
   * @param widget
   */
  void setWidget(QWidget* widget);

  /**
   * @brief setArrowOrientation
   * @param orientation
   */
  void setArrowOrientation(ArrowOrientation orientation);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  int m_ArrowHeight = {};
  int m_ArrowWidth = {};
  int m_ArrowOffset = {};

  ArrowOrientation m_ArrowOrientation;

public:
  PopUpWidget(const PopUpWidget&) = delete;            // Copy Constructor Not Implemented
  PopUpWidget(PopUpWidget&&) = delete;                 // Move Constructor Not Implemented
  PopUpWidget& operator=(const PopUpWidget&) = delete; // Copy Assignment Not Implemented
  PopUpWidget& operator=(PopUpWidget&&) = delete;      // Move Assignment Not Implemented
};

/* ============================================================================
 * Copyright (c) 2009-2017 BlueQuartz Software, LLC
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

#include <QtCore/QVariantAnimation>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/SVControlWidgets.h"

class SVWidgetsLib_EXPORT SVOverlayWidgetButton : public QPushButton
{
  Q_OBJECT

public:
  enum class TargetSide
  {
    Left,
    Right,
    Top,
    Bottom
  };

  SVOverlayWidgetButton(QWidget* parent);
  virtual ~SVOverlayWidgetButton() = default;

  /**
   * @brief Sets the side of the target widget to extend from
   * @param side
   */
  void setSide(TargetSide side);

  /**
   * @brief Sets the widget to overlay onto
   * @param target
   */
  void setTarget(QWidget* target);

  /**
   * @brief Sets the widget being shown / hidden
   */
  void setSource(QWidget* source);

  TargetSide getSide() const;

  QWidget* getTarget() const;

  QWidget* getSource() const;

  bool isExpanded() const;

  void setDuration(int duration);

  int getDuration() const;

  void setOverlappingButtons(QVector<SVOverlayWidgetButton*> buttons);

protected:
  void updateOverlay();
  void setExpanded(bool expanded);
  void startAnimation(bool forward);
  double getEndValue() const;

private:
  QWidget* m_Target = nullptr;
  QWidget* m_Source = nullptr;
  SVFrame* m_Frame = nullptr;
  QGridLayout* m_Layout = nullptr;
  TargetSide m_Side = TargetSide::Bottom;
  QVariantAnimation* m_Animation = nullptr;
  QVector<SVOverlayWidgetButton*> m_OverlappingButtons;
};

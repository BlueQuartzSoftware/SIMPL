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

#include <QtCore/QEvent>
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
   * @brief Returns the side that the target will be overlaying
   * @return
   */
  TargetSide getSide() const;

  /**
   * @brief Sets the side of the target widget that will be covered by the source
   * @param side
   */
  void setSide(TargetSide side);

  /**
   * @brief Returns the target widget
   * @return
   */
  QWidget* getTarget() const;

  /**
   * @brief Sets the widget to overlay onto
   * @param target
   */
  void setTarget(QWidget* target);

  /**
   * @brief Returns the source widget
   * @return
   */
  QWidget* getSource() const;

  /**
   * @brief Sets the widget being shown / hidden
   * @param source
   */
  void setSource(QWidget* source);

  /**
   * @brief Returns true if the widget is expanded.  Returns false otherwise.
   * @return
   */
  bool isExpanded() const;

  /**
   * @brief Sets the duration of the slide animation in milliseconds
   * @param duration
   */
  void setDuration(int duration);

  /**
   * @brief Returns the duration of the slide animation in milliseconds.
   * @return
   */
  int getDuration() const;

  /**
   * @brief Marks another SVOverlayWidgetButton as overlapping this one.\
   * @param button
   */
  void addOverlappingButton(SVOverlayWidgetButton* button);

  /**
   * @brief Sets the overlapping SVOverlayWidgetButton.
   * @param buttons
   */
  void setOverlappingButtons(QVector<SVOverlayWidgetButton*> buttons);

  /**
   * @brief Returns the overlapping SVOverlayWidgetButtons
   * @return
   */
  QVector<SVOverlayWidgetButton*> getOverlappingButtons() const;

protected:
  /**
   * @brief Updates the overlay position
   */
  void updateOverlay();

  void updateSourcePolicy();

  /**
   * @brief Handles changes to the target visibility
   * @param expanded
   */
  void setExpanded(bool expanded);

  /**
   * @brief Starts or stops the slide animation
   * @param forward
   */
  void startAnimation(bool forward);

  /**
   * @brief Returns the end value for the animation
   * @return
   */
  double getEndValue() const;

  /**
   * @brief Checks the validity of the button for user interaction
   * @return
   */
  bool checkValidity();

  /**
   * @brief The event filter handles resizing of the target widget to keep the overlay in an accurate size
   * @param obj
   * @param event
   */
  bool eventFilter(QObject* obj, QEvent* event) Q_DECL_OVERRIDE;

private:
  QWidget* m_Target = nullptr;
  QWidget* m_Source = nullptr;
  SVFrame* m_Frame = nullptr;
  QGridLayout* m_Layout = nullptr;
  TargetSide m_Side = TargetSide::Bottom;
  QVariantAnimation* m_Animation = nullptr;
  QVector<SVOverlayWidgetButton*> m_OverlappingButtons;
  QSpacerItem* m_TopSpacer = nullptr;
  QSpacerItem* m_BottomSpacer = nullptr;
  QSpacerItem* m_LeftSpacer = nullptr;
  QSpacerItem* m_RightSpacer = nullptr;
};

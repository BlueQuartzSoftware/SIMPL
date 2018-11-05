/* ============================================================================
 * Copyright (c) 2018 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QEvent>
#include <QtCore/QVariantAnimation>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>

#include "SVWidgetsLib/SVWidgetsLib.h"

/**
 * @class SVOverlayWidgetButton SVOverlayWidgetButton.h SVWidgetsLib/Widgets/SVOverlayWidgetButton.h
 * @brief The SVOverlayWidgetButton class is a QPushButton that toggles another
 * widget's visibility as it slides in or out over a target widget.  The direction
 * and duration can be changed between animations.  If the target widget is set to
 * a QDockWidget, the direction used will be based on the side the dock widget was
 * last docked on.
 */
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

  SVOverlayWidgetButton(QWidget* parent = nullptr);
  SVOverlayWidgetButton(QString text, QWidget* parent = nullptr);
  virtual ~SVOverlayWidgetButton() = default;

  /**
   * @brief Updates the internal state to allow the widget to be enabled.
   * Enables the widget if it passes validity checks.
   */
  void enable();

  /**
   * @brief Disables the button and updates the internal state to prevent the
   * widget from being enabled by validity checks.
   */
  void disable();

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
   * @brief Sets the widget to overlay onto and listens for change in the dock location
   * @param dockWidget
   */
  void setDockWidget(QMainWindow* window, QDockWidget* dockWidget);

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
   * @brief Returns the margin used by the overlay frame.
   * @return
   */
  int getMargin() const;

  /**
   * @brief Sets the margin used by the overlay frame.
   * @param margin
   */
  void setMargin(int margin);

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
   * @brief Returns true if the widget should expand the entire distance.  Returns false otherwise.
   * @return
   */
  bool expandsEntireDistance() const;

  /**
   * @brief Sets whether or not the widget should expand the entire distance.
   * @param expands
   */
  void setExpandsEntireDistance(bool expands);

  /**
   * @brief Marks another SVOverlayWidgetButton as overlapping this one.
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
   * @brief Performs initial setup work for the GUI
   */
  void setupGui();

  /**
   * @brief Updates the overlay position
   */
  void updateOverlay();

  /**
   * @brief Updates the source QSizePolicy
   */
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
   * @brief Updates the side based on the given location
   * @param dockLocation
   */
  void setDockLocation(Qt::DockWidgetArea dockLocation);

  /**
   * @brief The event filter handles resizing of the target widget to keep the overlay in an accurate size
   * @param obj
   * @param event
   */
  bool eventFilter(QObject* obj, QEvent* event) override;

private:
  QWidget* m_Target = nullptr;
  QWidget* m_Source = nullptr;
  QFrame* m_Frame = nullptr;
  QGridLayout* m_Layout = nullptr;
  TargetSide m_Side = TargetSide::Bottom;
  QVariantAnimation* m_Animation = nullptr;
  bool m_ExpandsEntireDistance = false;
  QVector<SVOverlayWidgetButton*> m_OverlappingButtons;
  QSpacerItem* m_TopSpacer = nullptr;
  QSpacerItem* m_BottomSpacer = nullptr;
  QSpacerItem* m_LeftSpacer = nullptr;
  QSpacerItem* m_RightSpacer = nullptr;
  QDockWidget* m_DockWidget = nullptr;
  bool m_Enabled = true;
};

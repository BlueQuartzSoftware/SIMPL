/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <QtCore/QParallelAnimationGroup>

#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

#include "SVWidgetsLib/SVWidgetsLib.h"

class QLayout;
class QRect;

/**
 * @class QtSDisclosableWidget QtSDisclosableWidget.h FilterWidgets/QtSDisclosableWidget.h
 * @brief  This class is a subclass of the QGroupBox class and is used to display
 * Filter Options that the user can set. This class is capable of constructing a
 * default GUI widget set for each type of Filter Option that is available. If
 * the programmer needs more specialized widgets then they can simply subclass
 * this class and over ride or implement their custom code.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 6, 2012
 * @version 1.0
 */
class SVWidgetsLib_EXPORT QtSDisclosableWidget : public QWidget
{
  Q_OBJECT
public:
  QtSDisclosableWidget(QWidget* parent = nullptr);
  ~QtSDisclosableWidget() override;

  virtual void setupGui();

  void setContentLayout(QLayout* contentLayout);

  void setTitle(const QString& title);

Q_SIGNALS:
  void dragStarted(QtSDisclosableWidget* widget);

public Q_SLOTS:
  //  void disclose(bool on);

  /**
   * @brief Sets the style of the Widget to indicate a selected or non-selected
   * state
   * @param selected Is the widget selected or not.
   */
  void changeStyle();

  /**
   *@brief
   */
  void updateWidgetStyle();

private:
  QGridLayout mainLayout;
  QToolButton toggleButton;
  QFrame headerLine;
  QParallelAnimationGroup toggleAnimation;
  QScrollArea contentArea;
  int animationDuration = 300;

public:
  QtSDisclosableWidget(const QtSDisclosableWidget&) = delete;            // Copy Constructor Not Implemented
  QtSDisclosableWidget(QtSDisclosableWidget&&) = delete;                 // Move Constructor Not Implemented
  QtSDisclosableWidget& operator=(const QtSDisclosableWidget&) = delete; // Copy Assignment Not Implemented
  QtSDisclosableWidget& operator=(QtSDisclosableWidget&&) = delete;      // Move Assignment Not Implemented
};

/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "QtSDisclosableWidget.h"

#include <iostream>

#include <QtCore/QPropertyAnimation>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSDisclosableWidget::QtSDisclosableWidget(QWidget* parent)
: QWidget(parent)
{
  //  connect(this, SIGNAL(toggled(bool)), this, SLOT(disclose(bool)));

  toggleButton.setStyleSheet("QToolButton { border: none; }");
  toggleButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  toggleButton.setArrowType(Qt::ArrowType::RightArrow);
  toggleButton.setCheckable(true);
  toggleButton.setChecked(false);

  headerLine.setFrameShape(QFrame::HLine);
  headerLine.setFrameShadow(QFrame::Sunken);
  headerLine.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

  //  contentArea.setStyleSheet("QScrollArea { border: none; }");
  contentArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  contentArea.setMaximumHeight(0);
  contentArea.setMinimumHeight(0);
  toggleAnimation.addAnimation(new QPropertyAnimation(this, "minimumHeight"));
  toggleAnimation.addAnimation(new QPropertyAnimation(this, "maximumHeight"));
  toggleAnimation.addAnimation(new QPropertyAnimation(&contentArea, "maximumHeight"));
  mainLayout.setVerticalSpacing(0);
  mainLayout.setContentsMargins(0, 0, 0, 0);
  int row = 0;
  mainLayout.addWidget(&toggleButton, row, 0, 1, 1, Qt::AlignLeft);
  mainLayout.addWidget(&headerLine, row++, 2, 1, 1);
  mainLayout.addWidget(&contentArea, row, 0, 1, 3);
  setLayout(&mainLayout);
  QObject::connect(&toggleButton, &QToolButton::clicked, [this](const bool checked) {
    toggleButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    toggleAnimation.setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    toggleAnimation.start();
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSDisclosableWidget::~QtSDisclosableWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSDisclosableWidget::changeStyle()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSDisclosableWidget::updateWidgetStyle()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSDisclosableWidget::setupGui()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSDisclosableWidget::setTitle(const QString& title)
{
  toggleButton.setText(title);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSDisclosableWidget::setContentLayout(QLayout* contentLayout)
{
  delete contentArea.layout();
  contentArea.setLayout(contentLayout);
  const auto collapsedHeight = sizeHint().height() - contentArea.maximumHeight();
  auto contentHeight = contentLayout->sizeHint().height();
  for(int i = 0; i < toggleAnimation.animationCount() - 1; ++i)
  {
    QPropertyAnimation* spoilerAnimation = static_cast<QPropertyAnimation*>(toggleAnimation.animationAt(i));
    spoilerAnimation->setDuration(animationDuration);
    spoilerAnimation->setStartValue(collapsedHeight);
    spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
  }
  QPropertyAnimation* contentAnimation = static_cast<QPropertyAnimation*>(toggleAnimation.animationAt(toggleAnimation.animationCount() - 1));
  contentAnimation->setDuration(animationDuration);
  contentAnimation->setStartValue(0);
  contentAnimation->setEndValue(contentHeight);
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSDisclosableWidget::disclose(bool on)
{
  if(on)
  {
    this->setMaximumHeight(260000);
  }
  else
  {
    this->setMaximumHeight(26);
  }

  QObjectList objs = children();
  for(QObject* obj : objs)
  {
    QWidget* w = qobject_cast<QWidget*>(obj);
    if(nullptr != w)
    {
      if(on)
      {
        w->show();
      }
      else
      {
        w->hide();
      }
    }
  }
}
#endif

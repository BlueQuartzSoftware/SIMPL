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

#include "SVOverlayWidgetButton.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVOverlayWidgetButton::SVOverlayWidgetButton(QWidget* parent)
: QPushButton(parent)
{
  m_Animation = new QVariantAnimation(this);
  m_Animation->setEasingCurve(QEasingCurve::InOutQuad);
  m_Animation->setStartValue(0.0);
  m_Animation->setEndValue(1.0);
  m_Animation->setDuration(500);
  m_Animation->setCurrentTime(0);

  setCheckable(true);
  connect(this, &SVOverlayWidgetButton::toggled, this, &SVOverlayWidgetButton::setExpanded);
  connect(m_Animation, &QVariantAnimation::valueChanged, this, [=] { updateOverlay(); });

  m_Layout = new QGridLayout(m_Frame);
  m_Layout->setMargin(4);

  m_Frame = new SVFrame();
  m_Frame->hide();
  m_Frame->setLayout(m_Layout);
  m_Frame->setAutoFillBackground(true);
  m_Frame->setFrameStyle(QFrame::Box | QFrame::Plain);

  checkValidity();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setSide(TargetSide side)
{
  m_Side = side;
  updateOverlay();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setTarget(QWidget* target)
{
  if(nullptr != m_Target)
  {
    m_Target->removeEventFilter(this);
  }

  m_Target = target;

  if(nullptr == target)
  {
    m_Frame->hide();
    m_Frame->setParent(nullptr);
  }
  else
  {
    m_Frame->setParent(m_Target);
    m_Target->installEventFilter(this);
  }

  if(m_Source && m_Target)
  {
    updateOverlay();
    // m_Frame->show();
  }

  checkValidity();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setSource(QWidget* source)
{
  m_Layout->removeWidget(m_Source);
  m_Source = source;

  if(source)
  {
    m_Layout->addWidget(m_Source);
    m_Source->show();
    if(m_Target)
    {
      updateOverlay();
    }
  }
  else
  {
    m_Frame->hide();
    setExpanded(false);
  }

  checkValidity();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVOverlayWidgetButton::checkValidity()
{
  bool valid = m_Source && m_Target;
  if(!valid)
  {
    setChecked(false);
  }
  setEnabled(valid);
  return valid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::updateOverlay()
{
  if(nullptr == m_Target || nullptr == m_Source)
  {
    return;
  }

  int maxWidth = 15;
  int maxHeight = 15;
  if(m_Source)
  {
    maxWidth = m_Source->sizeHint().width();
    maxHeight = m_Source->sizeHint().height();
  }

  if(TargetSide::Left == m_Side || TargetSide::Right == m_Side)
  {
    int height = m_Target->height();
    int currentWidth = m_Animation->currentValue().toDouble();

    if(TargetSide::Left == m_Side)
    {
      QRect geom(currentWidth - maxWidth, 0, maxWidth, height);
      m_Frame->setGeometry(geom);
    }
    else
    {
      QRect geom(m_Target->width() - currentWidth, 0, maxWidth, height);
      m_Frame->setGeometry(geom);
    }
  }
  else
  {
    int width = m_Target->width();
    int currentHeight = m_Animation->currentValue().toDouble();

    if(TargetSide::Bottom == m_Side)
    {
      QRect geom(0, m_Target->height() - currentHeight, width, maxHeight);
      m_Frame->setGeometry(geom);
    }
    else
    {
      QRect geom(0, currentHeight - maxHeight, width, maxHeight);
      m_Frame->setGeometry(geom);
    }
  }

  m_Frame->update();
  if(isChecked())
  {
    m_Frame->show();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::startAnimation(bool forward)
{
  m_Animation->stop();
  if(forward)
  {
    m_Animation->setStartValue(m_Animation->currentValue());
    m_Animation->setEndValue(getEndValue());
  }
  else
  {
    m_Animation->setStartValue(m_Animation->currentValue());
    m_Animation->setEndValue(0.0);
  }
  m_Animation->start();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double SVOverlayWidgetButton::getEndValue() const
{
  if(nullptr == m_Source)
  {
    return 50;
  }

  if(TargetSide::Left == m_Side || TargetSide::Right == m_Side)
  {
    return m_Source->sizeHint().width();
  }
  else
  {
    return m_Source->sizeHint().height();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVOverlayWidgetButton::TargetSide SVOverlayWidgetButton::getSide() const
{
  return m_Side;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* SVOverlayWidgetButton::getTarget() const
{
  return m_Target;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* SVOverlayWidgetButton::getSource() const
{
  return m_Source;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setExpanded(bool expanded)
{
  if(expanded)
  {
    // Do not allow multiple overlapping buttons to be checked
    for(SVOverlayWidgetButton* button : m_OverlappingButtons)
    {
      button->setChecked(false);
    }
  }

  m_Frame->raise();
  startAnimation(expanded);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVOverlayWidgetButton::isExpanded() const
{
  return isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setDuration(int duration)
{
  m_Animation->setDuration(duration);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SVOverlayWidgetButton::getDuration() const
{
  return m_Animation->duration();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::addOverlappingButton(SVOverlayWidgetButton* button)
{
  m_OverlappingButtons.push_back(button);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setOverlappingButtons(QVector<SVOverlayWidgetButton*> buttons)
{
  m_OverlappingButtons = buttons;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<SVOverlayWidgetButton*> SVOverlayWidgetButton::getOverlappingButtons() const
{
  return m_OverlappingButtons;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVOverlayWidgetButton::eventFilter(QObject* obj, QEvent* event)
{
  if(event->type() == QEvent::Resize)
  {
    updateOverlay();
  }

  return QPushButton::eventFilter(obj, event);
}

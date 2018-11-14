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

#include "SVOverlayWidgetButton.h"

#include <QtWidgets/QMainWindow>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVOverlayWidgetButton::SVOverlayWidgetButton(QWidget* parent)
: QPushButton(parent)
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVOverlayWidgetButton::SVOverlayWidgetButton(QString text, QWidget* parent)
: QPushButton(text, parent)
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setupGui()
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

  m_TopSpacer = new QSpacerItem(0, 0);
  m_BottomSpacer = new QSpacerItem(0, 0);
  m_LeftSpacer = new QSpacerItem(0, 0);
  m_RightSpacer = new QSpacerItem(0, 0);

  m_Layout = new QGridLayout(m_Frame);
  m_Layout->addItem(m_TopSpacer, 0, 1);
  m_Layout->addItem(m_LeftSpacer, 1, 0);
  m_Layout->addItem(m_RightSpacer, 1, 2);
  m_Layout->addItem(m_BottomSpacer, 2, 1);
  m_Layout->setMargin(2);

  m_Frame = new QFrame();
  m_Frame->hide();
  m_Frame->setLayout(m_Layout);
  m_Frame->setAutoFillBackground(true);
  m_Frame->setFrameStyle(QFrame::Box | QFrame::Plain);

  setSide(m_Side);
  checkValidity();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::enable()
{
  m_Enabled = true;
  checkValidity();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::disable()
{
  m_Enabled = false;
  setChecked(false);
  setEnabled(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setSide(TargetSide side)
{
  m_Side = side;

  QSizePolicy::Policy min = QSizePolicy::Policy::Minimum;
  QSizePolicy::Policy max = QSizePolicy::Policy::Expanding;

  m_Frame->setUpdatesEnabled(false);
  switch(m_Side)
  {
  case TargetSide::Top:
    m_TopSpacer->changeSize(0, 0, min, max);
    m_BottomSpacer->changeSize(0, 0, min, min);
    m_LeftSpacer->changeSize(0, 0, min, min);
    m_RightSpacer->changeSize(0, 0, min, min);
    break;
  case TargetSide::Bottom:
    m_TopSpacer->changeSize(0, 0, min, min);
    m_BottomSpacer->changeSize(0, 0, min, max);
    m_LeftSpacer->changeSize(0, 0, min, min);
    m_RightSpacer->changeSize(0, 0, min, min);
    break;
  case TargetSide::Left:
    m_TopSpacer->changeSize(0, 0, min, min);
    m_BottomSpacer->changeSize(0, 0, min, min);
    m_LeftSpacer->changeSize(0, 0, max, min);
    m_RightSpacer->changeSize(0, 0, min, min);
    break;
  case TargetSide::Right:
    m_TopSpacer->changeSize(0, 0, min, min);
    m_BottomSpacer->changeSize(0, 0, min, max);
    m_LeftSpacer->changeSize(0, 0, min, min);
    m_RightSpacer->changeSize(0, 0, max, min);
    break;
  default:
    m_TopSpacer->changeSize(0, 0, min, min);
    m_BottomSpacer->changeSize(0, 0, min, min);
    m_LeftSpacer->changeSize(0, 0, min, min);
    m_RightSpacer->changeSize(0, 0, min, min);
    break;
  }

  updateSourcePolicy();
  updateOverlay();
  m_Frame->setUpdatesEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setDockLocation(Qt::DockWidgetArea dockLocation)
{
  switch(dockLocation)
  {
  case Qt::DockWidgetArea::LeftDockWidgetArea:
    setSide(TargetSide::Left);
    break;
  case Qt::DockWidgetArea::RightDockWidgetArea:
    setSide(TargetSide::Right);
    break;
  case Qt::DockWidgetArea::TopDockWidgetArea:
    setSide(TargetSide::Top);
    break;
  case Qt::DockWidgetArea::BottomDockWidgetArea:
    setSide(TargetSide::Bottom);
    break;
  default:
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::updateSourcePolicy()
{
  if(nullptr == m_Source)
  {
    return;
  }

  if(expandsEntireDistance())
  {
    if(nullptr == m_Target)
    {
      return;
    }

    if(TargetSide::Left == m_Side || TargetSide::Right == m_Side)
    {
      m_Source->setMinimumWidth(m_Target->width() - 2 * m_Layout->margin());
      m_Source->setMinimumHeight(0);
    }
    else
    {
      m_Source->setMinimumWidth(0);
      m_Source->setMinimumHeight(m_Target->height() - 2 * m_Layout->margin());
    }
  }
  else
  {
    m_Source->setMinimumWidth(0);
    m_Source->setMinimumHeight(0);

    if(TargetSide::Left == m_Side || TargetSide::Right == m_Side)
    {
      m_Source->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Expanding);
    }
    else
    {
      m_Source->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
    }
  }

  m_Frame->update();
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
    m_Frame->setFixedSize(m_Target->size());
  }

  if((m_Source != nullptr) && (m_Target != nullptr))
  {
    updateOverlay();
  }

  checkValidity();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setDockWidget(QMainWindow* window, QDockWidget* dockWidget)
{
  if(m_DockWidget != nullptr)
  {
    m_DockWidget->removeEventFilter(this);
  }

  if(nullptr == dockWidget)
  {
    m_DockWidget = dockWidget;
    return;
  }

  m_DockWidget = dockWidget;
  setTarget(dockWidget->widget());
  dockWidget->installEventFilter(this);
  setDockLocation(window->dockWidgetArea(dockWidget));
  connect(dockWidget, &QDockWidget::dockLocationChanged, this, &SVOverlayWidgetButton::setDockLocation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setSource(QWidget* source)
{
  m_Frame->setUpdatesEnabled(false);
  bool addItem = (nullptr == m_Source);
  if(m_Source != nullptr)
  {
    m_Layout->replaceWidget(m_Source, source);
    m_Source->hide();
  }
  m_Source = source;

  if(source != nullptr)
  {
    if(addItem)
    {
      m_Layout->addWidget(m_Source, 1, 1);
    }

    m_Source->show();
    if(m_Target != nullptr)
    {
      if(isExpanded())
      {
        setExpanded(true);
      }
      updateOverlay();
    }
  }
  else
  {
    m_Frame->hide();
    m_Frame->setUpdatesEnabled(true);
    setChecked(false);

    if(m_Target != nullptr)
    {
      m_Target->update();
    }
  }

  updateSourcePolicy();
  m_Frame->setUpdatesEnabled(true);
  checkValidity();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SVOverlayWidgetButton::getMargin() const
{
  return m_Layout->margin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setMargin(int margin)
{
  m_Layout->setMargin(margin);
  updateSourcePolicy();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SVOverlayWidgetButton::checkValidity()
{
  bool valid = (m_Source != nullptr) && (m_Target != nullptr);
  if(!valid)
  {
    setChecked(false);
  }
  setEnabled(valid && m_Enabled);
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

  m_Frame->setUpdatesEnabled(false);
  int targetHeight = m_Target->height();
  int targetWidth = m_Target->width();

  bool animationComplete = m_Animation->currentTime() == m_Animation->duration();
  if(TargetSide::Left == m_Side || TargetSide::Right == m_Side)
  {
    int currentWidth = m_Animation->currentValue().toInt();
    if(animationComplete && m_ExpandsEntireDistance && currentWidth != 0)
    {
      currentWidth = targetWidth;
    }

    if(TargetSide::Left == m_Side)
    {
      QPoint pos(currentWidth - targetWidth, 0);
      m_Frame->move(pos);
    }
    else
    {
      QPoint pos(targetWidth - currentWidth, 0);
      m_Frame->move(pos);
    }
  }
  else
  {
    int currentHeight = m_Animation->currentValue().toInt();
    if(animationComplete && m_ExpandsEntireDistance && currentHeight != 0)
    {
      currentHeight = targetHeight;
    }

    if(TargetSide::Bottom == m_Side)
    {
      QPoint pos(0, targetHeight - currentHeight);
      m_Frame->move(pos);
    }
    else
    {
      QPoint pos(0, currentHeight - targetHeight);
      m_Frame->move(pos);
    }
  }

  if(isChecked())
  {
    m_Frame->show();
  }
  m_Frame->setUpdatesEnabled(true);
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
    return 0;
  }

  if(m_ExpandsEntireDistance)
  {
    if(TargetSide::Left == m_Side || TargetSide::Right == m_Side)
    {
      return m_Target->size().width();
    }

      return m_Target->size().height();
  }

    if(TargetSide::Left == m_Side || TargetSide::Right == m_Side)
    {
      return m_Source->sizeHint().width() + m_Layout->margin() * 2;
    }

      return m_Source->sizeHint().height() + m_Layout->margin() * 2;
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
    // Force the dock widget to be visible.  This is only done for dock widgets
    if(m_DockWidget != nullptr)
    {
      m_DockWidget->setVisible(true);
    }

    // Do not allow multiple overlapping buttons to be checked
    for(SVOverlayWidgetButton* button : m_OverlappingButtons)
    {
      button->setChecked(false);
    }
    m_Frame->raise();
  }

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
bool SVOverlayWidgetButton::expandsEntireDistance() const
{
  return m_ExpandsEntireDistance;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SVOverlayWidgetButton::setExpandsEntireDistance(bool expands)
{
  m_ExpandsEntireDistance = expands;

  if(expands)
  {
    updateSourcePolicy();
    if(isExpanded())
    {
      startAnimation(true);
    }
  }
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
  QEvent::Type type = event->type();
  if(type == QEvent::Resize)
  {
    m_Frame->setUpdatesEnabled(false);
    m_Frame->setFixedSize(m_Target->size());
    updateSourcePolicy();
    updateOverlay();
    m_Frame->setUpdatesEnabled(true);
  }

  return QPushButton::eventFilter(obj, event);
}

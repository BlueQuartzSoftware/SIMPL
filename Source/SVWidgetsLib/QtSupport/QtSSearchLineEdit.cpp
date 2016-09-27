
#include "QtSSearchLineEdit.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QString>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStyle>

#include "moc_QtSSearchLineEdit.cpp"

/**
 * @brief execMenuAtWidget
 * @param menu
 * @param widget
 */
static void execMenuAtWidget(QMenu* menu, QWidget* widget)
{
  QPoint p;
  QRect screen = QApplication::desktop()->availableGeometry(widget);
  QSize sh = menu->sizeHint();
  QRect rect = widget->rect();
  if(widget->isRightToLeft())
  {
    if(widget->mapToGlobal(QPoint(0, rect.bottom())).y() + sh.height() <= screen.height())
    {
      p = widget->mapToGlobal(rect.bottomRight());
    }
    else
    {
      p = widget->mapToGlobal(rect.topRight() - QPoint(0, sh.height()));
    }
    p.rx() -= sh.width();
  }
  else
  {
    if(widget->mapToGlobal(QPoint(0, rect.bottom())).y() + sh.height() <= screen.height())
    {
      p = widget->mapToGlobal(rect.bottomLeft());
    }
    else
    {
      p = widget->mapToGlobal(rect.topLeft() - QPoint(0, sh.height()));
    }
  }
  p.rx() = qMax(screen.left(), qMin(p.x(), screen.right() - sh.width()));
  p.ry() += 1;

  menu->exec(p);
}

enum
{
  margin = 6
};

#define ICONBUTTON_HEIGHT 18
#define FADE_TIME 160

/**
 * @brief The SearchLineEditPrivate class
 */
class SearchLineEditPrivate : public QObject
{
public:
  explicit SearchLineEditPrivate(QtSSearchLineEdit* parent);

  virtual bool eventFilter(QObject* obj, QEvent* event);

  QtSSearchLineEdit* m_LineEdit;
  QPixmap m_PixMaps[2];
  QMenu* m_ButtonMenus[2];
  bool m_MenuTabFocusTriggers[2];
  IconButton* m_IconButtons[2];
  bool m_IconEnabled[2];
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SearchLineEditPrivate::SearchLineEditPrivate(QtSSearchLineEdit* parent)
: QObject(parent)
, m_LineEdit(parent)
{
  for(int i = 0; i < 2; ++i)
  {
    m_ButtonMenus[i] = 0;
    m_MenuTabFocusTriggers[i] = false;
    m_IconButtons[i] = new IconButton(parent);
    m_IconButtons[i]->installEventFilter(this);
    m_IconButtons[i]->hide();
    m_IconButtons[i]->setAutoHide(false);
    m_IconEnabled[i] = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SearchLineEditPrivate::eventFilter(QObject* obj, QEvent* event)
{
  int buttonIndex = -1;
  for(int i = 0; i < 2; ++i)
  {
    if(obj == m_IconButtons[i])
    {
      buttonIndex = i;
      break;
    }
  }
  if(buttonIndex == -1)
  {
    return QObject::eventFilter(obj, event);
  }
  switch(event->type())
  {
  case QEvent::FocusIn:
    if(m_MenuTabFocusTriggers[buttonIndex] && m_ButtonMenus[buttonIndex])
    {
      m_LineEdit->setFocus();
      execMenuAtWidget(m_ButtonMenus[buttonIndex], m_IconButtons[buttonIndex]);
      return true;
    }
  default:
    break;
  }
  return QObject::eventFilter(obj, event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSSearchLineEdit::QtSSearchLineEdit(QWidget* parent)
: QLineEdit(parent)
, d(new SearchLineEditPrivate(this))
{
  ensurePolished();
  updateMargins();

  connect(this, SIGNAL(textChanged(QString)), this, SLOT(checkButtons(QString)));
  connect(d->m_IconButtons[Left], SIGNAL(clicked()), this, SLOT(iconClicked()));
  connect(d->m_IconButtons[Right], SIGNAL(clicked()), this, SLOT(iconClicked()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::checkButtons(const QString& text)
{
  if(m_OldText.isEmpty() || text.isEmpty())
  {
    for(int i = 0; i < 2; ++i)
    {
      if(d->m_IconButtons[i]->hasAutoHide())
      {
        d->m_IconButtons[i]->animateShow(!text.isEmpty());
      }
    }
    m_OldText = text;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QtSSearchLineEdit::~QtSSearchLineEdit()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::setButtonVisible(Side side, bool visible)
{
  d->m_IconButtons[side]->setVisible(visible);
  d->m_IconEnabled[side] = visible;
  updateMargins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QtSSearchLineEdit::isButtonVisible(Side side) const
{
  return d->m_IconEnabled[side];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::iconClicked()
{
  IconButton* button = qobject_cast<IconButton*>(sender());
  int index = -1;
  for(int i = 0; i < 2; ++i)
    if(d->m_IconButtons[i] == button)
    {
      index = i;
    }
  if(index == -1)
  {
    return;
  }
  if(d->m_ButtonMenus[index])
  {
    execMenuAtWidget(d->m_ButtonMenus[index], button);
  }
  else
  {
    emit buttonClicked((Side)index);
    if(index == Left)
    {
      emit leftButtonClicked();
    }
    else if(index == Right)
    {
      emit rightButtonClicked();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::updateMargins()
{
  bool leftToRight = (layoutDirection() == Qt::LeftToRight);
  Side realLeft = (leftToRight ? Left : Right);
  Side realRight = (leftToRight ? Right : Left);

  int leftMargin = d->m_IconButtons[realLeft]->pixmap().width() + 8;
  int rightMargin = d->m_IconButtons[realRight]->pixmap().width() + 8;
  // Note KDE does not reserve space for the highlight color
  if(style()->inherits("OxygenStyle"))
  {
    leftMargin = qMax(24, leftMargin);
    rightMargin = qMax(24, rightMargin);
  }

  QMargins margins((d->m_IconEnabled[realLeft] ? leftMargin : 0), 0, (d->m_IconEnabled[realRight] ? rightMargin : 0), 0);

  setTextMargins(margins);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::updateButtonPositions()
{
  QRect contentRect = rect();
  for(int i = 0; i < 2; ++i)
  {
    Side iconpos = (Side)i;
    if(layoutDirection() == Qt::RightToLeft)
    {
      iconpos = (iconpos == Left ? Right : Left);
    }

    if(iconpos == QtSSearchLineEdit::Right)
    {
      const int iconoffset = textMargins().right() + 4;
      d->m_IconButtons[i]->setGeometry(contentRect.adjusted(width() - iconoffset, 0, 0, 0));
    }
    else
    {
      const int iconoffset = textMargins().left() + 4;
      d->m_IconButtons[i]->setGeometry(contentRect.adjusted(0, 0, -width() + iconoffset, 0));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::resizeEvent(QResizeEvent*)
{
  updateButtonPositions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::setButtonPixmap(Side side, const QPixmap& buttonPixmap)
{
  d->m_IconButtons[side]->setPixmap(buttonPixmap);
  updateMargins();
  updateButtonPositions();
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPixmap QtSSearchLineEdit::buttonPixmap(Side side) const
{
  return d->m_PixMaps[side];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::setButtonMenu(Side side, QMenu* buttonMenu)
{
  d->m_ButtonMenus[side] = buttonMenu;
  d->m_IconButtons[side]->setIconOpacity(1.0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* QtSSearchLineEdit::buttonMenu(Side side) const
{
  return d->m_ButtonMenus[side];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QtSSearchLineEdit::hasMenuTabFocusTrigger(Side side) const
{
  return d->m_MenuTabFocusTriggers[side];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::setMenuTabFocusTrigger(Side side, bool v)
{
  if(d->m_MenuTabFocusTriggers[side] == v)
  {
    return;
  }

  d->m_MenuTabFocusTriggers[side] = v;
  d->m_IconButtons[side]->setFocusPolicy(v ? Qt::TabFocus : Qt::NoFocus);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QtSSearchLineEdit::hasAutoHideButton(Side side) const
{
  return d->m_IconButtons[side]->hasAutoHide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::setAutoHideButton(Side side, bool h)
{
  d->m_IconButtons[side]->setAutoHide(h);
  if(h)
  {
    d->m_IconButtons[side]->setIconOpacity(text().isEmpty() ? 0.0 : 1.0);
  }
  else
  {
    d->m_IconButtons[side]->setIconOpacity(1.0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::setButtonToolTip(Side side, const QString& tip)
{
  d->m_IconButtons[side]->setToolTip(tip);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QtSSearchLineEdit::setButtonFocusPolicy(Side side, Qt::FocusPolicy policy)
{
  d->m_IconButtons[side]->setFocusPolicy(policy);
}

// IconButton - helper class to represent a clickable icon
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IconButton::IconButton(QWidget* parent)
: QAbstractButton(parent)
, m_autoHide(false)
{
  setCursor(Qt::ArrowCursor);
  setFocusPolicy(Qt::NoFocus);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IconButton::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  QRect pixmapRect = QRect(0, 0, m_pixmap.width(), m_pixmap.height());
  pixmapRect.moveCenter(rect().center());

  if(m_autoHide)
  {
    painter.setOpacity(m_iconOpacity);
  }

  painter.drawPixmap(pixmapRect, m_pixmap);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IconButton::animateShow(bool visible)
{
  if(visible)
  {
    QPropertyAnimation* animation = new QPropertyAnimation(this, "iconOpacity");
    animation->setDuration(FADE_TIME);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  }
  else
  {
    QPropertyAnimation* animation = new QPropertyAnimation(this, "iconOpacity");
    animation->setDuration(FADE_TIME);
    animation->setEndValue(0.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
  }
}

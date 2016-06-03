

#include "DSplashScreen.h"


#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QTextDocument>
#include <QtGui/QTextCursor>
#include <QtCore/QDebug>
#include <QtWidgets/QWidget>

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_DSplashScreen.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DSplashScreenPrivate
{

  public:
    QPixmap pixmap;
    QString currStatus;
    QColor currColor;
    int currAlign;

    inline DSplashScreenPrivate();
};

/**
    Construct a splash screen that will display the \a pixmap.

    There should be no need to set the widget flags, \a f, except
    perhaps Qt::WindowStaysOnTopHint.
*/
DSplashScreen::DSplashScreen(const QPixmap& pixmap, Qt::WindowFlags f)
  : QWidget(0, Qt::SplashScreen | Qt::FramelessWindowHint | f)
{
  d_ptr = new DSplashScreenPrivate;
  setPixmap(pixmap);  // Does an implicit repaint
}

/**
    \overload

    This function allows you to specify a parent for your splashscreen. The
    typical use for this constructor is if you have a multiple screens and
    prefer to have the splash screen on a different screen than your primary
    one. In that case pass the proper desktop() as the \a parent.
*/
DSplashScreen::DSplashScreen(QWidget* parent, const QPixmap& pixmap, Qt::WindowFlags f)
  : QWidget(parent, Qt::SplashScreen | f)
{
  d_ptr = new DSplashScreenPrivate;
  d_func()->pixmap = pixmap;
  setPixmap(d_func()->pixmap);  // Does an implicit repaint
}

/**
  Destructor.
*/
DSplashScreen::~DSplashScreen()
{
  delete d_ptr;
}

/**
    \reimp
*/
void DSplashScreen::mousePressEvent(QMouseEvent*)
{
  hide();
}

/**
    This overrides QWidget::repaint(). It differs from the standard
    repaint function in that it also calls QApplication::flush() to
    ensure the updates are displayed, even when there is no event loop
    present.
*/
void DSplashScreen::repaint()
{
  QWidget::repaint();
  QApplication::flush();
}

/**
    \fn DSplashScreen::messageChanged(const QString &message)

    This signal is emitted when the message on the splash screen
    changes. \a message is the new message and is a null-string
    when the message has been removed.

    \sa showMessage(), clearMessage()
*/



/**
    Draws the \a message text onto the splash screen with color \a
    color and aligns the text according to the flags in \a alignment.

    To make sure the splash screen is repainted immediately, you can
    call \l{QCoreApplication}'s
    \l{QCoreApplication::}{processEvents()} after the call to
    showMessage(). You usually want this to make sure that the message
    is kept up to date with what your application is doing (e.g.,
    loading files).

    \sa Qt::Alignment, clearMessage()
*/
void DSplashScreen::showMessage(const QString& message, int alignment,
                                const QColor& color)
{
  Q_D(DSplashScreen);
  d->currStatus = message;
  d->currAlign = alignment;
  d->currColor = color;
  emit messageChanged(d->currStatus);
  repaint();
}

/**
    Removes the message being displayed on the splash screen

    \sa showMessage()
 */
void DSplashScreen::clearMessage()
{
  d_func()->currStatus.clear();
  emit messageChanged(d_func()->currStatus);
  repaint();
}

/**
    Makes the splash screen wait until the widget \a mainWin is displayed
    before calling close() on itself.
*/
void DSplashScreen::finish(QWidget* mainWin)
{
  if (mainWin)
  {
#if defined(Q_WS_X11)
    /** FIXME: This fails to compile on Linux with Qt 4.8.6. Commented out for now.
        qt_x11_wait_for_window_manager(mainWin);
     */
#endif
  }
  close();
}

/**
    Sets the pixmap that will be used as the splash screen's image to
    \a pixmap.
*/
void DSplashScreen::setPixmap(const QPixmap& pixmap)
{
  Q_D(DSplashScreen);

  d->pixmap = pixmap;
  setAttribute(Qt::WA_TranslucentBackground, pixmap.hasAlpha());

  QRect r(QPoint(), d->pixmap.size());
  resize(r.size());
  move(QApplication::desktop()->screenGeometry().center() - r.center());
  if (isVisible())
  { repaint(); }
}

/**
    Returns the pixmap that is used in the splash screen. The image
    does not have any of the text drawn by showMessage() calls.
*/
const QPixmap DSplashScreen::pixmap() const
{
  return d_func()->pixmap;
}

/**
    \internal
*/
inline DSplashScreenPrivate::DSplashScreenPrivate() : currAlign(Qt::AlignLeft)
{
}

/**
    Draw the contents of the splash screen using painter \a painter.
    The default implementation draws the message passed by showMessage().
    Reimplement this function if you want to do your own drawing on
    the splash screen.
*/
void DSplashScreen::drawContents(QPainter* painter)
{
  Q_D(DSplashScreen);
  painter->setPen(d->currColor);
  QRect r = rect();
  r = rect().adjusted(r.width() * .4054, r.height() * .6765, 0, 0);
  {
    painter->setBrush(QBrush(QColor(255, 255, 255, 255)));
    QPen penHText(QColor("#FFFFFF"));
    painter->setPen(penHText);
    painter->drawText(r, d->currAlign, d->currStatus);
  }
}

/** \reimp */
bool DSplashScreen::event(QEvent* e)
{
  if (e->type() == QEvent::Paint)
  {
    Q_D(DSplashScreen);
    QPainter painter(this);
    if (!d->pixmap.isNull())
    { painter.drawPixmap(QPoint(), d->pixmap); }
    drawContents(&painter);
  }
  return QWidget::event(e);
}




#include "SVControlWidgets.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPushButton::SVPushButton(QWidget* parent) :
QPushButton(parent)
{}

SVPushButton::~SVPushButton() = default;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVIconPushButton::SVIconPushButton(QWidget* parent) :
QPushButton(parent)
{}

SVIconPushButton::~SVIconPushButton() = default;



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVToolButton::SVToolButton(QWidget* parent) :
QToolButton(parent)
{}

SVToolButton::~SVToolButton() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVSpinBox::SVSpinBox(QWidget* parent) :
QSpinBox(parent)
{
  setAttribute(Qt::WA_MacShowFocusRect, false);
  QStyle* style = this->style();
  QIcon arrowUpIcon = style->standardIcon(QStyle::SP_TitleBarShadeButton);
  QPixmap pixmap = arrowUpIcon.pixmap(QSize(24, 24));
  pixmap.save("/Users/bluequartz/Downloads/up_arrow.png");
}

SVSpinBox::~SVSpinBox() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVDoubleSpinBox::SVDoubleSpinBox(QWidget* parent) :
QDoubleSpinBox(parent)
{
  setAttribute(Qt::WA_MacShowFocusRect, false);
}

SVDoubleSpinBox::~SVDoubleSpinBox() = default;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVLineEdit::SVLineEdit(QWidget* parent) :
QLineEdit(parent)
{
  setAttribute(Qt::WA_MacShowFocusRect, false);
}

SVLineEdit::~SVLineEdit() = default;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVTreeView::SVTreeView(QWidget* parent) :
QTreeView(parent)
{
  setAttribute(Qt::WA_MacShowFocusRect, false);
}

SVTreeView::~SVTreeView() = default;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVListView::SVListView(QWidget* parent) :
QListView(parent)
{
  setAttribute(Qt::WA_MacShowFocusRect, false);
}

SVListView::~SVListView() = default;

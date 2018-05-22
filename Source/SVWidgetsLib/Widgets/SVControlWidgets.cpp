
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

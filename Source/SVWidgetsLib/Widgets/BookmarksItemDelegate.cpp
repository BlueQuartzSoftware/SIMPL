/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <QtCore/QFileInfo>

#include <QtWidgets/QLineEdit>

#include <QtGui/QIntValidator>
#include <QtGui/QPainter>

#include "SVWidgetsLib/Widgets/SVStyle.h"
#include "SVWidgetsLib/Widgets/BookmarksItem.h"
#include "SVWidgetsLib/Widgets/BookmarksItemDelegate.h"
#include "SVWidgetsLib/Widgets/BookmarksModel.h"

namespace
{
  const int k_ButtonSize = 16;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItemDelegate::BookmarksItemDelegate(QObject* parent)
: QStyledItemDelegate(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItemDelegate::~BookmarksItemDelegate() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* BookmarksItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* editor = new QLineEdit(parent);
  return editor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  QString value = index.model()->data(index, BookmarksModel::Roles::NameRole).toString();
  QLineEdit* line = static_cast<QLineEdit*>(editor);
  line->setText(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  BookmarksModel* bModel = qobject_cast<BookmarksModel*>(model);

  QLineEdit* line = static_cast<QLineEdit*>(editor);
  QString value = line->text();

  if(value.isEmpty() == false)
  {
    QModelIndex bIndex = bModel->index(index.row(), BookmarksItem::Contents, index.parent());
    bModel->setData(bIndex, value, BookmarksModel::Roles::NameRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  editor->setGeometry(option.rect);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSize BookmarksItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  return QSize(size.width(), 20);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  SVStyle* styles = SVStyle::Instance();

  BookmarksModel* model = BookmarksModel::Instance();

  // Place any painting code here
  painter->save();

  QColor bgColor;
  if (option.state & QStyle::State_Selected)
  {
    bgColor = styles->getQTreeViewItemSelectedActive_background_color();
  }
  else if (option.state & QStyle::State_MouseOver)
  {
    bgColor = styles->getQTreeViewItemHover_background_color();
  }
  else
  {
    if (model->data(index, BookmarksModel::Roles::ErrorsRole).toBool())
    {
      bgColor = styles->getQTreeViewItem_error_background_color();
    }
    else
    {
      bgColor = styles->getQTreeViewItem_background_color();
    }
  }

  if (!bgColor.isValid())
  {
    if (option.state & QStyle::State_Selected)
    {
      bgColor = option.palette.color(QPalette::Highlight);
    }
    else if (option.features & QStyleOptionViewItem::Alternate)
    {
      bgColor = option.palette.color(QPalette::AlternateBase);
    }
    else
    {
      if (model->data(index, BookmarksModel::Roles::ErrorsRole).toBool())
      {
        bgColor = styles->getQTreeViewItem_error_background_color();
      }
      else
      {
        bgColor = option.palette.color(QPalette::Base);
      }
    }
  }

  painter->fillRect(option.rect, bgColor);

  QColor fontColor;
  int fontSize = styles->getQTreeViewItem_font_size();
  QFont font = painter->font();
  font.setPixelSize(fontSize);
  painter->setFont(font);

  if (option.state & QStyle::State_MouseOver)
  {
    fontColor = styles->getQTreeViewItemHover_color();
  }
  else
  {
    if (model->data(index, BookmarksModel::Roles::ErrorsRole).toBool() && !(option.state & QStyle::State_Selected))
    {
      fontColor = styles->getQTreeViewItem_error_color();
    }
    else
    {
      fontColor = styles->getQTreeViewItem_color();
    }
  }

  if (!fontColor.isValid())
  {
    if (option.state & QStyle::State_Selected)
    {
      fontColor = option.palette.color(QPalette::HighlightedText);
    }
    else
    {
      if (model->data(index, BookmarksModel::Roles::ErrorsRole).toBool() && !(option.state & QStyle::State_Selected))
      {
        fontColor = styles->getQTreeViewItem_error_color();
      }
      else
      {
        fontColor = option.palette.color(QPalette::WindowText);
      }
    }
  }

  painter->setPen(QPen(fontColor));

  const int margin = 6;

  QRect contentsRect = option.rect;
  contentsRect.setLeft(contentsRect.left() + margin);

  // Draw the item type icon
  QRect iconRect = contentsRect;
  iconRect.setWidth(::k_ButtonSize);
  iconRect.setHeight(::k_ButtonSize);

  BookmarksItem::ItemType itemType = static_cast<BookmarksItem::ItemType>(model->data(index, BookmarksModel::Roles::ItemTypeRole).toInt());
  QPixmap iconPixmap;
  if (itemType == BookmarksItem::ItemType::Bookmark)
  {
    iconPixmap = QPixmap(":/bookmark.png");
    if (painter->device()->devicePixelRatio() == 2)
    {
      iconPixmap = QPixmap(":/bookmark@2x.png");
    }
  }
  else
  {
    iconPixmap = QPixmap(":/folder_blue.png");
    if (painter->device()->devicePixelRatio() == 2)
    {
      iconPixmap = QPixmap(":/folder_blue@2x.png");
    }
  }

  painter->drawPixmap(iconRect, iconPixmap);

  QFontMetrics fontMetrics(painter->font());
  int fontHeight = fontMetrics.height();
  int fontMargin = ((option.rect.height() - fontHeight) / 2) - 1;

  QRect textRect = contentsRect;
  textRect.setTop(textRect.top() + fontMargin);
  textRect.setLeft(textRect.left() + ::k_ButtonSize + margin);
  painter->drawText(textRect, 0, model->data(index, BookmarksModel::Roles::NameRole).toString());

  painter->restore();
}

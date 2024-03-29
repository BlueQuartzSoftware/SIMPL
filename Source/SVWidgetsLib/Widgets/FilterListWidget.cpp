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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FilterListWidget.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>

#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListWidget::FilterListWidget(QWidget* parent)
: QListWidget(parent)
{
  setAcceptDrops(false);
  setDragEnabled(true);
  setDragDropMode(QAbstractItemView::DragOnly);
  setAlternatingRowColors(true);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setUniformItemSizes(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListWidget::~FilterListWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListWidget::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton)
  {
    startPos = event->pos();
  }
  QListWidget::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListWidget::mouseMoveEvent(QMouseEvent* event)
{
  if(event->buttons() & Qt::LeftButton)
  {
    int distance = (event->pos() - startPos).manhattanLength();
    if(distance >= QApplication::startDragDistance())
    {
      performDrag();
    }
  }
  QListWidget::mouseMoveEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListWidget::performDrag()
{
  QListWidgetItem* item = currentItem();
  if(item != nullptr)
  {
    QJsonObject obj;
    QString filterHumanLabel = item->text();
    QString filterClassName = item->data(Qt::UserRole).toString();

    obj[item->text()] = item->data(Qt::UserRole).toString();

    QJsonDocument doc(obj);
    QByteArray jsonArray = doc.toJson();

    QMimeData* mimeData = new QMimeData;
    mimeData->setData(SIMPLView::DragAndDrop::FilterListItem, jsonArray);

    QString grpName = "";
    FilterManager* fm = FilterManager::Instance();
    if(nullptr != fm)
    {
      IFilterFactory::Pointer wf = fm->getFactoryFromClassName(filterClassName);
      if(nullptr != wf)
      {
        AbstractFilter::Pointer filter = wf->create();
        grpName = filter->getGroupName();
      }
    }

    QColor grpColor = SVStyle::Instance()->ColorForFilterGroup(grpName);
    const QPixmap dragIcon = DataArrayPathSelectionWidget::CreateDragIcon(filterHumanLabel, grpColor);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(dragIcon);
    drag->exec(Qt::CopyAction);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListWidget::dragEnterEvent(QDragEnterEvent* event)
{
  FilterListWidget* source = qobject_cast<FilterListWidget*>(event->source());
  if((source != nullptr) && source != this)
  {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListWidget::dragMoveEvent(QDragMoveEvent* event)
{
  FilterListWidget* source = qobject_cast<FilterListWidget*>(event->source());
  if((source != nullptr) && source != this)
  {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListWidget::dropEvent(QDropEvent* event)
{
  FilterListWidget* source = qobject_cast<FilterListWidget*>(event->source());
  if((source != nullptr) && source != this)
  {
    addItem(event->mimeData()->text());
    event->setDropAction(Qt::MoveAction);
    event->accept();
  }
}

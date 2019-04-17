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

#include "FilterListView.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QMimeData>

#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>

#include "SIMPLib/Common/DocRequestManager.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"

#include "SVWidgetsLib/Widgets/SVStyle.h"
#include "SVWidgetsLib/Widgets/FilterListModel.h"
#include "SVWidgetsLib/Widgets/FilterListItem.h"
#include "SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListView::FilterListView(QWidget* parent)
: QTreeView(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  setRootIsDecorated(false);
  setAttribute(Qt::WA_MacShowFocusRect, false);
  
  FilterListModel* model = new FilterListModel(this);
  setModel(model);

  connectSignalsSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListView::~FilterListView() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::connectSignalsSlots()
{
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(requestContextMenu(const QPoint&)));

  connect(this, &FilterListView::doubleClicked, [=] (const QModelIndex &index) {
    FilterListModel* model = getFilterListModel();

    QString filterClassName = model->data(index, FilterListModel::Roles::ClassNameRole).toString();
    emit filterItemDoubleClicked(filterClassName);
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::addFilter(AbstractFilter::Pointer filter)
{
  addFilter(filter, QModelIndex());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::addFilter(AbstractFilter::Pointer filter, SearchGroup group)
{
  if(!m_SearchGroupIndexMap.contains(group))
  {
    addGroup(group);
  }

  QModelIndex groupIndex = m_SearchGroupIndexMap[group];
  addFilter(filter, groupIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::addFilter(AbstractFilter::Pointer filter, const QModelIndex &parent)
{
  QString humanName = filter->getHumanLabel();
  QString iconName(":/Groups/");
  iconName.append(filter->getGroupName());

  QIcon icon = SVStyle::Instance()->IconForGroup(filter->getGroupName());

  // Create the QListWidgetItem and add it to the filterListView
  FilterListModel* model = getFilterListModel();

  int row = model->rowCount(parent);
  model->insertRow(row, parent);

  QModelIndex index = model->index(row, FilterListModel::Column::Contents, parent);
  model->setData(index, humanName, Qt::DisplayRole);
  model->setData(index, icon, Qt::DecorationRole);
  model->setData(index, FilterListItem::ItemType::Filter, FilterListModel::Roles::ItemTypeRole);

  // Set an "internal" QString that is the name of the filter. We need this value
  // when the item is clicked in order to retreive the Filter Widget from the
  // filter widget manager.
  model->setData(index, filter->getNameOfClass(), FilterListModel::Roles::ClassNameRole);

  // Allow a basic mouse hover tool tip that gives some summary information on the filter.
  model->setData(index, filter->generateHtmlSummary(), Qt::ToolTipRole);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::addGroup(SearchGroup group)
{
  // Create the QListWidgetItem and add it to the filterListView
  FilterListModel* model = getFilterListModel();

  int row = model->rowCount();
  model->insertRow(row);

  QModelIndex index = model->index(row, FilterListModel::Column::Contents);

  switch (group)
  {
    case SearchGroup::HumanLabel:
    {
      model->setData(index, "Human Label", Qt::DisplayRole);
      break;
    }
    case SearchGroup::GroupName:
    {
      model->setData(index, "Group Name", Qt::DisplayRole);
      break;
    }
    case SearchGroup::SubgroupName:
    {
      model->setData(index, "Subgroup Name", Qt::DisplayRole);
      break;
    }
    case SearchGroup::BrandingName:
    {
      model->setData(index, "Branding Name", Qt::DisplayRole);
      break;
    }
    case SearchGroup::CompiledLibraryName:
    {
      model->setData(index, "Compiled Library Name", Qt::DisplayRole);
      break;
    }
    case SearchGroup::Keywords:
    {
      model->setData(index, "Keywords", Qt::DisplayRole);
      break;
    }
  }

  m_SearchGroupIndexMap.insert(group, index);

  model->setData(index, FilterListItem::ItemType::Group, FilterListModel::Roles::ItemTypeRole);

  setExpanded(index, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex FilterListView::findIndexByName(const QString &name)
{
  FilterListModel* model = getFilterListModel();
  for (int i = 0; i < model->rowCount(); i++)
  {
    QModelIndex index = model->index(i, FilterListModel::Column::Contents);
    QString filterHumanLabel = model->data(index, Qt::DisplayRole).toString();
    if (filterHumanLabel == name)
    {
      return index;
    }
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::clear()
{
  FilterListModel* model = getFilterListModel();
  model->removeRows(0, model->rowCount());

  m_SearchGroupIndexMap.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::keyPressEvent(QKeyEvent* event)
{
  listenKeyPressed(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::listenKeyPressed(QKeyEvent* event)
{
  FilterListModel* model = getFilterListModel();
  if (model->rowCount() > 0)
  {
    QModelIndexList selectedIndexes = selectionModel()->selectedRows();

    if(event->key() == Qt::Key_Down)
    {
      QModelIndex index = findNextSelectableIndex();
      if (index.isValid())
      {
        scrollTo(index);
        selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
        setFocus();
      }

      return;
    }
    if(event->key() == Qt::Key_Up)
    {
      QModelIndex index = findPreviousSelectableIndex();
      if (index.isValid())
      {
        if (index.row() == 0)
        {
          scrollTo(index.parent());
        }
        else
        {
          scrollTo(index);
        }
        selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
        setFocus();
      }
      else
      {
        selectionModel()->clear();
      }

      return;
    }
    if(event->key() == Qt::Key_Return)
    {
      if(selectedIndexes.size() == 1)
      {
        QModelIndex selectedIndex = selectedIndexes[0];
        QString filterClassName = model->data(selectedIndex, FilterListModel::Roles::ClassNameRole).toString();
        emit filterItemDoubleClicked(filterClassName);
        return;
      }
    }
  }

  QTreeView::keyPressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex FilterListView::findNextSelectableIndex()
{
  FilterListModel* model = getFilterListModel();

  QModelIndexList selectedIndexes = selectionModel()->selectedRows();
  if (selectedIndexes.size() > 1)
  {
    return QModelIndex();
  }

  if (selectedIndexes.isEmpty())
  {
    QModelIndex parent = model->index(0, FilterListModel::Column::Contents);
    return model->index(0, FilterListModel::Column::Contents, parent);
  }

  QModelIndex parent = selectedIndexes[0].parent();

  if (selectedIndexes[0].row() == (model->rowCount(parent) - 1))
  {
    QModelIndex nextParent = model->index(parent.row() + 1, FilterListModel::Column::Contents);
    if (nextParent.isValid())
    {
      return model->index(0, FilterListModel::Column::Contents, nextParent);
    }

      return selectedIndexes[0];
  }

    return model->index(selectedIndexes[0].row() + 1, FilterListModel::Column::Contents, parent);

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex FilterListView::findPreviousSelectableIndex()
{
  FilterListModel* model = getFilterListModel();

  QModelIndexList selectedIndexes = selectionModel()->selectedRows();
  if (selectedIndexes.size() > 1)
  {
    return QModelIndex();
  }

  if (selectedIndexes.isEmpty())
  {
    return QModelIndex();
  }

  QModelIndex parent = selectedIndexes[0].parent();

  if (selectedIndexes[0].row() == 0)
  {
    QModelIndex previousParent = model->index(parent.row() - 1, FilterListModel::Column::Contents);
    if (previousParent.isValid())
    {
      return model->index(model->rowCount(previousParent) - 1, FilterListModel::Column::Contents, previousParent);
    }
  }
  else
  {
    return model->index(selectedIndexes[0].row() - 1, FilterListModel::Column::Contents, parent);
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton)
  {
    m_StartPos = event->pos();
  }

  QTreeView::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::mouseMoveEvent(QMouseEvent* event)
{
  if(event->buttons() & Qt::LeftButton)
  {
    int distance = (event->pos() - m_StartPos).manhattanLength();
    if(distance >= QApplication::startDragDistance())
    {
      performDrag();
    }
  }
  QTreeView::mouseMoveEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::requestContextMenu(const QPoint& pos)
{
  QModelIndex index = indexAt(pos);

  QPoint mapped;
  if(index.isValid())
  {
    // Note: We must map the point to global from the viewport to
    // account for the header.
    mapped = viewport()->mapToGlobal(pos);
  }
  else
  {
    index = QModelIndex();
    mapped = mapToGlobal(pos);
  }

  FilterListModel* model = getFilterListModel();

  QMenu menu;

  QAction* actionLaunchHelp = new QAction("Filter Help", this);
  connect(actionLaunchHelp, &QAction::triggered, [=] {
    QString itemName = model->data(index, Qt::DisplayRole).toString();
    launchHelpForItem(itemName);
  });

  menu.addAction(actionLaunchHelp);
  menu.exec(QCursor::pos());

  menu.exec(mapped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::launchHelpForItem(const QString &humanLabel)
{
  FilterManager* fm = FilterManager::Instance();
  if(nullptr == fm)
  {
    return;
  }
  IFilterFactory::Pointer factory = fm->getFactoryFromHumanName(humanLabel);
  if(nullptr == factory.get())
  {
    return;
  }
  AbstractFilter::Pointer filter = factory->create();
  if(nullptr == filter.get())
  {
    return;
  }
  QString className = filter->getNameOfClass();

  DocRequestManager* docRequester = DocRequestManager::Instance();
  docRequester->requestFilterDocs(className);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListView::performDrag()
{  
  QModelIndex index = indexAt(m_StartPos);
  if(index.isValid())
  {
    FilterListModel* model = getFilterListModel();

    if (static_cast<FilterListItem::ItemType>(model->data(index, FilterListModel::ItemTypeRole).toInt()) == FilterListItem::ItemType::Filter)
    {
      QJsonObject obj;
      QString filterHumanLabel = model->data(index, Qt::DisplayRole).toString();
      QString filterClassName = model->data(index, FilterListModel::Roles::ClassNameRole).toString();

      obj[filterHumanLabel] = filterClassName;

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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListModel* FilterListView::getFilterListModel()
{
  return dynamic_cast<FilterListModel*>(model());
}

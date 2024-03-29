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
#include "FilterLibraryToolboxWidget.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/DocRequestManager.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"

#include "FilterListToolboxWidget.h"

#include "SVWidgetsLib/Widgets/SVStyle.h"

#define LIBRARY_NODE_TYPE 0
#define PLUGIN_NODE_TYPE 1
#define SUBGROUP_NODE_TYPE 2
#define FILTER_NODE_TYPE 3

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterLibraryToolboxWidget::FilterLibraryToolboxWidget(QWidget* parent)
: QWidget(parent)
, m_ContextMenu(new QMenu(this))
, m_Mapper(nullptr)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterLibraryToolboxWidget::~FilterLibraryToolboxWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryToolboxWidget::connectFilterList(FilterListToolboxWidget* filterListWidget)
{
  connect(this, SIGNAL(filterListGenerated(const QStringList&, bool)), filterListWidget, SLOT(updateFilterList(const QStringList&, bool)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryToolboxWidget::refreshFilterGroups()
{
  FilterManager* fm = FilterManager::Instance();
  QSet<QString> grpNames = fm->getGroupNames();

  QLIST_FROM_QSET(QList<QString>, grpNameSorted, grpNames)
  std::sort(grpNameSorted.begin(), grpNameSorted.end());

  // Clear out the default stuff
  filterLibrary->clear();

#if 1
  QTreeWidgetItem* library = new QTreeWidgetItem(filterLibrary);
  library->setText(0, SIMPL::Settings::Library);
  library->setIcon(0, QIcon(":/SIMPL/icons/images/BlankGroup_Icon.png"));
  library->setData(0, Qt::UserRole, QVariant(LIBRARY_NODE_TYPE));
#else
  QTreeWidgetItem* library = bookmarksTreeView->invisibleRootItem();
  library->setText(0, SIMPL::Settings::Library);
  library->setIcon(0, QIcon(":/SIMPL/icons/images/cubes.png"));
#endif
  for(QList<QString>::iterator iter = grpNameSorted.begin(); iter != grpNameSorted.end(); ++iter)
  {
    //   qDebug() << *iter << "\n";
    QString groupName = *iter;

#if 0
    QString iconName(":/Groups/");
    iconName.append(groupName);
    iconName.append("_Icon.png");
    // Validate the icon is in the resource system
    QFileInfo iconInfo(iconName);
    if(iconInfo.exists() == false)
    {
      iconName = ":/SIMPL/icons/images/Groups/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
    }
    QIcon icon(iconName);
#else
    QIcon icon = SVStyle::Instance()->IconForGroup(groupName);
#endif
    QTreeWidgetItem* filterGroup = new QTreeWidgetItem(library);
    filterGroup->setText(0, groupName);
    filterGroup->setIcon(0, icon);
    filterGroup->setData(0, Qt::UserRole, QVariant(PLUGIN_NODE_TYPE));
    QSet<QString> subGroupNames = fm->getSubGroupNames(groupName);
    for(QSet<QString>::iterator iter2 = subGroupNames.begin(); iter2 != subGroupNames.end(); ++iter2)
    {
      QTreeWidgetItem* filterSubGroup = new QTreeWidgetItem(filterGroup);
      QString subGroupName = *iter2;
      filterSubGroup->setText(0, subGroupName);
      filterSubGroup->setIcon(0, QIcon(":/SIMPL/icons/images/BlankGroup_Icon.png"));
      filterSubGroup->setData(0, Qt::UserRole, QVariant(SUBGROUP_NODE_TYPE));

      // Now find all the filters that are in this Group/SubGroup
      FilterManager::Collection filters = fm->getFactories(groupName, subGroupName);
      FilterManager::CollectionIterator iter(filters);
      while(iter.hasNext())
      {
        iter.next();
        IFilterFactory::Pointer factory = iter.value();
        QTreeWidgetItem* filterTreeItem = new QTreeWidgetItem(filterSubGroup);
        filterTreeItem->setText(0, factory->getFilterHumanLabel());

#if 0
        if(groupName.compare(SIMPL::FilterGroups::Unsupported) == 0)
        {
          filterTreeItem->setIcon(0, QIcon(":/SIMPL/icons/images/Groups/Unsupported_Icon.png"));
        }
        else
        {
          QString iconName(":/Groups/");
          iconName.append(filter->getGroupName());
          iconName.append("_Icon.png");

          // Validate the icon is in the resource system
          QFileInfo iconInfo(iconName);
          if(iconInfo.exists() == false)
          {
            iconName = ":/SIMPL/icons/images/Groups/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
          }

          QIcon icon(iconName);
          filterTreeItem->setIcon(0, icon);
        }
#else
        filterTreeItem->setIcon(0, icon);
#endif
        filterTreeItem->setData(0, Qt::UserRole, QVariant(FILTER_NODE_TYPE));
        filterTreeItem->setData(0, Qt::UserRole + 1, QVariant(factory->getFilterClassName()));
        filterTreeItem->setToolTip(0, factory->getFilterHtmlSummary());
      }
    }
  }
  library->setExpanded(true);
  on_filterLibrary_currentItemChanged(library, nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryToolboxWidget::setupGui()
{
  refreshFilterGroups();
  //  QString css(" QToolTip {\
//              border: 2px solid #434343;\
//              padding: 2px;\
//              border-radius: 3px;\
//              opacity: 255;\
//              background-color: #FFFFFF;\
//              color: #000000;\
//              }");
  //  bookmarksTreeView->setStyleSheet(css);

  filterLibrary->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(filterLibrary, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenuForWidget(const QPoint&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryToolboxWidget::showContextMenuForWidget(const QPoint& pos)
{
  QTreeWidgetItem* item = filterLibrary->itemAt(pos);

  if(nullptr != item && item->childCount() == 0)
  {
    // Clear the existing context menu
    m_ContextMenu->clear();

    QString itemName = item->text(0);

    m_Mapper = new QSignalMapper(this);

    QAction* actionLaunchHelp = new QAction(m_ContextMenu);
    actionLaunchHelp->setObjectName(QString::fromUtf8("actionLaunchHelp"));
    actionLaunchHelp->setText(QApplication::translate("SIMPLView_UI", "Filter Help", nullptr));
    connect(actionLaunchHelp, SIGNAL(triggered()), m_Mapper, SLOT(map()));
    m_Mapper->setMapping(actionLaunchHelp, itemName);
    connect(m_Mapper, SIGNAL(mapped(QString)), this, SLOT(launchHelpForItem(QString)));

    m_ContextMenu->addAction(actionLaunchHelp);
    m_ContextMenu->exec(QCursor::pos());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryToolboxWidget::launchHelpForItem(QString humanLabel)
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
void FilterLibraryToolboxWidget::on_filterLibrary_currentItemChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous)
{
  if(nullptr == item)
  {
    return;
  }

  // Get the SVPipelineFilterWidget Manager Instance
  FilterManager* fm = FilterManager::Instance();
  FilterManager::Collection factories;

  // If the user clicks on Detail::Library, display all
  if(item->text(0).compare(SIMPL::Settings::Library) == 0)
  {
    factories = fm->getFactories();
    updateFilterGroupList(factories);
  }
  else if(nullptr != item->parent() && item->parent()->text(0).compare(SIMPL::Settings::Library) == 0)
  {
    factories = fm->getFactories(item->text(0));
    updateFilterGroupList(factories);
  }
  else if(nullptr != item->parent() && nullptr != item->parent()->parent() && item->parent()->parent()->text(0).compare(SIMPL::Settings::Library) == 0)
  {
    factories = fm->getFactories(item->parent()->text(0), item->text(0));
    updateFilterGroupList(factories);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryToolboxWidget::on_filterLibrary_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
  Q_UNUSED(column)

  QVariant nodeType = item->data(0, Qt::UserRole);
  if(nodeType.toInt() == FILTER_NODE_TYPE)
  {
    QString filterClassName = item->data(0, Qt::UserRole + 1).toString();
    Q_EMIT filterItemDoubleClicked(filterClassName);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryToolboxWidget::updateFilterGroupList(FilterManager::Collection& factories)
{
  QStringList filterNames;
  for(FilterManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    filterNames << factory.key();
  }
  Q_EMIT filterListGenerated(filterNames, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryToolboxWidget::writeSettings(QtSSettings* prefs)
{
  prefs->beginGroup("DockWidgetSettings");
  prefs->beginGroup("Filter Library Dock Widget");

  prefs->setValue(objectName(), isHidden());

  prefs->endGroup();
  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryToolboxWidget::readSettings(QtSSettings* prefs)
{
  prefs->beginGroup("DockWidgetSettings");
  prefs->beginGroup("Filter Library Dock Widget");

  bool b = prefs->value(objectName(), QVariant(false)).toBool();
  setHidden(b);

  prefs->endGroup();
  prefs->endGroup();
}

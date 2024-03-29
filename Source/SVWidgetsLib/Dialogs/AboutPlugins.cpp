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
#include "AboutPlugins.h"

#include <QtCore/QPluginLoader>
#include <QtCore/QTextStream>

#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <QtCore/QDebug>

#include "SIMPLib/Filtering/FilterManager.h"

#include "SVWidgetsLib/Core/FilterWidgetManager.h"

#include "SVWidgetsLib/QtSupport/QtSSettings.h"

#include "PluginDetails.h"

enum ColumnIndex
{
  NAME_INDEX = 0,
  CHECKBOX_INDEX = 1,
  STATUS_INDEX = 2,
  VERSION_INDEX = 3,
};

enum SettingIndex
{
  PATH_INDEX = 0,
  ENABLED_INDEX = 1
};

static const QString NOT_FOUND_STRING = "Not Found";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AboutPlugins::AboutPlugins(QWidget* parent)
: SVDialog(parent)
, m_loadPreferencesDidChange(false)
{
  setupUi(this);

  setupGui();

  // Hide the add and remove plugin buttons until we are ready to use them
  addPluginBtn->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AboutPlugins::~AboutPlugins()
{
#if defined(Q_OS_MAC)
  delete m_CloseAction;
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::setupGui()
{
  // Resize the column widths so that all text is showing
  pluginsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  pluginsTable->horizontalHeader()->setSectionResizeMode(NAME_INDEX, QHeaderView::ResizeToContents);
  pluginsTable->horizontalHeader()->setSectionResizeMode(CHECKBOX_INDEX, QHeaderView::ResizeToContents);
  pluginsTable->horizontalHeader()->setSectionResizeMode(STATUS_INDEX, QHeaderView::ResizeToContents);

#if SIMPLView_PLUGINDETAILS_PLUGIN_CONFIG
#else
  pluginsTable->horizontalHeader()->hideSection(CHECKBOX_INDEX);
  pluginsTable->horizontalHeader()->hideSection(STATUS_INDEX);
#endif

  // Set selection behavior so that only full rows can be selected
  pluginsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  QList<PluginProxy::Pointer> currentCache = readPluginCache();
  loadPlugins(currentCache);

  // Set default cell to the first item in the list
  pluginsTable->setCurrentCell(0, 0);
  pluginsTable->setCurrentIndex(pluginsTable->model()->index(0, 0));
  QTableWidgetItem* statusItem = pluginsTable->item(pluginsTable->currentRow(), STATUS_INDEX);

  if((statusItem != nullptr) && statusItem->text() == NOT_FOUND_STRING)
  {
    removePluginBtn->setVisible(true);
  }
  else
  {
    removePluginBtn->setVisible(false);
  }

  setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

#if defined(Q_OS_MAC)
  m_CloseAction = new QAction(this);
  m_CloseAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
  connect(m_CloseAction, SIGNAL(triggered()), this, SLOT(close()));
  addAction(m_CloseAction);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::loadPlugins(QList<PluginProxy::Pointer> proxies)
{
  PluginManager* manager = PluginManager::Instance();
  QVector<ISIMPLibPlugin*> vector = manager->getPluginsVector();
  pluginsTable->setRowCount(vector.size());

  // Iterate over PluginManager and add each entry to the plugin table
  for(int row = 0; row < vector.size(); row++)
  {
    ISIMPLibPlugin* plugin = vector.at(row);
    addPluginToTable(plugin, row);
  }

  // Iterate over proxy and find all entries that are not in PluginManager
  QList<QString> managerNames = manager->getPluginNames();
  QList<QString> pluginNames;

  for(QList<PluginProxy::Pointer>::iterator proxyIter = proxies.begin(); proxyIter != proxies.end(); proxyIter++)
  {
    pluginNames.push_back((*proxyIter)->getPluginName());
  }

  for(QList<QString>::iterator nameIter = pluginNames.begin(); nameIter != pluginNames.end(); nameIter++)
  {
    QString proxyName = *nameIter;
    if(!managerNames.contains(proxyName))
    {
      qDebug() << "The plugin " << proxyName << " was not found in the PluginManager.";
      SIMPLibPlugin* plugin = new SIMPLibPlugin();
      plugin->setPluginName(proxyName);
      plugin->setStatus(NOT_FOUND_STRING);
      addPlaceHolderToTable(plugin, 0);
      delete plugin;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::addPluginToTable(ISIMPLibPlugin* plugin, int row)
{
  QColor defaultColor(Qt::white);

  // Add name of plugin to the row
  QTableWidgetItem* nameItem = new QTableWidgetItem(plugin->getPluginBaseName());
  nameItem->setBackground(defaultColor);
  pluginsTable->setItem(row, NAME_INDEX, nameItem);

  // Set checkbox cell to default settings
  QTableWidgetItem* enabledItem = new QTableWidgetItem();
  enabledItem->setBackground(defaultColor);
  pluginsTable->setItem(row, ENABLED_INDEX, enabledItem);

  // Add check box that is centered in the cell
  QCheckBox* checkBox = new QCheckBox(nullptr);
  readCheckState(checkBox, plugin->getPluginBaseName());

  connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(togglePluginState(int)));
  connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(setLoadPreferencesFlag(int)));

  QHBoxLayout* layout = new QHBoxLayout(nullptr);
  layout->addWidget(checkBox);
  layout->setAlignment(Qt::AlignCenter);
  layout->setContentsMargins(0, 0, 0, 0);
  QWidget* widget = new QWidget(nullptr);
  widget->setLayout(layout);
  pluginsTable->setCellWidget(row, CHECKBOX_INDEX, widget);

  QTableWidgetItem* statusItem;
  // Add load status information
  if(plugin->getDidLoad())
  {
    statusItem = new QTableWidgetItem("Enabled");
  }
  else
  {
    statusItem = new QTableWidgetItem("Disabled");
  }
  statusItem->setBackground(Qt::white);
  pluginsTable->setItem(row, STATUS_INDEX, statusItem);

  // Add version information
  QTableWidgetItem* versionItem = new QTableWidgetItem(plugin->getVersion());
  versionItem->setBackground(Qt::white);
  pluginsTable->setItem(row, VERSION_INDEX, versionItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::addPlaceHolderToTable(SIMPLibPlugin* plugin, int row)
{
  // Insert the empty row
  pluginsTable->insertRow(row);

  // Create bold font
  QFont errorFont;
  errorFont.setBold(true);
  QColor errorColor(240, 165, 179);

  // Add name of plugin to the row
  QTableWidgetItem* nameItem = new QTableWidgetItem(plugin->getPluginName());
  nameItem->setFont(errorFont);
  nameItem->setBackground(errorColor);
  pluginsTable->setItem(row, NAME_INDEX, nameItem);

  // Add empty checkbox item to the row
  QTableWidgetItem* enabledItem = new QTableWidgetItem();
  enabledItem->setBackground(errorColor);
  pluginsTable->setItem(row, ENABLED_INDEX, enabledItem);

  // Add status to the row
  QTableWidgetItem* statusItem = new QTableWidgetItem(NOT_FOUND_STRING);
  statusItem->setBackground(errorColor);
  statusItem->setFont(errorFont);
  pluginsTable->setItem(row, STATUS_INDEX, statusItem);

  // Add version information
  QTableWidgetItem* versionItem = new QTableWidgetItem(plugin->getVersion());
  versionItem->setBackground(errorColor);
  pluginsTable->setItem(row, VERSION_INDEX, versionItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::on_closeBtn_clicked()
{
  // Close the dialog box
  close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::on_pluginsTable_itemSelectionChanged()
{
  QTableWidgetItem* item = pluginsTable->item(pluginsTable->currentRow(), NAME_INDEX);
  if(item != nullptr)
  {
    m_PluginDetails->setPluginName(item->text());
    m_PluginDetails->loadPluginDetails();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::on_pluginsTable_currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
  if(nullptr != current && current->text() == NOT_FOUND_STRING)
  {
    removePluginBtn->setVisible(true);
  }
  else
  {
    removePluginBtn->setVisible(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::setLoadPreferencesFlag()
{
  setLoadPreferencesFlag(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::setLoadPreferencesFlag(int state)
{
  m_loadPreferencesDidChange = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::on_addPluginBtn_clicked()
{
  QString pluginPath = QFileDialog::getOpenFileName(this, tr("Open Plugin File Path"), "", tr("Plugin Files (*.plugin)"));
  QFileInfo fileInfo(pluginPath);
  if(fileInfo.suffix() == "plugin")
  {
    addPlugin(pluginPath);
  }
  else
  {
    qDebug() << "Could not add plugin because " << pluginPath << " does not have the 'plugin' file extension!";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::addPlugin(QString pluginPath)
{
  PluginManager* pluginManager = PluginManager::Instance();
  FilterManager* fm = FilterManager::Instance();
  FilterWidgetManager* fwm = FilterWidgetManager::Instance();

  QApplication::instance()->processEvents();
  QPluginLoader* loader = new QPluginLoader(pluginPath);
  QFileInfo fi(pluginPath);
  QString fileName = fi.fileName();
  QObject* plugin = loader->instance();
  if(plugin != nullptr)
  {
    ISIMPLibPlugin* ipPlugin = qobject_cast<ISIMPLibPlugin*>(plugin);
    if(ipPlugin != nullptr)
    {
      QString pluginName = ipPlugin->getPluginBaseName();

      ipPlugin->registerFilterWidgets(fwm);
      ipPlugin->registerFilters(fm);
      ipPlugin->setDidLoad(true);

      ipPlugin->setLocation(pluginPath);
      pluginManager->addPlugin(ipPlugin);

      addPluginToTable(ipPlugin, 0);
      setLoadPreferencesFlag();
    }
  }
  else
  {
    QString message("The plugin did not load with the following error\n");
    message.append(loader->errorString());
    QMessageBox::critical(this, "SIMPLView Plugin Load Error", message, QMessageBox::Ok | QMessageBox::Default);
    delete loader;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::on_removePluginBtn_clicked()
{
  int currentRow = pluginsTable->currentRow();
  QTableWidgetItem* nameItem = pluginsTable->item(currentRow, NAME_INDEX);

  if(nullptr != nameItem)
  {
    QString pluginName = nameItem->text();

    QMessageBox confirm;
    confirm.setText("Are you sure that you want to remove '" + pluginName + "'?");
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::Yes);
    confirm.setWindowTitle("Remove Plugin");
    int answer = confirm.exec();

    if(answer == QMessageBox::Yes)
    {
      deletePlugin(nameItem);
      QTableWidgetItem* newSelection = pluginsTable->item(currentRow, NAME_INDEX);
      if(nullptr != newSelection)
      {
        // Select the row that is next after the one that was just deleted
        pluginsTable->setCurrentItem(newSelection);
        pluginsTable->setFocus();
      }
      else
      {
        // We are at the end, so select the previous row
        currentRow--;
        newSelection = pluginsTable->item(currentRow, NAME_INDEX);
        pluginsTable->setCurrentItem(newSelection);
        pluginsTable->setFocus();
      }
      m_loadPreferencesDidChange = true;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::deletePlugin(QTableWidgetItem* nameItem)
{
  QString pluginName = nameItem->text();
  QtSSettings prefs;

  // Remove plugin from cache
  prefs.beginGroup("Plugin Preferences");
  prefs.remove(pluginName);
  prefs.endGroup();

  // Remove entry from plugin table
  pluginsTable->removeRow(nameItem->row());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::writePluginCache()
{
  QtSSettings prefs;

  prefs.beginGroup("Plugin Preferences");

  QList<PluginProxy::Pointer> proxies = getPluginCheckBoxSettingsFromGUI();

  for(QList<PluginProxy::Pointer>::iterator proxyIter = proxies.begin(); proxyIter != proxies.end(); proxyIter++)
  {
    QString pluginName = (*proxyIter)->getPluginName();
    QString filePath = (*proxyIter)->getFilePath();
    bool enabled = (*proxyIter)->getEnabled();

    prefs.beginGroup(pluginName);
    prefs.setValue("Plugin Path", filePath);
    prefs.setValue("Enabled", enabled);
    prefs.endGroup();
  }

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::readCheckState(QCheckBox* checkBox, QString pluginName)
{
  QtSSettings prefs;

  prefs.beginGroup("Plugin Preferences");
  prefs.beginGroup(pluginName);
  checkBox->setChecked(prefs.value("Enabled", true).toBool());
  prefs.endGroup();
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<PluginProxy::Pointer> AboutPlugins::getPluginCheckBoxSettingsFromGUI()
{
  PluginManager* manager = PluginManager::Instance();
  QList<PluginProxy::Pointer> list;

  for(int row = 0; row < pluginsTable->rowCount(); row++)
  {
    QString pluginName = pluginsTable->item(row, NAME_INDEX)->text();
    ISIMPLibPlugin* plugin = manager->findPlugin(pluginName);

    PluginProxy::Pointer proxy = PluginProxy::New();
    proxy->setPluginName(pluginName);

    if(pluginsTable->cellWidget(row, CHECKBOX_INDEX) != nullptr)
    {
      QWidget* widget = pluginsTable->cellWidget(row, CHECKBOX_INDEX);

      if(nullptr == widget)
      {
        return QList<PluginProxy::Pointer>();
      }
      QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(widget->layout());

      if(nullptr == layout)
      {
        return QList<PluginProxy::Pointer>();
      }
      QCheckBox* checkBox = qobject_cast<QCheckBox*>(layout->itemAt(0)->widget());

      if(nullptr == checkBox)
      {
        return QList<PluginProxy::Pointer>();
      }

      if(checkBox->checkState() == Qt::Checked)
      {
        proxy->setEnabled(true);
      }
      else
      {
        proxy->setEnabled(false);
      }
    }

    if(plugin != nullptr)
    {
      QString filePath = plugin->getLocation();
      proxy->setFilePath(filePath);
    }

    list.push_back(proxy);
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<PluginProxy::Pointer> AboutPlugins::readPluginCache()
{
  QList<PluginProxy::Pointer> proxyList;

  QtSSettings prefs;

  prefs.beginGroup("Plugin Preferences");
  QStringList pluginNameList = prefs.childGroups();

  for(QStringList::iterator iter = pluginNameList.begin(); iter != pluginNameList.end(); iter++)
  {
    PluginProxy::Pointer proxy = PluginProxy::New();
    QString pluginName = *iter;

    prefs.beginGroup(pluginName);
    proxy->setPluginName(pluginName);
    proxy->setFilePath(prefs.value("Plugin Path", QString("")).toString());
    proxy->setEnabled(prefs.value("Enabled", QVariant(true)).toBool());
    prefs.endGroup();

    proxyList.push_back(proxy);
  }
  prefs.endGroup();

  return proxyList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AboutPlugins::getLoadPreferencesDidChange()
{
  return m_loadPreferencesDidChange;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::togglePluginState(int state)
{
}

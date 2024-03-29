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

#pragma once

#include <QtWidgets/QCheckBox>

#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/PluginProxy.h"
#include "SIMPLib/Plugin/SIMPLibPlugin.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/SVControlWidgets.h"

//-- UIC generated Header
#include "ui_AboutPlugins.h"

class SVWidgetsLib_EXPORT AboutPlugins : public SVDialog, private Ui::AboutPlugins
{
  Q_OBJECT

public:
  AboutPlugins(QWidget* parent = nullptr);
  ~AboutPlugins() override;

  void setupGui();

  void addPluginToTable(ISIMPLibPlugin* plugin, int row);
  void addPlaceHolderToTable(SIMPLibPlugin* plugin, int row);

  void loadPlugins(QList<PluginProxy::Pointer> proxies);

  void readCheckState(QCheckBox* checkBox, QString pluginName);

  QList<PluginProxy::Pointer> getPluginCheckBoxSettingsFromGUI();
  static QList<PluginProxy::Pointer> readPluginCache();

  void writePluginCache();

  bool getLoadPreferencesDidChange();

  void deletePlugin(QTableWidgetItem* nameItem);

  void addPlugin(QString pluginPath);

  void setLoadPreferencesFlag();

public Q_SLOTS:
  void on_closeBtn_clicked();
  void on_addPluginBtn_clicked();
  void on_removePluginBtn_clicked();
  void on_pluginsTable_currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);
  void setLoadPreferencesFlag(int state);
  void togglePluginState(int state);

  void on_pluginsTable_itemSelectionChanged();

private:
  bool m_loadPreferencesDidChange;
  QAction* m_CloseAction = nullptr;

public:
  AboutPlugins(const AboutPlugins&) = delete;            // Copy Constructor Not Implemented
  AboutPlugins(AboutPlugins&&) = delete;                 // Move Constructor Not Implemented
  AboutPlugins& operator=(const AboutPlugins&) = delete; // Copy Assignment Not Implemented
  AboutPlugins& operator=(AboutPlugins&&) = delete;      // Move Assignment Not Implemented
};

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

#include "PluginManager.h"

#include <QtCore/QJsonArray>

#include "SIMPLib/Filtering/CorePlugin.h"
#include "SIMPLib/Plugin/SIMPLPluginConstants.h"



PluginManager* PluginManager::m_Self = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginManager::PluginManager()
{
  //  qDebug() << "PluginManager()" << this;
  Q_ASSERT_X(!m_Self, "PluginManager", "There should be only one PluginManager object");
  PluginManager::m_Self = this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginManager::~PluginManager()
{
  //  qDebug() << "~PluginManager()" << this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginManager* PluginManager::Instance()
{

  if(m_Self == nullptr)
  {
    //  qDebug() << "PluginManager::Instance self was nullptr" << "\n";
    m_Self = new PluginManager();
    CorePlugin* corePlug = new CorePlugin;
    m_Self->addPlugin(corePlug);
  }
  return m_Self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginManager::printPluginNames() const
{
  for(auto const plugin : m_Plugins )
  {
    if(nullptr != plugin)
    {
      qDebug() << "Name: " << plugin->getPluginBaseName() << "\n";
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginManager::addPlugin(ISIMPLibPlugin* plugin)
{
  m_Plugins.push_back(plugin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> PluginManager::getPluginNames() const
{
  QList<QString> pluginNames;
  for(auto const plugin : m_Plugins )
  {
    if(nullptr != plugin)
    {
      pluginNames.push_back(plugin->getPluginBaseName());
    }
  }
  return pluginNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginManager::getPluginName(QString filtName) const
{
  for(auto const plugin : m_Plugins )
  {
    if(nullptr != plugin)
    {
      if(plugin->getFilters().contains(filtName))
      {
        return plugin->getPluginBaseName();
      }
    }
  }
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<ISIMPLibPlugin*> PluginManager::getPluginsVector() const
{
  return m_Plugins;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ISIMPLibPlugin* PluginManager::findPlugin(QString pluginName)
{
  for(auto const plugin : m_Plugins )
  {
    if(nullptr != plugin)
    {
      if(plugin->getPluginBaseName() == pluginName)
      {
        return plugin;
      }
    }
  }
  // If the plugin isn't found in the vector...
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonArray PluginManager::toJsonArray() const
{
  QVector<ISIMPLibPlugin*> plugins = getPluginsVector();
  QJsonArray plugArray;
  
  for(auto const plugin : m_Plugins )
  {
    if(nullptr != plugin)
    {
      QJsonObject jobj;
      jobj[SIMPL::JSON::BaseName] = plugin->getPluginBaseName();
      jobj[SIMPL::JSON::Version] = plugin->getVersion();
      jobj[SIMPL::JSON::Vendor] = plugin->getVendor();
      jobj[SIMPL::JSON::CompatibilityVersion] = plugin->getCompatibilityVersion();
      jobj[SIMPL::JSON::DisplayName] = plugin->getPluginDisplayName();
      jobj[SIMPL::JSON::URL] = plugin->getURL();
      plugArray.append(jobj);
    }
  }
  return plugArray;
}

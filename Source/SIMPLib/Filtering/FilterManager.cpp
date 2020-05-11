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

#include "FilterManager.h"

#include <QtCore/QJsonObject>
#include <QtCore/QDebug>
#include <QtCore/QSet>

#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/CorePlugin.h"
#include "SIMPLib/Plugin/SIMPLPluginConstants.h"

FilterManager* FilterManager::s_Self = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterManager::FilterManager()
{
  Q_ASSERT_X(!s_Self, "FilterManager", "there should be only one FilterManager object");
  FilterManager::s_Self = this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterManager::~FilterManager() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterManager* FilterManager::Instance()
{
  if(s_Self == nullptr)
  {
    s_Self = new FilterManager();
    // Always register the Core Filters
    CorePlugin cp;
    cp.registerFilters(s_Self);
  }
  return s_Self;
}

// -----------------------------------------------------------------------------
//  Static Method
// -----------------------------------------------------------------------------
void FilterManager::RegisterFilterFactory(const QString& name, IFilterFactory::Pointer factory)
{
  if(nullptr != factory.get())
  {
    // Instantiate the Instance Manager for IFilterFactory
    FilterManager* idManager = FilterManager::Instance();
    idManager->addFilterFactory(name, factory);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterManager::RegisterKnownFilters(FilterManager* /* fm */)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterManager::Collection FilterManager::getFactories() const
{
  return m_Factories;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterManager::printFactoryNames() const
{
  QList<QString> keys = m_Factories.keys();
  for(auto const key : keys)
  {
    qDebug() << "Name: " << key << "\n";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterManager::Collection FilterManager::getFactories(const QString& groupName)
{
  FilterManager::Collection groupFactories;

  for(FilterManager::Collection::iterator factory = m_Factories.begin(); factory != m_Factories.end(); ++factory)
  {
    IFilterFactory::Pointer filterFactory = factory.value();
    if(nullptr != filterFactory.get() && factory.value()->getFilterGroup().compare(groupName) == 0)
    {
      groupFactories[factory.key()] = factory.value();
    }
  }
  return groupFactories;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterManager::Collection FilterManager::getFactories(const QString& groupName, const QString& subGroupName)
{
  FilterManager::Collection groupFactories;
  for(FilterManager::Collection::iterator factoryIter = m_Factories.begin(); factoryIter != m_Factories.end(); ++factoryIter)
  {
    IFilterFactory::Pointer filterFactory = factoryIter.value();
    if(nullptr != filterFactory.get() && factoryIter.value()->getFilterGroup().compare(groupName) == 0 && factoryIter.value()->getFilterSubGroup().compare(subGroupName) == 0)
    {
      groupFactories[factoryIter.key()] = factoryIter.value();
    }
  }
  return groupFactories;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterManager::addFilterFactory(const QString& name, IFilterFactory::Pointer factory)
{
  // std::cout << this << " - Registering Filter: " << name.toStdString() << std::endl;
  m_Factories[name] = factory;
  m_UuidFactories[factory->getUuid()] = factory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> FilterManager::getGroupNames()
{
  // qDebug() << "FilterManager::getGroupNames" << "\n";
  // Get all the  Factories and loop over each one we know about and instantiate a new one
  FilterManager* fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();
  QSet<QString> groupNames;
  for(FilterManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    IFilterFactory::Pointer filterFactory = factory.value();
    if(nullptr != filterFactory)
    {
      groupNames.insert(filterFactory->getFilterGroup());
    }
    //  qDebug() << factory.value()->getFilterGroup() << "\n";
  }
  return groupNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> FilterManager::getSubGroupNames(const QString& groupName)
{
  // qDebug() << "FilterManager::getGroupNames" << "\n";
  // Get all the  Factories and loop over each one we know about and instantiate a new one
  FilterManager* fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();
  QSet<QString> subGroupNames;
  for(FilterManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    IFilterFactory::Pointer filterFactory = factory.value();
    if(nullptr != filterFactory.get() && factory.value()->getFilterGroup().compare(groupName) == 0)
    {
      subGroupNames.insert(factory.value()->getFilterSubGroup());
    }
  }
  return subGroupNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IFilterFactory::Pointer FilterManager::getFactoryFromClassName(const QString& filterName) const
{
  if(m_Factories.contains(filterName))
  {
    return m_Factories[filterName];
  }
  return IFilterFactory::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IFilterFactory::Pointer FilterManager::getFactoryFromUuid(const QUuid& uuid) const
{
  if(m_UuidFactories.contains(uuid))
  {
    return m_UuidFactories[uuid];
  }
  return IFilterFactory::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IFilterFactory::Pointer FilterManager::getFactoryFromHumanName(const QString& humanName)
{
  IFilterFactory::Pointer Factory;

  for(FilterManager::Collection::iterator factory = m_Factories.begin(); factory != m_Factories.end(); ++factory)
  {
    IFilterFactory::Pointer filterFactory = factory.value();
    if(nullptr != filterFactory.get() && filterFactory->getFilterHumanLabel().compare(humanName) == 0)
    {
      Factory = filterFactory;
      break;
    }
  }
  return Factory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonArray FilterManager::toJsonArray() const
{
  QJsonArray filterArray;

  FilterManager::Collection factories = getFactories();
  for(auto factory : factories)
  {
    AbstractFilter::Pointer filter = factory->create();
    QJsonObject filtJson;

    filtJson[SIMPL::JSON::Name] = filter->getHumanLabel();
    filtJson[SIMPL::JSON::ClassName] = filter->getNameOfClass();
    filtJson[SIMPL::JSON::Uuid] = filter->getUuid().toString();
    filtJson[SIMPL::JSON::PluginName] = filter->getCompiledLibraryName();
    filtJson[SIMPL::JSON::Version] = filter->getFilterVersion();
    filtJson[SIMPL::JSON::GroupName] = filter->getGroupName();
    filtJson[SIMPL::JSON::SubGroupName] = filter->getSubGroupName();

    filterArray.append(filtJson);
  }
  return filterArray;
}

// -----------------------------------------------------------------------------
QString FilterManager::getNameOfClass() const
{
  return QString("FilterManager");
}

// -----------------------------------------------------------------------------
QString FilterManager::ClassName()
{
  return QString("FilterManager");
}

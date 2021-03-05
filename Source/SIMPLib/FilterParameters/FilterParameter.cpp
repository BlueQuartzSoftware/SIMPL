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

#include "FilterParameter.h"

#include <QtCore/QJsonObject>

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::FilterParameter()
: m_HumanLabel("")
, m_PropertyName("")
, m_Category(FilterParameter::Category::Uncategorized)
, m_ReadOnly(false)
, m_GroupIndex(-1)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::~FilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameter::readJson(const QJsonObject& json)
{
  Q_UNUSED(json)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameter::writeJson(QJsonObject& json)
{
  Q_UNUSED(json)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameter::dataArrayPathRenamed(AbstractFilter* filter, const DataArrayPath::RenameType& renamePath)
{
  QVariant var = filter->property(qPrintable(getPropertyName()));
  if(!var.isValid())
  {
    return;
  }

  if(var.canConvert<DataArrayPath>())
  {
    DataArrayPath path = var.value<DataArrayPath>();
    if(path == renamePath.first)
    {
      return;
    }
    if(path.updatePath(renamePath))
    {
      var.setValue(path);
      filter->setProperty(qPrintable(getPropertyName()), var);
      Q_EMIT filter->dataArrayPathUpdated(getPropertyName(), renamePath);
    }
  }
  else if(var.canConvert<DataContainerArrayProxy>())
  {
    DataContainerArrayProxy proxy = var.value<DataContainerArrayProxy>();
    proxy.updatePath(renamePath);
    var.setValue(proxy);
    filter->setProperty(qPrintable(getPropertyName()), var);
    Q_EMIT filter->dataArrayPathUpdated(getPropertyName(), renamePath);
  }
  else if(var.canConvert<DataContainerProxy>())
  {
    DataContainerProxy proxy = var.value<DataContainerProxy>();
    proxy.updatePath(renamePath);
    var.setValue(proxy);
    filter->setProperty(qPrintable(getPropertyName()), var);
    Q_EMIT filter->dataArrayPathUpdated(getPropertyName(), renamePath);
  }
  else if(var.canConvert<AttributeMatrixProxy>())
  {
    AttributeMatrixProxy proxy = var.value<AttributeMatrixProxy>();
    proxy.updatePath(renamePath);
    var.setValue(proxy);
    filter->setProperty(qPrintable(getPropertyName()), var);
    Q_EMIT filter->dataArrayPathUpdated(getPropertyName(), renamePath);
  }
  else if(var.canConvert<DataArrayProxy>())
  {
    DataArrayProxy proxy = var.value<DataArrayProxy>();
    proxy.updatePath(renamePath);
    var.setValue(proxy);
    filter->setProperty(qPrintable(getPropertyName()), var);
    Q_EMIT filter->dataArrayPathUpdated(getPropertyName(), renamePath);
  }
}

// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
QString FilterParameter::getNameOfClass() const
{
  return QString("FilterParameter");
}

// -----------------------------------------------------------------------------
QString FilterParameter::ClassName()
{
  return QString("FilterParameter");
}

// -----------------------------------------------------------------------------
void FilterParameter::setHumanLabel(const QString& value)
{
  m_HumanLabel = value;
}

// -----------------------------------------------------------------------------
QString FilterParameter::getHumanLabel() const
{
  return m_HumanLabel;
}

// -----------------------------------------------------------------------------
void FilterParameter::setPropertyName(const QString& value)
{
  m_PropertyName = value;
}

// -----------------------------------------------------------------------------
QString FilterParameter::getPropertyName() const
{
  return m_PropertyName;
}

// -----------------------------------------------------------------------------
void FilterParameter::setLegacyPropertyName(const QString& value)
{
  m_LegacyPropertyName = value;
}

// -----------------------------------------------------------------------------
QString FilterParameter::getLegacyPropertyName() const
{
  return m_LegacyPropertyName;
}

// -----------------------------------------------------------------------------
void FilterParameter::setCategory(const FilterParameter::Category& value)
{
  m_Category = value;
}

// -----------------------------------------------------------------------------
FilterParameter::Category FilterParameter::getCategory() const
{
  return m_Category;
}

// -----------------------------------------------------------------------------
void FilterParameter::setReadOnly(bool value)
{
  m_ReadOnly = value;
}

// -----------------------------------------------------------------------------
bool FilterParameter::getReadOnly() const
{
  return m_ReadOnly;
}

// -----------------------------------------------------------------------------
void FilterParameter::setGroupIndex(int value)
{
  m_GroupIndex = value;
}

// -----------------------------------------------------------------------------
int FilterParameter::getGroupIndex() const
{
  return m_GroupIndex;
}

// -----------------------------------------------------------------------------
void FilterParameter::setDefaultValue(const QVariant& value)
{
  m_DefaultValue = value;
}

// -----------------------------------------------------------------------------
QVariant FilterParameter::getDefaultValue() const
{
  return m_DefaultValue;
}

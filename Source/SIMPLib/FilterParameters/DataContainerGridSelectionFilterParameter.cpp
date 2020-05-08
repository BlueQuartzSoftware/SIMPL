/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DataContainerGridSelectionFilterParameter.h"

#include "SIMPLib/Filtering/AbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridSelectionFilterParameter::DataContainerGridSelectionFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridSelectionFilterParameter::~DataContainerGridSelectionFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridSelectionFilterParameter::Pointer DataContainerGridSelectionFilterParameter::New(const QString& humanLabel, const QString& propertyName, const DataContainerGrid& defaultValue,
                                                                                                  Category category, SetterCallbackType setterCallback, GetterCallbackType getterCallback,
                                                                                                  int groupIndex)
{
  DataContainerGridSelectionFilterParameter::Pointer ptr = DataContainerGridSelectionFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerGridSelectionFilterParameter::getWidgetType() const
{
  return QString("DataContainerGridSelectionWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonObject obj = jsonValue.toObject();
    DataArrayPath dap;
    if(dap.readJson(obj))
    {
      // m_SetterCallback(dap);
    }
    else // this is in here for historical where we used to save the value as a string
    {
      QString dcName = jsonValue.toString("");
      DataArrayPath dap(dcName, "", "");
      // m_SetterCallback(dap);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    /*
    DataArrayPath dap = m_GetterCallback();
    QJsonObject obj;
    dap.writeJson(obj);
    json[getPropertyName()] = obj;
    */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionFilterParameter::dataArrayPathRenamed(AbstractFilter* filter, const DataArrayPath::RenameType& renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  /*
  if(oldPath == m_GetterCallback() && oldPath.getDataContainerName() != newPath.getDataContainerName())
  {
    m_SetterCallback(newPath);
    emit filter->dataArrayPathUpdated(getPropertyName(), renamePath);
  }
  */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridSelectionFilterParameter::Pointer DataContainerGridSelectionFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridSelectionFilterParameter::Pointer DataContainerGridSelectionFilterParameter::New()
{
  return Pointer(new DataContainerGridSelectionFilterParameter());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerGridSelectionFilterParameter::getNameOfClass() const
{
  return "DataContainerGridSelectionFilterParameter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerGridSelectionFilterParameter::ClassName()
{
  return "DataContainerGridSelectionFilterParameter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Types DataContainerGridSelectionFilterParameter::getDefaultGeometryTypes() const
{
  return m_DefaultGeometryTypes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionFilterParameter::setDefaultGeometryTypes(IGeometry::Types value)
{
  m_DefaultGeometryTypes = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridSelectionFilterParameter::SetterCallbackType DataContainerGridSelectionFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionFilterParameter::setSetterCallback(const SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGridSelectionFilterParameter::GetterCallbackType DataContainerGridSelectionFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerGridSelectionFilterParameter::setGetterCallback(const GetterCallbackType& value)
{
  m_GetterCallback = value;
}

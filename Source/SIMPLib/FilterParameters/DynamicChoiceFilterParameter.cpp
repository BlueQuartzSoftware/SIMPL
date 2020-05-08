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

#include "DynamicChoiceFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicChoiceFilterParameter::DynamicChoiceFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicChoiceFilterParameter::~DynamicChoiceFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicChoiceFilterParameter::Pointer DynamicChoiceFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QString& defaultValue, Category category,
                                                                        const SetterCallbackType& setterCallback, const GetterCallbackType& getterCallback, const QString& listProperty, int groupIndex)
{
  DynamicChoiceFilterParameter::Pointer ptr = DynamicChoiceFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setDefaultValue(defaultValue);
  ptr->setCategory(category);
  ptr->setListProperty(listProperty);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicChoiceFilterParameter::getWidgetType() const
{
  return QString("DynamicChoiceWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicChoiceFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    m_SetterCallback(jsonValue.toString(""));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicChoiceFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    json[getPropertyName()] = m_GetterCallback();
  }
}

// -----------------------------------------------------------------------------
DynamicChoiceFilterParameter::Pointer DynamicChoiceFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
DynamicChoiceFilterParameter::Pointer DynamicChoiceFilterParameter::New()
{
  Pointer sharedPtr(new(DynamicChoiceFilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString DynamicChoiceFilterParameter::getNameOfClass() const
{
  return QString("DynamicChoiceFilterParameter");
}

// -----------------------------------------------------------------------------
QString DynamicChoiceFilterParameter::ClassName()
{
  return QString("DynamicChoiceFilterParameter");
}

// -----------------------------------------------------------------------------
void DynamicChoiceFilterParameter::setListProperty(const QString& value)
{
  m_ListProperty = value;
}

// -----------------------------------------------------------------------------
QString DynamicChoiceFilterParameter::getListProperty() const
{
  return m_ListProperty;
}

// -----------------------------------------------------------------------------
void DynamicChoiceFilterParameter::setUnits(const QString& value)
{
  m_Units = value;
}

// -----------------------------------------------------------------------------
QString DynamicChoiceFilterParameter::getUnits() const
{
  return m_Units;
}

// -----------------------------------------------------------------------------
void DynamicChoiceFilterParameter::setSetterCallback(const DynamicChoiceFilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
DynamicChoiceFilterParameter::SetterCallbackType DynamicChoiceFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void DynamicChoiceFilterParameter::setGetterCallback(const DynamicChoiceFilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
DynamicChoiceFilterParameter::GetterCallbackType DynamicChoiceFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

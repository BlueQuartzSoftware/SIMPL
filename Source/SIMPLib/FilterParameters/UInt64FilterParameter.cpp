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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "UInt64FilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt64FilterParameter::UInt64FilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt64FilterParameter::~UInt64FilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt64FilterParameter::Pointer UInt64FilterParameter::New(const QString& humanLabel, const QString& propertyName, uint64_t defaultValue, Category category, const SetterCallbackType& setterCallback,
                                                          const GetterCallbackType& getterCallback, int groupIndex)
{

  UInt64FilterParameter::Pointer ptr = UInt64FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setDefaultValue(static_cast<quint64>(defaultValue));
  ptr->setCategory(category);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UInt64FilterParameter::getWidgetType() const
{
  return QString("UInt64Widget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UInt64FilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    bool ok = false;
    QString strvalue = jsonValue.toString();
    m_SetterCallback(strvalue.toULongLong(&ok));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UInt64FilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    uint64_t value = m_GetterCallback();
    QString strvalue = QString::number(value);
    json[getPropertyName()] = strvalue; // Store the value as a string so we maintain all the fidelity of the value
  }
}

// -----------------------------------------------------------------------------
UInt64FilterParameter::Pointer UInt64FilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
UInt64FilterParameter::Pointer UInt64FilterParameter::New()
{
  Pointer sharedPtr(new(UInt64FilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString UInt64FilterParameter::getNameOfClass() const
{
  return QString("UInt64FilterParameter");
}

// -----------------------------------------------------------------------------
QString UInt64FilterParameter::ClassName()
{
  return QString("UInt64FilterParameter");
}

// -----------------------------------------------------------------------------
void UInt64FilterParameter::setSetterCallback(const UInt64FilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
UInt64FilterParameter::SetterCallbackType UInt64FilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void UInt64FilterParameter::setGetterCallback(const UInt64FilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
UInt64FilterParameter::GetterCallbackType UInt64FilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

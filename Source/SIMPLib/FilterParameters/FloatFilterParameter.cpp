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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FloatFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatFilterParameter::FloatFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatFilterParameter::~FloatFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatFilterParameter::Pointer FloatFilterParameter::New(const QString& humanLabel, const QString& propertyName, float defaultValue, Category category, const SetterCallbackType& setterCallback,
                                                        const GetterCallbackType& getterCallback, int groupIndex)
{

  FloatFilterParameter::Pointer ptr = FloatFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setDefaultValue(defaultValue);
  ptr->setCategory(category);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FloatFilterParameter::getWidgetType() const
{
  return QString("FloatWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    float f = static_cast<float>(jsonValue.toDouble());
    m_SetterCallback(f);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    json[getPropertyName()] = m_GetterCallback();
  }
}

// -----------------------------------------------------------------------------
FloatFilterParameter::Pointer FloatFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
FloatFilterParameter::Pointer FloatFilterParameter::New()
{
  Pointer sharedPtr(new(FloatFilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString FloatFilterParameter::getNameOfClass() const
{
  return QString("FloatFilterParameter");
}

// -----------------------------------------------------------------------------
QString FloatFilterParameter::ClassName()
{
  return QString("FloatFilterParameter");
}

// -----------------------------------------------------------------------------
void FloatFilterParameter::setSetterCallback(const FloatFilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
FloatFilterParameter::SetterCallbackType FloatFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void FloatFilterParameter::setGetterCallback(const FloatFilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
FloatFilterParameter::GetterCallbackType FloatFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

/* ============================================================================
 * Copyright (c) 2016-2016 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FloatVec2FilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec2FilterParameter::FloatVec2FilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec2FilterParameter::~FloatVec2FilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec2FilterParameter::Pointer FloatVec2FilterParameter::New(const QString& humanLabel, const QString& propertyName, const FloatVec2Type& defaultValue, Category category,
                                                                const SetterCallbackType& setterCallback, const GetterCallbackType& getterCallback, int groupIndex)
{

  FloatVec2FilterParameter::Pointer ptr = FloatVec2FilterParameter::New();
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
QString FloatVec2FilterParameter::getWidgetType() const
{
  return QString("FloatVec2Widget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec2FilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonObject json = jsonValue.toObject();
    FloatVec2Type floatVec2;
    if(json["x"].isDouble() && json["y"].isDouble())
    {
      floatVec2[0] = static_cast<float>(json["x"].toDouble());
      floatVec2[1] = static_cast<float>(json["y"].toDouble());
    }
    m_SetterCallback(floatVec2);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec2FilterParameter::writeJson(QJsonObject& obj)
{
  if(m_GetterCallback)
  {
    FloatVec2Type floatVec2 = m_GetterCallback();
    QJsonObject json;
    json["x"] = static_cast<double>(floatVec2.getX());
    json["y"] = static_cast<double>(floatVec2.getY());
    obj[getPropertyName()] = json;
  }
}

// -----------------------------------------------------------------------------
FloatVec2FilterParameter::Pointer FloatVec2FilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
FloatVec2FilterParameter::Pointer FloatVec2FilterParameter::New()
{
  Pointer sharedPtr(new(FloatVec2FilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString FloatVec2FilterParameter::getNameOfClass() const
{
  return QString("FloatVec2FilterParameter");
}

// -----------------------------------------------------------------------------
QString FloatVec2FilterParameter::ClassName()
{
  return QString("FloatVec2FilterParameter");
}

// -----------------------------------------------------------------------------
void FloatVec2FilterParameter::setSetterCallback(const FloatVec2FilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
FloatVec2FilterParameter::SetterCallbackType FloatVec2FilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void FloatVec2FilterParameter::setGetterCallback(const FloatVec2FilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
FloatVec2FilterParameter::GetterCallbackType FloatVec2FilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

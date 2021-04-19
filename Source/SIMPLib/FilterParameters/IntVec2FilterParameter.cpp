/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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

#include "IntVec2FilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec2FilterParameter::IntVec2FilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec2FilterParameter::~IntVec2FilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec2FilterParameter::Pointer IntVec2FilterParameter::Create(const QString& humanLabel, const QString& propertyName, const IntVec2Type& defaultValue, Category category,
                                                               const SetterCallbackType& setterCallback, const GetterCallbackType& getterCallback, int groupIndex)
{

  IntVec2FilterParameter::Pointer ptr = IntVec2FilterParameter::New();
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
QString IntVec2FilterParameter::getWidgetType() const
{
  return QString("IntVec2Widget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec2FilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonObject json = jsonValue.toObject();
    IntVec2Type intVec2;
    if(json["x"].isDouble() && json["y"].isDouble())
    {
      intVec2[0] = json["x"].toInt();
      intVec2[1] = json["y"].toInt();
    }
    m_SetterCallback(intVec2);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec2FilterParameter::writeJson(QJsonObject& obj) const
{
  if(m_GetterCallback)
  {
    IntVec2Type intVec2 = m_GetterCallback();
    QJsonObject json;
    json["x"] = intVec2.getX();
    json["y"] = intVec2.getY();
    obj[getPropertyName()] = json;
  }
}

// -----------------------------------------------------------------------------
IntVec2FilterParameter::Pointer IntVec2FilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
IntVec2FilterParameter::Pointer IntVec2FilterParameter::New()
{
  Pointer sharedPtr(new(IntVec2FilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString IntVec2FilterParameter::getNameOfClass() const
{
  return QString("IntVec2FilterParameter");
}

// -----------------------------------------------------------------------------
QString IntVec2FilterParameter::ClassName()
{
  return QString("IntVec2FilterParameter");
}

// -----------------------------------------------------------------------------
void IntVec2FilterParameter::setSetterCallback(const IntVec2FilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
IntVec2FilterParameter::SetterCallbackType IntVec2FilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void IntVec2FilterParameter::setGetterCallback(const IntVec2FilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
IntVec2FilterParameter::GetterCallbackType IntVec2FilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

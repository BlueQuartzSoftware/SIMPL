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

#include "IntVec3FilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3FilterParameter::IntVec3FilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3FilterParameter::~IntVec3FilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3FilterParameter::Pointer IntVec3FilterParameter::New(const QString& humanLabel, const QString& propertyName, const IntVec3Type& defaultValue, Category category,
                                                            const SetterCallbackType& setterCallback, const GetterCallbackType& getterCallback, int groupIndex)
{

  IntVec3FilterParameter::Pointer ptr = IntVec3FilterParameter::New();
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
QString IntVec3FilterParameter::getWidgetType() const
{
  return QString("IntVec3Widget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec3FilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonObject json = jsonValue.toObject();
    IntVec3Type intVec3;
    if(json["x"].isDouble() && json["y"].isDouble() && json["z"].isDouble())
    {
      intVec3[0] = json["x"].toInt();
      intVec3[1] = json["y"].toInt();
      intVec3[2] = json["z"].toInt();
    }
    m_SetterCallback(intVec3);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec3FilterParameter::writeJson(QJsonObject& obj)
{
  if(m_GetterCallback)
  {
    IntVec3Type intVec3 = m_GetterCallback();
    QJsonObject json;
    json["x"] = intVec3.getX();
    json["y"] = intVec3.getY();
    json["z"] = intVec3.getZ();
    obj[getPropertyName()] = json;
  }
}

// -----------------------------------------------------------------------------
IntVec3FilterParameter::Pointer IntVec3FilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
IntVec3FilterParameter::Pointer IntVec3FilterParameter::New()
{
  Pointer sharedPtr(new(IntVec3FilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString IntVec3FilterParameter::getNameOfClass() const
{
  return QString("IntVec3FilterParameter");
}

// -----------------------------------------------------------------------------
QString IntVec3FilterParameter::ClassName()
{
  return QString("IntVec3FilterParameter");
}

// -----------------------------------------------------------------------------
void IntVec3FilterParameter::setSetterCallback(const IntVec3FilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
IntVec3FilterParameter::SetterCallbackType IntVec3FilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void IntVec3FilterParameter::setGetterCallback(const IntVec3FilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
IntVec3FilterParameter::GetterCallbackType IntVec3FilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

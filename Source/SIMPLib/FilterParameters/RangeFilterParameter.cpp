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

#include "RangeFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RangeFilterParameter::RangeFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RangeFilterParameter::~RangeFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RangeFilterParameter::Pointer RangeFilterParameter::New(const QString& humanLabel, const QString& propertyName, const FPRangePair& defaultPair, Category category,
                                                        const SetterCallbackType& setterCallback, const GetterCallbackType& getterCallback, int groupIndex)
{

  RangeFilterParameter::Pointer ptr = RangeFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setDefaultPair(defaultPair);
  ptr->setCategory(category);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RangeFilterParameter::getWidgetType() const
{
  return QString("RangeWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RangeFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonObject obj = jsonValue.toObject();
    FPRangePair dataPair;

    dataPair.first = obj["Min"].toDouble();
    dataPair.second = obj["Max"].toDouble();

    m_SetterCallback(dataPair);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RangeFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    FPRangePair dataPair = m_GetterCallback();
    QJsonObject obj;

    obj["Min"] = dataPair.first;
    obj["Max"] = dataPair.second;

    json[getPropertyName()] = obj;
  }
}

// -----------------------------------------------------------------------------
RangeFilterParameter::Pointer RangeFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
RangeFilterParameter::Pointer RangeFilterParameter::New()
{
  Pointer sharedPtr(new(RangeFilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString RangeFilterParameter::getNameOfClass() const
{
  return QString("RangeFilterParameter");
}

// -----------------------------------------------------------------------------
QString RangeFilterParameter::ClassName()
{
  return QString("RangeFilterParameter");
}

// -----------------------------------------------------------------------------
void RangeFilterParameter::setDefaultPair(const FPRangePair& value)
{
  m_DefaultPair = value;
}

// -----------------------------------------------------------------------------
FPRangePair RangeFilterParameter::getDefaultPair() const
{
  return m_DefaultPair;
}

// -----------------------------------------------------------------------------
void RangeFilterParameter::setSetterCallback(const RangeFilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
RangeFilterParameter::SetterCallbackType RangeFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void RangeFilterParameter::setGetterCallback(const RangeFilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
RangeFilterParameter::GetterCallbackType RangeFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

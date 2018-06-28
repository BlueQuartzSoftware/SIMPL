/* ============================================================================
* Copyright (c) 2017 BlueQuartz Software, LLC
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
* The code contained herein was partially funded by the following contracts:
*    United States Air Force Prime Contract FA8650-15-D-5231
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ParagraphFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ParagraphFilterParameter::ParagraphFilterParameter()
: FilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ParagraphFilterParameter::~ParagraphFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ParagraphFilterParameter::Pointer ParagraphFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QString& defaultValue, Category category,
                                                                SetterCallbackType setterCallback, GetterCallbackType getterCallback, bool allowPreflight, int groupIndex)
{

  ParagraphFilterParameter::Pointer ptr = ParagraphFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setDefaultValue(defaultValue);
  ptr->setCategory(category);
  ptr->setGroupIndex(groupIndex);
  ptr->setAllowPreflight(allowPreflight);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ParagraphFilterParameter::getWidgetType() const
{
  return QString("ParagraphWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ParagraphFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    m_SetterCallback(jsonValue.toString(""));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ParagraphFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    json[getPropertyName()] = m_GetterCallback();
  }
}

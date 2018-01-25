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

#include "PhaseTypeSelectionFilterParameter.h"

#include <QtCore/QJsonArray>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionFilterParameter::PhaseTypeSelectionFilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionFilterParameter::~PhaseTypeSelectionFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionFilterParameter::Pointer PhaseTypeSelectionFilterParameter::New(const QString& humanLabel, const QString& phaseTypeDataProperty, const DataArrayPath attributeMatrixDefault,
                                                                                  Category category, SetterCallbackType setterCallback, GetterCallbackType getterCallback,
                                                                                  const QString& PhaseTypesArrayName, const QString& phaseTypeCountProperty, const QString& attributeMatrixProperty,
                                                                                  const QStringList phaseListChoices, int groupIndex)
{
  PhaseTypeSelectionFilterParameter::Pointer ptr = PhaseTypeSelectionFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(PhaseTypesArrayName);
  ptr->setCategory(category);
  ptr->setGroupIndex(groupIndex);
  ptr->setPhaseListChoices(phaseListChoices);
  ptr->setPhaseTypeCountProperty(phaseTypeCountProperty);
  ptr->setPhaseTypeDataProperty(phaseTypeDataProperty);
  ptr->setAttributeMatrixPathProperty(attributeMatrixProperty);
  ptr->setAttributeMatrixPathDefault(attributeMatrixDefault);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhaseTypeSelectionFilterParameter::getWidgetType() const
{
  return QString("PhaseTypeSelectionWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonArray jsonArray = jsonValue.toArray();
    PhaseType::Types vec;
    for(int i = 0; i < jsonArray.size(); i++)
    {
      vec.push_back(static_cast<PhaseType::Type>(jsonArray[i].toInt()));
    }
    m_SetterCallback(vec);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    PhaseType::Types vec = m_GetterCallback();
    QJsonArray jsonArray;

    for(int i = 0; i < vec.size(); i++)
    {
      jsonArray.push_back(static_cast<int>(vec[i]));
    }

    json[getPropertyName()] = jsonArray;
  }
}

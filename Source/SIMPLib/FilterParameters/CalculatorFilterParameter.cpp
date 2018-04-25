/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "CalculatorFilterParameter.h"

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/CoreFilters/ArrayCalculator.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculatorFilterParameter::CalculatorFilterParameter()
: FilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculatorFilterParameter::~CalculatorFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculatorFilterParameter::Pointer CalculatorFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QString& defaultValue, Category category,
                                                                  SetterCallbackType setterCallback, GetterCallbackType getterCallback, int groupIndex)
{

  CalculatorFilterParameter::Pointer ptr = CalculatorFilterParameter::New();
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
QString CalculatorFilterParameter::getWidgetType() const
{
  return QString("CalculatorWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorFilterParameter::readJson(const QJsonObject& json)
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
void CalculatorFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    json[getPropertyName()] = m_GetterCallback();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorFilterParameter::dataArrayPathRenamed(AbstractFilter* filter, DataArrayPath::RenameType renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  // This filter parameter requires an ArrayCalculator filter
  ArrayCalculator* calc = dynamic_cast<ArrayCalculator*>(filter);
  if(nullptr != calc)
  {
    DataArrayPath amPath = calc->getSelectedAttributeMatrix();

    bool dcChanged = oldPath.getDataContainerName() != newPath.getDataContainerName();
    bool amChanged = !dcChanged && oldPath.getAttributeMatrixName() != newPath.getAttributeMatrixName();
    bool daChanged = !amChanged && oldPath.getDataArrayName() != newPath.getDataArrayName();

    bool amConsistent = false == dcChanged && false == amChanged
      && amPath.getDataContainerName() == oldPath.getDataContainerName()
      && amPath.getAttributeMatrixName() == oldPath.getAttributeMatrixName();

    // Only update the widget if the DataContainer and AttributeMatrix match and were not renamed
    if(daChanged && amConsistent)
    {
      QString inputStr = m_GetterCallback();
      inputStr.replace(oldPath.getDataArrayName(), newPath.getDataArrayName());
      m_SetterCallback(inputStr);

      emit filter->dataArrayPathUpdated(getPropertyName(), renamePath);
    }
  }
}

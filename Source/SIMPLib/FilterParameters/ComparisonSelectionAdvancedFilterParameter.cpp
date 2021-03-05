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

#include "ComparisonSelectionAdvancedFilterParameter.h"

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/ComparisonValue.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionAdvancedFilterParameter::ComparisonSelectionAdvancedFilterParameter()
: m_ShowOperators(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionAdvancedFilterParameter::~ComparisonSelectionAdvancedFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionAdvancedFilterParameter::Pointer ComparisonSelectionAdvancedFilterParameter::Create(const QString& humanLabel, const QString& propertyName, ComparisonInputsAdvanced defaultValue,
                                                                                                       Category category, const SetterCallbackType& setterCallback,
                                                                                                       const GetterCallbackType& getterCallback, const std::vector<QString>& choices,
                                                                                                       bool showOperators, int groupIndex)
{
  ComparisonSelectionAdvancedFilterParameter::Pointer ptr = ComparisonSelectionAdvancedFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant var;
  var.setValue(defaultValue);
  ptr->setDefaultValue(var);
  ptr->setCategory(category);
  ptr->setChoices(choices);
  ptr->setShowOperators(showOperators);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComparisonSelectionAdvancedFilterParameter::getWidgetType() const
{
  return QString("ComparisonSelectionAdvancedWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonObject jsonObject = jsonValue.toObject();

    ComparisonInputsAdvanced inputs;
    inputs.readJson(jsonObject);

    m_SetterCallback(inputs);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    ComparisonInputsAdvanced inputs = m_GetterCallback();

    QJsonObject inputsObj;
    inputs.writeJson(inputsObj);

    json[getPropertyName()] = inputsObj;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedFilterParameter::dataArrayPathRenamed(AbstractFilter* filter, const DataArrayPath::RenameType& renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  ComparisonInputsAdvanced inputs = m_GetterCallback();
  bool hasChanges = false;

  bool hasAttributeMatrix = !oldPath.getAttributeMatrixName().isEmpty();
  bool hasDataArray = !oldPath.getDataArrayName().isEmpty();

  // Update the values contained by ComparisonInputsAdvanced
  bool sameDC = inputs.getDataContainerName() == oldPath.getDataContainerName();
  bool sameAM = inputs.getAttributeMatrixName() == oldPath.getAttributeMatrixName();
  if(sameDC && (!hasAttributeMatrix || (sameAM && !hasDataArray)))
  {
    inputs.setDataContainerName(newPath.getDataContainerName());

    if(hasAttributeMatrix)
    {
      inputs.setAttributeMatrixName(newPath.getAttributeMatrixName());
    }

    hasChanges = true;
  }

  // Update all comparisons in ComparisonInputsAdvanced
  QVector<AbstractComparison::Pointer> comparisons = inputs.getComparisonValues();
  for(AbstractComparison::Pointer comparison : comparisons)
  {
    hasChanges |= comparison->renameDataArrayPath(renamePath);
  }

  if(hasChanges)
  {
    m_SetterCallback(inputs);
    Q_EMIT filter->dataArrayPathUpdated(getPropertyName(), renamePath);
  }
}

// -----------------------------------------------------------------------------
ComparisonSelectionAdvancedFilterParameter::Pointer ComparisonSelectionAdvancedFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
ComparisonSelectionAdvancedFilterParameter::Pointer ComparisonSelectionAdvancedFilterParameter::New()
{
  Pointer sharedPtr(new(ComparisonSelectionAdvancedFilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString ComparisonSelectionAdvancedFilterParameter::getNameOfClass() const
{
  return QString("ComparisonSelectionAdvancedFilterParameter");
}

// -----------------------------------------------------------------------------
QString ComparisonSelectionAdvancedFilterParameter::ClassName()
{
  return QString("ComparisonSelectionAdvancedFilterParameter");
}

// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedFilterParameter::setChoices(const std::vector<QString>& value)
{
  m_Choices = value;
}

// -----------------------------------------------------------------------------
std::vector<QString> ComparisonSelectionAdvancedFilterParameter::getChoices() const
{
  return m_Choices;
}

// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedFilterParameter::setShowOperators(bool value)
{
  m_ShowOperators = value;
}

// -----------------------------------------------------------------------------
bool ComparisonSelectionAdvancedFilterParameter::getShowOperators() const
{
  return m_ShowOperators;
}

// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedFilterParameter::setDefaultGeometryTypes(const IGeometry::Types& value)
{
  m_DefaultGeometryTypes = value;
}

// -----------------------------------------------------------------------------
IGeometry::Types ComparisonSelectionAdvancedFilterParameter::getDefaultGeometryTypes() const
{
  return m_DefaultGeometryTypes;
}

// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedFilterParameter::setDefaultAttributeMatrixTypes(const AttributeMatrix::Types& value)
{
  m_DefaultAttributeMatrixTypes = value;
}

// -----------------------------------------------------------------------------
AttributeMatrix::Types ComparisonSelectionAdvancedFilterParameter::getDefaultAttributeMatrixTypes() const
{
  return m_DefaultAttributeMatrixTypes;
}

// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedFilterParameter::setSetterCallback(const ComparisonSelectionAdvancedFilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
ComparisonSelectionAdvancedFilterParameter::SetterCallbackType ComparisonSelectionAdvancedFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void ComparisonSelectionAdvancedFilterParameter::setGetterCallback(const ComparisonSelectionAdvancedFilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
ComparisonSelectionAdvancedFilterParameter::GetterCallbackType ComparisonSelectionAdvancedFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

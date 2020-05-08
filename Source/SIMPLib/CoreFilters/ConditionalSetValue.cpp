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
#include "ConditionalSetValue.h"

#include <tuple>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConditionalSetValue::ConditionalSetValue()
: m_SelectedArrayPath("", "", "")
, m_ConditionalArrayPath("", "", "")
, m_ReplaceValue(0.0)
//, m_Array(nullptr)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConditionalSetValue::~ConditionalSetValue() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConditionalSetValue::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("New Value", ReplaceValue, FilterParameter::Parameter, ConditionalSetValue));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Conditional Array", ConditionalArrayPath, FilterParameter::RequiredArray, ConditionalSetValue, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array", SelectedArrayPath, FilterParameter::RequiredArray, ConditionalSetValue, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConditionalSetValue::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setConditionalArrayPath(reader->readDataArrayPath("ConditionalArrayPath", getConditionalArrayPath()));
  setSelectedArrayPath(reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()));
  setReplaceValue(reader->readValue("ReplaceValue", getReplaceValue()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void checkValuesInt(AbstractFilter* filter, double replaceValue, QString strType)
{
  QString ss;

  if(!((replaceValue >= std::numeric_limits<T>::min()) && (replaceValue <= std::numeric_limits<T>::max())))
  {
    ss = QObject::tr("The %1 replace value was invalid. The valid range is %2 to %3").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
    filter->setErrorCondition(-100, ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void checkValuesFloatDouble(AbstractFilter* filter, double replaceValue, QString strType)
{
  QString ss;

  if(!(((replaceValue >= static_cast<T>(-1) * std::numeric_limits<T>::max()) && (replaceValue <= static_cast<T>(-1) * std::numeric_limits<T>::min())) || (replaceValue == 0) ||
       ((replaceValue >= std::numeric_limits<T>::min()) && (replaceValue <= std::numeric_limits<T>::max()))))
  {
    ss = QObject::tr("The %1 replace value was invalid. The valid ranges are -%3 to -%2, 0, %2 to %3").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
    filter->setErrorCondition(-101, ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

template <typename T>
void replaceValue(AbstractFilter* filter, IDataArray::Pointer inDataPtr, BoolArrayType::Pointer condDataPtr, double replaceValue)
{
  std::ignore = filter;
  typename DataArray<T>::Pointer inputArrayPtr = std::dynamic_pointer_cast<DataArray<T>>(inDataPtr);

  T replaceVal = static_cast<T>(replaceValue);

  bool* condData = condDataPtr->getPointer(0);
  size_t numTuples = inputArrayPtr->getNumberOfTuples();

  for(size_t iter = 0; iter < numTuples; iter++)
  {
    if(condData[iter])
    {
      inputArrayPtr->initializeTuple(iter, &replaceVal);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConditionalSetValue::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConditionalSetValue::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  QVector<DataArrayPath> dataArrayPaths;

  m_ArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, getSelectedArrayPath());
  if(getErrorCode() < 0)
  {
    return;
  }
  dataArrayPaths.push_back(getSelectedArrayPath());

  std::vector<size_t> cDims(1, 1);
  m_ConditionalArrayPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>>(this, getConditionalArrayPath(), cDims);
  if(getErrorCode() < 0)
  {
    return;
  }
  if(nullptr != m_ConditionalArrayPtr.lock())
  {
    m_ConditionalArray = m_ConditionalArrayPtr.lock()->getPointer(0);
  }
  dataArrayPaths.push_back(getConditionalArrayPath());

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);

  QString dType = m_ArrayPtr.lock()->getTypeAsString();
  if(dType.compare(SIMPL::TypeNames::Int8) == 0)
  {
    checkValuesInt<int8_t>(this, m_ReplaceValue, SIMPL::TypeNames::Int8);
  }
  else if(dType.compare(SIMPL::TypeNames::UInt8) == 0)
  {
    checkValuesInt<uint8_t>(this, m_ReplaceValue, SIMPL::TypeNames::UInt8);
  }
  else if(dType.compare(SIMPL::TypeNames::Int16) == 0)
  {
    checkValuesInt<int16_t>(this, m_ReplaceValue, SIMPL::TypeNames::Int16);
  }
  else if(dType.compare(SIMPL::TypeNames::UInt16) == 0)
  {
    checkValuesInt<uint16_t>(this, m_ReplaceValue, SIMPL::TypeNames::UInt16);
  }
  else if(dType.compare(SIMPL::TypeNames::Int32) == 0)
  {
    checkValuesInt<int32_t>(this, m_ReplaceValue, SIMPL::TypeNames::Int32);
  }
  else if(dType.compare(SIMPL::TypeNames::UInt32) == 0)
  {
    checkValuesInt<uint32_t>(this, m_ReplaceValue, SIMPL::TypeNames::UInt32);
  }
  else if(dType.compare(SIMPL::TypeNames::Int64) == 0)
  {
    checkValuesInt<int64_t>(this, m_ReplaceValue, SIMPL::TypeNames::Int64);
  }
  else if(dType.compare(SIMPL::TypeNames::UInt64) == 0)
  {
    checkValuesInt<uint64_t>(this, m_ReplaceValue, SIMPL::TypeNames::UInt64);
  }
  else if(dType.compare(SIMPL::TypeNames::Float) == 0)
  {
    checkValuesFloatDouble<float>(this, m_ReplaceValue, SIMPL::TypeNames::Float);
  }
  else if(dType.compare(SIMPL::TypeNames::Double) == 0)
  {
    checkValuesFloatDouble<double>(this, m_ReplaceValue, SIMPL::TypeNames::Double);
  }
  else if(dType.compare(SIMPL::TypeNames::Bool) == 0)
  {
    if(m_ReplaceValue != 0.0)
    {
      m_ReplaceValue = 1.0; // anything that is not a zero is a one
    }
  }
  else
  {
    QString ss = QObject::tr("Incorrect data scalar type");
    setErrorCondition(-4060, ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConditionalSetValue::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  EXECUTE_FUNCTION_TEMPLATE(this, replaceValue, m_ArrayPtr.lock(), this, m_ArrayPtr.lock(), m_ConditionalArrayPtr.lock(), m_ReplaceValue)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConditionalSetValue::newFilterInstance(bool copyFilterParameters) const
{
  ConditionalSetValue::Pointer filter = ConditionalSetValue::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConditionalSetValue::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConditionalSetValue::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConditionalSetValue::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConditionalSetValue::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ConditionalSetValue::getUuid() const
{
  return QUuid("{47cafe63-83cc-5826-9521-4fb5bea684ef}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConditionalSetValue::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConditionalSetValue::getHumanLabel() const
{
  return "Replace Value in Array (Conditional)";
}

// -----------------------------------------------------------------------------
ConditionalSetValue::Pointer ConditionalSetValue::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ConditionalSetValue> ConditionalSetValue::New()
{
  struct make_shared_enabler : public ConditionalSetValue
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ConditionalSetValue::getNameOfClass() const
{
  return QString("ConditionalSetValue");
}

// -----------------------------------------------------------------------------
QString ConditionalSetValue::ClassName()
{
  return QString("ConditionalSetValue");
}

// -----------------------------------------------------------------------------
void ConditionalSetValue::setSelectedArrayPath(const DataArrayPath& value)
{
  m_SelectedArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ConditionalSetValue::getSelectedArrayPath() const
{
  return m_SelectedArrayPath;
}

// -----------------------------------------------------------------------------
void ConditionalSetValue::setConditionalArrayPath(const DataArrayPath& value)
{
  m_ConditionalArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ConditionalSetValue::getConditionalArrayPath() const
{
  return m_ConditionalArrayPath;
}

// -----------------------------------------------------------------------------
void ConditionalSetValue::setReplaceValue(double value)
{
  m_ReplaceValue = value;
}

// -----------------------------------------------------------------------------
double ConditionalSetValue::getReplaceValue() const
{
  return m_ReplaceValue;
}

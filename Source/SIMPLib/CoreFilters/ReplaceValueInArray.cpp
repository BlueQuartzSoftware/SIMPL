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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ReplaceValueInArray.h"

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReplaceValueInArray::ReplaceValueInArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReplaceValueInArray::~ReplaceValueInArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceValueInArray::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Value to Replace", RemoveValue, FilterParameter::Category::Parameter, ReplaceValueInArray));

  parameters.push_back(SIMPL_NEW_DOUBLE_FP("New Value", ReplaceValue, FilterParameter::Category::Parameter, ReplaceValueInArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array", SelectedArray, FilterParameter::Category::RequiredArray, ReplaceValueInArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceValueInArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedArray(reader->readDataArrayPath("SelectedArray", getSelectedArray()));
  setRemoveValue(reader->readValue("RemoveValue", getRemoveValue()));
  setReplaceValue(reader->readValue("ReplaceValue", getReplaceValue()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void checkValuesInt(AbstractFilter* filter, double removeValue, double replaceValue, QString strType)
{
  QString ss;

  if(!((removeValue >= std::numeric_limits<T>::min()) && (removeValue <= std::numeric_limits<T>::max())))
  {
    ss = QObject::tr("The %1 remove value was invalid. The valid range is %2 to %3").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
    filter->setErrorCondition(-100, ss);
  }
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
void checkValuesFloatDouble(AbstractFilter* filter, double removeValue, double replaceValue, QString strType)
{
  QString ss;

  if(!(((removeValue >= static_cast<T>(-1) * std::numeric_limits<T>::max()) && (removeValue <= static_cast<T>(-1) * std::numeric_limits<T>::min())) || (removeValue == 0) ||
       ((removeValue >= std::numeric_limits<T>::min()) && (removeValue <= std::numeric_limits<T>::max()))))
  {
    ss = QObject::tr("The %1 remove value was invalid. The valid ranges are -%3 to -%2, 0, %2 to %3").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
    filter->setErrorCondition(-101, ss);
  }
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
void replaceValue(AbstractFilter* filter, IDataArray::Pointer inDataPtr, double removeValue, double replaceValue)
{
  typename DataArray<T>::Pointer inputArrayPtr = std::dynamic_pointer_cast<DataArray<T>>(inDataPtr);

  T removeVal = static_cast<T>(removeValue);
  T replaceVal = static_cast<T>(replaceValue);

  T* inData = inputArrayPtr->getPointer(0);
  size_t numTuples = inputArrayPtr->getNumberOfTuples();

  for(size_t iter = 0; iter < numTuples; iter++)
  {
    if(inData[iter] == removeVal)
    {
      inData[iter] = replaceVal;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceValueInArray::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceValueInArray::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  m_ArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, getSelectedArray());
  if(getErrorCode() < 0)
  {
    return;
  }

  if(m_ArrayPtr.lock()->getNumberOfComponents() > 1)
  {
    QString ss = QObject::tr("Selected array '%1' must be a scalar array (1 component). The number of components is %2")
                     .arg(getSelectedArray().getDataArrayName())
                     .arg(m_ArrayPtr.lock()->getNumberOfComponents());
    setErrorCondition(-11002, ss);
    return;
  }

  QString dType = m_ArrayPtr.lock()->getTypeAsString();
  if(dType.compare(SIMPL::TypeNames::Int8) == 0)
  {
    checkValuesInt<int8_t>(this, m_RemoveValue, m_ReplaceValue, SIMPL::TypeNames::Int8);
  }
  else if(dType.compare(SIMPL::TypeNames::UInt8) == 0)
  {
    checkValuesInt<uint8_t>(this, m_RemoveValue, m_ReplaceValue, SIMPL::TypeNames::UInt8);
  }
  else if(dType.compare(SIMPL::TypeNames::Int16) == 0)
  {
    checkValuesInt<int16_t>(this, m_RemoveValue, m_ReplaceValue, SIMPL::TypeNames::Int16);
  }
  else if(dType.compare(SIMPL::TypeNames::UInt16) == 0)
  {
    checkValuesInt<uint16_t>(this, m_RemoveValue, m_ReplaceValue, SIMPL::TypeNames::UInt16);
  }
  else if(dType.compare(SIMPL::TypeNames::Int32) == 0)
  {
    checkValuesInt<int32_t>(this, m_RemoveValue, m_ReplaceValue, SIMPL::TypeNames::Int32);
  }
  else if(dType.compare(SIMPL::TypeNames::UInt32) == 0)
  {
    checkValuesInt<uint32_t>(this, m_RemoveValue, m_ReplaceValue, SIMPL::TypeNames::UInt32);
  }
  else if(dType.compare(SIMPL::TypeNames::Int64) == 0)
  {
    checkValuesInt<int64_t>(this, m_RemoveValue, m_ReplaceValue, SIMPL::TypeNames::Int64);
  }
  else if(dType.compare(SIMPL::TypeNames::UInt64) == 0)
  {
    checkValuesInt<uint64_t>(this, m_RemoveValue, m_ReplaceValue, SIMPL::TypeNames::UInt64);
  }
  else if(dType.compare(SIMPL::TypeNames::Float) == 0)
  {
    checkValuesFloatDouble<float>(this, m_RemoveValue, m_ReplaceValue, SIMPL::TypeNames::Float);
  }
  else if(dType.compare(SIMPL::TypeNames::Double) == 0)
  {
    checkValuesFloatDouble<double>(this, m_RemoveValue, m_ReplaceValue, SIMPL::TypeNames::Double);
  }
  else if(dType.compare(SIMPL::TypeNames::Bool) == 0)
  {
    if(m_RemoveValue != 0.0)
    {
      m_RemoveValue = 1.0; // anything that is not a zero is a one
    }
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
void ReplaceValueInArray::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  EXECUTE_FUNCTION_TEMPLATE(this, replaceValue, m_ArrayPtr.lock(), this, m_ArrayPtr.lock(), m_RemoveValue, m_ReplaceValue)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReplaceValueInArray::newFilterInstance(bool copyFilterParameters) const
{
  ReplaceValueInArray::Pointer filter = ReplaceValueInArray::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceValueInArray::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceValueInArray::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceValueInArray::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceValueInArray::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ReplaceValueInArray::getUuid() const
{
  return QUuid("{a37f2e24-7400-5005-b9a7-b2224570cbe9}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceValueInArray::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceValueInArray::getHumanLabel() const
{
  return "Replace Value in Array";
}

// -----------------------------------------------------------------------------
ReplaceValueInArray::Pointer ReplaceValueInArray::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ReplaceValueInArray> ReplaceValueInArray::New()
{
  struct make_shared_enabler : public ReplaceValueInArray
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ReplaceValueInArray::getNameOfClass() const
{
  return QString("ReplaceValueInArray");
}

// -----------------------------------------------------------------------------
QString ReplaceValueInArray::ClassName()
{
  return QString("ReplaceValueInArray");
}

// -----------------------------------------------------------------------------
void ReplaceValueInArray::setSelectedArray(const DataArrayPath& value)
{
  m_SelectedArray = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ReplaceValueInArray::getSelectedArray() const
{
  return m_SelectedArray;
}

// -----------------------------------------------------------------------------
void ReplaceValueInArray::setRemoveValue(double value)
{
  m_RemoveValue = value;
}

// -----------------------------------------------------------------------------
double ReplaceValueInArray::getRemoveValue() const
{
  return m_RemoveValue;
}

// -----------------------------------------------------------------------------
void ReplaceValueInArray::setReplaceValue(double value)
{
  m_ReplaceValue = value;
}

// -----------------------------------------------------------------------------
double ReplaceValueInArray::getReplaceValue() const
{
  return m_ReplaceValue;
}

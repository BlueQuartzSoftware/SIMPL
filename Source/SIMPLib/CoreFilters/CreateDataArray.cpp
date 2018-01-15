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

#include "CreateDataArray.h"

#include <QtCore/QDateTime>

#include <limits>
#include <random>
#include <chrono>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/ScalarTypeFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

/**
* @brief initializeArrayWithInts Initializes the array p with integers, either from the
* manual value entered in the filter, or with a random number.  This function does not
* check that the template type actually is an integer, so it will most likely cause
* unexpected results when passing anything other than an integer as a template parameter.
* @param p The array that will be initialized
*/
template <typename T> void initializeArrayWithInts(IDataArray::Pointer outputArrayPtr, int initializationType, FPRangePair initializationRange, const QString& initializationValue, SIMPL::ScalarTypes::Type scalarType)
{

  typename DataArray<T>::Pointer array = std::dynamic_pointer_cast<DataArray<T>>(outputArrayPtr);
  size_t count = array->getSize();
  T* rawPointer = array->getPointer(0);

  if(initializationType == CreateDataArray::Manual)
  {

    qint8 i8 = static_cast<qint8>(initializationValue.toInt());
    quint8 ui8 = static_cast<quint8>(initializationValue.toUInt());
    qint16 i16 = static_cast<qint16>(initializationValue.toShort());
    quint16 ui16 = static_cast<quint16>(initializationValue.toUShort());
    qint32 i32 = static_cast<qint32>(initializationValue.toInt());
    quint32 ui32 = static_cast<quint32>(initializationValue.toUInt());
    qint64 i64 = static_cast<qint64>(initializationValue.toLongLong());
    quint64 ui64 = static_cast<quint64>(initializationValue.toULongLong());

    switch(scalarType)
    {
    case SIMPL::ScalarTypes::Type::Int8:
      for(size_t i = 0; i < count; i++)
      {
        rawPointer[i] = static_cast<T>(i8);
      }
      break;
    case SIMPL::ScalarTypes::Type::UInt8:
      for(size_t i = 0; i < count; i++)
      {
        rawPointer[i] = static_cast<T>(ui8);
      }
      break;
    case SIMPL::ScalarTypes::Type::Int16:
      for(size_t i = 0; i < count; i++)
      {
        rawPointer[i] = static_cast<T>(i16);
      }
      break;
    case SIMPL::ScalarTypes::Type::UInt16:
      for(size_t i = 0; i < count; i++)
      {
        rawPointer[i] = static_cast<T>(ui16);
      }
      break;
    case SIMPL::ScalarTypes::Type::Int32:
      for(size_t i = 0; i < count; i++)
      {
        rawPointer[i] = static_cast<T>(i32);
      }
      break;
    case SIMPL::ScalarTypes::Type::UInt32:
      for(size_t i = 0; i < count; i++)
      {
        rawPointer[i] = static_cast<T>(ui32);
      }
      break;
    case SIMPL::ScalarTypes::Type::Int64:
      for(size_t i = 0; i < count; i++)
      {
        rawPointer[i] = static_cast<T>(i64);
      }
      break;
    case SIMPL::ScalarTypes::Type::UInt64:
      for(size_t i = 0; i < count; i++)
      {
        rawPointer[i] = static_cast<T>(ui64);
      }
      break;
    default:
      Q_ASSERT_X(false, __FILE__, "Incorrent use of CreateDataArray::initializeArrayWithInts when using a non-integer type");
      break;
    }
  }
  else
  {
    T rangeMin = static_cast<T>(initializationRange.first);
    T rangeMax = static_cast<T>(initializationRange.second);

    std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::mt19937_64 generator(seed); // Standard mersenne_twister_engine seeded with milliseconds
    std::uniform_int_distribution<> distribution(rangeMin , rangeMax);

    for(size_t i = 0; i < count; i++)
    {
      T value = static_cast<T>(distribution(generator));
      rawPointer[i] = value;
    }
  }
}

/**
* @brief initializeArrayWithInts Initializes the array p with integers, either from the
* manual value entered in the filter, or with a random number.  This function does not
* check that the template type actually is an integer, so it will most likely cause
* unexpected results when passing anything other than an integer as a template parameter.
* @param p The array that will be initialized
*/
template <> void initializeArrayWithInts<bool>(IDataArray::Pointer outputArrayPtr, int initializationType, FPRangePair initializationRange, const QString& initializationValue, SIMPL::ScalarTypes::Type scalarType)
{
  DataArray<bool>::Pointer array = std::dynamic_pointer_cast<DataArray<bool>>(outputArrayPtr);
  size_t count = array->getSize();
  bool* rawPointer = array->getPointer(0);

  if(initializationType == CreateDataArray::Manual)
  {
    bool result;
    if(initializationValue.toInt() == 0)
    {
      result = false;
    }
    else
    {
      result = true;
    }

    for(size_t i = 0; i < count; i++)
    {
      rawPointer[i] = result;
    }
  }
  else
  {
    std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::mt19937_64 generator(seed); // Standard mersenne_twister_engine seeded with milliseconds
    std::uniform_int_distribution<int32_t> distribution(0, 1);

    for(size_t i = 0; i < count; i++)
    {
      int8_t result = static_cast<int8_t>(distribution(generator));
      if(result == 0)
      {
        rawPointer[i] = false;
      }
      else
      {
        rawPointer[i] = true;
      }
    }
  }
}

/**
* @brief initializeArrayWithReals Initializes the array p with real numbers, either from the
* manual value entered in the filter, or with a random number.  This function does not
* check that the template type actually is a non-integer, so it will most likely cause
* unexpected results when passing anything other than a float or double as a template
* parameter.
* @param p The array that will be initialized
*/
template <typename T> void initializeArrayWithReals(IDataArray::Pointer outputArrayPtr, int initializationType, FPRangePair initializationRange, const QString& initializationValue)
{
  typename DataArray<T>::Pointer array = std::dynamic_pointer_cast<DataArray<T>>(outputArrayPtr);
  size_t count = array->getSize();
  T* rawPointer = array->getPointer(0);

  if(initializationType == CreateDataArray::Manual)
  {
    T value = static_cast<T>(initializationValue.toDouble());
    for(size_t i = 0; i < count; i++)
    {
      rawPointer[i] = value;
    }
  }
  else
  {
    T rangeMin = static_cast<T>(initializationRange.first);
    T rangeMax = static_cast<T>(initializationRange.second);

    std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::mt19937_64 generator(seed); // Standard mersenne_twister_engine seeded with milliseconds
    std::uniform_real_distribution<> distribution(rangeMin, rangeMax);

    size_t count = array->getSize();
    for(size_t i = 0; i < count; i++)
    {
      T value = distribution(generator);
      rawPointer[i] = value;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateDataArray::CreateDataArray()
: AbstractFilter()
, m_ScalarType(SIMPL::ScalarTypes::Type::Int8)
, m_NumberOfComponents(0)
, m_NewArray("", "", "")
, m_InitializationType(Manual)
, m_InitializationValue("0")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateDataArray::~CreateDataArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_SCALARTYPE_FP("Scalar Type", ScalarType, FilterParameter::Parameter, CreateDataArray));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Components", NumberOfComponents, FilterParameter::Parameter, CreateDataArray));

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Initialization Type");
    parameter->setPropertyName("InitializationType");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(CreateDataArray, this, InitializationType));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(CreateDataArray, this, InitializationType));

    parameter->setDefaultValue(Manual);

    QVector<QString> choices;
    choices.push_back("Manual");
    choices.push_back("Random With Range");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "InitializationValue"
                << "InitializationRange";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_STRING_FP("Initialization Value", InitializationValue, FilterParameter::Parameter, CreateDataArray, Manual));
  parameters.push_back(SIMPL_NEW_RANGE_FP("Initialization Range", InitializationRange, FilterParameter::Parameter, CreateDataArray, RandomWithRange));
  {
    DataArrayCreationFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Created Attribute Array", NewArray, FilterParameter::CreatedArray, CreateDataArray, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setScalarType(static_cast<SIMPL::ScalarTypes::Type>(reader->readValue("ScalarType", static_cast<int>(getScalarType()))));
  setNumberOfComponents(reader->readValue("NumberOfComponents", getNumberOfComponents()));
  setNewArray(reader->readDataArrayPath("NewArray", getNewArray()));
  setInitializationValue(reader->readString("InitializationValue", getInitializationValue()));
  setInitializationType(reader->readValue("InitializationType", getInitializationType()));
  setInitializationRange(reader->readPairOfDoubles("InitializationRange", getInitializationRange()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  if(getErrorCondition() < 0)
  {
    return;
  }

  if(m_InitializationType == RandomWithRange && m_InitializationRange.first > m_InitializationRange.second)
  {
    QString ss = "Invalid initialization range.  Minimum value is larger than maximum value.";
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(getNumberOfComponents() < 0)
  {
    setErrorCondition(-8050);
    QString ss = QObject::tr("The number of components must non-negative");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if(getNumberOfComponents() == 0)
  {
    setErrorCondition(-8051);
    QString ss = QObject::tr("The number of components is Zero. This will result in an array that has no memory allocated. Are you sure you wanted to do this?");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if(!getNewArray().isValid())
  {
    setErrorCondition(-8051);
    QString ss = QObject::tr("The Created DataArrayPath is invalid. Please select the Data Container, Attribute Matrix and set an output DataArray name.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  QVector<size_t> cDims(1, getNumberOfComponents());
  if(getErrorCondition() < 0)
  {
    return;
  }
  // Create the data array and initialize it to a placeholder value
  m_OutputArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromTypeEnum()(this, getNewArray(), cDims, static_cast<int>(getScalarType()), 0);

  QString dataArrayName = getNewArray().getDataArrayName();

  if(m_ScalarType == SIMPL::ScalarTypes::Type::Int8)
  {
    checkInitialization<int8_t>(dataArrayName); // check the initialization for that data type
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Int16)
  {
    checkInitialization<int16_t>(dataArrayName); // check the initialization for that data type
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Int32)
  {
    checkInitialization<int32_t>(dataArrayName); // check the initialization for that data type
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Int64)
  {
    checkInitialization<int64_t>(dataArrayName); // check the initialization for that data type
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt8)
  {
    checkInitialization<uint8_t>(dataArrayName); // check the initialization for that data type
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt16)
  {
    checkInitialization<uint16_t>(dataArrayName); // check the initialization for that data type
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt32)
  {
    checkInitialization<uint32_t>(dataArrayName); // check the initialization for that data type
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt64)
  {
    checkInitialization<uint64_t>(dataArrayName); // check the initialization for that data type
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Float)
  {
    checkInitialization<float>(dataArrayName); // check the initialization for that data type
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Double)
  {
    checkInitialization<double>(dataArrayName); // check the initialization for that data type
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Bool)
  {
    checkInitialization<bool>(dataArrayName); // check the initialization for that data type
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(m_ScalarType == SIMPL::ScalarTypes::Type::Int8)
  {
    initializeArrayWithInts<int8_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, m_InitializationValue, m_ScalarType);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Int16)
  {
    initializeArrayWithInts<int16_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, m_InitializationValue, m_ScalarType);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Int32)
  {
    initializeArrayWithInts<int32_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, m_InitializationValue, m_ScalarType);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Int64)
  {
    initializeArrayWithInts<int64_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, m_InitializationValue, m_ScalarType);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt8)
  {
    initializeArrayWithInts<uint8_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, m_InitializationValue, m_ScalarType);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt16)
  {
    initializeArrayWithInts<uint16_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, m_InitializationValue, m_ScalarType);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt32)
  {
    initializeArrayWithInts<uint32_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, m_InitializationValue, m_ScalarType);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt64)
  {
    initializeArrayWithInts<uint64_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, m_InitializationValue, m_ScalarType);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Float)
  {
    initializeArrayWithReals<float>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, m_InitializationValue);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Double)
  {
    initializeArrayWithReals<double>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, m_InitializationValue);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Bool)
  {
    initializeArrayWithInts<bool>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, m_InitializationValue, m_ScalarType);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> void CreateDataArray::checkInitialization(QString dataArrayName)
{
  if(m_InitializationType == Manual)
  {
    bool ok;
    double input = m_InitializationValue.toDouble(&ok);
    if(ok == false)
    {
      QString ss = "Could not convert initialization value to a double.";
      setErrorCondition(-5559);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    if(input < static_cast<double>(std::numeric_limits<T>().lowest()) || input > static_cast<double>(std::numeric_limits<T>().max()))
    {
      setErrorCondition(-4000);
      QString ss = QObject::tr("%1: Invalid initialization value. The valid range is %2 to %3").arg(dataArrayName).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
  else if(m_InitializationType == RandomWithRange)
  {
    double min = m_InitializationRange.first;
    double max = m_InitializationRange.second;
    if(min > max)
    {
      QString ss = dataArrayName + ": Invalid initialization range.  Minimum value is larger than maximum value.";
      setErrorCondition(-5550);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    else if(min < static_cast<double>(std::numeric_limits<T>().lowest()) || max > static_cast<double>(std::numeric_limits<T>().max()))
    {
      setErrorCondition(-4001);
      QString ss = QObject::tr("%1: The initialization range can only be from %2 to %3").arg(dataArrayName).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    else if(min == max)
    {
      setErrorCondition(-4002);
      QString ss = dataArrayName + ": The initialization range must have differing values";
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateDataArray::newFilterInstance(bool copyFilterParameters)
{
  CreateDataArray::Pointer filter = CreateDataArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getGroupName()
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CreateDataArray::getUuid()
{
  return QUuid("{77f392fb-c1eb-57da-a1b1-e7acf9239fb8}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getSubGroupName()
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getHumanLabel()
{
  return "Create Data Array";
}

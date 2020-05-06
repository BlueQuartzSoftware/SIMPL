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

#include <memory>

#include "CreateDataArray.h"


#include <limits>
#include <random>
#include <chrono>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/ScalarTypeFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID = 1
};

#define CDA_CREATE_CONVERTOR(type, func)                                                                                                                                                               \
  struct From_##type                                                                                                                                                                                   \
  {                                                                                                                                                                                                    \
    type convert(const QString& input)                                                                                                                                                                 \
    {                                                                                                                                                                                                  \
      bool ok = false;                                                                                                                                                                                 \
      type value = static_cast<type>(input.func(&ok));                                                                                                                                                 \
      if(!ok)                                                                                                                                                                                          \
      {                                                                                                                                                                                                \
        return 0;                                                                                                                                                                                      \
      }                                                                                                                                                                                                \
      return value;                                                                                                                                                                                    \
    }                                                                                                                                                                                                  \
  };

CDA_CREATE_CONVERTOR(uint8_t, toUInt)
CDA_CREATE_CONVERTOR(int8_t, toInt)
CDA_CREATE_CONVERTOR(uint16_t, toUShort)
CDA_CREATE_CONVERTOR(int16_t, toShort)
CDA_CREATE_CONVERTOR(uint32_t, toUInt)
CDA_CREATE_CONVERTOR(int32_t, toInt)
CDA_CREATE_CONVERTOR(uint64_t, toULongLong)
CDA_CREATE_CONVERTOR(int64_t, toLongLong)
CDA_CREATE_CONVERTOR(bool, toInt)

CDA_CREATE_CONVERTOR(float, toDouble)
CDA_CREATE_CONVERTOR(double, toDouble)

#define CDA_SEP ";"
#define CDA_LB "["
#define CDA_RB "]"
/**
 *
 */
template <typename T, typename Conversion>
std::vector<T> createInitVector(const QString& initializationValue, int32_t numComponents)
{
  std::vector<T> values(numComponents);
  Conversion convert;
  if(initializationValue.startsWith("[") && initializationValue.endsWith("]"))
  {
    QString input = initializationValue;
    input = input.replace('[', "");
    input = input.replace(']', "");
    QStringList tokens = input.split(CDA_SEP, QString::SplitBehavior::KeepEmptyParts);
    if(tokens.size() == 1)
    {
      for(int32_t i = 0; i < numComponents; i++)
      {
        values[i] = convert.convert(tokens[0]);
      }
    }
    else
    {
      for(int32_t i = 0; i < numComponents; i++)
      {
        values[i] = convert.convert(tokens[i]);
      }
    }
  }
  else
  {
    T value = convert.convert(initializationValue);
    values.assign(numComponents, value);
  }
  return values;
}

/**
 * @brief initializeArrayWithInts Initializes the array p with integers, either from the
 * manual value entered in the filter, or with a random number.  This function does not
 * check that the template type actually is an integer, so it will most likely cause
 * unexpected results when passing anything other than an integer as a template parameter.
 * @param p The array that will be initialized
 */
template <typename T>
void initializeArrayWithInts(IDataArray::Pointer outputArrayPtr, int initializationType, FPRangePair initializationRange, const std::vector<T>& initValues)
{
  typename DataArray<T>::Pointer array = std::dynamic_pointer_cast<DataArray<T>>(outputArrayPtr);
  size_t numTuples = array->getNumberOfTuples();

  if(initializationType == CreateDataArray::Manual)
  {
    for(size_t t = 0; t < numTuples; t++)
    {
      array->setTuple(t, initValues);
    }
  }
  else
  {
    T rangeMin = static_cast<T>(initializationRange.first);
    T rangeMax = static_cast<T>(initializationRange.second);

    std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::mt19937_64 generator(seed); // Standard mersenne_twister_engine seeded with milliseconds
    std::uniform_int_distribution<> distribution(rangeMin, rangeMax);

    size_t count = array->size();
    T* rawPointer = array->getPointer(0);
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
void initializeArrayWithBool(IDataArray::Pointer outputArrayPtr, int initializationType, const std::vector<bool>& initValues)
{
  DataArray<bool>::Pointer array = std::dynamic_pointer_cast<DataArray<bool>>(outputArrayPtr);
  size_t numTuples = array->getNumberOfTuples();
  int32_t numComp = array->getNumberOfComponents();
  if(initializationType == CreateDataArray::Manual)
  {
    for(size_t t = 0; t < numTuples; t++)
    {
      for(int32_t c = 0; c < numComp; c++)
      {
        array->setComponent(t, c, initValues[c]);
      }
    }
  }
  else
  {
    std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::mt19937_64 generator(seed); // Standard mersenne_twister_engine seeded with milliseconds
    std::uniform_int_distribution<int32_t> distribution(0, 1);
    size_t count = array->getSize();
    bool* rawPointer = array->getPointer(0);

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
template <typename T>
void initializeArrayWithReals(IDataArray::Pointer outputArrayPtr, int initializationType, FPRangePair initializationRange, const std::vector<T>& initValues)
{
  typename DataArray<T>::Pointer array = std::dynamic_pointer_cast<DataArray<T>>(outputArrayPtr);
  size_t numTuples = array->getNumberOfTuples();

  if(initializationType == CreateDataArray::Manual)
  {
    for(size_t t = 0; t < numTuples; t++)
    {
      array->setTuple(t, initValues);
    }
  }
  else
  {
    size_t count = array->getSize();
    T* rawPointer = array->getPointer(0);
    T rangeMin = static_cast<T>(initializationRange.first);
    T rangeMax = static_cast<T>(initializationRange.second);

    std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::mt19937_64 generator(seed); // Standard mersenne_twister_engine seeded with milliseconds
    std::uniform_real_distribution<> distribution(rangeMin, rangeMax);

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
: m_ScalarType(SIMPL::ScalarTypes::Type::Int8)
, m_NumberOfComponents(0)
, m_NewArray("", "", "")
, m_InitializationType(Manual)
, m_InitializationValue("0")
{
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
  FilterParameterVectorType parameters;

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
  clearErrorCode();
  clearWarningCode();

  if(getErrorCode() < 0)
  {
    return;
  }

  if(m_InitializationType == RandomWithRange && m_InitializationRange.first > m_InitializationRange.second)
  {
    QString ss = "Invalid initialization range.  Minimum value is larger than maximum value.";
    setErrorCondition(-5550, ss);
    return;
  }

  if(getNumberOfComponents() < 0)
  {
    QString ss = QObject::tr("The number of components must non-negative");
    setErrorCondition(-8050, ss);
  }
  if(getNumberOfComponents() == 0)
  {
    QString ss = QObject::tr("The number of components is Zero. This will result in an array that has no memory allocated. Are you sure you wanted to do this?");
    setErrorCondition(-8051, ss);
  }
  if(!getNewArray().isValid())
  {
    QString ss = QObject::tr("The Created DataArrayPath is invalid. Please select the Data Container, Attribute Matrix and set an output DataArray name.");
    setErrorCondition(-8051, ss);
  }
  std::vector<size_t> cDims(1, getNumberOfComponents());
  if(getErrorCode() < 0)
  {
    return;
  }
  // Create the data array and initialize it to a placeholder value
  m_OutputArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromTypeEnum()(this, getNewArray(), cDims, static_cast<int>(getScalarType()), 0, DataArrayID);

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
void CreateDataArray::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(m_ScalarType == SIMPL::ScalarTypes::Type::Int8)
  {
    std::vector<int8_t> values = createInitVector<int8_t, From_int8_t>(m_InitializationValue, m_NumberOfComponents);
    initializeArrayWithInts<int8_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, values);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Int16)
  {
    std::vector<int16_t> values = createInitVector<int16_t, From_int16_t>(m_InitializationValue, m_NumberOfComponents);
    initializeArrayWithInts<int16_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, values);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Int32)
  {
    std::vector<int32_t> values = createInitVector<int32_t, From_int32_t>(m_InitializationValue, m_NumberOfComponents);
    initializeArrayWithInts<int32_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, values);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Int64)
  {
    std::vector<int64_t> values = createInitVector<int64_t, From_int64_t>(m_InitializationValue, m_NumberOfComponents);
    initializeArrayWithInts<int64_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, values);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt8)
  {
    std::vector<uint8_t> values = createInitVector<uint8_t, From_uint8_t>(m_InitializationValue, m_NumberOfComponents);
    initializeArrayWithInts<uint8_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, values);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt16)
  {
    std::vector<uint16_t> values = createInitVector<uint16_t, From_uint16_t>(m_InitializationValue, m_NumberOfComponents);
    initializeArrayWithInts<uint16_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, values);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt32)
  {
    std::vector<uint32_t> values = createInitVector<uint32_t, From_uint32_t>(m_InitializationValue, m_NumberOfComponents);
    initializeArrayWithInts<uint32_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, values);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::UInt64)
  {
    std::vector<uint64_t> values = createInitVector<uint64_t, From_uint64_t>(m_InitializationValue, m_NumberOfComponents);
    initializeArrayWithInts<uint64_t>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, values);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Float)
  {
    std::vector<float> values = createInitVector<float, From_float>(m_InitializationValue, m_NumberOfComponents);
    initializeArrayWithReals<float>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, values);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Double)
  {
    std::vector<double> values = createInitVector<double, From_double>(m_InitializationValue, m_NumberOfComponents);
    initializeArrayWithReals<double>(m_OutputArrayPtr.lock(), m_InitializationType, m_InitializationRange, values);
  }
  else if(m_ScalarType == SIMPL::ScalarTypes::Type::Bool)
  {
    std::vector<bool> values = createInitVector<bool, From_bool>(m_InitializationValue, m_NumberOfComponents);
    initializeArrayWithBool(m_OutputArrayPtr.lock(), m_InitializationType, values);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void CreateDataArray::checkInitialization(QString dataArrayName)
{
  if(m_InitializationType == Manual)
  {
    bool ok;

    // Check to see if we have multiple values
    bool startsWithSqBracket = m_InitializationValue.trimmed().startsWith("[");
    bool endsWithSqBracket = m_InitializationValue.trimmed().endsWith("]");
    // If only 1 bracket appears that is bad.
    if((!startsWithSqBracket && endsWithSqBracket) || (startsWithSqBracket && !endsWithSqBracket))
    {
      QString ss = QString("Init string is not valid. Starting or ending square brackets are missing.");
      setErrorCondition(-5458, ss);
      return;
    }

    // If there are no brackets then the string should be able to be converted to a number.
    // double is the easiest to sanity check because whether the value has a decimal point or not
    // it would convert to a double. If we tried an int and the string had a decimal point
    // the conversion would fail.
    if(!startsWithSqBracket && !endsWithSqBracket)
    {
      QString input = m_InitializationValue.trimmed();
      std::ignore = input.toDouble(&ok);
      if(!ok)
      {
        QString ss = QString("Init string cannot be converted to a number. If this is a multicomponent surround the string with square brackets [ ]");
        setErrorCondition(-5457, ss);
        return;
      }
    }

    // Just validate the number of components are valid and each token can be converted to a double
    if(startsWithSqBracket && endsWithSqBracket)
    {
      QString input = m_InitializationValue.trimmed();
      input = input.replace('[', "");
      input = input.replace(']', "");
      QStringList tokens = input.split(CDA_SEP, QString::SplitBehavior::KeepEmptyParts);
      if(m_NumberOfComponents != tokens.size() && tokens.size() != 1)
      {
        QString ss = QString("Number of initialization components does not match number of components.%1 vs %2").arg(tokens.size()).arg(m_NumberOfComponents);
        setErrorCondition(-5459, ss);
        return;
      }
      for(const auto& token : tokens)
      {
        std::ignore = token.toDouble(&ok);
        if(!ok)
        {
          QString ss = QString("Token cannot be converted to a number. %1").arg(token);
          setErrorCondition(-5452, ss);
          return;
        }
      }
    }

    if(m_ScalarType == SIMPL::ScalarTypes::Type::Double || m_ScalarType == SIMPL::ScalarTypes::Type::Float)
    {
      double input = m_InitializationValue.toDouble(&ok);
      if(!ok)
      {
        QString ss = "Could not convert initialization value to a double.";
        setErrorCondition(-5559, ss);
        return;
      }

      if(input < static_cast<double>(std::numeric_limits<T>().lowest()) || input > static_cast<double>(std::numeric_limits<T>().max()))
      {
        QString ss = QObject::tr("%1: Invalid initialization value. The valid range is %2 to %3").arg(dataArrayName).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
        setErrorCondition(-4000, ss);
        return;
      }
    }
  }
  else if(m_InitializationType == RandomWithRange)
  {
    double min = m_InitializationRange.first;
    double max = m_InitializationRange.second;
    if(min > max)
    {
      QString ss = dataArrayName + ": Invalid initialization range.  Minimum value is larger than maximum value.";
      setErrorCondition(-5550, ss);
      return;
    }
    if(min < static_cast<double>(std::numeric_limits<T>().lowest()) || max > static_cast<double>(std::numeric_limits<T>().max()))
    {
      QString ss = QObject::tr("%1: The initialization range can only be from %2 to %3").arg(dataArrayName).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
      setErrorCondition(-4001, ss);
      return;
    }
    if(min == max)
    {
      QString ss = dataArrayName + ": The initialization range must have differing values";
      setErrorCondition(-4002, ss);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateDataArray::newFilterInstance(bool copyFilterParameters) const
{
  CreateDataArray::Pointer filter = CreateDataArray::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateDataArray::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateDataArray::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateDataArray::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateDataArray::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid CreateDataArray::getUuid() const
{
  return QUuid("{77f392fb-c1eb-57da-a1b1-e7acf9239fb8}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateDataArray::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateDataArray::getHumanLabel() const
{
  return "Create Data Array";
}

// -----------------------------------------------------------------------------
CreateDataArray::Pointer CreateDataArray::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<CreateDataArray> CreateDataArray::New()
{
  struct make_shared_enabler : public CreateDataArray
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString CreateDataArray::getNameOfClass() const
{
  return QString("CreateDataArray");
}

// -----------------------------------------------------------------------------
QString CreateDataArray::ClassName()
{
  return QString("CreateDataArray");
}

// -----------------------------------------------------------------------------
void CreateDataArray::setScalarType(SIMPL::ScalarTypes::Type value)
{
  m_ScalarType = value;
}

// -----------------------------------------------------------------------------
SIMPL::ScalarTypes::Type CreateDataArray::getScalarType() const
{
  return m_ScalarType;
}

// -----------------------------------------------------------------------------
void CreateDataArray::setNumberOfComponents(int value)
{
  m_NumberOfComponents = value;
}

// -----------------------------------------------------------------------------
int CreateDataArray::getNumberOfComponents() const
{
  return m_NumberOfComponents;
}

// -----------------------------------------------------------------------------
void CreateDataArray::setNewArray(const DataArrayPath& value)
{
  m_NewArray = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateDataArray::getNewArray() const
{
  return m_NewArray;
}

// -----------------------------------------------------------------------------
void CreateDataArray::setInitializationType(int value)
{
  m_InitializationType = value;
}

// -----------------------------------------------------------------------------
int CreateDataArray::getInitializationType() const
{
  return m_InitializationType;
}

// -----------------------------------------------------------------------------
void CreateDataArray::setInitializationValue(const QString& value)
{
  m_InitializationValue = value;
}

// -----------------------------------------------------------------------------
QString CreateDataArray::getInitializationValue() const
{
  return m_InitializationValue;
}

// -----------------------------------------------------------------------------
void CreateDataArray::setInitializationRange(const FPRangePair& value)
{
  m_InitializationRange = value;
}

// -----------------------------------------------------------------------------
FPRangePair CreateDataArray::getInitializationRange() const
{
  return m_InitializationRange;
}

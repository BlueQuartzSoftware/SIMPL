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

#include "CreateDataArray.h"

#include <QtCore/QDateTime>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"

// Include the MOC generated file for this class
#include "moc_CreateDataArray.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateDataArray::CreateDataArray() :
  AbstractFilter(),
  m_ScalarType(0),
  m_NumberOfComponents(0),
  m_NewArray("", "", ""),
  m_InitializationType(Manual),
  m_InitializationValue("0")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateDataArray::~CreateDataArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Scalar Type");
    parameter->setPropertyName("ScalarType");

    QVector<QString> choices;
    choices.push_back(Int8);
    choices.push_back(UInt8);
    choices.push_back(Int16);
    choices.push_back(UInt16);
    choices.push_back(Int32);
    choices.push_back(UInt32);
    choices.push_back(Int64);
    choices.push_back(UInt64);
    choices.push_back(Float);
    choices.push_back(Double);
    choices.push_back(Bool);
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(IntFilterParameter::New("Number of Components", "NumberOfComponents", getNumberOfComponents(), FilterParameter::Parameter));

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Initialization Type");
    parameter->setPropertyName("InitializationType");

    parameter->setDefaultValue(Manual);

    QVector<QString> choices;
    choices.push_back("Manual");
    choices.push_back("Random With Range");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "InitializationValue" << "InitializationRange";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(StringFilterParameter::New("Initialization Value", "InitializationValue", getInitializationValue(), FilterParameter::Parameter, Manual));
  parameters.push_back(RangeFilterParameter::New("Initialization Range", "InitializationRange", getInitializationRange(), FilterParameter::Parameter, RandomWithRange));

  {
    DataArrayCreationFilterParameter::RequirementType req;
    parameters.push_back(DataArrayCreationFilterParameter::New("Created Attribute Array", "NewArray", getNewArray(), FilterParameter::CreatedArray, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setScalarType( reader->readValue("ScalarType", getScalarType()) );
  setNumberOfComponents( reader->readValue("NumberOfComponents", getNumberOfComponents()) );
  setNewArray(reader->readDataArrayPath("NewArray", getNewArray()));
  setInitializationValue(reader->readString("InitializationValue", getInitializationValue()));
  setInitializationType(reader->readValue("InitializationType", getInitializationType()));
  setInitializationRange(reader->readPairOfDoubles("InitializationRange", getInitializationRange()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CreateDataArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion);
  SIMPL_FILTER_WRITE_PARAMETER(ScalarType);
  SIMPL_FILTER_WRITE_PARAMETER(NumberOfComponents);
  SIMPL_FILTER_WRITE_PARAMETER(NewArray);
  SIMPL_FILTER_WRITE_PARAMETER(InitializationValue);
  SIMPL_FILTER_WRITE_PARAMETER(InitializationType);
  SIMPL_FILTER_WRITE_PARAMETER(InitializationRange);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::dataCheck()
{
  setErrorCondition(0);

  if (getErrorCondition() < 0) { return; }

  if (m_InitializationType == RandomWithRange && m_InitializationRange.first > m_InitializationRange.second)
  {
    QString ss = "Invalid initialization range.  Minimum value is larger than maximum value.";
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (getNumberOfComponents() < 0)
  {
    setErrorCondition(-8050);
    QString ss = QObject::tr("The number of components must non-negative");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  if(getNumberOfComponents() == 0)
  {
    setErrorCondition(0);
    QString ss = QObject::tr("The number of components is Zero. This will result in an array that has no memory allocated. Are you sure you wanted to do this?");
    notifyWarningMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> cDims(1, getNumberOfComponents());

  // Create the data array and initialize it to a placeholder value
  m_OutputArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromTypeEnum()(this, getNewArray(), cDims, getScalarType(), 0);

  QString dataArrayName = getNewArray().getDataArrayName();

  if (m_ScalarType == Int8Choice)
  {
    checkInitialization<int8_t>(dataArrayName); // check the initialization for that data type
  }
  else if (m_ScalarType == Int16Choice)
  {
    checkInitialization<int16_t>(dataArrayName); // check the initialization for that data type
  }
  else if (m_ScalarType == Int32Choice)
  {
    checkInitialization<int32_t>(dataArrayName); // check the initialization for that data type
  }
  else if (m_ScalarType == Int64Choice)
  {
    checkInitialization<int64_t>(dataArrayName); // check the initialization for that data type
  }
  else if (m_ScalarType == UInt8Choice)
  {
    checkInitialization<uint8_t>(dataArrayName); // check the initialization for that data type
  }
  else if (m_ScalarType == UInt16Choice)
  {
    checkInitialization<uint16_t>(dataArrayName); // check the initialization for that data type
  }
  else if (m_ScalarType == UInt32Choice)
  {
    checkInitialization<uint32_t>(dataArrayName); // check the initialization for that data type
  }
  else if (m_ScalarType == UInt64Choice)
  {
    checkInitialization<uint64_t>(dataArrayName); // check the initialization for that data type
  }
  else if (m_ScalarType == FloatChoice)
  {
    checkInitialization<float>(dataArrayName); // check the initialization for that data type
  }
  else if (m_ScalarType == DoubleChoice)
  {
    checkInitialization<double>(dataArrayName); // check the initialization for that data type
  }
  else if (m_ScalarType == BoolChoice)
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
  dataCheck();
  if(getErrorCondition() < 0)
  { return; }

  if (m_ScalarType == Int8Choice)
  {
    initializeArrayWithInts<int8_t>();
  }
  else if (m_ScalarType == Int16Choice)
  {
    initializeArrayWithInts<int16_t>();
  }
  else if (m_ScalarType == Int32Choice)
  {
    initializeArrayWithInts<int32_t>();
  }
  else if (m_ScalarType == Int64Choice)
  {
    initializeArrayWithInts<int64_t>();
  }
  else if (m_ScalarType == UInt8Choice)
  {
    initializeArrayWithInts<uint8_t>();
  }
  else if (m_ScalarType == UInt16Choice)
  {
    initializeArrayWithInts<uint16_t>();
  }
  else if (m_ScalarType == UInt32Choice)
  {
    initializeArrayWithInts<uint32_t>();
  }
  else if (m_ScalarType == UInt64Choice)
  {
    initializeArrayWithInts<uint64_t>();
  }
  else if (m_ScalarType == FloatChoice)
  {
    initializeArrayWithReals<float>();
  }
  else if (m_ScalarType == DoubleChoice)
  {
    initializeArrayWithReals<double>();
  }
  else if (m_ScalarType == BoolChoice)
  {
    initializeArrayWithInts<bool>();
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void CreateDataArray::initializeArrayWithInts()
{
  if (m_InitializationType == Manual)
  {

    qint8 i8 = static_cast<qint8>(m_InitializationValue.toInt());
    quint8 ui8 = static_cast<quint8>(m_InitializationValue.toUInt());
    qint16 i16 = static_cast<qint16>(m_InitializationValue.toShort());
    quint16 ui16 = static_cast<qint16>(m_InitializationValue.toUShort());
    qint32 i32 = static_cast<qint32>(m_InitializationValue.toInt());
    quint32 ui32 = static_cast<quint32>(m_InitializationValue.toUInt());
    qint64 i64 = static_cast<qint64>(m_InitializationValue.toLongLong());
    quint64 iu64 = static_cast<quint64>(m_InitializationValue.toULongLong());

    for (size_t i = 0; i < m_OutputArrayPtr.lock()->getNumberOfTuples(); i++)
    {
      switch (m_ScalarType) {
        case Int8Choice:
          m_OutputArrayPtr.lock()->initializeTuple(i, &i8);
          break;
        case UInt8Choice:
          m_OutputArrayPtr.lock()->initializeTuple(i, &ui8);
          break;
        case Int16Choice:
          m_OutputArrayPtr.lock()->initializeTuple(i, &i16);
          break;
        case UInt16Choice:
          m_OutputArrayPtr.lock()->initializeTuple(i, &ui16);
          break;
        case Int32Choice:
          m_OutputArrayPtr.lock()->initializeTuple(i, &i32);
          break;
        case UInt32Choice:
          m_OutputArrayPtr.lock()->initializeTuple(i, &ui32);
          break;
        case Int64Choice:
          m_OutputArrayPtr.lock()->initializeTuple(i, &i64);
          break;
        case UInt64Choice:
          m_OutputArrayPtr.lock()->initializeTuple(i, &iu64);
          break;
        default:
          Q_ASSERT_X(false, __FILE__, "Incorrent use of CreateDataArray::initializeArrayWithInts when using a non-integer type");
          break;
      }
    }
  }
  else
  {
    T rangeMin = static_cast<T>(m_InitializationRange.first);
    T rangeMax = static_cast<T>(m_InitializationRange.second);

    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::uniform_int<T> IntDistribution;
    typedef boost::variate_generator<RandomNumberGenerator&, IntDistribution> IntGenerator;

    std::shared_ptr<IntDistribution> distribution = std::shared_ptr<IntDistribution>(new IntDistribution(rangeMin, rangeMax));
    std::shared_ptr<RandomNumberGenerator> randomNumberGenerator = std::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator);
    randomNumberGenerator->seed(static_cast<size_t>(QDateTime::currentMSecsSinceEpoch())); // seed with the current time
    std::shared_ptr<IntGenerator> intGeneratorPtr = std::shared_ptr<IntGenerator>(new IntGenerator(*randomNumberGenerator, *distribution));
    IntGenerator& intGenerator = *intGeneratorPtr;

    for (size_t i = 0; i < m_OutputArrayPtr.lock()->getSize(); i++)
    {
      T value = intGenerator();
      m_OutputArrayPtr.lock()->initializeTuple(i, &value);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <>
void CreateDataArray::initializeArrayWithInts<bool>()
{
  if (m_InitializationType == Manual)
  {
    bool result;
    if (m_InitializationValue.toInt() == 0)
    {
      result = false;
    }
    else
    {
      result = true;
    }

    for (size_t i = 0; i < m_OutputArrayPtr.lock()->getSize(); i++)
    {
      m_OutputArrayPtr.lock()->initializeTuple(i, &result);
    }
  }
  else
  {
    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::uniform_int<int8_t> IntDistribution;
    typedef boost::variate_generator<RandomNumberGenerator&, IntDistribution> IntGenerator;

    std::shared_ptr<IntDistribution> distribution = std::shared_ptr<IntDistribution>(new IntDistribution(0, 1));
    std::shared_ptr<RandomNumberGenerator> randomNumberGenerator = std::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator);
    randomNumberGenerator->seed(static_cast<size_t>(QDateTime::currentMSecsSinceEpoch())); // seed with the current time
    std::shared_ptr<IntGenerator> intGeneratorPtr = std::shared_ptr<IntGenerator>(new IntGenerator(*randomNumberGenerator, *distribution));
    IntGenerator& intGenerator = *intGeneratorPtr;

    for (size_t i = 0; i < m_OutputArrayPtr.lock()->getSize(); i++)
    {
      int8_t result = intGenerator();
      if (result == 0)
      {
        m_OutputArrayPtr.lock()->initializeTuple(i, &result);
      }
      else
      {
        result = 1;
        m_OutputArrayPtr.lock()->initializeTuple(i, &result);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void CreateDataArray::initializeArrayWithReals()
{
  if (m_InitializationType == Manual)
  {
    double value = m_InitializationValue.toDouble();
    for (size_t i = 0; i < m_OutputArrayPtr.lock()->getSize(); i++)
    {
      m_OutputArrayPtr.lock()->initializeTuple(i, &value);
    }
  }
  else
  {
    T rangeMin = static_cast<T>(m_InitializationRange.first);
    T rangeMax = static_cast<T>(m_InitializationRange.second);

    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::uniform_real<T> RealDistribution;
    typedef boost::variate_generator<RandomNumberGenerator&, RealDistribution> RealGenerator;

    std::shared_ptr<RealDistribution> distribution = std::shared_ptr<RealDistribution>(new RealDistribution(rangeMin, rangeMax));
    std::shared_ptr<RandomNumberGenerator> randomNumberGenerator = std::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator);
    randomNumberGenerator->seed(static_cast<size_t>(QDateTime::currentMSecsSinceEpoch())); // seed with the current time
    std::shared_ptr<RealGenerator> realGeneratorPtr = std::shared_ptr<RealGenerator>(new RealGenerator(*randomNumberGenerator, *distribution));
    RealGenerator& realGenerator = *realGeneratorPtr;

    for (size_t i = 0; i < m_OutputArrayPtr.lock()->getSize(); i++)
    {
      double value = realGenerator();
      m_OutputArrayPtr.lock()->initializeTuple(i, &value);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void CreateDataArray::checkInitialization(QString dataArrayName)
{
  if (m_InitializationType == Manual)
  {
    bool ok;
    double input = m_InitializationValue.toDouble(&ok);
    if (ok == false)
    {
      QString ss = "Could not convert initialization value to a double.";
      setErrorCondition(-5559);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    if (input < static_cast<double>(std::numeric_limits<T>().lowest()) || input > static_cast<double>(std::numeric_limits<T>().max()))
    {
      setErrorCondition(-4000);
      QString ss = QObject::tr("%1: Invalid initialization value. The valid range is %2 to %3").arg(dataArrayName).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
  else if (m_InitializationType == RandomWithRange)
  {
    double min = m_InitializationRange.first;
    double max = m_InitializationRange.second;
    if (min > max)
    {
      QString ss = dataArrayName + ": Invalid initialization range.  Minimum value is larger than maximum value.";
      setErrorCondition(-5550);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    else if (min < static_cast<double>(std::numeric_limits<T>().lowest()) || max > static_cast<double>(std::numeric_limits<T>().max()))
    {
      setErrorCondition(-4001);
      QString ss = QObject::tr("%1: The initialization range can only be from %2 to %3").arg(dataArrayName).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    else if (min == max)
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
{ return Core::CoreBaseName; }

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
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getGroupName()
{ return SIMPL::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getSubGroupName()
{ return SIMPL::FilterSubGroups::GenerationFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getHumanLabel()
{ return "Create Data Array"; }

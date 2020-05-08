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
#include "InitializeData.h"

#include <random>
#include <chrono>

#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void delay(int seconds)
{
  QTime dieTime = QTime::currentTime().addSecs(seconds);
  while(QTime::currentTime() < dieTime)
  {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeData::InitializeData()
: m_XMin(0)
, m_YMin(0)
, m_ZMin(0)
, m_XMax(0)
, m_YMax(0)
, m_ZMax(0)
, m_InitType(Manual)
, m_InitValue(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeData::~InitializeData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    MultiDataArraySelectionFilterParameter::RequirementType req =
        MultiDataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Cell Arrays", CellAttributeMatrixPaths, FilterParameter::RequiredArray, InitializeData, req));
  }
  parameters.push_back(SIMPL_NEW_INTEGER_FP("X Min (Column)", XMin, FilterParameter::Parameter, InitializeData));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Y Min (Row)", YMin, FilterParameter::Parameter, InitializeData));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Z Min (Plane)", ZMin, FilterParameter::Parameter, InitializeData));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("X Max (Column)", XMax, FilterParameter::Parameter, InitializeData));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Y Max (Row)", YMax, FilterParameter::Parameter, InitializeData));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Z Max (Plane)", ZMax, FilterParameter::Parameter, InitializeData));

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Initialization Type");
    parameter->setPropertyName("InitType");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(InitializeData, this, InitType));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(InitializeData, this, InitType));

    parameter->setDefaultValue(Manual);

    QVector<QString> choices;
    choices.push_back("Manual");
    choices.push_back("Random");
    choices.push_back("Random With Range");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "InitValue"
                << "InitRange";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Initialization Value", InitValue, FilterParameter::Parameter, InitializeData, Manual));
  parameters.push_back(SIMPL_NEW_RANGE_FP("Initialization Range", InitRange, FilterParameter::Parameter, InitializeData, RandomWithRange));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellAttributeMatrixPaths(reader->readDataArrayPathVector("CellAttributeMatrixPaths", getCellAttributeMatrixPaths()));
  setXMin(reader->readValue("XMin", getXMin()));
  setYMin(reader->readValue("YMin", getYMin()));
  setZMin(reader->readValue("ZMin", getZMin()));
  setXMax(reader->readValue("XMax", getXMax()));
  setYMax(reader->readValue("YMax", getYMax()));
  setZMax(reader->readValue("ZMax", getZMax()));
  setInitType(reader->readValue("InitType", getInitType()));
  setInitValue(reader->readValue("InitValue", getInitValue()));
  setInitRange(reader->readPairOfDoubles("InitRange", getInitRange()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(m_CellAttributeMatrixPaths.empty())
  {
    QString ss = "At least one data array must be selected.";
    setErrorCondition(-5550, ss);
    return;
  }

  DataArrayPath attributeMatrixPath(m_CellAttributeMatrixPaths[0].getDataContainerName(), m_CellAttributeMatrixPaths[0].getAttributeMatrixName(), "");
  getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, attributeMatrixPath, -301);

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom>(this, attributeMatrixPath.getDataContainerName());
  if(nullptr == image.get())
  {
    return;
  }

  if(getXMax() < getXMin())
  {
    QString ss = QObject::tr("X Max (%1) less than X Min (%2)").arg(getXMax()).arg(getXMin());
    setErrorCondition(-5551, ss);
  }
  if(getYMax() < getYMin())
  {
    QString ss = QObject::tr("Y Max (%1) less than Y Min (%2)").arg(getYMax()).arg(getYMin());
    setErrorCondition(-5552, ss);
  }
  if(getZMax() < getZMin())
  {
    QString ss = QObject::tr("Z Max (%1) less than Z Min (%2)").arg(getZMax()).arg(getZMin());
    setErrorCondition(-5553, ss);
  }
  if(getXMin() < 0)
  {
    QString ss = QObject::tr("X Min (%1) less than 0").arg(getXMin());
    setErrorCondition(-5554, ss);
  }
  if(getYMin() < 0)
  {
    QString ss = QObject::tr("Y Min (%1) less than 0").arg(getYMin());
    setErrorCondition(-5555, ss);
  }
  if(getZMin() < 0)
  {
    QString ss = QObject::tr("Z Min (%1) less than 0").arg(getZMin());
    setErrorCondition(-5556, ss);
  }
  if(getXMax() > (static_cast<int64_t>(image->getXPoints()) - 1))
  {
    QString ss = QObject::tr("The X Max you entered of %1 is greater than your Max X Point of %2").arg(getXMax()).arg(static_cast<int64_t>(image->getXPoints()) - 1);
    setErrorCondition(-5557, ss);
  }
  if(getYMax() > (static_cast<int64_t>(image->getYPoints()) - 1))
  {
    QString ss = QObject::tr("The Y Max you entered of %1 is greater than your Max Y Point of %2").arg(getYMax()).arg(static_cast<int64_t>(image->getYPoints()) - 1);
    setErrorCondition(-5558, ss);
  }
  if(getZMax() > (static_cast<int64_t>(image->getZPoints()) - 1))
  {
    QString ss = QObject::tr("The Z Max you entered of %1) greater than your Max Z Point of %2").arg(getZMax()).arg(static_cast<int64_t>(image->getZPoints()) - 1);
    setErrorCondition(-5559, ss);
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(attributeMatrixPath.getDataContainerName());

  // SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  QString attrMatName = attributeMatrixPath.getAttributeMatrixName();
  QList<QString> voxelArrayNames = DataArrayPath::GetDataArrayNames(m_CellAttributeMatrixPaths);

  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);

    QString type = p->getTypeAsString();
    if(type == "int8_t")
    {
      checkInitialization<int8_t>(p);
    }
    else if(type == "int16_t")
    {
      checkInitialization<int16_t>(p);
    }
    else if(type == "int32_t")
    {
      checkInitialization<int32_t>(p);
    }
    else if(type == "int64_t")
    {
      checkInitialization<int64_t>(p);
    }
    else if(type == "uint8_t")
    {
      checkInitialization<uint8_t>(p);
    }
    else if(type == "uint16_t")
    {
      checkInitialization<uint16_t>(p);
    }
    else if(type == "uint32_t")
    {
      checkInitialization<uint32_t>(p);
    }
    else if(type == "uint64_t")
    {
      checkInitialization<uint64_t>(p);
    }
    else if(type == "float")
    {
      checkInitialization<float>(p);
    }
    else if(type == "double")
    {
      checkInitialization<double>(p);
    }

    if(getErrorCode() < 0)
    {
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void InitializeData::checkInitialization(IDataArray::Pointer p)
{
  QString arrayName = p->getName();

  if(m_InitType == Manual)
  {
    double input = m_InitValue;
    if(input < static_cast<double>(std::numeric_limits<T>().lowest()) || input > static_cast<double>(std::numeric_limits<T>().max()))
    {
      QString ss = QObject::tr("%1: The initialization value could not be converted. The valid range is %2 to %3").arg(arrayName).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
      setErrorCondition(-4000, ss);
      return;
    }
  }
  else if(m_InitType == RandomWithRange)
  {
    double min = m_InitRange.first;
    double max = m_InitRange.second;
    if(min > max)
    {
      QString ss = arrayName + ": Invalid initialization range.  Minimum value is larger than maximum value.";
      setErrorCondition(-5550, ss);
      return;
    }
    if(min < static_cast<double>(std::numeric_limits<T>().lowest()) || max > static_cast<double>(std::numeric_limits<T>().max()))
    {
      QString ss = QObject::tr("%1: The initialization range can only be from %2 to %3").arg(arrayName).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
      setErrorCondition(-4001, ss);
      return;
    }
    if(min == max)
    {
      QString ss = arrayName + ": The initialization range must have differing values";
      setErrorCondition(-4002, ss);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataArrayPath attributeMatrixPath(m_CellAttributeMatrixPaths[0].getDataContainerName(), m_CellAttributeMatrixPaths[0].getAttributeMatrixName(), "");
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(attributeMatrixPath.getDataContainerName());

  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]),
      static_cast<int64_t>(udims[1]),
      static_cast<int64_t>(udims[2]),
  };

  QString attrMatName = attributeMatrixPath.getAttributeMatrixName();
  QList<QString> voxelArrayNames = DataArrayPath::GetDataArrayNames(m_CellAttributeMatrixPaths);

  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);

    QString type = p->getTypeAsString();
    if(type == "int8_t")
    {
      initializeArrayWithInts<int8_t>(p, dims);
    }
    else if(type == "int16_t")
    {
      initializeArrayWithInts<int16_t>(p, dims);
    }
    else if(type == "int32_t")
    {
      initializeArrayWithInts<int32_t>(p, dims);
    }
    else if(type == "int64_t")
    {
      initializeArrayWithInts<int64_t>(p, dims);
    }
    else if(type == "uint8_t")
    {
      initializeArrayWithInts<uint8_t>(p, dims);
    }
    else if(type == "uint16_t")
    {
      initializeArrayWithInts<uint16_t>(p, dims);
    }
    else if(type == "uint32_t")
    {
      initializeArrayWithInts<uint32_t>(p, dims);
    }
    else if(type == "uint64_t")
    {
      initializeArrayWithInts<uint64_t>(p, dims);
    }
    else if(type == "float")
    {
      initializeArrayWithReals<float>(p, dims);
    }
    else if(type == "double")
    {
      initializeArrayWithReals<double>(p, dims);
    }

    delay(1); // Delay the execution by 1 second to avoid the exact same seedings for each array
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void InitializeData::initializeArrayWithInts(IDataArray::Pointer p, int64_t dims[3])
{
  T rangeMin;
  T rangeMax;
  if(m_InitType == RandomWithRange)
  {
    rangeMin = m_InitRange.first;
    rangeMax = m_InitRange.second;
  }
  else
  {
    rangeMin = std::numeric_limits<T>().min();
    rangeMax = std::numeric_limits<T>().max();
  }

  std::random_device randomDevice;           // Will be used to obtain a seed for the random number engine
  std::mt19937_64 generator(randomDevice()); // Standard mersenne_twister_engine seeded with rd()
  std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
  generator.seed(seed);
  std::uniform_int_distribution<> distribution(rangeMin, rangeMax);

  for(int32_t k = m_ZMin; k < m_ZMax + 1; k++)
  {
    for(int32_t j = m_YMin; j < m_YMax + 1; j++)
    {
      for(int32_t i = m_XMin; i < m_XMax + 1; i++)
      {
        size_t index = (k * dims[0] * dims[1]) + (j * dims[0]) + i;

        if(m_InitType == Manual)
        {
          T num = static_cast<T>(m_InitValue);
          p->initializeTuple(index, &num);
        }
        else
        {
          T temp = distribution(generator);
          p->initializeTuple(index, &temp);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void InitializeData::initializeArrayWithReals(IDataArray::Pointer p, int64_t dims[3])
{
  T rangeMin;
  T rangeMax;
  if(m_InitType == RandomWithRange)
  {
    rangeMin = static_cast<T>(m_InitRange.first);
    rangeMax = static_cast<T>(m_InitRange.second);
  }
  else
  {
    rangeMin = std::numeric_limits<T>().min();
    rangeMax = std::numeric_limits<T>().max();
  }

  std::random_device randomDevice;           // Will be used to obtain a seed for the random number engine
  std::mt19937_64 generator(randomDevice()); // Standard mersenne_twister_engine seeded with rd()
  std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
  generator.seed(seed);
  std::uniform_real_distribution<T> distribution(rangeMin, rangeMax);

  for(int32_t k = m_ZMin; k < m_ZMax + 1; k++)
  {
    for(int32_t j = m_YMin; j < m_YMax + 1; j++)
    {
      for(int32_t i = m_XMin; i < m_XMax + 1; i++)
      {
        size_t index = (k * dims[0] * dims[1]) + (j * dims[0]) + i;

        if(m_InitType == Manual)
        {
          T num = static_cast<T>(m_InitValue);
          p->initializeTuple(index, &num);
        }
        else
        {
          T temp = distribution(generator);
          p->initializeTuple(index, &temp);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer InitializeData::newFilterInstance(bool copyFilterParameters) const
{
  InitializeData::Pointer filter = InitializeData::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InitializeData::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InitializeData::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InitializeData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InitializeData::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid InitializeData::getUuid() const
{
  return QUuid("{dfab9921-fea3-521c-99ba-48db98e43ff8}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InitializeData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InitializeData::getHumanLabel() const
{
  return "Initialize Data";
}

// -----------------------------------------------------------------------------
InitializeData::Pointer InitializeData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<InitializeData> InitializeData::New()
{
  struct make_shared_enabler : public InitializeData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString InitializeData::getNameOfClass() const
{
  return QString("InitializeData");
}

// -----------------------------------------------------------------------------
QString InitializeData::ClassName()
{
  return QString("InitializeData");
}

// -----------------------------------------------------------------------------
void InitializeData::setCellAttributeMatrixPaths(const QVector<DataArrayPath>& value)
{
  m_CellAttributeMatrixPaths = value;
}

// -----------------------------------------------------------------------------
QVector<DataArrayPath> InitializeData::getCellAttributeMatrixPaths() const
{
  return m_CellAttributeMatrixPaths;
}

// -----------------------------------------------------------------------------
void InitializeData::setXMin(int value)
{
  m_XMin = value;
}

// -----------------------------------------------------------------------------
int InitializeData::getXMin() const
{
  return m_XMin;
}

// -----------------------------------------------------------------------------
void InitializeData::setYMin(int value)
{
  m_YMin = value;
}

// -----------------------------------------------------------------------------
int InitializeData::getYMin() const
{
  return m_YMin;
}

// -----------------------------------------------------------------------------
void InitializeData::setZMin(int value)
{
  m_ZMin = value;
}

// -----------------------------------------------------------------------------
int InitializeData::getZMin() const
{
  return m_ZMin;
}

// -----------------------------------------------------------------------------
void InitializeData::setXMax(int value)
{
  m_XMax = value;
}

// -----------------------------------------------------------------------------
int InitializeData::getXMax() const
{
  return m_XMax;
}

// -----------------------------------------------------------------------------
void InitializeData::setYMax(int value)
{
  m_YMax = value;
}

// -----------------------------------------------------------------------------
int InitializeData::getYMax() const
{
  return m_YMax;
}

// -----------------------------------------------------------------------------
void InitializeData::setZMax(int value)
{
  m_ZMax = value;
}

// -----------------------------------------------------------------------------
int InitializeData::getZMax() const
{
  return m_ZMax;
}

// -----------------------------------------------------------------------------
void InitializeData::setInitType(int value)
{
  m_InitType = value;
}

// -----------------------------------------------------------------------------
int InitializeData::getInitType() const
{
  return m_InitType;
}

// -----------------------------------------------------------------------------
void InitializeData::setRandom(bool value)
{
  m_Random = value;
}

// -----------------------------------------------------------------------------
bool InitializeData::getRandom() const
{
  return m_Random;
}

// -----------------------------------------------------------------------------
void InitializeData::setInitValue(double value)
{
  m_InitValue = value;
}

// -----------------------------------------------------------------------------
double InitializeData::getInitValue() const
{
  return m_InitValue;
}

// -----------------------------------------------------------------------------
void InitializeData::setInitRange(const FPRangePair& value)
{
  m_InitRange = value;
}

// -----------------------------------------------------------------------------
FPRangePair InitializeData::getInitRange() const
{
  return m_InitRange;
}

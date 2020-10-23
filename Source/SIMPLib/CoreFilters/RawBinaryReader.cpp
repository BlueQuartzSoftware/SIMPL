/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "RawBinaryReader.h"

#include <algorithm>
#include <cstddef>

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/ScopedFileMonitor.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/NumericTypeFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/UInt64FilterParameter.h"

#if defined(_MSC_VER)
#define FSEEK _fseeki64
#else
#define FSEEK std::fseek
#endif

namespace
{
#ifdef CMP_WORDS_BIGENDIAN
constexpr int32_t k_EndianCheck = 0;
#else
constexpr int32_t k_EndianCheck = 1;
#endif

constexpr int32_t RBR_NO_ERROR = 0;
constexpr int32_t RBR_FILENAME_EMPTY = -387;
constexpr int32_t RBR_FILE_NOT_EXIST = -388;
constexpr int32_t RBR_NUM_COMPONENTS_ERROR = -391;
constexpr int32_t RBR_FILE_NOT_OPEN = -1000;
constexpr int32_t RBR_FILE_TOO_SMALL = -1010;
constexpr int32_t RBR_FILE_TOO_BIG = -1020;
constexpr int32_t RBR_READ_EOF = -1030;
constexpr int32_t RBR_DCA_ERROR = -1040;
constexpr int32_t RBR_DA_ERROR = -1050;
constexpr int32_t RBR_COMPONENT_ERROR = -1060;
constexpr int32_t RBR_DA_NULL = -1070;

// -----------------------------------------------------------------------------
int32_t SanityCheckFileSizeVersusAllocatedSize(size_t allocatedBytes, size_t fileSize, size_t skipHeaderBytes)
{
  if(fileSize - skipHeaderBytes < allocatedBytes)
  {
    return -1;
  }
  if(fileSize - skipHeaderBytes > allocatedBytes)
  {
    return 1;
  }
  // File Size and Allocated Size are equal so we are good to go
  return 0;
}

// -----------------------------------------------------------------------------
template <typename T>
int32_t readBinaryFile(IDataArray* dataArrayPtr, const std::string& filename, uint64_t skipHeaderBytes, int32_t endian)
{
  auto dataArray = dynamic_cast<DataArray<T>*>(dataArrayPtr);

  if(dataArray == nullptr)
  {
    return RBR_DA_NULL;
  }

  const size_t fileSize = fs::file_size(filename);
  const size_t numBytesToRead = dataArray->getSize() * sizeof(T);
  int32_t err = SanityCheckFileSizeVersusAllocatedSize(numBytesToRead, fileSize, skipHeaderBytes);

  if(err < 0)
  {
    return RBR_FILE_TOO_SMALL;
  }

  FILE* f = std::fopen(filename.c_str(), "rb");
  if(f == nullptr)
  {
    return RBR_FILE_NOT_OPEN;
  }

  ScopedFileMonitor monitor(f);

  // Skip some header bytes if the user asked for it.
  if(skipHeaderBytes > 0)
  {
    FSEEK(f, skipHeaderBytes, SEEK_SET);
  }

  std::byte* chunkptr = reinterpret_cast<std::byte*>(dataArray->data());

  // Now start reading the data in chunks if needed.
  size_t chunkSize = std::min(numBytesToRead, SIMPL::DEFAULT_BLOCKSIZE);

  size_t master_counter = 0;
  while(master_counter < numBytesToRead)
  {
    size_t bytes_read = std::fread(chunkptr, sizeof(std::byte), chunkSize, f);
    chunkptr += bytes_read;
    master_counter += bytes_read;

    size_t bytesLeft = numBytesToRead - master_counter;

    if(bytesLeft < chunkSize)
    {
      chunkSize = bytesLeft;
    }
  }

  if(endian == k_EndianCheck)
  {
    dataArray->byteSwapElements();
  }

  return RBR_NO_ERROR;
}
} // namespace

// -----------------------------------------------------------------------------
RawBinaryReader::RawBinaryReader()
: m_CreatedAttributeArrayPath("")
, m_ScalarType(SIMPL::NumericTypes::Type::Int8)
, m_Endian(0)
, m_NumberOfComponents(0)
, m_SkipHeaderBytes(0)
, m_InputFile("")
{
}

// -----------------------------------------------------------------------------
RawBinaryReader::~RawBinaryReader() = default;

// -----------------------------------------------------------------------------
void RawBinaryReader::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, RawBinaryReader, "*.raw *.bin"));
  parameters.push_back(SIMPL_NEW_NUMERICTYPE_FP("Scalar Type", ScalarType, FilterParameter::Parameter, RawBinaryReader));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Components", NumberOfComponents, FilterParameter::Parameter, RawBinaryReader));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Endian");
    parameter->setPropertyName("Endian");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(RawBinaryReader, this, Endian));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(RawBinaryReader, this, Endian));

    QVector<QString> choices;
    choices.push_back("Little");
    choices.push_back("Big");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_UINT64_FP("Skip Header Bytes", SkipHeaderBytes, FilterParameter::Parameter, RawBinaryReader));
  {
    DataArrayCreationFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Output Attribute Array", CreatedAttributeArrayPath, FilterParameter::CreatedArray, RawBinaryReader, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void RawBinaryReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCreatedAttributeArrayPath(reader->readDataArrayPath("CreatedAttributeArrayPath", getCreatedAttributeArrayPath()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  setScalarType(static_cast<SIMPL::NumericTypes::Type>(reader->readValue("ScalarType", static_cast<int>(getScalarType()))));
  setNumberOfComponents(reader->readValue("NumberOfComponents", getNumberOfComponents()));
  setEndian(reader->readValue("Endian", getEndian()));
  setSkipHeaderBytes(reader->readValue("SkipHeaderBytes", getSkipHeaderBytes()));

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
void RawBinaryReader::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  auto dca = getDataContainerArray();

  const std::string inputFile = m_InputFile.toStdString();

  if(inputFile.empty())
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(RBR_FILENAME_EMPTY, ss);
  }
  else if(!fs::exists(inputFile))
  {
    QString ss = QObject::tr("The input file does not exist: %1").arg(getInputFile());
    setErrorCondition(RBR_FILE_NOT_EXIST, ss);
  }

  if(m_NumberOfComponents < 1)
  {
    QString ss = QObject::tr("The number of components must be positive");
    setErrorCondition(RBR_NUM_COMPONENTS_ERROR, ss);
  }

  AttributeMatrix::Pointer attrMat = dca->getPrereqAttributeMatrixFromPath(this, getCreatedAttributeArrayPath(), -30003);
  if(getErrorCode() < 0)
  {
    return;
  }

  const std::vector<size_t> tDims = attrMat->getTupleDimensions();
  const size_t totalDim = std::accumulate(tDims.cbegin(), tDims.cend(), static_cast<size_t>(1), std::multiplies<size_t>());
  const size_t totalSize = m_NumberOfComponents * totalDim;

  size_t allocatedBytes = 0;
  std::vector<size_t> cDims = {static_cast<size_t>(m_NumberOfComponents)};
  if(m_ScalarType == SIMPL::NumericTypes::Type::Int8)
  {
    dca->createNonPrereqArrayFromPath<Int8ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(int8_t) * totalSize;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt8)
  {
    dca->createNonPrereqArrayFromPath<UInt8ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(uint8_t) * totalSize;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int16)
  {
    dca->createNonPrereqArrayFromPath<Int16ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(int16_t) * totalSize;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt16)
  {
    dca->createNonPrereqArrayFromPath<UInt16ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(uint16_t) * totalSize;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int32)
  {
    dca->createNonPrereqArrayFromPath<Int32ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(int32_t) * totalSize;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt32)
  {
    dca->createNonPrereqArrayFromPath<UInt32ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(uint32_t) * totalSize;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int64)
  {
    dca->createNonPrereqArrayFromPath<Int64ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(int64_t) * totalSize;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt64)
  {
    dca->createNonPrereqArrayFromPath<UInt64ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(uint64_t) * totalSize;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Float)
  {
    dca->createNonPrereqArrayFromPath<FloatArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(float) * totalSize;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Double)
  {
    dca->createNonPrereqArrayFromPath<DoubleArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(double) * totalSize;
  }

  // Sanity Check Allocated Bytes versus size of file
  const uint64_t fileSize = fs::file_size(inputFile);
  const int32_t check = SanityCheckFileSizeVersusAllocatedSize(allocatedBytes, fileSize, m_SkipHeaderBytes);
  if(check == -1)
  {

    QString ss = QObject::tr("The file size is %1 but the number of bytes needed to fill the array is %2. This condition would cause an error reading the input file."
                             " Please adjust the input parameters to match the size of the file or select a different data file")
                     .arg(fileSize)
                     .arg(allocatedBytes);
    setErrorCondition(RBR_FILE_TOO_SMALL, ss);
  }
  else if(check == 1)
  {

    QString ss = QObject::tr("The file size is %1 but the number of bytes needed to fill the array is %2 which is less than the size of the file."
                             " SIMPLView will read only the first part of the file into the array")
                     .arg(fileSize)
                     .arg(allocatedBytes);
    setWarningCondition(RBR_FILE_TOO_BIG, ss);
  }
}

// -----------------------------------------------------------------------------
void RawBinaryReader::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  auto dca = getDataContainerArray();

  if(dca == nullptr)
  {
    setErrorCondition(RBR_DCA_ERROR, "Failed to acquire DataContainerArray");
    return;
  }

  const std::vector<size_t> cDims = {static_cast<size_t>(m_NumberOfComponents)};

  auto dataArray = dca->getPrereqIDataArrayFromPath(this, getCreatedAttributeArrayPath());

  if(dataArray == nullptr)
  {
    setErrorCondition(RBR_DA_ERROR, "Failed to acquire DataArray");
    return;
  }

  if(dataArray->getComponentDimensions() != cDims)
  {
    setErrorCondition(RBR_COMPONENT_ERROR, "Failed to acquire DataArray with the correct dimensions");
    return;
  }

  const std::string inputFile = m_InputFile.toStdString();

  int32_t err = 0;
  switch(m_ScalarType)
  {
  case SIMPL::NumericTypes::Type::Int8:
    err = readBinaryFile<int8_t>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::UInt8:
    err = readBinaryFile<uint8_t>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::Int16:
    err = readBinaryFile<int16_t>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::UInt16:
    err = readBinaryFile<uint16_t>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::Int32:
    err = readBinaryFile<int32_t>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::UInt32:
    err = readBinaryFile<uint32_t>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::Int64:
    err = readBinaryFile<int64_t>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::UInt64:
    err = readBinaryFile<uint64_t>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::Float:
    err = readBinaryFile<float>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::Double:
    err = readBinaryFile<double>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::Bool:
    err = readBinaryFile<uint8_t>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::SizeT:
    err = readBinaryFile<size_t>(dataArray.get(), inputFile, m_SkipHeaderBytes, m_Endian);
    break;
  case SIMPL::NumericTypes::Type::UnknownNumType:
    break;
  }

  if(err == RBR_FILE_NOT_OPEN)
  {
    setErrorCondition(RBR_FILE_NOT_OPEN, "Unable to open the specified file");
  }
  else if(err == RBR_FILE_TOO_SMALL)
  {
    setErrorCondition(RBR_FILE_TOO_SMALL, "The file size is smaller than the allocated size");
  }
  else if(err == RBR_FILE_TOO_BIG)
  {
    setWarningCondition(RBR_FILE_TOO_BIG, "The file size is larger than the allocated size");
  }
  else if(err == RBR_READ_EOF)
  {
    setErrorCondition(RBR_READ_EOF, "RawBinaryReader read past the end of the specified file");
  }
  else if(err == RBR_DA_NULL)
  {
    setErrorCondition(RBR_DA_NULL, "Failed DataArray cast");
  }
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer RawBinaryReader::newFilterInstance(bool copyFilterParameters) const
{
  RawBinaryReader::Pointer filter = RawBinaryReader::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString RawBinaryReader::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
QString RawBinaryReader::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
QString RawBinaryReader::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString RawBinaryReader::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
QUuid RawBinaryReader::getUuid() const
{
  return QUuid("{0791f556-3d73-5b1e-b275-db3f7bb6850d}");
}

// -----------------------------------------------------------------------------
QString RawBinaryReader::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
QString RawBinaryReader::getHumanLabel() const
{
  return "Raw Binary Importer";
}

// -----------------------------------------------------------------------------
RawBinaryReader::Pointer RawBinaryReader::NullPointer()
{
  return nullptr;
}

// -----------------------------------------------------------------------------
RawBinaryReader::Pointer RawBinaryReader::New()
{
  struct make_shared_enabler : public RawBinaryReader
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RawBinaryReader::getNameOfClass() const
{
  return ClassName();
}

// -----------------------------------------------------------------------------
QString RawBinaryReader::ClassName()
{
  return "RawBinaryReader";
}

// -----------------------------------------------------------------------------
void RawBinaryReader::setCreatedAttributeArrayPath(const DataArrayPath& value)
{
  m_CreatedAttributeArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RawBinaryReader::getCreatedAttributeArrayPath() const
{
  return m_CreatedAttributeArrayPath;
}

// -----------------------------------------------------------------------------
void RawBinaryReader::setScalarType(SIMPL::NumericTypes::Type value)
{
  m_ScalarType = value;
}

// -----------------------------------------------------------------------------
SIMPL::NumericTypes::Type RawBinaryReader::getScalarType() const
{
  return m_ScalarType;
}

// -----------------------------------------------------------------------------
void RawBinaryReader::setEndian(int32_t value)
{
  m_Endian = value;
}

// -----------------------------------------------------------------------------
int32_t RawBinaryReader::getEndian() const
{
  return m_Endian;
}

// -----------------------------------------------------------------------------
void RawBinaryReader::setNumberOfComponents(int32_t value)
{
  m_NumberOfComponents = value;
}

// -----------------------------------------------------------------------------
int32_t RawBinaryReader::getNumberOfComponents() const
{
  return m_NumberOfComponents;
}

// -----------------------------------------------------------------------------
void RawBinaryReader::setSkipHeaderBytes(uint64_t value)
{
  m_SkipHeaderBytes = value;
}

// -----------------------------------------------------------------------------
uint64_t RawBinaryReader::getSkipHeaderBytes() const
{
  return m_SkipHeaderBytes;
}

// -----------------------------------------------------------------------------
void RawBinaryReader::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString RawBinaryReader::getInputFile() const
{
  return m_InputFile;
}

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

#include "RawBinaryReader.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/ScopedFileMonitor.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/SIMPLibVersion.h"

#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/NumericTypeFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#define RBR_FILE_NOT_OPEN -1000
#define RBR_FILE_TOO_SMALL -1010
#define RBR_FILE_TOO_BIG -1020
#define RBR_READ_EOF -1030
#define RBR_NO_ERROR 0


#ifdef CMP_WORDS_BIGENDIAN
#define SWAP_ARRAY(array)                                                                                                                                                                              \
  if(m_Endian == 0)                                                                                                                                                                                    \
  {                                                                                                                                                                                                    \
    array->byteSwapElements();                                                                                                                                                                         \
  }

#else
#define SWAP_ARRAY(array)                                                                                                                                                                              \
  if(m_Endian == 1)                                                                                                                                                                                    \
  {                                                                                                                                                                                                    \
    array->byteSwapElements();                                                                                                                                                                         \
  }

#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t SanityCheckFileSizeVersusAllocatedSize(size_t allocatedBytes, size_t fileSize, int skipHeaderBytes)
{
  if(fileSize - skipHeaderBytes < allocatedBytes)
  {
    return -1;
  }
  else if(fileSize - skipHeaderBytes > allocatedBytes)
  {
    return 1;
  }
  // File Size and Allocated Size are equal so we  are good to go
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> int32_t readBinaryFile(typename DataArray<T>::Pointer p, const QString& filename, int32_t skipHeaderBytes)
{
  int32_t err = 0;
  QFileInfo fi(filename);
  uint64_t fileSize = static_cast<size_t>(fi.size());
  size_t allocatedBytes = p->getSize() * sizeof(T);
  err = SanityCheckFileSizeVersusAllocatedSize(allocatedBytes, fileSize, skipHeaderBytes);

  if(err < 0)
  {
    return RBR_FILE_TOO_SMALL;
  }

  FILE* f = fopen(filename.toLatin1().data(), "rb");
  if(nullptr == f)
  {
    return RBR_FILE_NOT_OPEN;
  }

  ScopedFileMonitor monitor(f);
  size_t numBytesToRead = p->getNumberOfTuples() * static_cast<size_t>(p->getNumberOfComponents()) * sizeof(T);
  size_t numRead = 0;

  uint8_t* chunkptr = reinterpret_cast<uint8_t*>(p->getPointer(0));

  // Skip some header bytes by just reading those bytes into the pointer knowing that the next
  // thing we are going to do it over write those bytes with the real data that we are after.
  if(skipHeaderBytes > 0)
  {
    numRead = fread(chunkptr, 1, static_cast<size_t>(skipHeaderBytes), f);
  }
  numRead = 0;
  // Now start reading the data in chunks if needed.
  size_t chunkSize = DEFAULT_BLOCKSIZE;

  if(numBytesToRead < DEFAULT_BLOCKSIZE)
  {
    chunkSize = numBytesToRead;
  }

  size_t master_counter = 0;
  size_t bytes_read = 0;
  while(1)
  {
    bytes_read = fread(chunkptr, sizeof(uint8_t), chunkSize, f);
    chunkptr = chunkptr + bytes_read;
    master_counter += bytes_read;

    if(numBytesToRead - master_counter < chunkSize)
    {
      chunkSize = numBytesToRead - master_counter;
    }
    if(master_counter >= numBytesToRead)
    {
      break;
    }
  }

  return RBR_NO_ERROR;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RawBinaryReader::RawBinaryReader()
: AbstractFilter()
, m_CreatedAttributeArrayPath("")
, m_ScalarType(SIMPL::NumericTypes::Type::Int8)
, m_Endian(0)
, m_NumberOfComponents(0)
, m_SkipHeaderBytes(0)
, m_InputFile("")
{

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RawBinaryReader::~RawBinaryReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::setupFilterParameters()
{
  FilterParameterVector parameters;

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
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Skip Header Bytes", SkipHeaderBytes, FilterParameter::Parameter, RawBinaryReader));
  {
    DataArrayCreationFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Output Attribute Array", CreatedAttributeArrayPath, FilterParameter::CreatedArray, RawBinaryReader, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
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
//
// -----------------------------------------------------------------------------
void RawBinaryReader::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  QFileInfo fi(getInputFile());
  if(getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_NumberOfComponents < 1)
  {
    QString ss = QObject::tr("The number of components must be positive");
    setErrorCondition(-391);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  AttributeMatrix::Pointer attrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCreatedAttributeArrayPath(), -30003);
  if(getErrorCondition() < 0)
  {
    return;
  }

  QVector<size_t> tDims = attrMat->getTupleDimensions();
  size_t totalDim = 1;
  for(int i = 0; i < tDims.size(); i++)
  {
    totalDim = totalDim * tDims[i];
  }

  size_t allocatedBytes = 0;
  QVector<size_t> cDims(1, m_NumberOfComponents);
  if(m_ScalarType == SIMPL::NumericTypes::Type::Int8)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int8ArrayType, AbstractFilter, int8_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(int8_t) * m_NumberOfComponents * totalDim;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt8)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt8ArrayType, AbstractFilter, uint8_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(uint8_t) * m_NumberOfComponents * totalDim;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int16)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int16ArrayType, AbstractFilter, int16_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(int16_t) * m_NumberOfComponents * totalDim;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt16)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt16ArrayType, AbstractFilter, uint16_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(uint16_t) * m_NumberOfComponents * totalDim;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int32)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType, AbstractFilter, int32_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(int32_t) * m_NumberOfComponents * totalDim;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt32)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType, AbstractFilter, uint32_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(uint32_t) * m_NumberOfComponents * totalDim;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int64)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int64ArrayType, AbstractFilter, int64_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(int64_t) * m_NumberOfComponents * totalDim;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt64)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt64ArrayType, AbstractFilter, uint64_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(uint64_t) * m_NumberOfComponents * totalDim;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Float)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter, float>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(float) * m_NumberOfComponents * totalDim;
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Double)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType, AbstractFilter, double>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
    allocatedBytes = sizeof(double) * m_NumberOfComponents * totalDim;
  }

  // Sanity Check Allocated Bytes versus size of file
  uint64_t fileSize = fi.size();
  int32_t check = SanityCheckFileSizeVersusAllocatedSize(allocatedBytes, fileSize, m_SkipHeaderBytes);
  if(check == -1)
  {

    QString ss = QObject::tr("The file size is %1 but the number of bytes needed to fill the array is %2. This condition would cause an error reading the input file."
                             " Please adjust the input parameters to match the size of the file or select a different data file")
                     .arg(fileSize)
                     .arg(allocatedBytes);
    setErrorCondition(RBR_FILE_TOO_SMALL);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(check == 1)
  {

    QString ss = QObject::tr("The file size is %1 but the number of bytes needed to fill the array is %2 which is less than the size of the file."
                             " SIMPLView will read only the first part of the file into the array")
                     .arg(fileSize)
                     .arg(allocatedBytes);
    setWarningCondition(RBR_FILE_TOO_BIG);
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::preflight()
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
void RawBinaryReader::execute()
{
  int32_t err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCreatedAttributeArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, m_NumberOfComponents);
  if(m_ScalarType == SIMPL::NumericTypes::Type::Int8)
  {
    Int8ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<Int8ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readBinaryFile<int8_t>(p, m_InputFile, m_SkipHeaderBytes);
    if(err >= 0)
    {
      SWAP_ARRAY(p)
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt8)
  {
    UInt8ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readBinaryFile<uint8_t>(p, m_InputFile, m_SkipHeaderBytes);
    if(err >= 0)
    {
      SWAP_ARRAY(p)
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int16)
  {
    Int16ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<Int16ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readBinaryFile<int16_t>(p, m_InputFile, m_SkipHeaderBytes);
    if(err >= 0)
    {
      SWAP_ARRAY(p)
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt16)
  {
    UInt16ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<UInt16ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readBinaryFile<uint16_t>(p, m_InputFile, m_SkipHeaderBytes);
    if(err >= 0)
    {
      SWAP_ARRAY(p)
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int32)
  {
    Int32ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readBinaryFile<int32_t>(p, m_InputFile, m_SkipHeaderBytes);
    if(err >= 0)
    {
      SWAP_ARRAY(p)
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt32)
  {
    UInt32ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<UInt32ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readBinaryFile<uint32_t>(p, m_InputFile, m_SkipHeaderBytes);
    if(err >= 0)
    {
      SWAP_ARRAY(p)
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int64)
  {
    Int64ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<Int64ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readBinaryFile<int64_t>(p, m_InputFile, m_SkipHeaderBytes);
    if(err >= 0)
    {
      SWAP_ARRAY(p)
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt64)
  {
    UInt64ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<UInt64ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readBinaryFile<uint64_t>(p, m_InputFile, m_SkipHeaderBytes);
    if(err >= 0)
    {
      SWAP_ARRAY(p)
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Float)
  {
    FloatArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readBinaryFile<float>(p, m_InputFile, m_SkipHeaderBytes);
    if(err >= 0)
    {
      SWAP_ARRAY(p)
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Double)
  {
    DoubleArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readBinaryFile<double>(p, m_InputFile, m_SkipHeaderBytes);
    if(err >= 0)
    {
      SWAP_ARRAY(p)
      m_Array = p;
    }
  }

  if(err == RBR_FILE_NOT_OPEN)
  {
    setErrorCondition(RBR_FILE_NOT_OPEN);
    notifyErrorMessage(getHumanLabel(), "Unable to open the specified file", getErrorCondition());
  }
  else if(err == RBR_FILE_TOO_SMALL)
  {
    setErrorCondition(RBR_FILE_TOO_SMALL);
    notifyErrorMessage(getHumanLabel(), "The file size is smaller than the allocated size", getErrorCondition());
  }
  else if(err == RBR_FILE_TOO_BIG)
  {
    setWarningCondition(RBR_FILE_TOO_BIG);
    notifyWarningMessage(getHumanLabel(), "The file size is larger than the allocated size", getWarningCondition());
  }
  else if(err == RBR_READ_EOF)
  {
    setErrorCondition(RBR_READ_EOF);
    notifyErrorMessage(getHumanLabel(), "RawBinaryReader read past the end of the specified file", getErrorCondition());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RawBinaryReader::newFilterInstance(bool copyFilterParameters)
{
  RawBinaryReader::Pointer filter = RawBinaryReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryReader::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryReader::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryReader::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryReader::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid RawBinaryReader::getUuid()
{
  return QUuid("{0791f556-3d73-5b1e-b275-db3f7bb6850d}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryReader::getSubGroupName()
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryReader::getHumanLabel()
{
  return "Raw Binary Importer";
}

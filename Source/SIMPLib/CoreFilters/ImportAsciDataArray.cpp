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
#include "ImportAsciDataArray.h"

#include <locale>
#include <fstream>

#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/ScopedFileMonitor.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/NumericTypeFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AsciiArrayID = 1
};

#define RBR_FILE_NOT_OPEN -1000
#define RBR_FILE_TOO_SMALL -1010
#define RBR_FILE_TOO_BIG -1020
#define RBR_READ_EOF -1030
#define RBR_READ_ERROR -1040
#define RBR_NO_ERROR 0

#define kBufferSize 1024

namespace Detail
{
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t readLine(std::istream& in, char* result, size_t length)
{
  in.getline(result, length);
  if(in.fail())
  {
    if(in.eof())
    {
      return 0;
    }
    if(in.gcount() == static_cast<std::streamsize>(length))
    {
      // Read kBufferSize chars; ignoring the rest of the line.
      in.clear();
      in.ignore(std::numeric_limits<int>::max(), '\n');
    }
  }
  return 1;
}

class DelimiterType : public std::ctype<char>
{
  mask my_table[table_size];

public:
  DelimiterType(char delimiter, size_t refs = 0)
  : std::ctype<char>(&my_table[0], false, refs)
  {
    std::copy_n(classic_table(), table_size, my_table);
    my_table[static_cast<mask>(delimiter)] = (mask)space;
  }
};

} // namespace Detail

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int check_error_bits(std::ifstream* f)
{
  int stop = RBR_NO_ERROR;
  if(f->eof())
  {
    // std::perror("stream eofbit. error state");
    stop = RBR_READ_EOF;
  }
  else if(f->fail())
  {
    // std::perror("stream failbit (or badbit). error state");
    stop = RBR_READ_ERROR;
  }
  else if(f->bad())
  {
    // std::perror("stream badbit. error state");
    stop = RBR_READ_ERROR;
  }
  return stop;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T, typename K>
int32_t readAsciFile(typename DataArray<T>::Pointer data, const QString& filename, int32_t skipHeaderLines, char delimiter, bool inputIsBool = false)
{
  int32_t err = 0;
  QFileInfo fi(filename);
  if(err < 0)
  {
    return RBR_FILE_TOO_SMALL;
  }

  std::ifstream in(filename.toLatin1().constData(), std::ios_base::in | std::ios_base::binary);
  if(!in.is_open())
  {
    return RBR_FILE_NOT_OPEN;
  }

  in.imbue(std::locale(std::locale(), new Detail::DelimiterType(delimiter)));

  QByteArray buf(kBufferSize, '\0');
  char* buffer = buf.data();

  // Skip some header bytes by just reading those bytes into the pointer knowing that the next
  // thing we are going to do it over write those bytes with the real data that we are after.
  for(int i = 0; i < skipHeaderLines; i++)
  {
    buf.fill(0x00);                                  // Splat Null Chars across the line
    err = Detail::readLine(in, buffer, kBufferSize); // Read Line 1 - VTK Version Info
    if(err < 0)
    {
      return RBR_READ_ERROR;
    }
  }

  size_t numTuples = data->getNumberOfTuples();
  int scalarNumComp = data->getNumberOfComponents();

  size_t totalSize = numTuples * static_cast<size_t>(scalarNumComp);
  err = RBR_NO_ERROR;
  if(inputIsBool)
  {
    double value = 0.0;
    int64_t* si64Ptr = reinterpret_cast<int64_t*>(&value);

    for(size_t i = 0; i < totalSize; ++i)
    {
      in >> value;
      if(*si64Ptr == 0)
      {
        data->setValue(i, false);
      }
      else
      {
        data->setValue(i, true);
      }
      err = check_error_bits(&in);
      if(err == RBR_READ_EOF && i < totalSize - 1)
      {
        return err;
      }
      if(err == RBR_READ_ERROR)
      {
        return err;
      }
    }
  }
  else
  {
    K value = static_cast<T>(0.0);
    for(size_t i = 0; i < totalSize; ++i)
    {
      in >> value;
      data->setValue(i, static_cast<T>(value));
      err = check_error_bits(&in);
      if(err == RBR_READ_EOF && i < totalSize - 1)
      {
        return err;
      }
      if(err == RBR_READ_ERROR)
      {
        return err;
      }
    }
  }
  return RBR_NO_ERROR;
}

/* ############## Start Private Implementation ############################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class ImportAsciDataArrayPrivate
{
  Q_DISABLE_COPY(ImportAsciDataArrayPrivate)
  Q_DECLARE_PUBLIC(ImportAsciDataArray)
  ImportAsciDataArray* const q_ptr;
  ImportAsciDataArrayPrivate(ImportAsciDataArray* ptr);
  QString m_FirstLine;
  QString m_InputFile_Cache;
  QDateTime m_LastRead;
  int m_HeaderLines;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportAsciDataArrayPrivate::ImportAsciDataArrayPrivate(ImportAsciDataArray* ptr)
: q_ptr(ptr)
, m_FirstLine()
, m_InputFile_Cache("")
, m_HeaderLines(-1)
{
}
/* ############## End Private Implementation ############################### */

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportAsciDataArray::ImportAsciDataArray()
: m_CreatedAttributeArrayPath("")
, m_ScalarType(SIMPL::NumericTypes::Type::Int8)
, m_InputFile("")
, d_ptr(new ImportAsciDataArrayPrivate(this))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportAsciDataArray::~ImportAsciDataArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void ImportAsciDataArray::setFirstLine(const QString& value)
{
  Q_D(ImportAsciDataArray);
  d->m_FirstLine = value;
}

// -----------------------------------------------------------------------------
QString ImportAsciDataArray::getFirstLine() const
{
  Q_D(const ImportAsciDataArray);
  return d->m_FirstLine;
}

// -----------------------------------------------------------------------------
void ImportAsciDataArray::setInputFile_Cache(const QString& value)
{
  Q_D(ImportAsciDataArray);
  d->m_InputFile_Cache = value;
}

// -----------------------------------------------------------------------------
QString ImportAsciDataArray::getInputFile_Cache() const
{
  Q_D(const ImportAsciDataArray);
  return d->m_InputFile_Cache;
}

// -----------------------------------------------------------------------------
void ImportAsciDataArray::setLastRead(const QDateTime& value)
{
  Q_D(ImportAsciDataArray);
  d->m_LastRead = value;
}

// -----------------------------------------------------------------------------
QDateTime ImportAsciDataArray::getLastRead() const
{
  Q_D(const ImportAsciDataArray);
  return d->m_LastRead;
}

// -----------------------------------------------------------------------------
void ImportAsciDataArray::setHeaderLines(int value)
{
  Q_D(ImportAsciDataArray);
  d->m_HeaderLines = value;
}

// -----------------------------------------------------------------------------
int ImportAsciDataArray::getHeaderLines() const
{
  Q_D(const ImportAsciDataArray);
  return d->m_HeaderLines;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAsciDataArray::readHeaderPortion()
{
  int32_t err = 0;
  QString filename = getInputFile();

  QFileInfo fi(getInputFile());

  QDateTime lastModified(fi.lastModified());
  if(getInputFile() == getInputFile_Cache() && getLastRead().isValid() && lastModified.msecsTo(getLastRead()) >= 0 && getSkipHeaderLines() == getHeaderLines())
  {
    // We are reading from the cache, so set the FileWasRead flag to false
  }
  else
  {
    // We are reading from the file

    std::ifstream in(filename.toLatin1().constData(), std::ios_base::in | std::ios_base::binary);
    if(!in.is_open())
    {
      QString errorMessage = QString("Error opening input file '%1'").arg(filename);
      setErrorCondition(RBR_FILE_NOT_OPEN, errorMessage);
      return;
    }

    in.imbue(std::locale(std::locale(), new Detail::DelimiterType(getDelimiter())));

    QByteArray buf(kBufferSize, '\0');
    char* buffer = buf.data();

    int skipHeaderLines = getSkipHeaderLines();
    for(int i = 0; i < skipHeaderLines; i++)
    {
      buf.fill(0x00); // Splat Null Chars across the line
      err = Detail::readLine(in, buffer, kBufferSize);
      if(err < 0)
      {
        QString errorMessage = QString("Error reading the input file at line %1 of the file").arg(i);
        setErrorCondition(RBR_READ_ERROR, errorMessage);
        return;
      }
    }

    err = Detail::readLine(in, buffer, kBufferSize);
    if(err < 0)
    {
      setErrorCondition(err, "Error reading the first line of data from the input file");
    }

    setFirstLine(buf);

    // Set the file path and time stamp into the cache
    setLastRead(QDateTime::currentDateTime());
    setInputFile_Cache(getInputFile());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAsciDataArray::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, ImportAsciDataArray, "*.*"));
  parameters.push_back(SIMPL_NEW_NUMERICTYPE_FP("Scalar Type", ScalarType, FilterParameter::Parameter, ImportAsciDataArray));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Components", NumberOfComponents, FilterParameter::Parameter, ImportAsciDataArray));

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Skip Header Lines", SkipHeaderLines, FilterParameter::Parameter, ImportAsciDataArray));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Delimiter");
    parameter->setPropertyName("Delimiter");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(ImportAsciDataArray, this, Delimiter));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(ImportAsciDataArray, this, Delimiter));

    QVector<QString> choices;
    choices.push_back(", (comma)");
    choices.push_back("; (semicolon)");
    choices.push_back("  (space)");
    choices.push_back(": (colon)");
    choices.push_back("\\t (Tab)");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  {
    DataArrayCreationFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Output Attribute Array", CreatedAttributeArrayPath, FilterParameter::CreatedArray, ImportAsciDataArray, req));
  }

  PreflightUpdatedValueFilterParameter::Pointer param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("First Line of Data:", FirstLine, FilterParameter::Parameter, ImportAsciDataArray);
  param->setReadOnly(true);
  parameters.push_back(param);

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAsciDataArray::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAsciDataArray::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  QFileInfo fi(getInputFile());
  if(getInputFile().isEmpty())
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387, ss);
  }
  else if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388, ss);
  }

  if(m_NumberOfComponents < 1)
  {
    QString ss = QObject::tr("The number of components must be positive");
    setErrorCondition(-391, ss);
  }

  AttributeMatrix::Pointer attrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, getCreatedAttributeArrayPath(), -30003);
  if(getErrorCode() < 0)
  {
    return;
  }

  std::vector<size_t> tDims = attrMat->getTupleDimensions();
  size_t totalDim = 1;
  for(size_t i = 0; i < tDims.size(); i++)
  {
    totalDim = totalDim * tDims[i];
  }

  std::vector<size_t> cDims(1, m_NumberOfComponents);
  if(m_ScalarType == SIMPL::NumericTypes::Type::Int8)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int8ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt8)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt8ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int16)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int16ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt16)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt16ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int32)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt32)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int64)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int64ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt64)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt64ArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Float)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Double)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Bool)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<BoolArrayType>(this, getCreatedAttributeArrayPath(), false, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::SizeT)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<SizeTArrayType>(this, getCreatedAttributeArrayPath(), false, cDims, "CreatedAttributeArrayPath", AsciiArrayID);
  }
  readHeaderPortion();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAsciDataArray::execute()
{
  clearErrorCode();
  clearWarningCode();

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCreatedAttributeArrayPath().getDataContainerName());

  char delimiter = converSelectedDelimiter();

  std::vector<size_t> cDims(1, m_NumberOfComponents);
  int32_t err = 0;
  if(m_ScalarType == SIMPL::NumericTypes::Type::Int8)
  {
    Int8ArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<Int8ArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<int8_t, int32_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt8)
  {
    UInt8ArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<UInt8ArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<uint8_t, uint32_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int16)
  {
    Int16ArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<Int16ArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<int16_t, int16_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt16)
  {
    UInt16ArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<UInt16ArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<uint16_t, uint16_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int32)
  {
    Int32ArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<int32_t, int32_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt32)
  {
    UInt32ArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<uint32_t, uint32_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int64)
  {
    Int64ArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<Int64ArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<int64_t, int64_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt64)
  {
    UInt64ArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<UInt64ArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<uint64_t, uint64_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Float)
  {
    FloatArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<float, float>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Double)
  {
    DoubleArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<double, double>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Bool)
  {
    BoolArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<BoolArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<bool, bool>(p, m_InputFile, m_SkipHeaderLines, delimiter, true);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::SizeT)
  {
    SizeTArrayType::Pointer p = getDataContainerArray()->getPrereqArrayFromPath<SizeTArrayType>(this, getCreatedAttributeArrayPath(), cDims);
    err = readAsciFile<size_t, size_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
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
  else if(err == RBR_READ_ERROR)
  {
    setErrorCondition(RBR_READ_ERROR, "General read error while importing ASCI file.");
  }
  else if(err == RBR_READ_EOF)
  {
    setErrorCondition(RBR_READ_EOF, "ImportAsciDataArray read past the end of the specified file");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char ImportAsciDataArray::converSelectedDelimiter()
{
  if(getDelimiter() == ImportAsciDataArray::Comma)
  {
    return ',';
  }
  if(getDelimiter() == ImportAsciDataArray::Semicolon)
  {
    return ';';
  }
  if(getDelimiter() == ImportAsciDataArray::Space)
  {
    return ' ';
  }
  if(getDelimiter() == ImportAsciDataArray::Colon)
  {
    return ':';
  }
  if(getDelimiter() == ImportAsciDataArray::Tab)
  {
    return '\t';
  }
  return ' ';
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportAsciDataArray::newFilterInstance(bool copyFilterParameters) const
{
  ImportAsciDataArray::Pointer filter = ImportAsciDataArray::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportAsciDataArray::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportAsciDataArray::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportAsciDataArray::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportAsciDataArray::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ImportAsciDataArray::getUuid() const
{
  return QUuid("{a7007472-29e5-5d0a-89a6-1aed11b603f8}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportAsciDataArray::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportAsciDataArray::getHumanLabel() const
{
  return "Import ASCII Attribute Array";
}

// -----------------------------------------------------------------------------
ImportAsciDataArray::Pointer ImportAsciDataArray::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ImportAsciDataArray> ImportAsciDataArray::New()
{
  struct make_shared_enabler : public ImportAsciDataArray
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ImportAsciDataArray::getNameOfClass() const
{
  return QString("ImportAsciDataArray");
}

// -----------------------------------------------------------------------------
QString ImportAsciDataArray::ClassName()
{
  return QString("ImportAsciDataArray");
}

// -----------------------------------------------------------------------------
void ImportAsciDataArray::setCreatedAttributeArrayPath(const DataArrayPath& value)
{
  m_CreatedAttributeArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ImportAsciDataArray::getCreatedAttributeArrayPath() const
{
  return m_CreatedAttributeArrayPath;
}

// -----------------------------------------------------------------------------
void ImportAsciDataArray::setScalarType(SIMPL::NumericTypes::Type value)
{
  m_ScalarType = value;
}

// -----------------------------------------------------------------------------
SIMPL::NumericTypes::Type ImportAsciDataArray::getScalarType() const
{
  return m_ScalarType;
}

// -----------------------------------------------------------------------------
void ImportAsciDataArray::setNumberOfComponents(int value)
{
  m_NumberOfComponents = value;
}

// -----------------------------------------------------------------------------
int ImportAsciDataArray::getNumberOfComponents() const
{
  return m_NumberOfComponents;
}

// -----------------------------------------------------------------------------
void ImportAsciDataArray::setSkipHeaderLines(int value)
{
  m_SkipHeaderLines = value;
}

// -----------------------------------------------------------------------------
int ImportAsciDataArray::getSkipHeaderLines() const
{
  return m_SkipHeaderLines;
}

// -----------------------------------------------------------------------------
void ImportAsciDataArray::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString ImportAsciDataArray::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void ImportAsciDataArray::setDelimiter(int value)
{
  m_Delimiter = value;
}

// -----------------------------------------------------------------------------
int ImportAsciDataArray::getDelimiter() const
{
  return m_Delimiter;
}

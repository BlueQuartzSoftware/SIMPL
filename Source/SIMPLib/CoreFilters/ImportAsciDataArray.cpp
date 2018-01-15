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
    if(in.gcount() == static_cast<std::streamsize>(length) )
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


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int check_error_bits(std::ifstream* f)
{
  int stop = RBR_NO_ERROR;
  if(f->eof())
  {
    //std::perror("stream eofbit. error state");
    stop = RBR_READ_EOF;
  }
  else if(f->fail())
  {
    //std::perror("stream failbit (or badbit). error state");
    stop = RBR_READ_ERROR;
  }
  else if(f->bad())
  {
    //std::perror("stream badbit. error state");
    stop = RBR_READ_ERROR;
  }
  return stop;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T, typename K> int32_t readAsciFile(typename DataArray<T>::Pointer data, const QString& filename, int32_t skipHeaderLines, char delimiter, bool inputIsBool = false)
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
    buf.fill(0x00); // Splat Null Chars across the line
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
      else if(err == RBR_READ_ERROR || err == RBR_READ_ERROR)
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
      data->setValue(i, static_cast<T>(value) );
      err = check_error_bits(&in);
      if(err == RBR_READ_EOF && i < totalSize - 1)
      {
        return err;
      }
      else if(err == RBR_READ_ERROR || err == RBR_READ_ERROR)
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
, m_LastRead()
, m_HeaderLines(-1)
{
}
/* ############## End Private Implementation ############################### */


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportAsciDataArray::ImportAsciDataArray()
: AbstractFilter()
, m_CreatedAttributeArrayPath("")
, m_ScalarType(SIMPL::NumericTypes::Type::Int8)
, m_NumberOfComponents(0)
, m_SkipHeaderLines(0)
, m_InputFile("")
, m_Delimiter(0)
, d_ptr(new ImportAsciDataArrayPrivate(this))

{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportAsciDataArray::~ImportAsciDataArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL_PIMPL_PROPERTY_DEF(ImportAsciDataArray, QString, FirstLine)
SIMPL_PIMPL_PROPERTY_DEF(ImportAsciDataArray, QString, InputFile_Cache)
SIMPL_PIMPL_PROPERTY_DEF(ImportAsciDataArray, QDateTime, LastRead)
SIMPL_PIMPL_PROPERTY_DEF(ImportAsciDataArray, int, HeaderLines)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAsciDataArray::readHeaderPortion()
{
  int32_t err = 0;
  QString filename = getInputFile();
  
  QFileInfo fi(getInputFile());
  
  QDateTime lastModified(fi.lastModified());
  if(getInputFile() == getInputFile_Cache() 
      && getLastRead().isValid() 
      && lastModified.msecsTo(getLastRead()) >= 0
      && getSkipHeaderLines() == getHeaderLines() )
  {
    // We are reading from the cache, so set the FileWasRead flag to false
  }
  else
  {
    // We are reading from the file
    
    std::ifstream in(filename.toLatin1().constData(), std::ios_base::in | std::ios_base::binary);
    if(!in.is_open())
    {
      setErrorCondition(RBR_FILE_NOT_OPEN);
      QString errorMessage = QString("Error opening input file '%1'").arg(filename);
      notifyErrorMessage(getHumanLabel(), errorMessage, getErrorCondition());
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
        notifyErrorMessage(getHumanLabel(), errorMessage, getErrorCondition());
        setErrorCondition(RBR_READ_ERROR);
        return;
      }
    }
    
    err = Detail::readLine(in, buffer, kBufferSize);
    if(err < 0)
    {
      setErrorCondition(err);
      notifyErrorMessage(getHumanLabel(), "Error reading the first line of data from the input file", getErrorCondition());
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
  FilterParameterVector parameters;

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
  
  PreflightUpdatedValueFilterParameter::Pointer param =
      SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("First Line of Data:", FirstLine, FilterParameter::Parameter, ImportAsciDataArray);
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

  QVector<size_t> cDims(1, m_NumberOfComponents);
  if(m_ScalarType == SIMPL::NumericTypes::Type::Int8)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int8ArrayType, AbstractFilter, int8_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt8)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt8ArrayType, AbstractFilter, uint8_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int16)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int16ArrayType, AbstractFilter, int16_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt16)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt16ArrayType, AbstractFilter, uint16_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int32)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType, AbstractFilter, int32_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt32)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType, AbstractFilter, uint32_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int64)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<Int64ArrayType, AbstractFilter, int64_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt64)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<UInt64ArrayType, AbstractFilter, uint64_t>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Float)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter, float>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Double)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType, AbstractFilter, double>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Bool)
  {
    getDataContainerArray()->createNonPrereqArrayFromPath<BoolArrayType, AbstractFilter, bool>(this, getCreatedAttributeArrayPath(), 0, cDims, "CreatedAttributeArrayPath");
  }
  
  
  readHeaderPortion();
  
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAsciDataArray::preflight()
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
void ImportAsciDataArray::execute()
{
  int32_t err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCreatedAttributeArrayPath().getDataContainerName());

  char delimiter = converSelectedDelimiter();

  QVector<size_t> cDims(1, m_NumberOfComponents);
  if(m_ScalarType == SIMPL::NumericTypes::Type::Int8)
  {
    Int8ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<Int8ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readAsciFile<int8_t, int32_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt8)
  {
    UInt8ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readAsciFile<uint8_t, uint32_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int16)
  {
    Int16ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<Int16ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readAsciFile<int16_t,int16_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt16)
  {
    UInt16ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<UInt16ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readAsciFile<uint16_t,uint16_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int32)
  {
    Int32ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readAsciFile<int32_t,int32_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt32)
  {
    UInt32ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<UInt32ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readAsciFile<uint32_t,uint32_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Int64)
  {
    Int64ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<Int64ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readAsciFile<int64_t,int64_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt64)
  {
    UInt64ArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<UInt64ArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readAsciFile<uint64_t,uint64_t>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Float)
  {
    FloatArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readAsciFile<float,float>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Double)
  {
    DoubleArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<DoubleArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readAsciFile<double,double>(p, m_InputFile, m_SkipHeaderLines, delimiter);
    if(err >= 0)
    {
      m_Array = p;
    }
  }
  else if(m_ScalarType == SIMPL::NumericTypes::Type::Bool)
  {
    BoolArrayType::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<BoolArrayType, AbstractFilter>(this, getCreatedAttributeArrayPath());
    err = readAsciFile<bool, bool>(p, m_InputFile, m_SkipHeaderLines, delimiter, true);
    if(err >= 0)
    {
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
  else if(err == RBR_READ_ERROR)
  {
    setErrorCondition(RBR_READ_ERROR);
    notifyErrorMessage(getHumanLabel(), "General read error while importing ASCI file.", getErrorCondition());
  }
  else if(err == RBR_READ_EOF)
  {
    setErrorCondition(RBR_READ_EOF);
    notifyErrorMessage(getHumanLabel(), "ImportAsciDataArray read past the end of the specified file", getErrorCondition());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char ImportAsciDataArray::converSelectedDelimiter()
{
  if (getDelimiter() ==  ImportAsciDataArray::Comma) return ',';
  if (getDelimiter() ==  ImportAsciDataArray::Semicolon) return ';';
  if (getDelimiter() ==  ImportAsciDataArray::Space) return ' ';
  if (getDelimiter() ==  ImportAsciDataArray::Colon) return ':';
  if (getDelimiter() ==  ImportAsciDataArray::Tab) return '\t';
  return ' ';
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportAsciDataArray::newFilterInstance(bool copyFilterParameters)
{
  ImportAsciDataArray::Pointer filter = ImportAsciDataArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAsciDataArray::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAsciDataArray::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAsciDataArray::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAsciDataArray::getGroupName()
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ImportAsciDataArray::getUuid()
{
  return QUuid("{a7007472-29e5-5d0a-89a6-1aed11b603f8}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAsciDataArray::getSubGroupName()
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAsciDataArray::getHumanLabel()
{
  return "Import ASCII Attribute Array";
}

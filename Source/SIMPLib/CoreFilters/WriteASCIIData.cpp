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

#include "WriteASCIIData.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"


/**
 * @brief The ExportDataPrivate class is a templated class that implements a method to generically
 * export data to an ASCII file
 */
template <typename TInputType> class WriteASCIIDataPrivate
{
public:
  using DataArrayType = DataArray<TInputType>;

  WriteASCIIDataPrivate() = default;
  virtual ~WriteASCIIDataPrivate() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool operator()(IDataArray::Pointer p)
  {
    return (std::dynamic_pointer_cast<DataArrayType>(p).get() != nullptr);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void static Execute(WriteASCIIData* filter, IDataArray::Pointer inputData, char delimiter, const QString& outputFile, int32_t MaxValPerLine)
  {
    typename DataArrayType::Pointer inputArray = std::dynamic_pointer_cast<DataArrayType>(inputData);

    QFile file(outputFile);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      QString ss = QObject::tr("The output file could not be opened: '%1'").arg(outputFile);
      filter->setErrorCondition(-11012);
      filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      return;
    }

    QTextStream out(&file);

    int32_t nComp = inputArray->getNumberOfComponents();

    TInputType* inputArrayPtr = inputArray->getPointer(0);
    size_t nTuples = inputArray->getNumberOfTuples();

    int32_t recCount = 0;
    for(size_t i = 0; i < nTuples; i++)
    {
      for(int32_t j = 0; j < nComp; j++)
      {
        out << inputArrayPtr[i * nComp + j];
        if(j < nComp - 1)
        {
          out << delimiter;
        }
      }
      recCount++;

      if(recCount >= MaxValPerLine)
      {
        out << '\n';
        recCount = 0;
      }
      else
      {
        out << delimiter;
      }
    }
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteASCIIData::WriteASCIIData()
: m_SelectedDataArrayPaths(QVector<DataArrayPath>())
, m_OutputPath("")
, m_Delimiter(0)
, m_FileExtension(".txt")
, m_MaxValPerLine(-1)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteASCIIData::~WriteASCIIData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteASCIIData::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Output Type");
    parameter->setPropertyName("OutputStyle");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(WriteASCIIData, this, OutputStyle));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(WriteASCIIData, this, OutputStyle));
    QVector<QString> choices;
    choices.push_back("Multiple Files");
    choices.push_back("Single File");
    parameter->setChoices(choices);

    QStringList linkedProps = {"OutputPath", "FileExtension", "MaxValPerLine", "OutputFilePath"};
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
    linkedProps.clear();
  }

  // Multiple File Output
  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path", OutputPath, FilterParameter::Parameter, WriteASCIIData, "*", "*", 0));
  parameters.push_back(SIMPL_NEW_STRING_FP("File Extension", FileExtension, FilterParameter::Parameter, WriteASCIIData, 0));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Maximum Tuples Per Line", MaxValPerLine, FilterParameter::Parameter, WriteASCIIData, 0));

  // Single File Output
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output File Path", OutputFilePath, FilterParameter::Parameter, WriteASCIIData, "*", "*", 1));

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New(); // Delimiter choice
    parameter->setHumanLabel("Delimiter");
    parameter->setPropertyName("Delimiter");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(WriteASCIIData, this, Delimiter));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(WriteASCIIData, this, Delimiter));

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
    MultiDataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Attribute Arrays to Export", SelectedDataArrayPaths, FilterParameter::RequiredArray, WriteASCIIData, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteASCIIData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedDataArrayPaths(reader->readDataArrayPathVector("SelectedDataArrayPaths", getSelectedDataArrayPaths()));
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setDelimiter(reader->readValue("Delimiter", getDelimiter()));
  setFileExtension(reader->readString("FileExtension", getFileExtension()));
  setMaxValPerLine(reader->readValue("MaxValPerLine", getMaxValPerLine()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteASCIIData::initialize()
{
  m_SelectedWeakPtrVector.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteASCIIData::dataCheck()
{
  // Make sure the weak pointer vector is cleared before we begin...
  m_SelectedWeakPtrVector.clear();

  clearErrorCondition();
  clearWarningCondition();

  //**************** MultiFile Checks ******************
  if(m_OutputStyle == MultiFile)
  {
    if(m_OutputPath.isEmpty())
    {
      setErrorCondition(-11002);
      QString ss = QObject::tr("The output path must be set");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    if(m_MaxValPerLine <= 0)
    {
      setErrorCondition(-11003);
      QString ss = QObject::tr("The Maximum Tuples Per Line (%1) must be positive").arg(m_MaxValPerLine);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    if(m_FileExtension.isEmpty())
    {
      setErrorCondition(-11004);
      QString ss = QObject::tr("The file extension must be set.");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
  //**************** MultiFile Checks ******************
  else if(m_OutputStyle == SingleFile)
  {
    if(m_OutputFilePath.isEmpty())
    {
      setErrorCondition(-11005);
      QString ss = QObject::tr("The output file path must be set");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    QFileInfo fi(m_OutputPath);
    QDir parentPath = fi.path();
    if(!parentPath.exists())
    {
      setWarningCondition(-11006);
      QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
    }
  }
  else
  {
    QString ss = QObject::tr("The type of output did not match either 0 (Multi-File) or 1 (Single File)");
    setErrorCondition(-11009);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_SelectedDataArrayPaths.isEmpty())
  {
    setErrorCondition(-11007);
    QString ss = QObject::tr("At least one Attribute Array must be selected");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<DataArrayPath> paths = getSelectedDataArrayPaths();

  if(!DataArrayPath::ValidateVector(paths))
  {
    setErrorCondition(-11008);
    QString ss = QObject::tr("There are Attribute Arrays selected that are not contained in the same Attribute Matrix. All selected Attribute Arrays must belong to the same Attribute Matrix");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  for(auto const& path : paths)
  {
    IDataArray::WeakPointer ptr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, path);

    if(getErrorCondition() < 0)
    {
      return;
    }

    if( ptr.lock()->getTypeAsString().compare("NeighborList<T>") == 0)
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedType);
      notifyErrorMessage(getHumanLabel(), "NeighborList is unsupported when writing ASCII Data.", getErrorCondition());
    }
    else if( ptr.lock()->getTypeAsString().compare("struct") == 0)
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedType);
      notifyErrorMessage(getHumanLabel(), "StructArray is unsupported when writing ASCII Data.", getErrorCondition());
    }
    else if( ptr.lock()->getTypeAsString().compare("StatsDataArray") == 0)
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedType);
      notifyErrorMessage(getHumanLabel(), "StatsDataArray is unsupported when writing ASCII Data.", getErrorCondition());
    }
    else
    {
      m_SelectedWeakPtrVector.push_back(ptr);
    }

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteASCIIData::preflight()
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
void WriteASCIIData::execute()
{
  clearErrorCondition();
  clearWarningCondition();
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(m_SelectedDataArrayPaths.count() != m_SelectedWeakPtrVector.count())
  {
    QString ss = QObject::tr("The number of selected Attribute Arrays does not equal the number of internal weak pointers");
    setErrorCondition(-11010);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_OutputStyle == MultiFile)
  {
    writeMultiFileOutput();
  }
  else if(m_OutputStyle == SingleFile)
  {
    writeSingleFileOutput();
  }
  else
  {
    QString ss = QObject::tr("The type of output did not match either 0 (Multi-File) or 1 (Single File)");
    setErrorCondition(-11009);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
}

// -----------------------------------------------------------------------------
void WriteASCIIData::writeMultiFileOutput()
{
  QString ss;
  QDir dir;
  if(!dir.mkpath(m_OutputPath))
  {
    ss = QObject::tr("Error creating output path '%1'").arg(m_OutputPath);
    setErrorCondition(-11004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(!m_FileExtension.startsWith(".")) // if no '.', add '.' to file extension
  {
    m_FileExtension = "." + m_FileExtension;
  }

  for(int32_t i = 0; i < m_SelectedWeakPtrVector.count(); i++)
  {
    IDataArray::Pointer selectedArrayPtr = m_SelectedWeakPtrVector.at(i).lock();

    QString message = QObject::tr("|| Exporting Dataset '%1'").arg(selectedArrayPtr->getName());
    notifyStatusMessageWithPrefix(getMessagePrefix(), message);

    QString exportArrayFile = m_OutputPath + QDir::separator() + selectedArrayPtr->getName() + m_FileExtension; // the complete output file path, name and extension

    char delimiter = lookupDelimiter();


    if( std::dynamic_pointer_cast<StringDataArray>(selectedArrayPtr).get() != nullptr)
    {
       writeStringArray(selectedArrayPtr, exportArrayFile, delimiter);
    }
    else if( selectedArrayPtr->getTypeAsString().compare("NeighborList<T>") == 0)
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedType);
      notifyErrorMessage(getHumanLabel(), "NeighborList is unsupported when writing ASCII Data.", getErrorCondition());
    }
    else if( selectedArrayPtr->getTypeAsString().compare("struct") == 0)
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedType);
      notifyErrorMessage(getHumanLabel(), "StructArray is unsupported when writing ASCII Data.", getErrorCondition());
    }
    else if( selectedArrayPtr->getTypeAsString().compare("StatsDataArray") == 0)
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedType);
      notifyErrorMessage(getHumanLabel(), "StatsDataArray is unsupported when writing ASCII Data.", getErrorCondition());
    }
    else
      EXECUTE_TEMPLATE(this, WriteASCIIDataPrivate, selectedArrayPtr, this, selectedArrayPtr, delimiter, exportArrayFile, m_MaxValPerLine)


    if(getErrorCondition() < 0)
    {
      break;
    }
  }
}

// -----------------------------------------------------------------------------

void WriteASCIIData::writeSingleFileOutput()
{
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFilePath());
  QDir parentPath(fi.path());
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-11020);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QFile file(getOutputFilePath());
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Output file could not be opened: %1").arg(getOutputFilePath());
    setErrorCondition(-11021);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QTextStream outFile(&file);

  std::vector<IDataArray::Pointer> data;

  // *********** Print the header Line *******************
  char delimiter = lookupDelimiter();

  for(int32_t i = 0; i < m_SelectedWeakPtrVector.count(); i++)
  {
    IDataArray::Pointer selectedArrayPtr = m_SelectedWeakPtrVector.at(i).lock();
    if(selectedArrayPtr->getNumberOfComponents() == 1)
    {
      outFile << selectedArrayPtr->getName();
    }
    else // There are more than a single component so we need to add multiple header values
    {
      for(int32_t k = 0; k < selectedArrayPtr->getNumberOfComponents(); ++k)
      {
        outFile << selectedArrayPtr->getName() << "_" << k;
        if(k < selectedArrayPtr->getNumberOfComponents() - 1)
        {
          outFile << delimiter;
        }
      }
    }
    if(i < m_SelectedWeakPtrVector.count() - 1)
    {
      outFile << delimiter;
    }
    // Get the IDataArray from the DataContainer
    data.push_back(selectedArrayPtr);
  }
  outFile << "\n";

  // Get the number of tuples in the arrays
  size_t numTuples = 0;
  if(!data.empty())
  {
    numTuples = data[0]->getNumberOfTuples();
  }

  float threshold = 0.0f;
  size_t numArrays = data.size();
  for(size_t i = 0; i < numTuples; ++i)
  {
    float percentIncrement = static_cast<float>(i) / static_cast<float>(numTuples) * 100.0f;
    if(percentIncrement > threshold)
    {
      QString ss = QObject::tr("Writing Output: %1%").arg(static_cast<int32_t>(percentIncrement));
      notifyStatusMessageWithPrefix(getMessagePrefix(), ss);
      threshold = threshold + 5.0f;
      if(threshold < percentIncrement)
      {
        threshold = percentIncrement;
      }
    }

    // Print a row of data
    for(size_t c = 0; c < numArrays; c++)
    {
      QString s;
      QTextStream out(&s);
      data.at(c)->printTuple(out, i, delimiter);

      if(c < numArrays - 1) // Last column
      {
        out << delimiter;
      }
      outFile << s;
    }
    outFile << "\n";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteASCIIData::writeStringArray(const IDataArray::Pointer& inputData, const QString& outputFile, char delimiter)
{
  StringDataArray::Pointer inputArray = std::dynamic_pointer_cast<StringDataArray>(inputData);

  QFile file(outputFile);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("The output file could not be opened: '%1'").arg(outputFile);
    setErrorCondition(-11011);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QTextStream out(&file);

  size_t nTuples = inputArray->getNumberOfTuples();

  int32_t recCount = 0;
  for(size_t i = 0; i < nTuples; i++)
  {
    out << inputArray->getValue(i);

    recCount++;

    if(recCount >= getMaxValPerLine())
    {
      out << '\n';
      recCount = 0;
    }
    else
    {
      out << delimiter;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char WriteASCIIData::lookupDelimiter()
{
  char del = ' ';
  switch(m_Delimiter)
  {
  case Comma:
    del = ',';
    break;
  case Semicolon:
    del = ';';
    break;
  case Space:
    del = ' ';
    break;
  case Colon:
    del = ':';
    break;
  case Tab:
    del = '\t';
    break;
  default:
    del = ' ';
  }
  return del;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WriteASCIIData::newFilterInstance(bool copyFilterParameters) const
{
  WriteASCIIData::Pointer filter = WriteASCIIData::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteASCIIData::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteASCIIData::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteASCIIData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteASCIIData::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid WriteASCIIData::getUuid()
{
  return QUuid("{5fbf9204-2c6c-597b-856a-f4612adbac38}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteASCIIData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteASCIIData::getHumanLabel() const
{
  return "Export ASCII Data";
}

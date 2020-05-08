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
#include "FeatureDataCSVWriter.h"

#include <QtCore/QDir>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureDataCSVWriter::FeatureDataCSVWriter()
: m_CellFeatureAttributeMatrixPath("", "", "")
, m_FeatureDataFile("")
, m_WriteNeighborListData(false)
, m_DelimiterChoice(0)
, m_WriteNumFeaturesLine(true)
, m_Delimiter(',')
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureDataCSVWriter::~FeatureDataCSVWriter() = default;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output File", FeatureDataFile, FilterParameter::Parameter, FeatureDataCSVWriter, "*.csv", "Comma Separated Data"));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Write Neighbor Data", WriteNeighborListData, FilterParameter::Parameter, FeatureDataCSVWriter));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Write Number of Features Line", WriteNumFeaturesLine, FilterParameter::Parameter, FeatureDataCSVWriter));

  {
    QVector<QString> choices;
    choices.push_back(",");
    choices.push_back(";");
    choices.push_back(":");
    choices.push_back("Tab");
    choices.push_back("Space");

    parameters.push_back(SIMPL_NEW_CHOICE_FP("Delimiter", DelimiterChoiceInt, FilterParameter::Parameter, FeatureDataCSVWriter, choices, false));
  }

  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Feature Attribute Matrix", CellFeatureAttributeMatrixPath, FilterParameter::RequiredArray, FeatureDataCSVWriter, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixPath(reader->readDataArrayPath("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath()));
  setFeatureDataFile(reader->readString("FeatureDataFile", getFeatureDataFile()));
  setWriteNeighborListData(reader->readValue("WriteNeighborListData", getWriteNeighborListData()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, getCellFeatureAttributeMatrixPath(), -301);

  QFileInfo fi(getFeatureDataFile());
  if(fi.suffix().compare("") == 0)
  {
    setFeatureDataFile(getFeatureDataFile().append(".csv"));
  }
  FileSystemPathHelper::CheckOutputFile(this, "Output File Name", getFeatureDataFile(), true);

  SIMPL::DelimiterTypes::Type delim = static_cast<SIMPL::DelimiterTypes::Type>(getDelimiterChoice());
  switch(delim)
  {
  case SIMPL::DelimiterTypes::Type::Comma:
    setDelimiter(',');
    break;
  case SIMPL::DelimiterTypes::Type::Semicolon:
    setDelimiter(';');
    break;
  case SIMPL::DelimiterTypes::Type::Colon:
    setDelimiter(':');
    break;
  case SIMPL::DelimiterTypes::Type::Tab:
    setDelimiter('\t');
    break;
  case SIMPL::DelimiterTypes::Type::Space:
    setDelimiter(' ');
    break;
  default:
    setDelimiter(',');
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getFeatureDataFile());
  QDir parentPath(fi.path());
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-1, ss);
    return;
  }

  QFile file(getFeatureDataFile());
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Output file could not be opened: %1").arg(getFeatureDataFile());
    setErrorCondition(-100, ss);
    return;
  }

  QTextStream outFile(&file);

  AttributeMatrix::Pointer cellFeatureAttrMat = getDataContainerArray()->getAttributeMatrix(getCellFeatureAttributeMatrixPath());

  // Write the total number of features
  if(getWriteNumFeaturesLine())
  {
    outFile << cellFeatureAttrMat->getNumberOfTuples() - 1 << "\n";
  }
  // Get all the names of the arrays from the Data Container
  QList<QString> headers = cellFeatureAttrMat->getAttributeArrayNames();

  std::vector<IDataArray::Pointer> data;

  // For checking if an array is a neighborlist
  NeighborList<int32_t>::Pointer neighborlistPtr = NeighborList<int32_t>::CreateArray(0, "_INTERNAL_USE_ONLY_JunkNeighborList", false);

  // Print the FeatureIds Header before the rest of the headers
  outFile << SIMPL::FeatureData::FeatureID;
  // Loop throught the list and print the rest of the headers, ignoring those we don't want
  for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    // Only get the array if the name does NOT match those listed
    IDataArray::Pointer p = cellFeatureAttrMat->getAttributeArray(*iter);
    if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
    {
      if(p->getNumberOfComponents() == 1)
      {
        outFile << m_Delimiter << (*iter);
      }
      else // There are more than a single component so we need to add multiple header values
      {
        for(int32_t k = 0; k < p->getNumberOfComponents(); ++k)
        {
          outFile << m_Delimiter << (*iter) << "_" << k;
        }
      }
      // Get the IDataArray from the DataContainer
      data.push_back(p);
    }
  }
  outFile << "\n";

  // Get the number of tuples in the arrays
  size_t numTuples = 0;
  if(!data.empty())
  {
    numTuples = data[0]->getNumberOfTuples();
  }

  float threshold = 0.0f;

  // Skip feature 0
  for(size_t i = 1; i < numTuples; ++i)
  {
    float percentIncrement = static_cast<float>(i) / static_cast<float>(numTuples) * 100.0f;
    if(percentIncrement > threshold)
    {
      QString ss = QObject::tr("Writing Feature Data || %1% Complete").arg(static_cast<double>(percentIncrement));
      notifyStatusMessage(ss);
      threshold = threshold + 5.0f;
      if(threshold < percentIncrement)
      {
        threshold = percentIncrement;
      }
    }

    // Print the feature id
    outFile << i;
    // Print a row of data
    for(std::vector<IDataArray::Pointer>::iterator p = data.begin(); p != data.end(); ++p)
    {
      outFile << m_Delimiter;
      (*p)->printTuple(outFile, i, m_Delimiter);
    }
    outFile << "\n";
  }

  if(m_WriteNeighborListData)
  {
    // Print the FeatureIds Header before the rest of the headers
    // Loop throught the list and print the rest of the headers, ignoring those we don't want
    for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
    {
      // Only get the array if the name does NOT match those listed
      IDataArray::Pointer p = cellFeatureAttrMat->getAttributeArray(*iter);
      if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) == 0)
      {
        outFile << SIMPL::FeatureData::FeatureID << m_Delimiter << SIMPL::FeatureData::NumNeighbors << m_Delimiter << (*iter) << "\n";
        numTuples = p->getNumberOfTuples();

        // Skip feature 0
        for(size_t i = 1; i < numTuples; ++i)
        {
          // Print the feature id
          outFile << i;
          // Print a row of data
          outFile << m_Delimiter;
          p->printTuple(outFile, i, m_Delimiter);
          outFile << "\n";
        }
      }
    }
  }
  file.close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FeatureDataCSVWriter::newFilterInstance(bool copyFilterParameters) const
{
  /*
   * FeatureDataFile
   * WriteNeighborListData
   */
  FeatureDataCSVWriter::Pointer filter = FeatureDataCSVWriter::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FeatureDataCSVWriter::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FeatureDataCSVWriter::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FeatureDataCSVWriter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FeatureDataCSVWriter::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FeatureDataCSVWriter::getUuid() const
{
  return QUuid("{737b8d5a-8622-50f9-9a8a-bfdb57608891}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FeatureDataCSVWriter::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FeatureDataCSVWriter::getHumanLabel() const
{
  return "Export Feature Data as CSV File";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FeatureDataCSVWriter::getDelimiterChoiceInt() const
{
  return static_cast<int>(this->m_DelimiterChoice);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::setDelimiterChoiceInt(int value)
{
  this->m_DelimiterChoice = 0; // Set the default and if the value falls within the correct range, use that.
  // Allowed integer values of the enum SIMPL::DelimiterTypes::Type; see defintion in Constants.h
  if(value >= 0 && value <= 4)
  {
    this->m_DelimiterChoice = value;
  }
}

// -----------------------------------------------------------------------------
FeatureDataCSVWriter::Pointer FeatureDataCSVWriter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FeatureDataCSVWriter> FeatureDataCSVWriter::New()
{
  struct make_shared_enabler : public FeatureDataCSVWriter
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FeatureDataCSVWriter::getNameOfClass() const
{
  return QString("FeatureDataCSVWriter");
}

// -----------------------------------------------------------------------------
QString FeatureDataCSVWriter::ClassName()
{
  return QString("FeatureDataCSVWriter");
}

// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::setCellFeatureAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellFeatureAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FeatureDataCSVWriter::getCellFeatureAttributeMatrixPath() const
{
  return m_CellFeatureAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::setFeatureDataFile(const QString& value)
{
  m_FeatureDataFile = value;
}

// -----------------------------------------------------------------------------
QString FeatureDataCSVWriter::getFeatureDataFile() const
{
  return m_FeatureDataFile;
}

// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::setWriteNeighborListData(bool value)
{
  m_WriteNeighborListData = value;
}

// -----------------------------------------------------------------------------
bool FeatureDataCSVWriter::getWriteNeighborListData() const
{
  return m_WriteNeighborListData;
}

// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::setDelimiterChoice(int value)
{
  m_DelimiterChoice = value;
}

// -----------------------------------------------------------------------------
int FeatureDataCSVWriter::getDelimiterChoice() const
{
  return m_DelimiterChoice;
}

// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::setWriteNumFeaturesLine(bool value)
{
  m_WriteNumFeaturesLine = value;
}

// -----------------------------------------------------------------------------
bool FeatureDataCSVWriter::getWriteNumFeaturesLine() const
{
  return m_WriteNumFeaturesLine;
}

// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::setDelimiter(char value)
{
  m_Delimiter = value;
}

// -----------------------------------------------------------------------------
char FeatureDataCSVWriter::getDelimiter() const
{
  return m_Delimiter;
}

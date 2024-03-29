/*
 * Your License or Copyright can go here
 */

#include "ReadASCIIData.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/ReadASCIIDataFilterParameter.h"
#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SIMPLib/Utilities/StringOperations.h"

#include "SIMPLib/CoreFilters/util/AbstractDataParser.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

namespace
{
const QString k_Skip("Skip");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadASCIIData::ReadASCIIData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadASCIIData::~ReadASCIIData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIData::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(ReadASCIIDataFilterParameter::Create("ASCII Wizard Data", "WizardData", "WizardData", FilterParameter::Category::Parameter));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  QString prefix = "Wizard_";

  ASCIIWizardData data;

  QString delimitersStr = reader->readString(prefix + "Delimiters", "");
  QList<char> delimiters;
  for(auto&& i : delimitersStr)
  {
    delimiters.push_back(i.toLatin1());
  }

  data.delimiters = delimiters;
  data.beginIndex = reader->readValue(prefix + "BeginIndex", -1);
  data.consecutiveDelimiters = reader->readValue(prefix + "ConsecutiveDelimiters", false);
  data.dataHeaders = reader->readStringList(prefix + "DataHeaders", QStringList());
  data.dataTypes = reader->readStringList(prefix + "DataTypes", QStringList());
  data.inputFilePath = reader->readString(prefix + "InputFilePath", "");
  data.numberOfLines = reader->readValue(prefix + "NumberOfLines", -1);
  data.automaticAM = reader->readValue(prefix + "AutomaticAM", false);

  QVector<uint64_t> tmpVec;
  std::vector<size_t> tDims;
  tmpVec = reader->readArray(prefix + "TupleDims", QVector<uint64_t>());
  for(uint64_t i : tmpVec)
  {
    tDims.push_back(static_cast<size_t>(i));
  }
  data.tupleDims = tDims;

  setWizardData(data);

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIData::readFilterParameters(QJsonObject& obj)
{
  AbstractFilter::readFilterParameters(obj);

  QString prefix = "Wizard_";

  m_WizardData.beginIndex = obj[prefix + "BeginIndex"].toInt();
  m_WizardData.consecutiveDelimiters = static_cast<bool>(obj[prefix + "ConsecutiveDelimiters"].toInt());
  m_WizardData.inputFilePath = obj[prefix + "InputFilePath"].toString();
  m_WizardData.numberOfLines = obj[prefix + "NumberOfLines"].toInt();
  m_WizardData.automaticAM = obj[prefix + "AutomaticAM"].toBool();
  m_WizardData.headerLine = obj[prefix + "HeaderLine"].toInt();
  m_WizardData.headerIsCustom = obj[prefix + "HeaderIsCustom"].toBool();
  m_WizardData.headerUsesDefaults = obj[prefix + "HeaderUseDefaults"].toBool();
  m_WizardData.attrMatType = obj[prefix + "AttributeMatrixType"].toInt(-1);

  DataArrayPath dap;
  QJsonObject dapObj = obj[prefix + "SelectedPath"].toObject();
  dap.readJson(dapObj);
  m_WizardData.selectedPath = dap;

  {
    QJsonArray jsonArray = obj[prefix + "DataHeaders"].toArray();
    QStringList dataHeaders;
    for(int i = 0, total = jsonArray.size(); i < total; ++i)
    {
      dataHeaders.push_back(jsonArray[i].toString());
    }
    m_WizardData.dataHeaders = dataHeaders;
  }

  {
    QJsonArray jsonArray = obj[prefix + "DataTypes"].toArray();
    QStringList dataTypes;
    for(auto i : jsonArray)
    {
      dataTypes.push_back(i.toString());
    }
    m_WizardData.dataTypes = dataTypes;
  }

  {
    QString delimitersStr = obj[prefix + "Delimiters"].toString();
    QList<char> delimiters;
    for(int i = 0, total = delimitersStr.size(); i < total; ++i)
    {
      delimiters.push_back(delimitersStr[i].toLatin1());
    }
    m_WizardData.delimiters = delimiters;
  }

  {
    QJsonArray jsonArray = obj[prefix + "TupleDims"].toArray();
    std::vector<size_t> tupleDims;
    for(int i = 0, total = jsonArray.size(); i < total; ++i)
    {
      tupleDims.push_back(static_cast<size_t>(jsonArray[i].toInt()));
    }

    m_WizardData.tupleDims = tupleDims;
  }
}

// FP: Check why these values are not connected to a filter parameter!

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIData::writeFilterParameters(QJsonObject& obj) const
{
  AbstractFilter::writeFilterParameters(obj);

  QString prefix = "Wizard_";

  obj[prefix + "BeginIndex"] = m_WizardData.beginIndex;
  obj[prefix + "ConsecutiveDelimiters"] = static_cast<int>(m_WizardData.consecutiveDelimiters);
  obj[prefix + "InputFilePath"] = m_WizardData.inputFilePath;
  obj[prefix + "NumberOfLines"] = m_WizardData.numberOfLines;
  obj[prefix + "AutomaticAM"] = m_WizardData.automaticAM;

  obj[prefix + "HeaderLine"] = m_WizardData.headerLine;
  obj[prefix + "HeaderIsCustom"] = m_WizardData.headerIsCustom;
  obj[prefix + "HeaderUseDefaults"] = m_WizardData.headerUsesDefaults;
  obj[prefix + "AttributeMatrixType"] = m_WizardData.attrMatType;

  QJsonObject dapObj;
  m_WizardData.selectedPath.writeJson(dapObj);
  obj[prefix + "SelectedPath"] = dapObj;

  {
    QJsonArray jsonArray;
    for(int i = 0; i < m_WizardData.dataHeaders.size(); i++)
    {
      jsonArray.push_back(m_WizardData.dataHeaders[i]);
    }
    obj[prefix + "DataHeaders"] = jsonArray;
  }

  {
    QJsonArray jsonArray;
    for(int i = 0; i < m_WizardData.dataTypes.size(); i++)
    {
      jsonArray.push_back(m_WizardData.dataTypes[i]);
    }
    obj[prefix + "DataTypes"] = jsonArray;
  }

  {
    QString delimitersStr = "";
    for(char delimiter : m_WizardData.delimiters)
    {
      delimitersStr.append(delimiter);
    }

    obj[prefix + "Delimiters"] = delimitersStr;
  }

  {
    QJsonArray jsonArray;
    for(unsigned long tupleDim : m_WizardData.tupleDims)
    {
      jsonArray.push_back(static_cast<int>(tupleDim));
    }
    obj[prefix + "TupleDims"] = jsonArray;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIData::renameDataArrayPath(const DataArrayPath::RenameType& renamePath)
{
  getWizardData().updateDataArrayPath(renamePath);

  AbstractFilter::renameDataArrayPath(renamePath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIData::initialize()
{
  m_ASCIIArrayMap.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIData::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  m_ASCIIArrayMap.clear();

  ASCIIWizardData wizardData = getWizardData();
  if(wizardData.isEmpty())
  {
    QString ss = "A file has not been chosen to import. Please pick a file to import.";
    setErrorCondition(EMPTY_FILE, ss);
    return;
  }

  QString inputFilePath = wizardData.inputFilePath;
  QStringList headers = wizardData.dataHeaders;
  QStringList dataTypes = wizardData.dataTypes;
  bool automaticAM = wizardData.automaticAM;
  DataArrayPath selectedPath = wizardData.selectedPath;
  std::vector<size_t> tDims = wizardData.tupleDims;
  std::vector<size_t> cDims(1, 1);

  QFileInfo fi(inputFilePath);
  if(inputFilePath.isEmpty())
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387, ss);
  }

  fi.setFile(inputFilePath);

  if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(inputFilePath);
    setErrorCondition(-388, ss);
  }

  if(!automaticAM)
  {
    AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(selectedPath);
    if(nullptr == am.get())
    {
      QString ss = "The attribute matrix input is empty. Please select an attribute matrix.";
      setErrorCondition(EMPTY_ATTR_MATRIX, ss);
      return;
    }

    QStringList amArrays = am->getAttributeArrayNames();
    for(int i = 0; i < amArrays.size(); i++)
    {
      QString amArrayName = amArrays[i];
      for(int j = 0; j < headers.size(); j++)
      {
        QString headerName = headers[j];
        if(amArrayName == headerName)
        {
          QString ss = "The header name \"" + headerName + "\" matches an array name that already exists in the selected attribute matrix.";
          setErrorCondition(DUPLICATE_NAMES, ss);
          return;
        }
        if(headerName.contains("&") || headerName.contains(":") || headerName.contains("/") || headerName.contains("\\"))
        {
          QString ss = "The header name \"" + headerName + "\" contains a character that will cause problems. Do Not use '&',':', '/' or '\\' in the header names.";
          setErrorCondition(ILLEGAL_NAMES, ss);
          return;
        }
      }
    }
  }
  else
  {
    AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(selectedPath);

    //    if (am->getTupleDimensions() != tDims)
    //    {
    //      QString ss = "The attribute matrix '" + selectedPath.getAttributeMatrixName() + "' does not have the same tuple dimensions as the data in the file '" + fi.fileName() + "'.";
    //      QTextStream out(&ss);
    //      out << selectedPath.getAttributeMatrixName() << " tuple dims: " << am->getTupleDimensions().at(0) << "\n";
    //      out << fi.fileName() << "tuple dims: " << tDims[0] << "\n";
    //      setErrorCondition(INCONSISTENT_TUPLES, ss);
    //      return;
    //    }

    if(am != nullptr)
    {
      // Attribute Matrix already exists, so you need to pick a different attribute matrix name
    }
    else
    {
      DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer(this, selectedPath.getDataContainerName());
      if(nullptr != dc.get())
      {
        AttributeMatrix::Type amType = static_cast<AttributeMatrix::Type>(wizardData.attrMatType);
        dc->createNonPrereqAttributeMatrix(this, selectedPath.getAttributeMatrixName(), tDims, amType);
      }
    }
  }

  // Create the arrays
  for(int i = 0; i < dataTypes.size(); i++)
  {
    QString dataType = dataTypes[i];
    if(dataType == ::k_Skip)
    {
      continue;
    }
    QString name = headers[i];

    DataArrayPath arrayPath = selectedPath;
    arrayPath.setDataArrayName(name);

    if(dataType == SIMPL::TypeNames::Double)
    {
      DoubleArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType>(this, arrayPath, 0, cDims, "");
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if(dataType == SIMPL::TypeNames::Float)
    {
      FloatArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType>(this, arrayPath, 0, cDims, "");
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if(dataType == SIMPL::TypeNames::Int8)
    {
      Int8ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int8ArrayType>(this, arrayPath, 0, cDims, "");
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if(dataType == SIMPL::TypeNames::Int16)
    {
      Int16ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int16ArrayType>(this, arrayPath, 0, cDims, "");
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if(dataType == SIMPL::TypeNames::Int32)
    {
      Int32ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType>(this, arrayPath, 0, cDims, "");
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if(dataType == SIMPL::TypeNames::Int64)
    {
      Int64ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int64ArrayType>(this, arrayPath, 0, cDims, "");
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if(dataType == SIMPL::TypeNames::UInt8)
    {
      UInt8ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt8ArrayType>(this, arrayPath, 0, cDims, "");
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if(dataType == SIMPL::TypeNames::UInt16)
    {
      UInt16ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt16ArrayType>(this, arrayPath, 0, cDims, "");
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if(dataType == SIMPL::TypeNames::UInt32)
    {
      UInt32ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType>(this, arrayPath, 0, cDims, "");
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if(dataType == SIMPL::TypeNames::UInt64)
    {
      UInt64ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt64ArrayType>(this, arrayPath, 0, cDims, "");
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if(dataType == SIMPL::TypeNames::String)
    {
      StringDataArray::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<StringDataArray>(this, arrayPath, "", cDims, "");
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else
    {
      QString ss = "The data type that was chosen for column number " + QString::number(i + 1) + " is not a valid data array type.";
      setErrorCondition(INVALID_ARRAY_TYPE, ss);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIData::execute()
{
  clearErrorCode();
  clearWarningCode();
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  ASCIIWizardData wizardData = getWizardData();
  QString inputFilePath = wizardData.inputFilePath;
  QStringList headers = wizardData.dataHeaders;
  QStringList dataTypes = wizardData.dataTypes;
  QList<char> delimiters = wizardData.delimiters;
  bool consecutiveDelimiters = wizardData.consecutiveDelimiters;
  int numLines = wizardData.numberOfLines;
  int beginIndex = wizardData.beginIndex;

  QList<AbstractDataParser::Pointer> dataParsers;
  for(int i = 0; i < headers.size(); i++)
  {
    QString dataType = dataTypes[i];
    QString name = headers[i];

    if(dataType == SIMPL::TypeNames::Double)
    {
      DoubleArrayType::Pointer data = std::dynamic_pointer_cast<DoubleArrayType>(m_ASCIIArrayMap.value(i));
      DoubleParserType::Pointer parser = DoubleParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if(dataType == SIMPL::TypeNames::Float)
    {
      FloatArrayType::Pointer data = std::dynamic_pointer_cast<FloatArrayType>(m_ASCIIArrayMap.value(i));
      FloatParserType::Pointer parser = FloatParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if(dataType == SIMPL::TypeNames::Int8)
    {
      Int8ArrayType::Pointer data = std::dynamic_pointer_cast<Int8ArrayType>(m_ASCIIArrayMap.value(i));
      Int8ParserType::Pointer parser = Int8ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if(dataType == SIMPL::TypeNames::Int16)
    {
      Int16ArrayType::Pointer data = std::dynamic_pointer_cast<Int16ArrayType>(m_ASCIIArrayMap.value(i));
      Int16ParserType::Pointer parser = Int16ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if(dataType == SIMPL::TypeNames::Int32)
    {
      Int32ArrayType::Pointer data = std::dynamic_pointer_cast<Int32ArrayType>(m_ASCIIArrayMap.value(i));
      Int32ParserType::Pointer parser = Int32ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if(dataType == SIMPL::TypeNames::Int64)
    {
      Int64ArrayType::Pointer data = std::dynamic_pointer_cast<Int64ArrayType>(m_ASCIIArrayMap.value(i));
      Int64ParserType::Pointer parser = Int64ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if(dataType == SIMPL::TypeNames::UInt8)
    {
      UInt8ArrayType::Pointer data = std::dynamic_pointer_cast<UInt8ArrayType>(m_ASCIIArrayMap.value(i));
      UInt8ParserType::Pointer parser = UInt8ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if(dataType == SIMPL::TypeNames::UInt16)
    {
      UInt16ArrayType::Pointer data = std::dynamic_pointer_cast<UInt16ArrayType>(m_ASCIIArrayMap.value(i));
      UInt16ParserType::Pointer parser = UInt16ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if(dataType == SIMPL::TypeNames::UInt32)
    {
      UInt32ArrayType::Pointer data = std::dynamic_pointer_cast<UInt32ArrayType>(m_ASCIIArrayMap.value(i));
      UInt32ParserType::Pointer parser = UInt32ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if(dataType == SIMPL::TypeNames::UInt64)
    {
      UInt64ArrayType::Pointer data = std::dynamic_pointer_cast<UInt64ArrayType>(m_ASCIIArrayMap.value(i));
      UInt64ParserType::Pointer parser = UInt64ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if(dataType == SIMPL::TypeNames::String)
    {
      StringDataArray::Pointer data = std::dynamic_pointer_cast<StringDataArray>(m_ASCIIArrayMap.value(i));
      StringParserType::Pointer parser = StringParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
  }

  int insertIndex = 0;

  QFile inputFile(inputFilePath);
  if(inputFile.open(QIODevice::ReadOnly))
  {
    QTextStream in(&inputFile);

    for(int i = 1; i < beginIndex; i++)
    {
      // Skip to the first data line
      in.readLine();
    }

    float threshold = 0.0f;
    size_t numTuples = numLines - beginIndex + 1;

    for(int lineNum = beginIndex; lineNum <= numLines; lineNum++)
    {
      QString line = in.readLine();
      QStringList tokens = StringOperations::TokenizeString(line, delimiters, consecutiveDelimiters);

      if(dataTypes.size() != tokens.size())
      {
        QString ss = "Line " + QString::number(lineNum) + " has an inconsistent number of columns.\n";
        QTextStream out(&ss);
        out << "Expecting " << dataTypes.size() << " but found " << tokens.size() << "\n";
        out << "Input line was:\n";
        out << line;
        setErrorCondition(INCONSISTENT_COLS, ss);
        return;
      }

      for(int i = 0; i < dataParsers.size(); i++)
      {
        AbstractDataParser::Pointer parser = dataParsers[i];
        int index = parser->getColumnIndex();

        ParserFunctor::ErrorObject obj = parser->parse(tokens[index], insertIndex);
        if(!obj.ok)
        {
          QString errorMessage = obj.errorMessage;
          QString ss = errorMessage + "(line " + QString::number(lineNum) + ", column " + QString::number(index) + ").";
          setErrorCondition(CONVERSION_FAILURE, ss);
          return;
        }
      }

      const float percentCompleted = (static_cast<float>(lineNum) / numTuples) * 100.0f;
      if(percentCompleted > threshold)
      {
        // Print the status of the import
        QString ss = QObject::tr("Importing ASCII Data || %1% Complete").arg(static_cast<double>(percentCompleted), 0, 'f', 0);
        notifyStatusMessage(ss);
        threshold = threshold + 5.0f;
        if(threshold < percentCompleted)
        {
          threshold = percentCompleted;
        }
      }

      if(getCancel())
      {
        return;
      }

      insertIndex++;
    }
    inputFile.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReadASCIIData::newFilterInstance(bool copyFilterParameters) const
{
  ReadASCIIData::Pointer filter = ReadASCIIData::New();
  if(copyFilterParameters)
  {
    filter->setWizardData(getWizardData());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadASCIIData::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadASCIIData::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadASCIIData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadASCIIData::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ReadASCIIData::getUuid() const
{
  return QUuid("{bdb978bc-96bf-5498-972c-b509c38b8d50}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadASCIIData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadASCIIData::getHumanLabel() const
{
  return "Import ASCII Data";
}

// -----------------------------------------------------------------------------
ReadASCIIData::Pointer ReadASCIIData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ReadASCIIData> ReadASCIIData::New()
{
  struct make_shared_enabler : public ReadASCIIData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ReadASCIIData::getNameOfClass() const
{
  return QString("ReadASCIIData");
}

// -----------------------------------------------------------------------------
QString ReadASCIIData::ClassName()
{
  return QString("ReadASCIIData");
}

// -----------------------------------------------------------------------------
void ReadASCIIData::setWizardData(const ASCIIWizardData& value)
{
  m_WizardData = value;
}

// -----------------------------------------------------------------------------
ASCIIWizardData ReadASCIIData::getWizardData() const
{
  return m_WizardData;
}

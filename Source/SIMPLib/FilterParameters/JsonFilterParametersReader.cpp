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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "JsonFilterParametersReader.h"

#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/CoreFilters/EmptyFilter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Messages/PipelineErrorMessage.h"
#include "SIMPLib/Utilities/StringOperations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersReader::JsonFilterParametersReader()
: m_MaxFilterIndex(-1)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersReader::~JsonFilterParametersReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString createErrorMessageFromJsonParseError(const QJsonParseError& parseError)
{
  QString msg;
  QTextStream ss(&msg);

  switch(parseError.error)
  {
  case QJsonParseError::NoError:
    ss << "No Error";
    break;
  case QJsonParseError::UnterminatedObject:
    ss << "Json Parsing Error: " << parseError.error << " of type UnterminatedObject. An object is not correctly terminated with a closing curly bracket";
    break;
  case QJsonParseError::MissingNameSeparator:
    ss << "Json Parsing Error: " << parseError.error << " of type MissingNameSeparator. A comma separating different items is missing";
    break;
  case QJsonParseError::UnterminatedArray:
    ss << "Json Parsing Error: " << parseError.error << " of type UnterminatedArray. The array is not correctly terminated with a closing square bracket";
    break;
  case QJsonParseError::MissingValueSeparator:
    ss << "Json Parsing Error: " << parseError.error << " of type MissingValueSeparator. A colon separating keys from values inside objects is missing";
    break;
  case QJsonParseError::IllegalValue:
    ss << "Json Parsing Error: " << parseError.error << " of type IllegalValue. The value is illegal";
    break;
  case QJsonParseError::TerminationByNumber:
    ss << "Json Parsing Error: " << parseError.error << " of type TerminationByNumber. The input stream ended while parsing a number";
    break;
  case QJsonParseError::IllegalNumber:
    ss << "Json Parsing Error: " << parseError.error << " of type IllegalNumber. The number is not well formed";
    break;
  case QJsonParseError::IllegalEscapeSequence:
    ss << "Json Parsing Error: " << parseError.error << " of type IllegalEscapeSequence. An illegal escape sequence occurred in the input";
    break;
  case QJsonParseError::IllegalUTF8String:
    ss << "Json Parsing Error: " << parseError.error << " of type IllegalUTF8String. An illegal UTF8 sequence occurred in the input";
    break;
  case QJsonParseError::UnterminatedString:
    ss << "Json Parsing Error: " << parseError.error << " of type UnterminatedString. A string wasn't terminated with a quote";
    break;
  case QJsonParseError::MissingObject:
    ss << "Json Parsing Error: " << parseError.error << " of type MissingObject. An object was expected but couldn't be found";
    break;
  case QJsonParseError::DeepNesting:
    ss << "Json Parsing Error: " << parseError.error << " of type DeepNesting. The JSON document is too deeply nested for the parser to parse it";
    break;
  case QJsonParseError::DocumentTooLarge:
    ss << "Json Parsing Error: " << parseError.error << " of type DocumentTooLarge. The JSON document is too large for the parser to parse it";
    break;
  case QJsonParseError::GarbageAtEnd:
    ss << "Json Parsing Error: " << parseError.error << " of type GarbageAtEnd. The parsed document contains additional garbage characters at the end";
    break;
  default:
    ss << "Json Parsing Error: " << parseError.error << " is of an unknown type.";
    break;
  }
  ss << "\nThe error occurred at offset " << parseError.offset << ".\nReported error message is: " << parseError.errorString();

  return msg;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer JsonFilterParametersReader::readPipelineFromFile(QString filePath, IObserver* obs)
{
  QFileInfo fInfo(filePath);

  if(filePath.isEmpty())
  {
    return FilterPipeline::NullPointer();
  }
  QFileInfo fi(filePath);
  if(!fi.exists())
  {
    return FilterPipeline::NullPointer();
  }

  QJsonParseError parseError = openFile(filePath);

  if(parseError.error != QJsonParseError::NoError)
  {
    if(nullptr != obs)
    {
      QString msg;
      QTextStream out(&msg);

      out << "File '" << fInfo.absoluteFilePath() << "' had errors while parsing the json data.\n";
      out << createErrorMessageFromJsonParseError(parseError) << ".\n";

      QFile source(filePath);
      source.open(QFile::ReadOnly);
      QString contents = source.readAll();
      source.close();
      QStringList list = contents.split(QRegExp("\\n"));
      QStringListIterator sourceLines(list);
      int32_t currentPos = 0;
      int32_t errPos = parseError.offset;
      int32_t currentLine = 1;
      while(sourceLines.hasNext())
      {
        QString line = sourceLines.next();

        if(currentPos + line.size() + 1 > errPos)
        {
          int32_t lineCol = errPos - currentPos;
          out << " The parse error occurred at or near line " << currentLine << ", position " << lineCol;
          break;
        }
        currentPos = currentPos + line.size() + 1;
        currentLine++;
      }

      PipelineErrorMessage::Pointer pm = PipelineErrorMessage::New("[NOT_READABLE]", QObject::tr("%1: %2").arg(JsonFilterParametersReader::ClassName()).arg(msg), -1);
      obs->processPipelineMessage(pm);
    }
    return FilterPipeline::NullPointer();
  }

  FilterPipeline::Pointer pipeline = readPipeline(obs);
  closeFile();

  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JsonFilterParametersReader::getJsonFromFile(QString filePath, IObserver* obs)
{
  QFileInfo fInfo(filePath);

  if(filePath.isEmpty())
  {
    return QString();
  }
  QFileInfo fi(filePath);
  if(!fi.exists())
  {
    return QString();
  }

  QString jsonString = "";
  if(!m_Root.isEmpty() || !m_CurrentFilterIndex.isEmpty())
  {
    closeFile();
  }

  QFile inputFile(filePath);
  if(inputFile.open(QIODevice::ReadOnly))
  {
    QJsonParseError parseError;
    QByteArray byteArray = inputFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
      if(nullptr != obs)
      {
        QString msg = createErrorMessageFromJsonParseError(parseError);
        msg = QString("File '%1' had errors while parsing the json data.%2").arg(filePath) + msg;
        PipelineErrorMessage::Pointer pm = PipelineErrorMessage::New("[NOT_READABLE]", QObject::tr("%1: %2").arg(JsonFilterParametersReader::ClassName()).arg(msg), -1);
        obs->processPipelineMessage(pm);
      }
      return QString();
    }

    byteArray = doc.toJson();
    jsonString = QString(byteArray);
  }

  closeFile();

  return jsonString;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString generateErrorHtml(const QString& errorText)
{
  QString html;
  QTextStream ss(&html);
  ss << "<html><head></head>\n";
  ss << "<body>\n";
  ss << "<h2>";
  ss << errorText;
  ss << "</h2>";
  ss << "</body></html>";
  return html;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JsonFilterParametersReader::HtmlSummaryFromFile(QString filePath, IObserver* obs)
{

  QFileInfo fInfo(filePath);

  if(filePath.isEmpty())
  {
    return generateErrorHtml(QString("The file path was empty"));
  }
  QFileInfo fi(filePath);
  if(!fi.exists())
  {
    return generateErrorHtml(QString("The file path does not exist on the system."));
  }

  QJsonObject m_Root;

  int err = -100;
  QFile inputFile(filePath);
  if(inputFile.open(QIODevice::ReadOnly))
  {
    QJsonParseError parseError;
    QByteArray byteArray = inputFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
      return generateErrorHtml(QString("Error parsing the Json file: (%1) %2").arg(parseError.error).arg(parseError.errorString()));
    }
    m_Root = doc.object();

    err = QJsonParseError::NoError;
    inputFile.close();
  }

  QJsonValueRef pBuildRef = m_Root["PipelineBuilder"];
  if(pBuildRef.isUndefined())
  {
    // Build up the Html formatted Error Message
    return generateErrorHtml(QString("The 'PipelineBuilder' object was not found in the pipeline file."));
  }

  QJsonObject pBuildObj = pBuildRef.toObject();
  QString name = pBuildObj["Name"].toString();
  QString dVers = pBuildObj["Version"].toString();
  int filterCount = pBuildObj["Number_Filters"].toInt();

  QString html;
  QTextStream ss(&html);
  ss << "<html><head></head>\n";
  ss << "<body>";

  // A table for the summary items
  ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\">\n";
  ss << "<tbody>\n";
  ss << "	<tr><th align=\"right\">Pipeline Name:</th><td>" << name << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Count:</th><td>" << filterCount << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Version:</th><td>" << dVers << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Location:</th><td>" << filePath << "</td></tr>\n";
  ss << "</tbody>\n";
  ss << "</table>\n";

  // Start the table of the Pipeline
  ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\" width=\"300px\">\n";
  ss << "<tbody>\n";
  ss << "<tr bgcolor=\"#A2E99C\"><th>Index</th><th>Name Label</th><th>Filter Name</th></tr>\n";

  //  FilterManager* filtManager = FilterManager::Instance();
  char rowColor = 0;
  QString red("#FFAAAA");
  QString odd("#FFFFFF");
  QString even("#B0E4FF");
  QString color = odd;
  bool unknownFilters = false;

  for(int i = 0; i < filterCount; ++i)
  {
    if(rowColor == 0)
    {
      rowColor = 1;
      color = odd;
    }
    else
    {
      rowColor = 0;
      color = even;
    }

    QString indexString = StringOperations::GenerateIndexString(i, filterCount - 1);
    QJsonValueRef filtRef = m_Root[indexString];

    if(filtRef.isUndefined())
    {
      ss << "<tr bgcolor=\"" << color << "\"><td>" << i << "</td><td> Filter Missing </td><td></td></tr>\n";
      unknownFilters = true;
    }
    else
    {
      QJsonObject filtObj = filtRef.toObject();
      QString filtName = filtObj["Filter_Name"].toString();
      QString filtLabel = filtObj["Filter_Human_Label"].toString();
      ss << "<tr bgcolor=\"" << color << "\"><td>" << i << "</td><td>" << filtLabel << "</td><td>" << filtName << "</td></tr>\n";
    }
  }

  if(unknownFilters)
  {
    color = red;
    ss << "<tr bgcolor=\"" << color << "\"><th colspan=\"3\">There are filters in the pipeline that the currently running version of SIMPLView does not know about. This ";
    ss << "can happen if you are missing plugins that contain the filters or if the pipeline was created in a prior version ";
    ss << "of SIMPLView in which case those filters may have been renamed. Please consult the SIMPLView documentation for more details ";
    ss << "or ask the individual who gave you the pipeline file for more details.</th></tr>\n";
  }
  ss << "</tbody></table>\n";

  ss << "</body></html>";

  return html;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer JsonFilterParametersReader::readPipelineFromString(QString contents, IObserver* obs)
{
  setPipelineContents(contents);

  FilterPipeline::Pointer pipeline = readPipeline(obs);
  closeFile();

  return pipeline;
}

// -----------------------------------------------------------------------------
FilterPipeline::Pointer JsonFilterParametersReader::readPipelineFromJson(const QJsonObject& contents, IObserver* obs)
{
  setPipelineContents(contents);

  FilterPipeline::Pointer pipeline = readPipeline(obs);
  closeFile();

  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer JsonFilterParametersReader::readPipeline(IObserver* obs)
{
  FilterManager* filtManager = FilterManager::Instance();
  FilterFactory<EmptyFilter>::Pointer emptyFilterFactory = FilterFactory<EmptyFilter>::New();
  if(!filtManager->contains(emptyFilterFactory->getUuid()))
  {
    filtManager->addFilterFactory("EmptyFilter", emptyFilterFactory);
  }

  if(!containsGroup(SIMPL::Settings::PipelineBuilderGroup))
  {
    return FilterPipeline::NullPointer();
  }

  QJsonObject builderObj = m_Root[SIMPL::Settings::PipelineBuilderGroup].toObject();
  int filterCount = builderObj[SIMPL::Settings::NumFilters].toInt();
  m_MaxFilterIndex = filterCount - 1; // Zero based indexing

  FilterPipeline::Pointer pipeline;

  if(filterCount >= 0)
  {
    pipeline = FilterPipeline::New();
    QString pipelineName = builderObj[SIMPL::Settings::PipelineName].toString("Empty Pipeline Name");
    pipeline->setName(pipelineName);
  }
  else
  {
    pipeline = FilterPipeline::NullPointer();
  }

  for(int i = 0; i < filterCount; ++i)
  {
    openFilterGroup(nullptr, i);

    IFilterFactory::Pointer factory = IFilterFactory::NullPointer();
    QString filterName = m_CurrentFilterIndex[SIMPL::Settings::FilterName].toString();
    bool filterEnabled = m_CurrentFilterIndex[SIMPL::Settings::FilterEnabled].toBool(true);
    // First try the UUID for the filter and see what we get.
    QUuid uuid = QUuid(m_CurrentFilterIndex[SIMPL::Settings::FilterUuid].toString(""));
    if(!uuid.isNull())
    {
      factory = filtManager->getFactoryFromUuid(uuid);
    }
    // If the UUID was not available, then try the filter class name
    if(nullptr == factory.get())
    {
      QJsonValue jsValue = m_CurrentFilterIndex[SIMPL::Settings::FilterName];
      if(jsValue.isString())
      {
        filterName = jsValue.toString("JSON Key 'Filter_Name' missing.");
        factory = filtManager->getFactoryFromClassName(filterName);
      }
    }

    if(nullptr != factory.get())
    {
      AbstractFilter::Pointer filter = factory->create();

      if(nullptr != filter.get())
      {
        filter->setEnabled(filterEnabled);
        filter->readFilterParameters(m_CurrentFilterIndex);
        pipeline->pushBack(filter);
      }
    }
    else // Could not find the filter because the specific name has not been registered. This could
         // be due to a name change for the filter.
    {
      EmptyFilter::Pointer filter = EmptyFilter::New();
      QString humanLabel = QString("UNKNOWN FILTER: ") + filterName;
      filter->setHumanLabel(humanLabel);
      filter->setOriginalFilterName(filterName);
      filter->setEnabled(filterEnabled);
      pipeline->pushBack(filter);

      if(nullptr != obs)
      {
        QString msg;
        QTextStream ss(&msg);
        ss << "An attempt to instantiate a filter from the pipeline file resulted in an error.\n"
           << "Possible reasons include a name change of the filter, plugin not loading or a simple spelling mistake? A \n"
           << "blank filter has been inserted in its place. The filter name is: " << filterName << "\n";

        QString prefix = "JsonFilterParametersReader::ReadPipelineFromFile()";
        PipelineErrorMessage::Pointer pm = PipelineErrorMessage::New(pipeline->getName(), QObject::tr("%1: %2").arg(prefix).arg(msg), -66066);
        obs->processPipelineMessage(pm);
      }
    }
    closeFilterGroup();
  }
  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JsonFilterParametersReader::readNameOfPipelineFromFile(QString filePath, QString& name, QString& version, IObserver* obs)
{
  QFileInfo fInfo(filePath);

  if(filePath.isEmpty())
  {
    name = QString("ERROR: No File Path Specified");
    version = QString("ERROR: No File Path Specified");
    return;
  }
  QFileInfo fi(filePath);
  if(!fi.exists())
  {
    name = QString("ERROR: File Path Does Not Exist");
    version = QString("ERROR: File Path Does Not Exist");
    return;
  }

  QJsonParseError parseError = openFile(filePath);

  if(parseError.error != QJsonParseError::NoError)
  {
    if(nullptr != obs)
    {
      QString msg = createErrorMessageFromJsonParseError(parseError);
      msg = QString("File '%1' had errors while parsing the json data.%2").arg(fInfo.absoluteFilePath()) + msg;
      PipelineErrorMessage::Pointer pm = PipelineErrorMessage::New("[NOT_READABLE]", QObject::tr("%1: %2").arg(JsonFilterParametersReader::ClassName()).arg(msg), -1);
      obs->processPipelineMessage(pm);
    }
    name = QString("ERROR: Could not open file specified.");
    version = QString("ERROR: Could not open file specified.");
    return;
  }

  openGroup(SIMPL::Settings::PipelineBuilderGroup);
  name = m_CurrentFilterIndex[SIMPL::Settings::PipelineName].toString();
  version = m_CurrentFilterIndex[SIMPL::Settings::Version].toString();
  closeGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject& JsonFilterParametersReader::getCurrentGroupObject()
{
  return m_CurrentFilterIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonParseError JsonFilterParametersReader::openFile(QString filePath)
{
  if(!m_Root.isEmpty() || !m_CurrentFilterIndex.isEmpty())
  {
    closeFile();
  }
  QJsonParseError parseError;
  QFile inputFile(filePath);
  if(inputFile.open(QIODevice::ReadOnly))
  {
    QByteArray byteArray = inputFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
      return parseError;
    }
    m_Root = doc.object();

    QJsonObject meta = m_Root[SIMPL::Settings::PipelineBuilderGroup].toObject();
    m_MaxFilterIndex = meta[SIMPL::Settings::NumFilters].toInt();
  }

  return parseError;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersReader::setPipelineContents(QString contents)
{
  if(!m_Root.isEmpty() || !m_CurrentFilterIndex.isEmpty())
  {
    closeFile();
  }

  int err = -100;

  QJsonParseError parseError;
  QByteArray byteArray = QByteArray::fromStdString(contents.toStdString());
  QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
  if(parseError.error != QJsonParseError::NoError)
  {
    return parseError.error;
  }
  m_Root = doc.object();

  err = QJsonParseError::NoError;

  return err;
}

// -----------------------------------------------------------------------------
void JsonFilterParametersReader::setPipelineContents(const QJsonObject& contents)
{
  if(!m_Root.isEmpty() || !m_CurrentFilterIndex.isEmpty())
  {
    closeFile();
  }

  m_Root = contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JsonFilterParametersReader::closeFile()
{
  m_Root = QJsonObject();
  m_CurrentFilterIndex = QJsonObject();
  m_MaxFilterIndex = -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersReader::openFilterGroup(AbstractFilter* unused, int index)
{
  Q_UNUSED(unused);
  Q_ASSERT(m_Root.isEmpty() == false);
  int err = 0;
  QString numStr = QString::number(index);
  if(m_Root.find(numStr) != m_Root.end())
  {
    m_CurrentFilterIndex = m_Root[numStr].toObject();
  }
  if(m_CurrentFilterIndex.isEmpty())
  {
    numStr = StringOperations::GenerateIndexString(index, m_MaxFilterIndex);
    m_CurrentFilterIndex = m_Root[numStr].toObject();
    if(m_CurrentFilterIndex.isEmpty())
    {
      err = -1;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersReader::closeFilterGroup()
{
  Q_ASSERT(m_Root.isEmpty() == false);
  m_CurrentFilterIndex = QJsonObject();
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool JsonFilterParametersReader::containsGroup(QString key)
{
  if(!m_Root.isEmpty())
  {
    if(m_Root[key].isObject())
    {
      QJsonObject obj = m_Root[key].toObject();
      if(!obj.isEmpty())
      {
        return true;
      }
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersReader::openGroup(QString key)
{
  Q_ASSERT(m_Root.isEmpty() == false);
  int err = 0;
  m_CurrentFilterIndex = m_Root[key].toObject();
  if(m_CurrentFilterIndex.isEmpty())
  {
    err = -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersReader::closeGroup()
{
  Q_ASSERT(m_Root.isEmpty() == false);
  m_CurrentFilterIndex = QJsonObject();
  return 0;
}

// -----------------------------------------------------------------------------
JsonFilterParametersReader::Pointer JsonFilterParametersReader::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
JsonFilterParametersReader::Pointer JsonFilterParametersReader::New()
{
  Pointer sharedPtr(new(JsonFilterParametersReader));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString JsonFilterParametersReader::getNameOfClass() const
{
  return QString("JsonFilterParametersReader");
}

// -----------------------------------------------------------------------------
QString JsonFilterParametersReader::ClassName()
{
  return QString("JsonFilterParametersReader");
}

// -----------------------------------------------------------------------------
void JsonFilterParametersReader::setFileName(const QString& value)
{
  m_FileName = value;
}

// -----------------------------------------------------------------------------
QString JsonFilterParametersReader::getFileName() const
{
  return m_FileName;
}

// -----------------------------------------------------------------------------
void JsonFilterParametersReader::setMaxFilterIndex(int value)
{
  m_MaxFilterIndex = value;
}

// -----------------------------------------------------------------------------
int JsonFilterParametersReader::getMaxFilterIndex() const
{
  return m_MaxFilterIndex;
}

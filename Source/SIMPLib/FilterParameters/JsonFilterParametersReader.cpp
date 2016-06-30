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
#include "JsonFilterParametersReader.h"

#include <QtCore/QMetaType>
#include <QtCore/QDataStream>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/CoreFilters/EmptyFilter.h"
#include "SIMPLib/Common/Constants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersReader::JsonFilterParametersReader() :
  AbstractFilterParametersReader(),
  m_MaxFilterIndex(-1)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersReader::~JsonFilterParametersReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer JsonFilterParametersReader::ReadPipelineFromFile(QString filePath, IObserver* obs)
{
  QFileInfo fInfo(filePath);

  if (filePath.isEmpty() == true)
  {
    return FilterPipeline::NullPointer();
  }
  QFileInfo fi(filePath);
  if (fi.exists() == false)
  {
    return FilterPipeline::NullPointer();
  }

  JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
  int err = reader->openFile(filePath);

  if (err != QJsonParseError::NoError)
  {
    if (NULL != obs)
    {
      PipelineMessage pm(JsonFilterParametersReader::ClassName(), "File '" + fInfo.fileName() + "' could not be opened for reading.", -1, PipelineMessage::Error);
      obs->processPipelineMessage(pm);
    }
    return FilterPipeline::NullPointer();
  }

  return ReadPipeline(reader, obs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString generateErrorHtml(const QString &errorText)
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

    if (filePath.isEmpty() == true)
    {
        // Build up the Html formatted Error Message
        return generateErrorHtml(QString("The file path was empty"));
    }
    QFileInfo fi(filePath);
    if (fi.exists() == false)
    {
        // Build up the Html formatted Error Message
        return generateErrorHtml(QString("The file path does not exist on the system."));
    }

    QJsonObject m_Root;

    int err = -100;
    QFile inputFile(filePath);
    if (inputFile.open(QIODevice::ReadOnly))
    {
      QJsonParseError parseError;
      QByteArray byteArray = inputFile.readAll();
      QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
      if (parseError.error != QJsonParseError::NoError)
      {
          // Build up the Html formatted Error Message
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
    ss << "<body>\n";

    // A table for the summary items
    ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\">\n";
    ss << "<tbody>\n";
    ss << "	<tr><th align=\"right\">Pipeline Name:</th><td>" << name << "</td></tr>\n";
    ss << "	<tr><th align=\"right\">Filter Count:</th><td>" << filterCount << "</td></tr>\n";
    ss << "	<tr><th align=\"right\">Version:</th><td>" << dVers << "</td></tr>\n";
    ss << "</tbody>\n";
    ss << "</table>\n";
    ss << "<p></p>\n";

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


    for (int i = 0; i < filterCount; ++i)
    {
      if (rowColor == 0) { rowColor = 1; color = odd; }
      else { rowColor = 0; color = even; }

      QJsonValueRef filtRef = m_Root[QString::number(i)];

      if(filtRef.isUndefined()) {
           ss << "<tr bgcolor=\"" << color << "\"><td>" << i << "</td><td> Filter Missing </td><td></td></tr>\n";
           unknownFilters = true;
      }
      else {
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
FilterPipeline::Pointer JsonFilterParametersReader::ReadPipelineFromString(QString contents, IObserver* obs)
{
  JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
  reader->setPipelineContents(contents);

  return ReadPipeline(reader, obs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JsonFilterParametersReader::generateIndexString(int index, int maxIndex)
{
  QString numStr = QString::number(index);

  if(maxIndex > 10)
  {
    int mag = 0;
    int max = maxIndex;
    while(max > 0) {
      mag++;
      max = max / 10;
    }
    numStr = ""; // Clear the string
    QTextStream ss(&numStr); // Create a QTextStream to set up the padding
    ss.setFieldWidth(mag);
    ss.setPadChar('0');
    ss << index;
  }
  return numStr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer JsonFilterParametersReader::ReadPipeline(JsonFilterParametersReader::Pointer reader, IObserver* obs)
{
  FilterManager* filtManager = FilterManager::Instance();
  FilterFactory<EmptyFilter>::Pointer emptyFilterFactory = FilterFactory<EmptyFilter>::New();
  filtManager->addFilterFactory("EmptyFilter", emptyFilterFactory);

  if (reader->containsGroup(SIMPL::Settings::PipelineBuilderGroup) == false)
  {
    return FilterPipeline::NullPointer();
  }

  reader->openGroup(SIMPL::Settings::PipelineBuilderGroup);
  int filterCount = reader->readValue(SIMPL::Settings::NumFilters, -1);
  reader->closeGroup();
  reader->setMaxFilterIndex(filterCount);

  FilterPipeline::Pointer pipeline;
  if (filterCount >= 0)
  {
    pipeline = FilterPipeline::New();
  }
  else
  {
    pipeline = FilterPipeline::NullPointer();
  }

  for (int i = 0; i < filterCount; ++i)
  {
    QString filterName;
    // Open the group to get the name of the filter then close again.
    QString groupName = reader->generateIndexString(i, filterCount);
    //First try generating a string with padding digits.
    int err = reader->openFilterGroup(groupName);
    if(err < 0)
    {
      // If that fails then try without the padding digits
      err = reader->openFilterGroup(NULL, i);
    }
    if (err == 0)
    {
      filterName = reader->readString(SIMPL::Settings::FilterName, "");
      reader->closeFilterGroup();
    }

    //qDebug() << "Group: " << gName << " FilterName: " << filterName;
    if (filterName.isEmpty() == false)
    {
      IFilterFactory::Pointer factory = filtManager->getFactoryForFilter(filterName);
      if (factory.get() != NULL)
      {
        AbstractFilter::Pointer filter = factory->create();

        if (NULL != filter.get())
        {
          filter->readFilterParameters(reader.get(), i);
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
        pipeline->pushBack(filter);

        if (NULL != obs)
        {
          QString ss = QObject::tr("An implementation for filter '%1' could not be located. Possible reasons include a name change of the filter, plugin not loading or a simple spelling mistake? A blank filter has been inserted in its place.").arg(filterName);
          PipelineMessage pm(filterName, ss, -66066, PipelineMessage::Error);
          pm.setPrefix("JsonFilterParametersReader::ReadPipelineFromFile()");
          obs->processPipelineMessage(pm);
        }
      }
    }
    else
    {
      EmptyFilter::Pointer filter = EmptyFilter::New();
      QString humanLabel = QString("MISSING FILTER: ") + filterName;
      filter->setHumanLabel(humanLabel);
      filter->setOriginalFilterName(filterName);
      pipeline->pushBack(filter);

      if (NULL != obs)
      {
        QString gName = QString::number(i);
        QString ss = QObject::tr("A filter for index '%1' is missing in the file. Is the numbering of the filters correct in the pipeline file?").arg(gName);
        PipelineMessage pm(filterName, ss, -66067, PipelineMessage::Error);
        pm.setPrefix("JsonFilterParametersReader::ReadPipelineFromFile()");
        obs->processPipelineMessage(pm);
      }
    }
  }
  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JsonFilterParametersReader::ReadNameOfPipelineFromFile(QString filePath, QString& name, QString& version, IObserver* obs)
{
  QFileInfo fInfo(filePath);

  if (filePath.isEmpty() == true)
  {
    name = QString("ERROR: No File Path Specified");
    version = QString("ERROR: No File Path Specified");
    return;
  }
  QFileInfo fi(filePath);
  if (fi.exists() == false)
  {
    name = QString("ERROR: File Path Does Not Exist");
    version = QString("ERROR: File Path Does Not Exist");
    return;
  }

  JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
  int err = reader->openFile(filePath);

  if (err != QJsonParseError::NoError)
  {
    if (NULL != obs)
    {
      PipelineMessage pm(JsonFilterParametersReader::ClassName(), "File '" + fInfo.fileName() + "' could not be opened for reading.", -1, PipelineMessage::Error);
      obs->processPipelineMessage(pm);
    }
    name = QString("ERROR: Could not open file specified.");
    version = QString("ERROR: Could not open file specified.");
    return;
  }

  err = reader->openGroup(SIMPL::Settings::PipelineBuilderGroup);
  name = reader->readString(SIMPL::Settings::PipelineName, "");
  version = reader->readString(SIMPL::Settings::Version, "Unknown DREAM3D Version");
  reader->closeGroup();
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
int JsonFilterParametersReader::openFile(QString filePath)
{
  if (m_Root.isEmpty() == false || m_CurrentFilterIndex.isEmpty() == false)
  {
    closeFile();
  }
  int err = -100;
  QFile inputFile(filePath);
  if (inputFile.open(QIODevice::ReadOnly))
  {
    QJsonParseError parseError;
    QByteArray byteArray = inputFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
      return parseError.error;
    }
    m_Root = doc.object();

    err = QJsonParseError::NoError;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersReader::setPipelineContents(QString contents)
{
  if (m_Root.isEmpty() == false || m_CurrentFilterIndex.isEmpty() == false)
  {
    closeFile();
  }

  int err = -100;

  QJsonParseError parseError;
  QByteArray byteArray = QByteArray::fromStdString(contents.toStdString());
  QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
  if (parseError.error != QJsonParseError::NoError)
  {
    return parseError.error;
  }
  m_Root = doc.object();

  err = QJsonParseError::NoError;

  return err;
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
  Q_ASSERT(m_Root.isEmpty() == false);
  int err = 0;
  QString numStr = QString::number(index);
  m_CurrentFilterIndex = m_Root[numStr].toObject();
  if(m_CurrentFilterIndex.isEmpty())
  {
    numStr = generateIndexString(index, m_MaxFilterIndex);
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
int JsonFilterParametersReader::openFilterGroup(const QString &numStr)
{
  Q_ASSERT(m_Root.isEmpty() == false);
  int err = 0;
  m_CurrentFilterIndex = m_Root[numStr].toObject();
  if(m_CurrentFilterIndex.isEmpty())
  {
    err = -1;
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
  if (m_Root.isEmpty() == false)
  {
    if (m_Root[key].isObject() == true)
    {
      QJsonObject obj = m_Root[key].toObject();
      if (obj.isEmpty() == false)
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
//
// -----------------------------------------------------------------------------
QString JsonFilterParametersReader::readString(const QString name, QString value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }
  return m_CurrentFilterIndex.value(name).toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> JsonFilterParametersReader::readStrings(const QString name, QVector<QString> value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
  QVector<QString> vector;
  for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
  {
    if ((*iter).isString())
    {
      QString str = (*iter).toString();
      vector.push_back(str);
    }
  }

  return vector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList JsonFilterParametersReader::readStringList(const QString name, QStringList value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
  QStringList vector;
  for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
  {
    if ((*iter).isString())
    {
      QString str = (*iter).toString();
      vector.push_back(str);
    }
  }

  return vector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int8_t JsonFilterParametersReader::readValue(const QString name, int8_t value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isDouble())
  {
    double val = m_CurrentFilterIndex.value(name).toDouble();
    if (val >= std::numeric_limits<int8_t>().min() && val <= std::numeric_limits<int8_t>().max())
    {
      return static_cast<int8_t>(val);
    }
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int16_t JsonFilterParametersReader::readValue(const QString name, int16_t value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isDouble())
  {
    double val = m_CurrentFilterIndex.value(name).toDouble();
    if (val >= std::numeric_limits<int16_t>().min() && val <= std::numeric_limits<int16_t>().max())
    {
      return static_cast<int16_t>(val);
    }
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t JsonFilterParametersReader::readValue(const QString name, int32_t value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isDouble())
  {
    double val = m_CurrentFilterIndex.value(name).toDouble();
    if (val >= std::numeric_limits<int32_t>().min() && val <= std::numeric_limits<int32_t>().max())
    {
      return static_cast<int32_t>(val);
    }
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t JsonFilterParametersReader::readValue(const QString name, int64_t value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isDouble())
  {
    double val = m_CurrentFilterIndex.value(name).toDouble();
    if (val >= std::numeric_limits<int64_t>().min() && val <= std::numeric_limits<int64_t>().max())
    {
      return static_cast<int64_t>(val);
    }
  }
  else if (m_CurrentFilterIndex.value(name).isString())
  {
    qint64 val = m_CurrentFilterIndex.value(name).toString().toLongLong();
    if (val >= std::numeric_limits<int64_t>().min() && val <= std::numeric_limits<int64_t>().max())
    {
      return static_cast<int64_t>(val);
    }
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint8_t JsonFilterParametersReader::readValue(const QString name, uint8_t value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isDouble())
  {
    double val = m_CurrentFilterIndex.value(name).toDouble();
    if (val >= std::numeric_limits<uint8_t>().min() && val <= std::numeric_limits<uint8_t>().max())
    {
      return static_cast<uint8_t>(val);
    }
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint16_t JsonFilterParametersReader::readValue(const QString name, uint16_t value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isDouble())
  {
    double val = m_CurrentFilterIndex.value(name).toDouble();
    if (val >= std::numeric_limits<uint16_t>().min() && val <= std::numeric_limits<uint16_t>().max())
    {
      return static_cast<uint16_t>(val);
    }
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t JsonFilterParametersReader::readValue(const QString name, uint32_t value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isDouble())
  {
    double val = m_CurrentFilterIndex.value(name).toDouble();
    if (val >= std::numeric_limits<uint32_t>().min() && val <= std::numeric_limits<uint32_t>().max())
    {
      return static_cast<uint32_t>(val);
    }
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t JsonFilterParametersReader::readValue(const QString name, uint64_t value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isDouble())
  {
    double val = m_CurrentFilterIndex.value(name).toDouble();
    if (val >= std::numeric_limits<uint64_t>().min() && val <= std::numeric_limits<uint64_t>().max())
    {
      return static_cast<uint64_t>(val);
    }
  }
  else if (m_CurrentFilterIndex.value(name).isString())
  {
    quint64 val = m_CurrentFilterIndex.value(name).toString().toULongLong();
    if (val <= std::numeric_limits<quint64>().max())
    {
      return static_cast<quint64>(val);
    }
  }
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float JsonFilterParametersReader::readValue(const QString name, float value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isDouble())
  {
    double val = m_CurrentFilterIndex.value(name).toDouble();
    if (val >= std::numeric_limits<float>().min() && val <= std::numeric_limits<float>().max())
    {
      return static_cast<float>(val);
    }
    else if (val >= -std::numeric_limits<float>().max() && val <= -std::numeric_limits<float>().min())
    {
      return static_cast<float>(val);
    }
    else if (val == 0.0f)
    {
      return static_cast<float>(val);
    }
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double JsonFilterParametersReader::readValue(const QString name, double value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isDouble())
  {
    return m_CurrentFilterIndex.value(name).toDouble();
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool JsonFilterParametersReader::readValue(const QString name, bool value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }
  int v = m_CurrentFilterIndex.value(name).toInt();
  return ( v == 0 ) ? false : true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int8_t> JsonFilterParametersReader::readArray(const QString name, QVector<int8_t> value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
    QVector<int8_t> vector;
    for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
    {
      if ((*iter).isDouble())
      {
        double val = (*iter).toDouble();
        if (val >= std::numeric_limits<int8_t>().min() && val <= std::numeric_limits<int8_t>().max())
        {
          vector.push_back(static_cast<int8_t>(val));
        }
      }
    }
    return vector;
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int16_t> JsonFilterParametersReader::readArray(const QString name, QVector<int16_t> value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
    QVector<int16_t> vector;
    for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
    {
      if ((*iter).isDouble())
      {
        double val = (*iter).toDouble();
        if (val >= std::numeric_limits<int16_t>().min() && val <= std::numeric_limits<int16_t>().max())
        {
          vector.push_back(static_cast<int16_t>(val));
        }
      }
    }
    return vector;
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int32_t> JsonFilterParametersReader::readArray(const QString name, QVector<int32_t> value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
    QVector<int32_t> vector;
    for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
    {
      if ((*iter).isDouble())
      {
        double val = (*iter).toDouble();
        if (val >= std::numeric_limits<int32_t>().min() && val <= std::numeric_limits<int32_t>().max())
        {
          vector.push_back(static_cast<int32_t>(val));
        }
      }
    }
    return vector;
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int64_t> JsonFilterParametersReader::readArray(const QString name, QVector<int64_t> value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
    QVector<int64_t> vector;
    for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
    {
      if ((*iter).isDouble())
      {
        double val = (*iter).toDouble();
        if (val >= std::numeric_limits<int64_t>().min() && val <= std::numeric_limits<int64_t>().max())
        {
          vector.push_back(static_cast<int64_t>(val));
        }
      }
      else if ((*iter).isString())
      {
        qint64 val = (*iter).toString().toLongLong();
        if (val >= std::numeric_limits<int64_t>().min() && val <= std::numeric_limits<int64_t>().max())
        {
          vector.push_back(static_cast<int64_t>(val));
        }
      }
    }
    return vector;
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint8_t> JsonFilterParametersReader::readArray(const QString name, QVector<uint8_t> value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
    QVector<uint8_t> vector;
    for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
    {
      if ((*iter).isDouble())
      {
        double val = (*iter).toDouble();
        if (val >= std::numeric_limits<uint8_t>().min() && val <= std::numeric_limits<uint8_t>().max())
        {
          vector.push_back(static_cast<uint8_t>(val));
        }
      }
    }
    return vector;
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint16_t> JsonFilterParametersReader::readArray(const QString name, QVector<uint16_t> value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
    QVector<uint16_t> vector;
    for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
    {
      if ((*iter).isDouble())
      {
        double val = (*iter).toDouble();
        if (val >= std::numeric_limits<uint16_t>().min() && val <= std::numeric_limits<uint16_t>().max())
        {
          vector.push_back(static_cast<uint16_t>(val));
        }
      }
    }
    return vector;
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint32_t> JsonFilterParametersReader::readArray(const QString name, QVector<uint32_t> value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
    QVector<uint32_t> vector;
    for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
    {
      if ((*iter).isDouble())
      {
        double val = (*iter).toDouble();
        if (val >= std::numeric_limits<uint32_t>().min() && val <= std::numeric_limits<uint32_t>().max())
        {
          vector.push_back(static_cast<uint32_t>(val));
        }
      }
    }
    return vector;
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint64_t> JsonFilterParametersReader::readArray(const QString name, QVector<uint64_t> value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
    QVector<uint64_t> vector;
    for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
    {
      if ((*iter).isDouble())
      {
        double val = (*iter).toDouble();
        if (val >= std::numeric_limits<uint64_t>().min() && val <= std::numeric_limits<uint64_t>().max())
        {
          vector.push_back(static_cast<uint64_t>(val));
        }
      }
      else if ((*iter).isString())
      {
        quint64 val = (*iter).toString().toULongLong();
        if (val <= std::numeric_limits<quint64>().max())
        {
          vector.push_back(static_cast<uint64_t>(val));
        }
      }
    }
    return vector;
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float> JsonFilterParametersReader::readArray(const QString name, QVector<float> value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
    QVector<float> vector;
    for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
    {
      if ((*iter).isDouble())
      {
        double val = (*iter).toDouble();
        if (val >= std::numeric_limits<float>().min() && val <= std::numeric_limits<float>().max())
        {
          vector.push_back(static_cast<float>(val));
        }
        else if (val >= -std::numeric_limits<float>().max() && val <= -std::numeric_limits<float>().min())
        {
          vector.push_back(static_cast<float>(val));
        }
        else if (val == 0.0f)
        {
          vector.push_back(static_cast<float>(val));
        }

      }
    }
    return vector;
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> JsonFilterParametersReader::readArray(const QString name, QVector<double> value)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return value;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex.value(name).toArray();
    QVector<double> vector;
    for (QJsonArray::iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter)
    {
      if ((*iter).isDouble())
      {
        vector.push_back((*iter).toDouble());
      }
    }
    return vector;
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3_t JsonFilterParametersReader::readIntVec3(const QString name, IntVec3_t defaultValue)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return defaultValue;
  }

  if (m_CurrentFilterIndex.value(name).isObject())
  {
    QJsonObject jsonObject = m_CurrentFilterIndex.value(name).toObject();
    IntVec3_t vec3;
    if (vec3.readJson(jsonObject) == true)
    {
      return vec3;
    }
  }

  return defaultValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3_t JsonFilterParametersReader::readFloatVec3(const QString name, FloatVec3_t defaultValue)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return defaultValue;
  }

  if (m_CurrentFilterIndex.value(name).isObject())
  {
    QJsonObject jsonObject = m_CurrentFilterIndex.value(name).toObject();
    FloatVec3_t vec3;
    if (vec3.readJson(jsonObject) == true)
    {
      return vec3;
    }
  }

  return defaultValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Float2ndOrderPoly_t JsonFilterParametersReader::readFloat2ndOrderPoly(const QString name, Float2ndOrderPoly_t defaultValue)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return defaultValue;
  }

  if (m_CurrentFilterIndex.value(name).isObject())
  {
    QJsonObject jsonObject = m_CurrentFilterIndex.value(name).toObject();
    Float2ndOrderPoly_t poly;
    if (poly.readJson(jsonObject) == true)
    {
      return poly;
    }
  }

  return defaultValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Float3rdOrderPoly_t JsonFilterParametersReader::readFloat3rdOrderPoly(const QString name, Float3rdOrderPoly_t defaultValue)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return defaultValue;
  }

  if (m_CurrentFilterIndex.value(name).isObject())
  {
    QJsonObject jsonObject = m_CurrentFilterIndex.value(name).toObject();
    Float3rdOrderPoly_t poly;
    if (poly.readJson(jsonObject) == true)
    {
      return poly;
    }
  }

  return defaultValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Float4thOrderPoly_t JsonFilterParametersReader::readFloat4thOrderPoly(const QString name, Float4thOrderPoly_t defaultValue)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return defaultValue;
  }

  if (m_CurrentFilterIndex.value(name).isObject())
  {
    QJsonObject jsonObject = m_CurrentFilterIndex.value(name).toObject();
    Float4thOrderPoly_t poly;
    if (poly.readJson(jsonObject) == true)
    {
      return poly;
    }
  }

  return defaultValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfo_t JsonFilterParametersReader::readFileListInfo(const QString name, FileListInfo_t defaultValue)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return defaultValue;
  }

  if (m_CurrentFilterIndex.value(name).isObject())
  {
    QJsonObject jsonObject = m_CurrentFilterIndex.value(name).toObject();
    FileListInfo_t fLInfo;
    if (fLInfo.readJson(jsonObject) == true)
    {
      return fLInfo;
    }
  }

  return defaultValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInput_t JsonFilterParametersReader::readComparisonInput(const QString name, ComparisonInput_t defaultValue, int arrayIndex)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return defaultValue;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray cInputsArray = m_CurrentFilterIndex.value(name).toArray();
    if (cInputsArray.size() > arrayIndex && cInputsArray[arrayIndex].isObject())
    {
      QJsonObject cInputObject = cInputsArray[arrayIndex].toObject();
      ComparisonInput_t cInput;
      if (cInput.readJson(cInputObject) == true)
      {
        return cInput;
      }
    }
  }

  return defaultValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInputs JsonFilterParametersReader::readComparisonInputs(const QString name, ComparisonInputs defValues)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return defValues;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray cInputsArray = m_CurrentFilterIndex.value(name).toArray();

    ComparisonInputs inputs;
    for (int i = 0; i < cInputsArray.size(); i++)
    {
      if (cInputsArray[i].isObject() == true)
      {
        QJsonObject cInputObj = cInputsArray[i].toObject();
        ComparisonInput_t cInput;
        if (cInput.readJson(cInputObj) == true)
        {
          inputs.addInput(cInput);
        }
        else
        {
          return defValues;
        }
      }
    }
    return inputs;
  }

  return defValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleInput_t JsonFilterParametersReader::readAxisAngle(const QString name, AxisAngleInput_t v, int vectorPos)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return v;
  }

  QJsonObject obj = m_CurrentFilterIndex.value(name).toObject();
  AxisAngleInput_t aInput;
  if (aInput.readJson(obj) == true)
  {
    return aInput;
  }

  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AxisAngleInput_t> JsonFilterParametersReader::readAxisAngles(const QString name, QVector<AxisAngleInput_t> v)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return v;
  }

  if (m_CurrentFilterIndex.value(name).isArray())
  {
    QJsonArray aInputsArray = m_CurrentFilterIndex.value(name).toArray();

    QVector<AxisAngleInput_t> inputs;
    for (int i = 0; i < aInputsArray.size(); i++)
    {
      if (aInputsArray[i].isObject() == true)
      {
        QJsonObject aInputObj = aInputsArray[i].toObject();
        AxisAngleInput_t aInput;
        if (aInput.readJson(aInputObj) == true)
        {
          inputs.push_back(aInput);
        }
        else
        {
          return v;
        }
      }
    }
    return inputs;
  }

  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> JsonFilterParametersReader::readArraySelections(const QString name, QSet<QString> v)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return v;
  }

  QSet<QString> set;

  if (m_CurrentFilterIndex[name].isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex[name].toArray();

    foreach(QJsonValue val, jsonArray)
    {
      if (val.isString())
      {
        set.insert(val.toString());
      }
    }

    return set;
  }

  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy JsonFilterParametersReader::readDataContainerArrayProxy(const QString& name, DataContainerArrayProxy v)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return v;
  }

  DataContainerArrayProxy proxy;
  if (m_CurrentFilterIndex[name].isObject())
  {
    QJsonObject obj = m_CurrentFilterIndex[name].toObject();
    proxy.readJson(obj);
    return proxy;
  }

  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath JsonFilterParametersReader::readDataArrayPath(const QString& name, DataArrayPath def)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return def;
  }

  DataArrayPath path;
  if (m_CurrentFilterIndex[name].isObject())
  {
    QJsonObject obj = m_CurrentFilterIndex[name].toObject();
    path.readJson(obj);
    return path;
  }

  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<DataArrayPath> JsonFilterParametersReader::readDataArrayPathVector(const QString& name, QVector<DataArrayPath> def)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return def;
  }

  QVector<DataArrayPath> paths;
  if (m_CurrentFilterIndex[name].isArray())
  {
    QJsonArray jsonArray = m_CurrentFilterIndex[name].toArray();
    foreach(QJsonValue val, jsonArray)
    {
      if (val.isObject())
      {
        DataArrayPath path;
        QJsonObject obj = val.toObject();
        path.readJson(obj);
        paths.push_back(path);
      }
    }
    return paths;
  }

  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData JsonFilterParametersReader::readDynamicTableData(const QString& name, DynamicTableData def)
{
  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return def;
  }

  DynamicTableData data;
  if (m_CurrentFilterIndex[name].isObject())
  {
    QJsonObject obj = m_CurrentFilterIndex[name].toObject();
    data.readJson(obj);
    return data;
  }

  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPair<double, double> JsonFilterParametersReader::readPairOfDoubles(const QString& name, QPair<double, double> v)
{
  //QJsonObject obj;
  //obj["Min"] = v.first;
  //obj["Max"] = v.second;

  //m_CurrentFilterIndex[name] = obj;



  Q_ASSERT(m_CurrentFilterIndex.isEmpty() == false);
  if (m_CurrentFilterIndex.contains(name) == false)
  {
    return v;
  }

  if (m_CurrentFilterIndex.value(name).isObject())
  {
    QJsonObject obj = m_CurrentFilterIndex.value(name).toObject();
    if (!obj.contains("Min") || !obj.contains("Max"))
    {
      return v;
    }

    QPair<double, double> pair;
    pair.first = obj["Min"].toDouble();
    pair.second = obj["Max"].toDouble();
    return pair;
  }

  return v;
}


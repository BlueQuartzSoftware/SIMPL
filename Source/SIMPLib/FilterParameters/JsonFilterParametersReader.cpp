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

  QJsonObject root = reader->getRoot();
  QJsonObject builderObj = root[SIMPL::Settings::PipelineBuilderGroup].toObject();
  int filterCount = builderObj[SIMPL::Settings::NumFilters].toInt();

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
    QJsonObject filterObj = root[QString::number(i)].toObject();
    QString filterName = filterObj[SIMPL::Settings::FilterName].toString();

    if (filterName.isEmpty() == false)
    {
      IFilterFactory::Pointer factory = filtManager->getFactoryForFilter(filterName);
      if (factory.get() != NULL)
      {
        AbstractFilter::Pointer filter = factory->create();

        if (NULL != filter.get())
        {
          filter->readFilterParametersFromJson(reader->m_Root, i);
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
QJsonObject JsonFilterParametersReader::getRoot()
{
  return m_Root;
}


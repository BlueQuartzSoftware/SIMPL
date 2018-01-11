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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "JsonFilterParametersWriter.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::JsonFilterParametersWriter()
:
 m_ExpandReaderFilters(true)
, m_MaxFilterIndex(-1)
, m_CurrentIndex(0)

{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::JsonFilterParametersWriter(QString& fileName, QString& pipelineName, int& numFilters)
: m_MaxFilterIndex(-1)
, m_CurrentIndex(0)
{
  m_FileName = fileName;
  m_PipelineName = pipelineName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::~JsonFilterParametersWriter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writePipelineToFile(FilterPipeline::Pointer pipeline, QString filePath, QString pipelineName, QList<IObserver *> obs)
{
  int err = 0;
  err = populateWriter(pipeline, pipelineName, obs);
  if(err < 0)
  {
    return err;
  }

  setFileName(filePath);
  writePipeline();
  clearWriter();
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JsonFilterParametersWriter::writePipelineToString(FilterPipeline::Pointer pipeline, QString pipelineName, QList<IObserver*> obs)
{
  populateWriter(pipeline, pipelineName, obs);

  QJsonDocument doc = toDocument();
  QString contents = QString::fromStdString(doc.toJson().toStdString());

  clearWriter();

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::populateWriter(FilterPipeline::Pointer pipeline, QString pipelineName, QList<IObserver*> obs)
{
  if(nullptr == pipeline.get())
  {
    if(obs.size() > 0)
    {
      PipelineMessage pm(JsonFilterParametersWriter::ClassName(), "FilterPipeline Object was nullptr for writing", -1, PipelineMessage::MessageType::Error);

      for (int i = 0; i < obs.size(); i++)
      {
        if (obs[i] != nullptr)
        {
          obs[i]->processPipelineMessage(pm);
        }
      }
    }
    return -1;
  }

  QFileInfo info(pipelineName);

  setPipelineName(info.completeBaseName());

  FilterPipeline::FilterContainerType& filters = pipeline->getFilterContainer();
  setMaxFilterIndex(filters.size());

  // Loop over each filter and write it's input parameters to the file
  int count = filters.size();
  int offset = 0;
  for(qint32 i = 0; i < count; ++i)
  {

    AbstractFilter::Pointer filter = filters.at(i);
    if(nullptr != filter.get())
    {
      DataContainerReader::Pointer reader = std::dynamic_pointer_cast<DataContainerReader>(filter);
      if(reader.get() && m_ExpandReaderFilters == true)
      {
        offset = reader->writeExistingPipelineToFile(m_Root, i);
      }
      openFilterGroup(filter.get(), i + offset);
      filter->writeFilterParameters(m_CurrentFilterIndex);
      closeFilterGroup();
    }
    else
    {
      AbstractFilter::Pointer badFilter = AbstractFilter::New();
      openFilterGroup(badFilter.get(), i + offset);
      m_CurrentFilterIndex["Unknown Filter"] = "ERROR: Filter instance was nullptr within the SVPipelineFilterWidget instance. Report this error to the DREAM3D Developers";
      closeFilterGroup();
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JsonFilterParametersWriter::writePipeline()
{
  // Write the contents
  if(m_FileName.isEmpty() == false)
  {
    QFile outputFile(m_FileName);
    QFileInfo info(outputFile);
    QString parentPath = info.absolutePath();
    QDir parentDir(parentPath);

    if(parentDir.exists() == false)
    {
      parentDir.mkpath(parentPath);
    }

    QJsonDocument doc = toDocument();

    if(outputFile.exists() == true)
    {
      outputFile.remove();
    }
    if(outputFile.open(QIODevice::WriteOnly))
    {
      outputFile.write(doc.toJson());
      outputFile.close();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JsonFilterParametersWriter::clearWriter()
{
  m_Root = QJsonObject();
  m_CurrentFilterIndex = QJsonObject();
  m_MaxFilterIndex = -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonDocument JsonFilterParametersWriter::toDocument()
{
  // Write our File Version and DREAM3D Version strings
  QJsonObject meta;
  meta[SIMPL::Settings::PipelineName] = m_PipelineName;
  meta[SIMPL::Settings::Version] = SIMPL::PipelineVersionNumbers::CurrentVersion;

  if(m_Root.size() > 0)
  {
    meta[SIMPL::Settings::NumFilters] = m_CurrentIndex + 1;
  }
  else
  {
    meta[SIMPL::Settings::NumFilters] = 0;
  }

  m_Root[SIMPL::Settings::PipelineBuilderGroup] = meta;
  QJsonDocument doc(m_Root);

  m_Root.remove(SIMPL::Settings::PipelineBuilderGroup);

  return doc;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject& JsonFilterParametersWriter::getCurrentGroupObject()
{
  return m_CurrentFilterIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::openFilterGroup(AbstractFilter* filter, int index)
{
  // We use the generateIndexString from this class and NOT the one from StringOperations
  // because this one is a slight variation on the one in StringOperations and works correctly
  // for SIMPL and DREAM3D. Don't change this code unless you test with Pipelines that are
  // 9, 10 and 11 filters long to make sure you can write them out and read them in again.
  m_CurrentIndex = index;
  QString numStr = generateIndexString(m_CurrentIndex);

  if(m_Root.contains(numStr))
  {
    m_CurrentFilterIndex = m_Root.value(numStr).toObject();
  }
  else
  {
    m_CurrentFilterIndex = QJsonObject();
    if(filter)
    {
      m_CurrentFilterIndex[SIMPL::Settings::FilterName] = filter->getNameOfClass();
      m_CurrentFilterIndex[SIMPL::Settings::HumanLabel] = filter->getHumanLabel();
      m_CurrentFilterIndex[SIMPL::Settings::FilterEnabled] = filter->getEnabled();
      m_CurrentFilterIndex[SIMPL::Settings::FilterUuid] = filter->getUuid().toString();
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::closeFilterGroup()
{
  // We use the generateIndexString from this class and NOT the one from StringOperations
  // because this one is a slight variation on the one in StringOperations and works correctly
  // for SIMPL and DREAM3D. Don't change this code unless you test with Pipelines that are
  // 9, 10 and 11 filters long to make sure you can write them out and read them in again.
  QString numStr = generateIndexString(m_CurrentIndex);
  m_Root[numStr] = m_CurrentFilterIndex;
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JsonFilterParametersWriter::generateIndexString(int currentIndex)
{
  Q_UNUSED(currentIndex);
  QString numStr = QString::number(m_CurrentIndex);

  if(m_MaxFilterIndex > 10)
  {
    int mag = 0;
    int max = m_MaxFilterIndex;
    while(max > 0)
    {
      mag++;
      max = max / 10;
    }
    numStr = "";             // Clear the string
    QTextStream ss(&numStr); // Create a QTextStream to set up the padding
    ss.setFieldWidth(mag);
    ss.setPadChar('0');
    ss << m_CurrentIndex;
  }
  return numStr;
}

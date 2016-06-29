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


#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

#include "SIMPLib/SIMPLibVersion.h"


#include "JsonFilterParametersWriter.h"
#include "SIMPLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::JsonFilterParametersWriter() :
  m_MaxFilterIndex(-1),
  m_CurrentIndex(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::JsonFilterParametersWriter(QString& fileName, QString& pipelineName, int& numFilters) :
  m_MaxFilterIndex(-1),
  m_CurrentIndex(0)
{
  m_FileName = fileName;
  m_PipelineName = pipelineName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::~JsonFilterParametersWriter()
{
  if (m_FileName.isEmpty() == false)
  {
    QFile outputFile(m_FileName);
    QFileInfo info(outputFile);
    QString parentPath = info.absolutePath();
    QDir parentDir(parentPath);

    if (parentDir.exists() == false)
    {
      parentDir.mkpath(parentPath);
    }

    QJsonDocument doc = toDocument();

    if (outputFile.exists() == true)
    {
      outputFile.remove();
    }
    if (outputFile.open(QIODevice::WriteOnly))
    {
      outputFile.write(doc.toJson());
      outputFile.close();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JsonFilterParametersWriter::writePipelineToFile(FilterPipeline::Pointer pipeline, QString filePath, QString pipelineName, IObserver* obs)
{
  JsonFilterParametersWriter::Pointer writer = createAndPopulateWriter(pipeline, pipelineName, obs);
  writer->setFileName(filePath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JsonFilterParametersWriter::writePipelineToString(FilterPipeline::Pointer pipeline, QString pipelineName, IObserver* obs)
{
  JsonFilterParametersWriter::Pointer writer = createAndPopulateWriter(pipeline, pipelineName, obs);

  QJsonDocument doc = writer->toDocument();
  QString contents = QString::fromStdString(doc.toJson().toStdString());

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::Pointer JsonFilterParametersWriter::createAndPopulateWriter(FilterPipeline::Pointer pipeline, QString pipelineName, IObserver* obs)
{
  if (NULL == pipeline.get())
  {
    if (NULL != obs)
    {
      PipelineMessage pm(JsonFilterParametersWriter::ClassName(), "FilterPipeline Object was NULL for writing", -1, PipelineMessage::Error);
      obs->processPipelineMessage(pm);
    }
    return JsonFilterParametersWriter::NullPointer();
  }

  QFileInfo info(pipelineName);

  // WRITE THE PIPELINE TO THE JSON FILE
  setPipelineName(info.completeBaseName());

  FilterPipeline::FilterContainerType& filters = pipeline->getFilterContainer();
  setMaxFilterIndex(filters.size());

  // Loop over each filter and write it's input parameters to the file
  int count = filters.size();
  for (qint32 i = 0; i < count; ++i)
  {
    AbstractFilter::Pointer filter = filters.at(i);
    if (NULL != filter.get())
    {
      QJsonObject fpObj = filter->writeFilterParametersToJson();
    }
    else
    {
      AbstractFilter::Pointer badFilter = AbstractFilter::New();
      writer->openFilterGroup(badFilter.get(), i);
      writer->writeValue("Unknown Filter", "ERROR: Filter instance was NULL within the SVPipelineFilterWidget instance. Report this error to the DREAM3D Developers");
      writer->closeFilterGroup();
    }
  }

  return writer;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonDocument JsonFilterParametersWriter::toDocument()
{
  // Write our File Version and DREAM3D Version strings
  QJsonObject meta;
  meta[SIMPL::Settings::PipelineName] = m_PipelineName;
  meta[SIMPL::Settings::Version] = SIMPLib::Version::Package();

  if (m_Root.size() > 0)
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
  m_CurrentIndex = index;
  QString numStr = generateIndexString(m_CurrentIndex);

  if (m_Root.contains(numStr))
  {
    m_CurrentFilterIndex = m_Root.value(numStr).toObject();
  }
  else
  {
    m_CurrentFilterIndex = QJsonObject();
    if(filter)
    {
      writeValue(SIMPL::Settings::FilterName, filter->getNameOfClass());
      writeValue(SIMPL::Settings::HumanLabel, filter->getHumanLabel());
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::closeFilterGroup()
{
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
    while(max > 0) {
      mag++;
      max = max / 10;
    }
    numStr = ""; // Clear the string
    QTextStream ss(&numStr); // Create a QTextStream to set up the padding
    ss.setFieldWidth(mag);
    ss.setPadChar('0');
    ss << m_CurrentIndex;
  }
  return numStr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject JsonFilterParametersWriter::getRoot()
{
  return m_Root;
}



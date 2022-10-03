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

#include "MultiInputFileFilterParameter.h"

#include <QtCore/QJsonArray>
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiInputFileFilterParameter::MultiInputFileFilterParameter()
: m_FileExtension("")
, m_FileType("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiInputFileFilterParameter::~MultiInputFileFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiInputFileFilterParameter::Pointer MultiInputFileFilterParameter::Create(const QString& humanLabel, const QString& propertyName, const VecString& defaultValue, Category category,
                                                                             const SetterCallbackType& setterCallback, const GetterCallbackType& getterCallback, const QString& fileExtension,
                                                                             const QString& fileType, const std::vector<int>& groupIndices)
{
  MultiInputFileFilterParameter::Pointer ptr = MultiInputFileFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  // ptr->setDefaultValue(defaultValue);
  ptr->setCategory(category);
  ptr->setFileExtension(fileExtension);
  ptr->setFileType(fileType);
  ptr->setGroupIndices(groupIndices);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiInputFileFilterParameter::getWidgetType() const
{
  return QString("MultiInputFileWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileFilterParameter::readJson(const QJsonObject& json)
{
  QJsonArray jFiles = json[getPropertyName()].toArray();
  if(m_SetterCallback)
  {
    std::vector<std::string> files;
    files.reserve(jFiles.size());
    for(const auto filePath : jFiles)
    {
      files.push_back(filePath.toString().toStdString());
    }
    m_SetterCallback(files);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileFilterParameter::writeJson(QJsonObject& json) const
{
  if(m_GetterCallback)
  {
    std::vector<std::string> files = m_GetterCallback();
    QJsonArray jFiles;
    for(const auto& filePath : files)
    {
      jFiles.append(QString::fromStdString(filePath));
    }
    json[getPropertyName()] = jFiles;
  }
}

// -----------------------------------------------------------------------------
MultiInputFileFilterParameter::Pointer MultiInputFileFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
MultiInputFileFilterParameter::Pointer MultiInputFileFilterParameter::New()
{
  Pointer sharedPtr(new(MultiInputFileFilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString MultiInputFileFilterParameter::getNameOfClass() const
{
  return QString("MultiInputFileFilterParameter");
}

// -----------------------------------------------------------------------------
QString MultiInputFileFilterParameter::ClassName()
{
  return QString("MultiInputFileFilterParameter");
}

// -----------------------------------------------------------------------------
void MultiInputFileFilterParameter::setFileExtension(const QString& value)
{
  m_FileExtension = value;
}

// -----------------------------------------------------------------------------
QString MultiInputFileFilterParameter::getFileExtension() const
{
  return m_FileExtension;
}

// -----------------------------------------------------------------------------
void MultiInputFileFilterParameter::setFileType(const QString& value)
{
  m_FileType = value;
}

// -----------------------------------------------------------------------------
QString MultiInputFileFilterParameter::getFileType() const
{
  return m_FileType;
}

// -----------------------------------------------------------------------------
void MultiInputFileFilterParameter::setSetterCallback(const MultiInputFileFilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
MultiInputFileFilterParameter::SetterCallbackType MultiInputFileFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void MultiInputFileFilterParameter::setGetterCallback(const MultiInputFileFilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
MultiInputFileFilterParameter::GetterCallbackType MultiInputFileFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

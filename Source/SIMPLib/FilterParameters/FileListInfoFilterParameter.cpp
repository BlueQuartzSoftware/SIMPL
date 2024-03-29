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

#include "FileListInfoFilterParameter.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfoFilterParameter::FileListInfoFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfoFilterParameter::~FileListInfoFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfoFilterParameter::Pointer FileListInfoFilterParameter::Create(const QString& humanLabel, const QString& propertyName, const StackFileListInfo& defaultValue,
                                                                         FilterParameter::Category category, SetterCallbackType setterCallback, GetterCallbackType getterCallback)
{

  FileListInfoFilterParameter::Pointer ptr = FileListInfoFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FileListInfoFilterParameter::getWidgetType() const
{
  return QString("FileListInfoWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonObject jsonObj = jsonValue.toObject();
    StackFileListInfo fileListInfo;
    fileListInfo.readJson(jsonObj);
    m_SetterCallback(fileListInfo);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoFilterParameter::writeJson(QJsonObject& json) const
{
  if(m_GetterCallback)
  {
    StackFileListInfo fileListInfo = m_GetterCallback();
    QJsonObject jsonObj;
    fileListInfo.writeJson(jsonObj);
    json[getPropertyName()] = jsonObj;
  }
}

// -----------------------------------------------------------------------------
FileListInfoFilterParameter::Pointer FileListInfoFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
FileListInfoFilterParameter::Pointer FileListInfoFilterParameter::New()
{
  Pointer sharedPtr(new(FileListInfoFilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString FileListInfoFilterParameter::getNameOfClass() const
{
  return QString("FileListInfoFilterParameter");
}

// -----------------------------------------------------------------------------
QString FileListInfoFilterParameter::ClassName()
{
  return QString("FileListInfoFilterParameter");
}

// -----------------------------------------------------------------------------
void FileListInfoFilterParameter::setSetterCallback(const FileListInfoFilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
FileListInfoFilterParameter::SetterCallbackType FileListInfoFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void FileListInfoFilterParameter::setGetterCallback(const FileListInfoFilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
FileListInfoFilterParameter::GetterCallbackType FileListInfoFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

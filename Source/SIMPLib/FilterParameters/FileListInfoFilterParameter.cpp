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

#include "FileListInfoFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfoFilterParameter::FileListInfoFilterParameter() :
  FilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfoFilterParameter::~FileListInfoFilterParameter()
{}

//************************** OLD FP API *******************************
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfoFilterParameter::Pointer FileListInfoFilterParameter::New(const QString& humanLabel, const QString& propertyName,
    const FileListInfo_t& defaultValue,
    FilterParameter::Category category)
{

  FileListInfoFilterParameter::Pointer ptr = FileListInfoFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);

  return ptr;
}
//************************** OLD FP API *******************************

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfoFilterParameter::Pointer FileListInfoFilterParameter::New(const QString& humanLabel, const QString& propertyName,
    const FileListInfo_t& defaultValue, FilterParameter::Category category, SetterCallbackType setterCallback, GetterCallbackType getterCallback)
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
QString FileListInfoFilterParameter::getWidgetType()
{
  return QString("FileListInfoWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoFilterParameter::readJson(const QJsonObject &json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(!jsonValue.isUndefined() )
  {
    QJsonObject jsonObj = jsonValue.toObject();
    FileListInfo_t fileListInfo;

    fileListInfo.PaddingDigits = static_cast<qint32>(jsonObj["PaddingDigits"].toDouble());
    fileListInfo.Ordering = static_cast<quint32>(jsonObj["Ordering"].toDouble());
    fileListInfo.StartIndex = static_cast<qint32>(jsonObj["StartIndex"].toDouble());
    fileListInfo.EndIndex = static_cast<qint32>(jsonObj["EndIndex"].toDouble());
    fileListInfo.InputPath = jsonObj["InputPath"].toString();
    fileListInfo.FilePrefix = jsonObj["FilePrefix"].toString();
    fileListInfo.FileSuffix = jsonObj["FileSuffix"].toString();
    fileListInfo.FileExtension = jsonObj["FileExtension"].toString();

    m_SetterCallback(fileListInfo);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoFilterParameter::writeJson(QJsonObject &json)
{
  FileListInfo_t fileListInfo = m_GetterCallback();

  QJsonObject jsonObj;
  jsonObj["PaddingDigits"] = static_cast<double>(fileListInfo.PaddingDigits);
  jsonObj["Ordering"] = static_cast<double>(fileListInfo.Ordering);
  jsonObj["StartIndex"] = static_cast<double>(fileListInfo.StartIndex);
  jsonObj["EndIndex"] = static_cast<double>(fileListInfo.EndIndex);
  jsonObj["InputPath"] = fileListInfo.InputPath;
  jsonObj["FilePrefix"] = fileListInfo.FilePrefix;
  jsonObj["FileSuffix"] = fileListInfo.FileSuffix;
  jsonObj["FileExtension"] = fileListInfo.FileExtension;

  json[getPropertyName()] = jsonObj;
}


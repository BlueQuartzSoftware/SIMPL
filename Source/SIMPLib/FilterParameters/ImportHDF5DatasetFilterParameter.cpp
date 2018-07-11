/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImportHDF5DatasetFilterParameter.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include "SIMPLib/CoreFilters/ImportHDF5Dataset.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5DatasetFilterParameter::ImportHDF5DatasetFilterParameter()
: FilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5DatasetFilterParameter::~ImportHDF5DatasetFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5DatasetFilterParameter::Pointer ImportHDF5DatasetFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QVariant& filePathDefaultValue,
                                                                                const QVariant& datasetDefaultValue, Category category, ImportHDF5Dataset* filter, int groupIndex)
{
  ImportHDF5DatasetFilterParameter::Pointer ptr = ImportHDF5DatasetFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setDefaultValue(filePathDefaultValue);
  ptr->setDataSetDefaultValue(datasetDefaultValue);
  ptr->setFilter(filter);
  ptr->setCategory(category);

  ptr->setGroupIndex(groupIndex);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportHDF5DatasetFilterParameter::getWidgetType() const
{
  return QString("ImportHDF5DatasetWidget");
}

// -----------------------------------------------------------------------------
// THIS IS A SPECIAL CASE AND IS NOT STANDARD.  DO NOT COPY THIS CODE.
// -----------------------------------------------------------------------------
void ImportHDF5DatasetFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json["HDF5FilePath"];
  if(!jsonValue.isUndefined())
  {
    m_Filter->setHDF5FilePath(jsonValue.toString(""));
  }

  QJsonArray importInfoArray = json["Dataset Import Info Array"].toArray();
  QList<ImportHDF5Dataset::DatasetImportInfo> importInfoList;
  for(int i = 0; i < importInfoArray.size(); i++)
  {
    QJsonObject importInfoObj = importInfoArray[i].toObject();

    ImportHDF5Dataset::DatasetImportInfo importInfo;
    importInfo.readJson(importInfoObj);
    importInfoList.push_back(importInfo);
  }
  m_Filter->setDatasetImportInfoList(importInfoList);
}

// -----------------------------------------------------------------------------
// THIS IS A SPECIAL CASE AND IS NOT STANDARD.  DO NOT COPY THIS CODE.
// -----------------------------------------------------------------------------
void ImportHDF5DatasetFilterParameter::writeJson(QJsonObject& json)
{
  json["HDF5FilePath"] = m_Filter->getHDF5FilePath();

  QList<ImportHDF5Dataset::DatasetImportInfo> importInfoList = m_Filter->getDatasetImportInfoList();
  QJsonArray importInfoArray;
  for(int i = 0; i < importInfoList.size(); i++)
  {
    ImportHDF5Dataset::DatasetImportInfo importInfo = importInfoList[i];

    QJsonObject importInfoObj;
    importInfo.writeJson(importInfoObj);
    importInfoArray.push_back(importInfoObj);
  }
  json["Dataset Import Info Array"] = importInfoArray;
}

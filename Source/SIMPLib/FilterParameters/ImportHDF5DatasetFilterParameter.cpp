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

#include "ImportHDF5DatasetFilterParameter.h"

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
ImportHDF5DatasetFilterParameter::~ImportHDF5DatasetFilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5DatasetFilterParameter::Pointer ImportHDF5DatasetFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QVariant& defaultValue, Category category, ImportHDF5Dataset* filter, int groupIndex)
{
  ImportHDF5DatasetFilterParameter::Pointer ptr = ImportHDF5DatasetFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setDefaultValue(defaultValue);
  ptr->setCategory(category);
  ptr->setFilter(filter);
  ptr->setGroupIndex(groupIndex);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportHDF5DatasetFilterParameter::getWidgetType()
{
  return QString("ImportHDF5DatasetWidget");
}

// -----------------------------------------------------------------------------
// THIS IS A SPECIAL CASE AND IS NOT STANDARD.  DO NOT COPY THIS CODE.
// -----------------------------------------------------------------------------
void ImportHDF5DatasetFilterParameter::readJson(const QJsonObject& json)
{
  if (json.contains("HDF5FilePath") && json["HDF5FilePath"].isString())
  {
    m_Filter->setHDF5FilePath(json["HDF5FilePath"].toString());
    m_Filter->setDatasetPath(json["DatasetPath"].toString());
  }
}

// -----------------------------------------------------------------------------
// THIS IS A SPECIAL CASE AND IS NOT STANDARD.  DO NOT COPY THIS CODE.
// -----------------------------------------------------------------------------
void ImportHDF5DatasetFilterParameter::writeJson(QJsonObject& json)
{
  json["HDF5FilePath"] = m_Filter->getHDF5FilePath();
  json["DatasetPath"] = m_Filter->getDatasetPath();
}

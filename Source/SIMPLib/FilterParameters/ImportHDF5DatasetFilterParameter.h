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

#pragma once

#include "SIMPLib/FilterParameters/FilterParameter.h"

class ImportHDF5Dataset;

class SIMPLib_EXPORT ImportHDF5DatasetFilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(ImportHDF5DatasetFilterParameter)
  SIMPL_STATIC_NEW_MACRO(ImportHDF5DatasetFilterParameter)
  SIMPL_TYPE_MACRO(ImportHDF5DatasetFilterParameter)

  static Pointer New(const QString& humanLabel, const QString& propertyName, const QVariant& filePathDefaultValue, const QVariant& datasetDefaultValue, Category category, ImportHDF5Dataset* filter,
                     int groupIndex = -1);

  ~ImportHDF5DatasetFilterParameter() override;

  SIMPL_VIRTUAL_INSTANCE_PROPERTY(QVariant, DataSetDefaultValue)
  SIMPL_POINTER_PROPERTY(ImportHDF5Dataset, Filter)

  QString getWidgetType() const override;

  /**
   * @brief readJson Reads this filter parameter's corresponding property out of a QJsonObject.
   * @param json The QJsonObject that the filter parameter reads from.
   */
  void readJson(const QJsonObject& json) override;

  /**
   * @brief writeJson Writes this filter parameter's corresponding property to a QJsonObject.
   * @param json The QJsonObject that the filter parameter writes to.
   */
  void writeJson(QJsonObject& json) override;

protected:
  ImportHDF5DatasetFilterParameter();

private:
  ImportHDF5DatasetFilterParameter(const ImportHDF5DatasetFilterParameter&) = delete; // Copy Constructor Not Implemented
  void operator=(const ImportHDF5DatasetFilterParameter&);                            // Move assignment Not Implemented
};

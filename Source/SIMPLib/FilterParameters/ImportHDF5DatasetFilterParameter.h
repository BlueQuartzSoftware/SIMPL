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

#include <memory>

#include "SIMPLib/FilterParameters/FilterParameter.h"

class ImportHDF5Dataset;

class SIMPLib_EXPORT ImportHDF5DatasetFilterParameter : public FilterParameter
{
public:
  using Self = ImportHDF5DatasetFilterParameter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for ImportHDF5DatasetFilterParameter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ImportHDF5DatasetFilterParameter
   */
  static QString ClassName();

  static Pointer New(const QString& humanLabel, const QString& propertyName, const QVariant& filePathDefaultValue, const QVariant& datasetDefaultValue, Category category, ImportHDF5Dataset* filter,
                     int groupIndex = -1);

  ~ImportHDF5DatasetFilterParameter() override;

  /**
   * @brief Setter property for DataSetDefaultValue
   */
  virtual void setDataSetDefaultValue(const QVariant& value);

  /**
   * @brief Setter property for Filter
   */
  void setFilter(ImportHDF5Dataset* value);
  /**
   * @brief Getter property for Filter
   * @return Value of Filter
   */
  ImportHDF5Dataset* getFilter() const;

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

public:
  ImportHDF5DatasetFilterParameter(const ImportHDF5DatasetFilterParameter&) = delete;            // Copy Constructor Not Implemented
  ImportHDF5DatasetFilterParameter(ImportHDF5DatasetFilterParameter&&) = delete;                 // Move Constructor Not Implemented
  ImportHDF5DatasetFilterParameter& operator=(const ImportHDF5DatasetFilterParameter&) = delete; // Copy Assignment Not Implemented
  ImportHDF5DatasetFilterParameter& operator=(ImportHDF5DatasetFilterParameter&&) = delete;      // Move Assignment Not Implemented

private:
  QVariant m_DataSetDefaultValue = {};

  ImportHDF5Dataset* m_Filter = nullptr;
};

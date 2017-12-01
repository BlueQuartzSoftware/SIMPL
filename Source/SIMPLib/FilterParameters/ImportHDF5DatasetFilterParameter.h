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

#ifndef _importhdf5datasetfilterparameter_h_
#define _importhdf5datasetfilterparameter_h_

#include "SIMPLib/FilterParameters/FilterParameter.h"

class ImportHDF5Dataset;

class SIMPLib_EXPORT ImportHDF5DatasetFilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(ImportHDF5DatasetFilterParameter)
  SIMPL_STATIC_NEW_MACRO(ImportHDF5DatasetFilterParameter)
  SIMPL_TYPE_MACRO(ImportHDF5DatasetFilterParameter)

  typedef std::function<void(QString)> SetterCallbackType;
  typedef std::function<QString(void)> GetterCallbackType;

  static Pointer New(const QString& humanLabel, const QString& propertyName, const QVariant& filePathDefaultValue, const QVariant& datasetDefaultValue, Category category,
                     SetterCallbackType filePathSetterCallback, GetterCallbackType filePathGetterCallback, SetterCallbackType dataSetSetterCallback, GetterCallbackType dataSetGetterCallback,
                     int groupIndex = -1);

  virtual ~ImportHDF5DatasetFilterParameter();

  SIMPL_VIRTUAL_INSTANCE_PROPERTY(QVariant, DataSetDefaultValue)

  QString getWidgetType();

  /**
 * @brief readJson Reads this filter parameter's corresponding property out of a QJsonObject.
 * @param json The QJsonObject that the filter parameter reads from.
 */
  void readJson(const QJsonObject& json);

  /**
 * @brief writeJson Writes this filter parameter's corresponding property to a QJsonObject.
 * @param json The QJsonObject that the filter parameter writes to.
 */
  void writeJson(QJsonObject& json);

  /**
  * @param SetterCallback The method in the AbstractFilter subclass that <i>sets</i> the value of the property
  * that this FilterParameter subclass represents.
  * from the filter parameter.
  */
  SIMPL_INSTANCE_PROPERTY(SetterCallbackType, FilePathSetterCallback)

  /**
  * @param GetterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
  * that this FilterParameter subclass represents.
  * @return The GetterCallback
  */
  SIMPL_INSTANCE_PROPERTY(GetterCallbackType, FilePathGetterCallback)

  /**
  * @param SetterCallback The method in the AbstractFilter subclass that <i>sets</i> the value of the property
  * that this FilterParameter subclass represents.
  * from the filter parameter.
  */
  SIMPL_INSTANCE_PROPERTY(SetterCallbackType, DataSetSetterCallback)

  /**
  * @param GetterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
  * that this FilterParameter subclass represents.
  * @return The GetterCallback
  */
  SIMPL_INSTANCE_PROPERTY(GetterCallbackType, DataSetGetterCallback)

protected:
  ImportHDF5DatasetFilterParameter();

private:
  ImportHDF5DatasetFilterParameter(const ImportHDF5DatasetFilterParameter&) = delete; // Copy Constructor Not Implemented
  void operator=(const ImportHDF5DatasetFilterParameter&);                   // Operator '=' Not Implemented
};

#endif /* _importhdf5datasetfilterparameter_h_ */

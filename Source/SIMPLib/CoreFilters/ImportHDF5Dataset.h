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

#include <QtCore/QJsonObject>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The ImportHDF5Dataset class. See [Filter documentation](@ref readhdf5file) for details.
 */
class SIMPLib_EXPORT ImportHDF5Dataset : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ImportHDF5Dataset SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(QString HDF5FilePath READ getHDF5FilePath WRITE setHDF5FilePath)
  PYB11_PROPERTY(QString HDF5Dimensions READ getHDF5Dimensions)
  PYB11_PROPERTY(QList<DatasetImportInfo> DatasetImportInfoList READ getDatasetImportInfoList WRITE setDatasetImportInfoList)
  PYB11_PROPERTY(DataArrayPath SelectedAttributeMatrix READ getSelectedAttributeMatrix WRITE setSelectedAttributeMatrix)

public:
  SIMPL_SHARED_POINTERS(ImportHDF5Dataset)
  SIMPL_FILTER_NEW_MACRO(ImportHDF5Dataset)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ImportHDF5Dataset, AbstractFilter)

  ~ImportHDF5Dataset() override;

  struct DatasetImportInfo
  {
    QString dataSetPath;
    QString componentDimensions;

    void readJson(QJsonObject json)
    {
      dataSetPath = json["Dataset Path"].toString();
      componentDimensions = json["Component Dimensions"].toString();
    }

    void writeJson(QJsonObject& json)
    {
      json["Dataset Path"] = dataSetPath;
      json["Component Dimensions"] = componentDimensions;
    }
  };

  SIMPL_FILTER_PARAMETER(QString, HDF5FilePath)
  Q_PROPERTY(QString HDF5FilePath READ getHDF5FilePath WRITE setHDF5FilePath)

  SIMPL_FILTER_PARAMETER(QList<DatasetImportInfo>, DatasetImportInfoList)
  Q_PROPERTY(QList<DatasetImportInfo> DatasetImportInfoList READ getDatasetImportInfoList WRITE setDatasetImportInfoList)

  QString getHDF5Dimensions();
  Q_PROPERTY(QString HDF5Dimensions READ getHDF5Dimensions)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SelectedAttributeMatrix)
  Q_PROPERTY(DataArrayPath SelectedAttributeMatrix READ getSelectedAttributeMatrix WRITE setSelectedAttributeMatrix)

  SIMPL_FILTER_PARAMETER(QStringList, DatasetPathsWithErrors)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
   * @brief preflight Reimplemented from @see AbstractFilter class
   */
  void preflight() override;

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  ImportHDF5Dataset();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  QString m_HDF5Dimensions = "";

  IDataArray::Pointer readIDataArray(hid_t gid, const QString& name, size_t numOfTuples, QVector<size_t> cDims, bool metaDataOnly);

  /**
   * @brief createComponentDimensions
   * @return
   */
  QVector<size_t> createComponentDimensions(const QString& cDimsStr);

public:
  ImportHDF5Dataset(const ImportHDF5Dataset&) = delete;            // Copy Constructor Not Implemented
  ImportHDF5Dataset(ImportHDF5Dataset&&) = delete;                 // Move Constructor Not Implemented
  ImportHDF5Dataset& operator=(const ImportHDF5Dataset&) = delete; // Copy Assignment Not Implemented
  ImportHDF5Dataset& operator=(ImportHDF5Dataset&&) = delete;      // Move Assignment Not Implemented
};
Q_DECLARE_METATYPE(ImportHDF5Dataset::DatasetImportInfo)

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

#include <hdf5.h>

#include <QtCore/QJsonObject>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

/**
 * @brief The ImportHDF5Dataset class. See [Filter documentation](@ref readhdf5file) for details.
 */
class SIMPLib_EXPORT ImportHDF5Dataset : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(ImportHDF5Dataset SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(ImportHDF5Dataset)
  PYB11_FILTER_NEW_MACRO(ImportHDF5Dataset)
  PYB11_FILTER_PARAMETER(QString, HDF5FilePath)
  PYB11_FILTER_PARAMETER(QList<ImportHDF5Dataset::DatasetImportInfo>, DatasetImportInfoList)
  PYB11_FILTER_PARAMETER(DataArrayPath, SelectedAttributeMatrix)
  PYB11_FILTER_PARAMETER(QStringList, DatasetPathsWithErrors)
  PYB11_PROPERTY(QString HDF5FilePath READ getHDF5FilePath WRITE setHDF5FilePath)
  PYB11_PROPERTY(QString HDF5Dimensions READ getHDF5Dimensions)
  PYB11_PROPERTY(QList<ImportHDF5Dataset::DatasetImportInfo> DatasetImportInfoList READ getDatasetImportInfoList WRITE setDatasetImportInfoList)
  PYB11_PROPERTY(DataArrayPath SelectedAttributeMatrix READ getSelectedAttributeMatrix WRITE setSelectedAttributeMatrix)
#endif

public:
  using Self = ImportHDF5Dataset;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer();

  static std::shared_ptr<ImportHDF5Dataset> New();

  /**
   * @brief Returns the name of the class for ImportHDF5Dataset
   */
  const QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ImportHDF5Dataset
   */
  static QString ClassName();

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

  /**
   * @brief Setter property for HDF5FilePath
   */
  void setHDF5FilePath(const QString& value);
  /**
   * @brief Getter property for HDF5FilePath
   * @return Value of HDF5FilePath
   */
  QString getHDF5FilePath() const;

  Q_PROPERTY(QString HDF5FilePath READ getHDF5FilePath WRITE setHDF5FilePath)

  /**
   * @brief Setter property for DatasetImportInfoList
   */
  void setDatasetImportInfoList(const QList<ImportHDF5Dataset::DatasetImportInfo>& value);
  /**
   * @brief Getter property for DatasetImportInfoList
   * @return Value of DatasetImportInfoList
   */
  QList<ImportHDF5Dataset::DatasetImportInfo> getDatasetImportInfoList() const;

  Q_PROPERTY(QList<DatasetImportInfo> DatasetImportInfoList READ getDatasetImportInfoList WRITE setDatasetImportInfoList)

  QString getHDF5Dimensions();
  Q_PROPERTY(QString HDF5Dimensions READ getHDF5Dimensions)

  /**
   * @brief Setter property for SelectedAttributeMatrix
   */
  void setSelectedAttributeMatrix(const DataArrayPath& value);
  /**
   * @brief Getter property for SelectedAttributeMatrix
   * @return Value of SelectedAttributeMatrix
   */
  DataArrayPath getSelectedAttributeMatrix() const;

  Q_PROPERTY(DataArrayPath SelectedAttributeMatrix READ getSelectedAttributeMatrix WRITE setSelectedAttributeMatrix)

  /**
   * @brief Setter property for DatasetPathsWithErrors
   */
  void setDatasetPathsWithErrors(const QStringList& value);
  /**
   * @brief Getter property for DatasetPathsWithErrors
   * @return Value of DatasetPathsWithErrors
   */
  QStringList getDatasetPathsWithErrors() const;

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
  QString m_HDF5FilePath = {};
  QList<ImportHDF5Dataset::DatasetImportInfo> m_DatasetImportInfoList = {};
  DataArrayPath m_SelectedAttributeMatrix = {};
  QStringList m_DatasetPathsWithErrors = {};

  QString m_HDF5Dimensions = "";

  IDataArray::Pointer readIDataArray(hid_t gid, const QString& name, size_t numOfTuples, std::vector<size_t> cDims, bool metaDataOnly);

  /**
   * @brief createComponentDimensions
   * @return
   */
  std::vector<size_t> createComponentDimensions(const QString& cDimsStr);

public:
  ImportHDF5Dataset(const ImportHDF5Dataset&) = delete;            // Copy Constructor Not Implemented
  ImportHDF5Dataset(ImportHDF5Dataset&&) = delete;                 // Move Constructor Not Implemented
  ImportHDF5Dataset& operator=(const ImportHDF5Dataset&) = delete; // Copy Assignment Not Implemented
  ImportHDF5Dataset& operator=(ImportHDF5Dataset&&) = delete;      // Move Assignment Not Implemented
};
Q_DECLARE_METATYPE(ImportHDF5Dataset::DatasetImportInfo)

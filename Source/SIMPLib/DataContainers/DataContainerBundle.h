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

#pragma once

#include <memory>

#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/IDataContainerBundle.h"

class SIMPLib_EXPORT DataContainerBundle : public IDataContainerBundle
{
  Q_OBJECT
public:
  using Self = DataContainerBundle;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for DataContainerBundle
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for DataContainerBundle
   */
  static QString ClassName();

  ~DataContainerBundle() override;

  /**
   * @brief Creates a new DataContainerBundle
   * @param name The Name of the Bundle
   * @return
   */
  static Pointer New(const QString& name)
  {
    Pointer sharedPtr(new DataContainerBundle);
    sharedPtr->setName(name);
    return sharedPtr;
  }

  /**
   * @brief GetMetaDataName
   * @return Returns the name of the Meta Data AttributeMatrix name. This name should be used for any AttributeMatrix
   * that is used in a DataContainer to hold the type of data that is used to group DataContainers into bundles.
   */
  static QString GetMetaDataName();

  /**
   * @brief The name of this DataContainerBundle
   */
  /**
   * @brief Setter property for Name
   */
  void setName(const QString& value) override;
  /**
   * @brief Getter property for Name
   * @return Value of Name
   */
  QString getName() const override;

  /*
   * @brief This is the list of array names that reside inside the "Meta Data" attribute matrix that are the
   * pieces of data that define why the data containers were grouped into a bundle. For example, if you have a time
   * series set of data where each Data Container is a step in "time" and there are several values for time:
   * @li Time Value Real value of time
   * @li Time Step Value Time Step value in the simulation
   * @li Time Index The raw index of the data, i.e., this is the actual ordering that could also be used as then index
   * into an array of data containers
   */
  /**
   * @brief Setter property for MetaDataArrays
   */
  void setMetaDataArrays(const QStringList& value);
  /**
   * @brief Getter property for MetaDataArrays
   * @return Value of MetaDataArrays
   */
  QStringList getMetaDataArrays() const;

  /**
   * @brief Sets All the DataContainers for this bundle
   * @param containers
   */
  void setDataContainers(QVector<DataContainer::Pointer>& containers);
  QVector<QString> getDataContainerNames() override;

  /**
   * @brief addDataContainer Adds a Data Container to the bundle
   * @param dc
   */
  void addOrReplaceDataContainer(DataContainer::Pointer dc) override;

  /**
   * @brief removeDataContainer Removes a Data Container from the bundle by pointer
   * @param dc
   */
  void removeDataContainer(DataContainer::Pointer dc) override;

  /**
   * @brief Removes a DataContainer by name from the bundle
   * @param name
   */
  void removeDataContainer(const QString& name) override;

  /**
   * @brief Removes a Data Container by index
   * @param i
   */
  void removeDataContainer(qint32 i) override;

  /**
   * @brief Returns the DataContainer at a given index
   * @param index
   * @return
   */
  DataContainer::Pointer getDataContainer(qint32 index) override;

  /**
   * @brief Returns the number of DataContainers held in the bundle
   * @return
   */
  qint32 count() override;

  /**
   * @brief Removes all references to the DataContainers stored in the class. Count will = 0
   * after this operation
   */
  void clear() override;

  /**
   * @brief This function returns all the Data Arrays Path objects that all the
   * DataContainers have in common with each other.
   */
  QVector<DataArrayPath> findCommonDataArrayPaths(bool filterMetaData = true);

  /**
   * @brief writeH5Data Write this bundle out to an HDF5 File
   * @param file_id The hdf5 file id or other group id where the bundle is going to be serialized to.
   * @return
   */
  int writeH5Data(hid_t groupId) override;

  /**
   * @brief readH5Data Reads this bundle from an HDF5 file
   * @param groupId
   * @return
   */
  int readH5Data(hid_t groupId) override;

protected:
  DataContainerBundle();

private:
  QString m_Name = {};
  QStringList m_MetaDataArrays = {};

  QVector<DataContainer::Pointer> m_DataContainers;
  QString m_MetaDataAMName;

public:
  DataContainerBundle(const DataContainerBundle&) = delete;            // Copy Constructor Not Implemented
  DataContainerBundle(DataContainerBundle&&) = delete;                 // Move Constructor Not Implemented
  DataContainerBundle& operator=(const DataContainerBundle&) = delete; // Copy Assignment Not Implemented
  DataContainerBundle& operator=(DataContainerBundle&&) = delete;      // Move Assignment Not Implemented
};

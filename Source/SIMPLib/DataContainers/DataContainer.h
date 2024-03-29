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

#include <cstddef>

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Observable.h"
#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/IDataStructureContainerNode.hpp"
#include "SIMPLib/DataContainers/RenameDataPath.h"
#include "SIMPLib/Geometry/IGeometry.h"
#include "SIMPLib/Utilities/ToolTipGenerator.h"

class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

class QTextStream;
class DataArrayPath;
class DataContainerArrayProxy;
class DataContainerProxy;
class AttributeMatrix;
class SIMPLH5DataReaderRequirements;
class AbstractFilter;

using AttributeMatrixShPtr = std::shared_ptr<AttributeMatrix>;

/**
 * @brief The DataContainer class
 */
class SIMPLib_EXPORT DataContainer : public Observable, public IDataStructureContainerNode<AttributeMatrix>
{

  // This line MUST be first when exposing a class and properties to Python
  // clang-format off
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(DataContainer)
  PYB11_SHARED_POINTERS(DataContainer)
  PYB11_STATIC_CREATION(New OVERLOAD QString)
  PYB11_STATIC_CREATION(New OVERLOAD DataArrayPath)
  PYB11_PROPERTY(QString Name READ getName WRITE setName)
  PYB11_PROPERTY(IGeometry Geometry READ getGeometry WRITE setGeometry)
  PYB11_METHOD(QString getInfoString ARGS InfoStringFormat)
  PYB11_METHOD(bool addOrReplaceAttributeMatrix ARGS AttributeMatrix)
  PYB11_METHOD(bool insertOrAssign ARGS AttributeMatrix)
  PYB11_METHOD(AttributeMatrix::Pointer getAttributeMatrix OVERLOAD const.QString.&,Name CONST_METHOD)
  PYB11_METHOD(AttributeMatrix::Pointer getAttributeMatrix OVERLOAD const.DataArrayPath.&,Path CONST_METHOD)
  PYB11_METHOD(AttributeMatrix removeAttributeMatrix ARGS Name)
  PYB11_METHOD(bool renameAttributeMatrix ARGS OldName NewName OverWrite)
  PYB11_METHOD(bool doesAttributeMatrixExist ARGS Name)
  PYB11_METHOD(void setGeometry ARGS Geometry)
  PYB11_CUSTOM()
  PYB11_END_BINDINGS()
  // End Python bindings declarations
  // clang-format on
public:
  using Self = DataContainer;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for DataContainer
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for DataContainer
   */
  static QString ClassName();

  ~DataContainer() override;

  DataContainer(const DataContainer&) = delete;            // Copy Constructor Not Implemented
  DataContainer(DataContainer&&) = delete;                 // Move Constructor Not Implemented
  DataContainer& operator=(const DataContainer&) = delete; // Copy Assignment Not Implemented
  DataContainer& operator=(DataContainer&&) = delete;      // Move Assignment Not Implemented

  /**
   * @brief The Type enum
   */
  enum class Type : unsigned int
  {
    Volume = 0,    //!<
    Surface = 1,   //!<
    EdgeData = 2,  //!<
    Vertex = 3,    //!<
    Unknown = 999, //!<
  };

  using Types = QVector<Type>;

  using Container_t = std::vector<AttributeMatrixShPtr>;

  /**
   * @brief AttributeMatrixMap_t
   */
  // using AttributeMatrixMap_t = QMap<QString, AttributeMatrixShPtr>;

  /**
   * @brief Creates a new shared pointer instance of this class
   * @param name The name to give to the DataContainer and must NOT be empty.
   * @return Shared Pointer to a DataContainer instance.
   */
  static Pointer New(const QString& name);

  /**
   * @brief Creates a new shared pointer instance of this class
   * @param name The name to give to the DataContainer and must NOT be empty.
   * @return Shared Pointer to a DataContainer instance.
   */
  static Pointer New(const DataArrayPath& name);

  /**
   * @brief Creates a new data container
   */
  virtual Pointer createNewDataContainer(const QString& name);

  /**
   * @brief ReadDataContainerStructure
   * @param dcArrayGroupId
   * @param proxy
   * @param h5InternalPath
   */
  static void ReadDataContainerStructure(hid_t dcArrayGroupId, DataContainerArrayProxy& proxy, SIMPLH5DataReaderRequirements* req, const QString& h5InternalPath);

  /**
   * @brief Creates and returns a DataArrayPath for the DataContainer
   * @return
   */
  DataArrayPath getDataArrayPath() const override;

  /**
   * @brief Sets the geometry of the data container
   * @param geometry
   */
  virtual void setGeometry(const IGeometry::Pointer& geometry);

  /**
   * @brief Returns the geometry of the data container
   * @return
   */
  virtual IGeometry::Pointer getGeometry() const;

  /**
   * @param format The format of the string to be returned.
   */
  virtual QString getInfoString(SIMPL::InfoStringFormat format) const;

  /**
   * @brief Returns a ToolTipGenerator for creating HTML tooltip tables
   * with values populated to match the current DataContainer.
   * @return
   */
  virtual ToolTipGenerator getToolTipGenerator() const;

  /**
   * @brief Adds the data for a named array. If an AttributeMatrix with the same
   * name already exists in the DataContainer then the add will fail.
   * @param matrix The IDataArrayShPtrType that will hold the data
   * @return Bool TRUE if the addition was successful, FALSE Otherwise.
   */
  bool addOrReplaceAttributeMatrix(const AttributeMatrixShPtr& matrix)
  {
    return insertOrAssign(matrix);
  }

  /**
   * @brief Returns the array for a given named array or the equivelant to a
   * null pointer if the name does not exist.
   * @param name The name of the data array
   */
  AttributeMatrixShPtr getAttributeMatrix(const QString& name) const
  {
    return getChildByName(name);
  }

  /**
   * @brief Returns the array for a given named array or the equivelant to a
   * null pointer if the name does not exist.
   * @param name The Name of the AttributeMatrix will be extracted from the DataArratPath object
   */
  AttributeMatrixShPtr getAttributeMatrix(const DataArrayPath& path) const
  {
    // Could this be sped-up if we hashed DataArrayPath as well?
    if(path.getDataContainerName() != getName())
    {
      return nullptr;
    }
    return getChildByName(path.getAttributeMatrixName());
  }

  /**
   * @brief Returns bool of whether a named array exists
   * @param name The name of the data array
   */
  bool doesAttributeMatrixExist(const QString& name) const
  {
    return contains(name);
  }

  /**
   * @brief Removes the named data array from the Data Container and returns it to the calling
   * method.
   * @param name The name of the array
   * @return
   */
  virtual AttributeMatrixShPtr removeAttributeMatrix(const QString& name);

  /**
   * @brief Renames a cell data array from the Data Container
   * @param name The name of the array
   */
  virtual bool renameAttributeMatrix(const QString& oldname, const QString& newname, bool overwrite = false);

  /**
   * @brief Removes all the Cell Arrays
   */
  virtual void clearAttributeMatrices();

  Container_t getAttributeMatrices() const
  {
    return getChildren();
  }

  /**
   * @brief Returns a list that contains the names of all the arrays currently stored in the
   * Cell (Formerly Cell) group
   * @return
   */
  virtual NameList getAttributeMatrixNames() const;

  /**
   * @brief Returns the total number of arrays that are stored in the Cell group
   * @return
   */
  int getNumAttributeMatrices() const
  {
    return static_cast<int>(size());
  }

  /**
   * @brief getAllDataArrayPaths
   * @return
   */
  virtual QVector<DataArrayPath> getAllDataArrayPaths() const;

  /**
   * @brief This method will check for the existance of a named AttributeMatrix. If that AttributeMatrix with the
   * given name does exist, the attributeMatrix will be checked for validity and if it passes the attribute matrix
   * will be returned. if any of the tests fail a Shared Pointer wrapping a nullptr value will be returned.
   * @param filter The filter object to use for error messages. Can be nullptr.
   * @param attributeMatrixName The name of the AttributeMatrix to look for. must NOT be nullptr.
   * @param err The unique error value to generate derived error values from. This helps debugging.
   * @return Shared Pointer to an AttributeMatrix object.
   */
  AttributeMatrixShPtr getPrereqAttributeMatrix(AbstractFilter* filter, const QString& attributeMatrixName, int err) const;

  /**
   * @brief createNonPrereqAttributeMatrix This method will create a new AttributeMatrix with the given tuple dimensions
   * and type. If the name is empty or an attribute matrix with the given name already exists the method will return
   * a nullptr shared pointer. This is a convenience method that simply pulls the
   * AttributeMatrix name from the DataArrayPath object.
   * @param filter The object to report errors through
   * @param path A DataArrayPath object that has the valid AttributeMatrix name
   * @param tDims The Tuple Dimensions of the Attribute Matrix
   * @param amType The Type of AttributeMatrix
   * @return A Shared Pointer to the AttributeMatrix
   */
  AttributeMatrixShPtr createNonPrereqAttributeMatrix(AbstractFilter* filter, const DataArrayPath& path, const std::vector<size_t>& tDims, AttributeMatrix::Type amType,
                                                      RenameDataPath::DataID_t id = RenameDataPath::k_Invalid_ID);

  /**
   * @brief createNonPrereqAttributeMatrix
   * @param filter
   * @param path
   * @param tDims
   * @param amType
   * @param id
   * @return
   */
  AttributeMatrixShPtr createNonPrereqAttributeMatrix(AbstractFilter* filter, const DataArrayPath& path, const SizeVec3Type& tDims, AttributeMatrix::Type amType, RenameDataPath::DataID_t id);

  /**
   * @brief createNonPrereqAttributeMatrix This method will create a new AttributeMatrix with the given tuple dimensions
   * and type. If the name is empty or an attribute matrix with the given name already exists the method will return
   * a nullptr shared pointer.
   * @param filter The object to report errors through
   * @param attributeMatrixName The name of the AttributeMatrix to create
   * @param tDims The Tuple Dimensions of the Attribute Matrix
   * @param amType The Type of AttributeMatrix
   * @return A Shared Pointer to the AttributeMatrix
   */
  AttributeMatrixShPtr createNonPrereqAttributeMatrix(AbstractFilter* filter, const QString& attributeMatrixName, const std::vector<size_t>& tDims, AttributeMatrix::Type amType,
                                                      RenameDataPath::DataID_t id = RenameDataPath::k_Invalid_ID);

  /**
   * @brief Returns the geometry as the templated type
   * @return
   */
  template <typename GeometryType>
  typename GeometryType::Pointer getGeometryAs() const
  {
    typename GeometryType::Pointer geom = std::dynamic_pointer_cast<GeometryType>(getGeometry());
    return geom;
  }

  /**
   * @brief Returns the prerequisite geometry as the templated type
   * @param filter
   * @return
   */
  template <typename GeometryType>
  typename GeometryType::Pointer getPrereqGeometry(AbstractFilter* filter) const
  {
    typename GeometryType::Pointer geom = GeometryType::NullPointer();
    IGeometry::Pointer igeom = getGeometry();
    if(nullptr == igeom.get())
    {
      if(filter)
      {
        QString ss = QObject::tr("Data Container Geometry is missing.");
        filter->setErrorCondition(-385, ss);
      }
      return geom;
    }
    geom = getGeometryAs<GeometryType>();
    if(nullptr == geom.get())
    {
      if(filter)
      {
        QString ss = QObject::tr("Data Container Geometry is not compatible. The selected Geometry type is %1").arg(igeom->getGeometryTypeAsString());
        filter->setErrorCondition(-384, ss);
      }
      return geom;
    }
    return geom;
  }

  /**
   * @brief Creates an AttributeMatrix and automatically adds it to the DataContainer. NO Checks are done if the AttributeMatrix
   * already exists in the DataContainer object. If an AttributeMatrix already exists then the current AttributeMatrix
   * is over written with the new one. If you need the checks use the @see createNonPrereqAttributeMatrix() method instead
   * @param tDims The tuple Dimensions of the new AttributeMatrix
   * @param attrMatName The name of the new AttributeMatrix
   * @param attrType The Type of AttributeMatrix to create
   * @return
   */
  virtual AttributeMatrixShPtr createAndAddAttributeMatrix(const std::vector<size_t>& tDims, const QString& attrMatName, AttributeMatrix::Type attrType);

  /**
   * @brief Writes all the Attribute Matrices to HDF5 file
   * @return
   */
  virtual int writeAttributeMatricesToHDF5(hid_t parentId) const;

  /**
   * @brief Reads desired Attribute Matrices from HDF5 file
   * @return
   */
  virtual int readAttributeMatricesFromHDF5(bool preflight, hid_t dcGid, DataContainerProxy& dcProxy);

  /**
   * @brief creates copy of dataContainer
   * @return
   */
  virtual DataContainer::Pointer deepCopy(bool forceNoAllocate = false) const;

  /**
   * @brief writeMeshToHDF5
   * @param dcGid
   * @return
   */
  virtual int writeMeshToHDF5(hid_t dcGid, bool writeXdmf) const;

  /**
   * @brief writeXdmf
   * @param out
   * @param hdfFileName
   * @return
   */
  virtual int writeXdmf(QTextStream& out, const QString& hdfFileName) const;

  /**
   * @brief readMeshDataFromHDF5
   * @param dcGid
   * @param preflight
   * @return
   */
  virtual int readMeshDataFromHDF5(hid_t dcGid, bool preflight);

protected:
  virtual void writeXdmfFooter(QTextStream& xdmf) const;

  DataContainer();
  explicit DataContainer(const QString& name);

private:
  IGeometry::Pointer m_Geometry;
};

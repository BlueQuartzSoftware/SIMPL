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

#pragma once

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif

//-- C++ includes
#include <memory>
#include <vector>

#include <hdf5.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

//-- DREAM3D Includes
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/Observable.h"
#include "SIMPLib/DataContainers/IDataStructureContainerNode.hpp"
#include "SIMPLib/DataContainers/RenameDataPath.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/ToolTipGenerator.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class AttributeMatrixProxy;
class DataContainerProxy;
class SIMPLH5DataReaderRequirements;
template <class T>
class DataArray;

enum RenameErrorCodes
{
  OLD_DOES_NOT_EXIST,
  SUCCESS,
  NEW_EXISTS,
  SAME_PATH
};

/**
 * @class AttributeMatrix AttributeMatrix DREAM3DLib/DataContainers/AttributeMatrix.h
 * @brief This data container holds data the represents a structured rectangular
 * grid of data typically referred to as a Voxel Volume
 *
 * @author Michael A. Groeber for AFRL
 * @date
 * @version 1.0
 */
class SIMPLib_EXPORT AttributeMatrix : public Observable, public IDataStructureContainerNode<IDataArray>
{
  // clang-format off
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(AttributeMatrix)
  PYB11_SHARED_POINTERS(AttributeMatrix)
  PYB11_STATIC_CREATION(New ARGS std::vector<size_t> QString AttributeMatrix::Type)
  
  PYB11_ENUMERATION(Type)
  PYB11_ENUMERATION(Category)
 
  PYB11_PROPERTY(QString Name READ getName WRITE setName)
  PYB11_PROPERTY(std::vector<size_t> TupleDimensions READ getTupleDimensions WRITE setTupleDimensions)
  PYB11_METHOD(bool doesAttributeArrayExist ARGS Name)
  PYB11_METHOD(bool addOrReplaceAttributeArray OVERLOAD const.IDataArrayShPtrType.&,Data)
  PYB11_METHOD(bool insertOrAssign ARGS IDataArrayShPtrType)
  PYB11_METHOD(IDataArray removeAttributeArray ARGS Name)
  PYB11_METHOD(int renameAttributeArray ARGS OldName NewName OverWrite)
  PYB11_METHOD(IDataArray::Pointer getAttributeArray OVERLOAD const.QString.&,Name CONST_METHOD)
  PYB11_METHOD(IDataArray::Pointer getAttributeArray OVERLOAD const.DataArrayPath.&,Path CONST_METHOD)
  PYB11_END_BINDINGS()
  // clang-format on
  // End Python bindings declarations

public:
  using Self = AttributeMatrix;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for AttributeMatrix
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for AttributeMatrix
   */
  static QString ClassName();

  ~AttributeMatrix() override;

  using EnumType = unsigned int;
  using Container_t = std::vector<std::shared_ptr<IDataArray>>;

  /**
   * @brief The Type is an enum that describes the type of data that the AttributeMatrix holds
   */
  enum class Type : EnumType
  {
    Vertex = 0,         //!<
    Edge = 1,           //!<
    Face = 2,           //!<
    Cell = 3,           //!<
    VertexFeature = 4,  //!<
    EdgeFeature = 5,    //!<
    FaceFeature = 6,    //!<
    CellFeature = 7,    //!<
    VertexEnsemble = 8, //!<
    EdgeEnsemble = 9,   //!<
    FaceEnsemble = 10,  //!<
    CellEnsemble = 11,  //!<
    MetaData = 12,      //!<
    Generic = 13,       //!<
    Unknown = 999,      //!<
    Any = 4294967295U
  };

  using Types = QVector<Type>;

  /**
   * @brief TypeToString Returns a String representation of the AttributeMatrix Type
   * @param t The type to convert
   * @return
   */
  static QString TypeToString(Type t);

  /**
   * @brief StringToType Returns the proper enumeration value for a given string or
   * Unknown if a string type was not know to AttributeMatrix
   * @param str The string to convert
   * @return
   */
  static Type StringToType(const QString& str);

  /**
   * @brief GetTypesAsStrings
   * @return
   */
  static QVector<QString> GetTypesAsStrings();

  /**
   * @brief The Category enum describes the general category of data in the AttributeMatrix
   */
  enum class Category : EnumType // C++11 scoped enum
  {
    Element = 0,
    Feature = 1,
    Ensemble = 2,
    Any = 3,
    Unknown = 999, //!<
  };

  using Categories = std::vector<Category>;

  /**
   * @brief New Creates an AttributeMatrix with the give name
   * @param tupleDims The dimensions of the Attribute matrix given in the order fastest moving to slowest moving (XYZ)
   * @param name The name of the AttributeMatrix. Each AttributeMatrix should have a unique name.
   * @param attrType The type of AttributeMatrix, one of
   * @return
   */
  static Pointer New(const std::vector<size_t>& tupleDims, const QString& name, AttributeMatrix::Type attrType)
  {
    Pointer sharedPtr(new AttributeMatrix(tupleDims, name, attrType));
    return sharedPtr;
  }

  /**
   * @brief ReadAttributeMatrixStructure
   * @param containerId
   * @param dataContainer
   * @param h5InternalPath
   */
  static void ReadAttributeMatrixStructure(hid_t containerId, DataContainerProxy* dcProxy, SIMPLH5DataReaderRequirements* req, const QString& h5InternalPath);

  /**
   * @brief Creates and returns a DataArrayPath for the AttributeMatrix
   * @return
   */
  DataArrayPath getDataArrayPath() const override;

  /**
   * @brief Type
   */
  /**
   * @brief Setter property for Type
   */
  void setType(const AttributeMatrix::Type& value);
  /**
   * @brief Getter property for Type
   * @return Value of Type
   */
  AttributeMatrix::Type getType() const;

  /**
   * @brief Adds the IDataArray to the AttributeMatrix.
   * @param data The IDataArrayShPtrType that will hold the data
   * @return Bool: True if the addition happened, FALSE if an IDataArray with the same name already exists.
   */
  bool addOrReplaceAttributeArray(const IDataArrayShPtrType& data);

  /**
   * @brief Returns the array for a given named array or the equivelant to a
   * null pointer if the name does not exist.
   * @param name The name of the data array
   */
  IDataArrayShPtrType getAttributeArray(const QString& name) const;

  /**
   * @brief getAttributeArray
   * @param path
   * @return
   */
  IDataArrayShPtrType getAttributeArray(const DataArrayPath& path) const;

  /**
   * @brief returns a IDataArray based object that is stored in the attribute matrix by a
   * given name.
   * @param name The name of the array
   */
  template <class ArrayType>
  typename ArrayType::Pointer getAttributeArrayAs(const QString& name) const
  {
    IDataArrayShPtrType iDataArray = getAttributeArray(name);
    return std::dynamic_pointer_cast<ArrayType>(iDataArray);
  }

  /**
   * @brief Returns bool of whether a named array exists
   * @param name The name of the data array
   */
  bool doesAttributeArrayExist(const QString& name) const;

  /**
   * @brief Removes the named data array from the Data Container and returns it to the calling
   * method.
   * @param name The name of the array
   * @return
   */
  virtual IDataArrayShPtrType removeAttributeArray(const QString& name);

  /**
   * @brief Renames a cell data array from the Data Container
   * @param name The name of the array
   */
  virtual RenameErrorCodes renameAttributeArray(const QString& oldname, const QString& newname, bool overwrite = false);

  /**
   * @brief Removes all the Cell Arrays
   */
  virtual void clearAttributeArrays();

  /**
   * @brief Returns the collection of contained DataArrays
   * @return
   */
  Container_t getAttributeArrays() const;

  /**
   * @brief Returns a list that contains the names of all the arrays currently stored in the
   * Cell (Formerly Cell) group
   * @return
   */
  virtual NameList getAttributeArrayNames() const;

  /**
   * @brief Returns the total number of arrays that are stored in the Cell group
   * @return
   */
  int getNumAttributeArrays() const;

  /**
   * @brief Resizes an array from the Attribute Matrix
   * @param size The new size of the array
   */
  void resizeAttributeArrays(const std::vector<size_t>& tDims);

  /**
   * @brief Returns bool of whether a named array exists
   * @param name The name of the data array
   */
  virtual bool validateAttributeArraySizes() const;

  /**
   * @brief getPrereqArray
   * @param filter An instance of an AbstractFilter that is calling this function. Can be nullptr in which case
   * no error message will be returned if there is an error. Will only check component dims if cDims is not empty.
   * @param attributeArrayName The name of the Attribute Array
   * @param err The error code to set into the filter if there is an error
   * @param cDims The component Dimensions of the Data Array
   * @return A valid IDataArray Subclass if the array exists otherwise a null shared pointer.
   */
  template <class ArrayType>
  typename ArrayType::Pointer getPrereqArray(AbstractFilter* filter, const QString& attributeArrayName, int err, const std::vector<size_t>& cDims = {}) const
  {
    QString ss;
    typename ArrayType::Pointer attributeArray = ArrayType::NullPointer();
    // Make sure the name is not empty for the AttributeArrayName. This would be detected below
    // in the call to get either one BUT the reason for the failure would not be evident so we make these explicit checks
    // here and send back nice error messages to ther user/programmer.
    if(attributeArrayName.isEmpty())
    {
      if(filter)
      {
        ss = QObject::tr("AttributeMatrix:'%1' The name of a requested Attribute Array was empty. Please provide a name for this array").arg(getName());
        filter->setErrorCondition(err, ss);
      }
    }
    // Now ask for the actual AttributeArray from the AttributeMatrix
    if(!doesAttributeArrayExist(attributeArrayName))
    {
      if(filter)
      {
        ss = QObject::tr("The AttributeMatrix named '%1' does NOT have a DataArray with name '%2'. This filter requires this DataArray in order to execute.").arg(getName()).arg(attributeArrayName);
        filter->setErrorCondition(err, ss);
      }
      return attributeArray;
    }

    if(!cDims.empty())
    {
      size_t numComp = std::accumulate(cDims.cbegin(), cDims.cend(), static_cast<size_t>(1), std::multiplies<size_t>());

      // Check to make sure the AttributeArray we have is of the proper type, size and number of components
      if(false == dataArrayCompatibility<ArrayType>(attributeArrayName, numComp, filter))
      {
        return attributeArray;
      }
    }

    IDataArrayShPtrType iDataArray = getAttributeArray(attributeArrayName);
    attributeArray = std::dynamic_pointer_cast<ArrayType>(iDataArray);
    if(nullptr == attributeArray.get() && filter)
    {
      ss = QObject::tr("The AttributeMatrix named '%1' contains an array with name '%2' but the DataArray could not be downcast using std::dynamic_pointer_cast<T>.")
               .arg(getName())
               .arg(attributeArrayName);
      filter->setErrorCondition(err, ss);
    }
    return attributeArray;
  }

  /**
   * @brief getExistingPrereqArray
   * @param filter
   * @param attributeArrayName
   * @param err
   * @return
   */
  IDataArray::Pointer getPrereqIDataArray(AbstractFilter* filter, const QString& attributeArrayName, int err) const;

  /**
   * @brief createNonPrereqArray This method will create a new DataArray in the AttributeMatrix. The condition for this
   * method to work properly is the name of the attribute array is not empty
   * @param filter The instance of the filter the filter that is requesting the new array
   * @param attributeArrayName The name of the AttributeArray to create
   * @param initValue The initial value of all the elements of the array
   * @param size The number of tuples in the Array
   * @param dims The dimensions of the components of the AttributeArray
   * @return A Shared Pointer to the newly created array
   */
  template <class ArrayType>
  typename ArrayType::Pointer createNonPrereqArray(AbstractFilter* filter, const QString& attributeArrayName, typename ArrayType::value_type initValue, const std::vector<size_t>& compDims,
                                                   RenameDataPath::DataID_t id = RenameDataPath::k_Invalid_ID)
  {
    typename ArrayType::Pointer attributeArray = ArrayType::NullPointer();

    QString ss;
    if(attributeArrayName.isEmpty())
    {
      if(filter)
      {
        ss = QObject::tr("The name of the array was empty. Please provide a name for this array.");
        filter->setErrorCondition(-10001, ss);
      }
      return attributeArray;
    }
    IDataArrayShPtrType iDataArray = getAttributeArray(attributeArrayName);
    if(nullptr == iDataArray.get())
    {
      createAndAddAttributeArray<ArrayType>(filter, attributeArrayName, initValue, compDims);
    }
    else if(filter)
    {
      ss = QObject::tr("AttributeMatrix:'%1' An Attribute Array already exists with the name %2.").arg(getName()).arg(attributeArrayName);
      filter->setErrorCondition(-10002, ss);
      return attributeArray;
    }
    iDataArray = getAttributeArray(attributeArrayName);
    if(nullptr == iDataArray && filter)
    {
      ss = QObject::tr("AttributeMatrix:'%1' An array with name '%2' could not be created.").arg(getName()).arg(attributeArrayName);
      filter->setErrorCondition(-10003, ss);
    }
    attributeArray = std::dynamic_pointer_cast<ArrayType>(iDataArray);
    if(nullptr == attributeArray.get() && filter)
    {
      ss = QObject::tr("AttributeMatrix:'%1' An array with name '%2' could not be downcast using std::dynamic_pointer_cast<T>.").arg(getName()).arg(attributeArrayName);
      filter->setErrorCondition(-10004, ss);
    }
    else if(nullptr != attributeArray && filter)
    {
      // Check if path was renamed
      // This will crash if no parent node is found
      IDataStructureNode* parentPtr = getParentNode();
      RenameDataPath::AlertFilterCreatedPath(filter, id, DataArrayPath(parentPtr->getName(), getName(), attributeArrayName));
    }
    return attributeArray;
  }

  /**
   * @brief Creates and Adds the data for a named array
   * @param name The name that the array will be known by
   * @param dims The size the data on each tuple
   */
  template <class ArrayType>
  void createAndAddAttributeArray(AbstractFilter* filter, const QString& name, typename ArrayType::value_type initValue, std::vector<size_t> compDims,
                                  RenameDataPath::DataID_t id = RenameDataPath::k_Invalid_ID)
  {
    bool allocateData = false;
    if(nullptr == filter)
    {
      allocateData = true;
    }
    else
    {
      allocateData = !filter->getInPreflight();
    }
    typename ArrayType::Pointer attributeArray = ArrayType::CreateArray(getNumberOfTuples(), compDims, name, allocateData);
    if(attributeArray.get() != nullptr)
    {
      if(allocateData)
      {
        attributeArray->initializeWithValue(initValue);
      }
      attributeArray->setInitValue(initValue);
      addOrReplaceAttributeArray(attributeArray);
      // Check if path was renamed
      DataArrayPath path = getDataArrayPath();
      path.setDataArrayName(name);
      RenameDataPath::AlertFilterCreatedPath(filter, id, path);
    }
  }

  /**
   * @brief dataArrayCompatibility
   * @param arrayName
   * @param numComp
   * @param filter
   * @return
   */
  template <class ArrayType>
  bool dataArrayCompatibility(const QString& arrayName, int numComp, AbstractFilter* filter) const
  {
    // Make sure the types are the same
    IDataArrayShPtrType ida = getAttributeArray(arrayName);
    typename ArrayType::Pointer targetDestArray = std::dynamic_pointer_cast<ArrayType>(ida);
    if(targetDestArray.get() == nullptr)
    {
      if(nullptr != filter)
      {
        typename ArrayType::Pointer validTargetArray = ArrayType::CreateArray(1, "JUNK_INTERNAL_ARRAY", false);
        IDataArrayShPtrType srcArray = getAttributeArray(arrayName);
        QString srcDesc = srcArray->getTypeAsString();
        QString desc = validTargetArray->getTypeAsString();
        QString ss =
            QObject::tr("The Filter '%1' requires an array of type '%2' but the data array '%3' has a type of '%4'").arg(filter->getHumanLabel()).arg(desc).arg(srcArray->getName()).arg(srcDesc);
        filter->setErrorCondition(-501, ss);
      }
      return false;
    }
    // Make sure the sizes are equal to what is being asked for
    if(getNumberOfTuples() != targetDestArray->getNumberOfTuples())
    {
      if(nullptr != filter)
      {
        QString ss = QObject::tr("Filter '%1' requires array with name '%2' to have Number of Tuples = %3. The currently selected array "
                                 " has %4")
                         .arg(filter->getHumanLabel())
                         .arg(arrayName)
                         .arg((getNumberOfTuples()))
                         .arg(targetDestArray->getNumberOfTuples());
        filter->setErrorCondition(-502, ss);
      }
      return false;
    }
    // Make sure the number of components match
    if(numComp != targetDestArray->getNumberOfComponents())
    {
      if(nullptr != filter)
      {
        QString ss = QObject::tr("Filter '%1' is trying to use array '%2' where the number of components is %3 but the filter requires that array "
                                 " to have %4.")
                         .arg(filter->getHumanLabel())
                         .arg(targetDestArray->getName())
                         .arg(targetDestArray->getNumberOfComponents())
                         .arg(numComp);
        filter->setErrorCondition(-503, ss);
      }
      return false;
    }
    // Make sure we can downcast to the proper type
    typename ArrayType::Pointer array = std::dynamic_pointer_cast<ArrayType>(targetDestArray);
    if(nullptr == array.get())
    {
      typename ArrayType::Pointer dat = ArrayType::CreateArray(1, "JUNK-INTERNAL-USE-ONLY", true);
      QString ss = QObject::tr(" - The filter requested an array named '%1' with type '%2' from the filter '%3'.\n"
                               "An Array with name '%4' is stored in the %5 but is of type %6\n")
                       .arg(arrayName)
                       .arg(dat->getTypeAsString())
                       .arg(getNameOfClass())
                       .arg(arrayName)
                       .arg(getNameOfClass())
                       .arg(targetDestArray->getTypeAsString());
      if(nullptr != filter)
      {
        filter->setErrorCondition(-504, ss);
      }
      return false;
    }
    return true;
  }

  /**
  * @brief Removes inactive objects from the Attribute Matrix and renumbers the active objects to preserve a compact matrix
    (only valid for feature or ensemble type matrices)
  * @param size The new size of the array
  */
  bool removeInactiveObjects(const QVector<bool>& activeObjects, DataArray<int32_t>* featureIds);

  /**
   * @brief Sets the Tuple Dimensions for the Attribute Matrix
   * @param tupleDims
   */
  void setTupleDimensions(const std::vector<size_t>& tupleDims);

  /**
   * @brief Returns the Tuple Dimensions of the AttributeMatrix
   * @return
   */
  std::vector<size_t> getTupleDimensions() const;

  /**
   * @brief Returns the number of Tuples that the feature data has. For example if there are 32 features
   * in during a set of filtering operations then the a value of '32' would be returned.
   * @return
   */
  size_t getNumberOfTuples() const;

  /**
   * @brief creates and returns a copy of the attribute matrix
   * @return On error, will return a null pointer.  It is the responsibility of the calling function to check for errors and return an error message using the PipelineMessage
   */
  virtual AttributeMatrix::Pointer deepCopy(bool forceNoAllocate = false) const;

  /**
   * @brief writeAttributeArraysToHDF5
   * @param parentId
   * @return
   */
  virtual int writeAttributeArraysToHDF5(hid_t parentId) const;

  /**
   * @brief addAttributeArrayFromHDF5Path
   * @param gid
   * @param name
   * @param preflight
   * @return
   */
  virtual int addAttributeArrayFromHDF5Path(hid_t gid, const QString& name, bool preflight);

  /**
   * @brief readAttributeArraysFromHDF5
   * @param amGid
   * @param preflight
   * @param attrMatProxy
   * @return
   */
  virtual int readAttributeArraysFromHDF5(hid_t amGid, bool preflight, AttributeMatrixProxy* attrMatProxy);

  /**
   * @brief generateXdmfText
   * @param centering
   * @param dataContainerName
   * @param hdfFileName
   * @param gridType
   * @return
   */
  virtual QString generateXdmfText(const QString& centering, const QString& dataContainerName, const QString& hdfFileName, uint8_t gridType = 0) const;

  /**
   * @brief getInfoString Returns a text string in the given format that has information
   * about the attribute Matrix
   * @param format A value from the SIMPL::InfoStringFormat enumeration
   * @return
   */
  virtual QString getInfoString(SIMPL::InfoStringFormat format) const;

  /**
   * @brief Returns a ToolTipGenerator for creating HTML tooltip tables
   * with values populated to match the current AttributeMatrix.
   * @return
   */
  virtual ToolTipGenerator getToolTipGenerator() const;

protected:
  AttributeMatrix(const std::vector<size_t>& tDims, const QString& name, AttributeMatrix::Type attrType);

  /**
   * @brief writeXdmfAttributeData
   * @param array
   * @param centering
   * @param dataContainerName
   * @param hdfFileName
   * @param gridType
   * @return
   */
  virtual QString writeXdmfAttributeData(const IDataArrayShPtrType& array, const QString& centering, const QString& dataContainerName, const QString& hdfFileName, uint8_t gridType = 0) const;

  /**
   * @brief writeXdmfAttributeDataHelper
   * @param numComp
   * @param attrType
   * @param dataContainerName
   * @param array
   * @param centering
   * @param precision
   * @param xdmfTypeName
   * @param hdfFileName
   * @param gridType
   * @return
   */
  virtual QString writeXdmfAttributeDataHelper(int numComp, const QString& attrType, const QString& dataContainerName, const IDataArrayShPtrType& array, const QString& centering, int precision,
                                               const QString& xdmfTypeName, const QString& hdfFileName, uint8_t gridType = 0) const;

private:
  std::vector<size_t> m_TupleDims;
  AttributeMatrix::Type m_Type = {};

  AttributeMatrix(const AttributeMatrix&);
  void operator=(const AttributeMatrix&);
};

Q_DECLARE_METATYPE(AttributeMatrix::Type)
Q_DECLARE_METATYPE(AttributeMatrix::Category)

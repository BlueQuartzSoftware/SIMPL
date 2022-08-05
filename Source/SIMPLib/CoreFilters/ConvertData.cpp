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
#include "ConvertData.h"

#include <iostream>

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/NumericTypeFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#define CHECK_AND_CONVERT(Type, DataContainer, ScalarType, Array, AttributeMatrixName, OutputName)                                                                                                     \
  if(false == completed)                                                                                                                                                                               \
  {                                                                                                                                                                                                    \
    DataArray<Type>::Pointer Type##Ptr = std::dynamic_pointer_cast<DataArray<Type>>(Array);                                                                                                            \
    if(nullptr != Type##Ptr)                                                                                                                                                                           \
    {                                                                                                                                                                                                  \
      std::vector<size_t> dims = Array->getComponentDimensions();                                                                                                                                      \
      Detail::ConvertData<Type>(this, Type##Ptr.get(), dims, DataContainer, ScalarType, AttributeMatrixName, OutputName);                                                                              \
      completed = true;                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
  }

namespace Detail
{
template <typename O, typename D>
/**
 * @brief ConvertData Templated function that converts an IDataArray to a given primitive type
 * @param origin IDataArray instance pointer
 * @param dims Component dimensions
 * @param m DataContainer instance pointer
 * @param attributeMatrixName Name of target AttributeMatrix
 * @param name Name of converted array
 *
 */
void ConvertData(DataArray<O>* origin, const std::vector<size_t>& dims, DataContainer::Pointer m, const QString attributeMatrixName, const QString& name)
{
  size_t voxels = origin->getNumberOfTuples();
  size_t size = origin->getSize();

  typename DataArray<D>::Pointer p = DataArray<D>::CreateArray(voxels, dims, name, true);
  m->getAttributeMatrix(attributeMatrixName)->insertOrAssign(p);
  for(size_t v = 0; v < size; ++v)
  {
    if constexpr(std::is_same<O, D>::value)
    {
      // Origin and destination arrays have the same type
      p->setValue(v, origin->getValue(v));
    }
    else if constexpr(std::is_same<O, bool>::value)
    {
      // Origin array is a boolean array
      p->setValue(v, (origin->getValue(v) ? 1 : 0));
    }
    else if constexpr(std::is_same<D, bool>::value)
    {
      // Destination array is a boolean array
      p->setValue(v, (origin->getValue(v) != 0));
    }
    else
    {
      // All other cases
      p->setValue(v, static_cast<D>(origin->getValue(v)));
    }
  }
}

template <typename T>
/**
 * @brief ConvertData Templated function that converts an IDataArray to a given primitive type
 * @param ptr IDataArray instance pointer
 * @param dims Component dimensions
 * @param m DataContainer instance pointer
 * @param scalarType Primitive type to convert to
 * @param attributeMatrixName Name of target AttributeMatrix
 * @param name Name of converted array
 */
void ConvertData(AbstractFilter* filter, DataArray<T>* ptr, const std::vector<size_t>& dims, DataContainer::Pointer m, SIMPL::NumericTypes::Type scalarType, const QString attributeMatrixName,
                 const QString& name)
{
  if(scalarType == SIMPL::NumericTypes::Type::Int8)
  {
    ConvertData<T, int8_t>(ptr, dims, m, attributeMatrixName, name);
  }
  else if(scalarType == SIMPL::NumericTypes::Type::UInt8)
  {
    ConvertData<T, uint8_t>(ptr, dims, m, attributeMatrixName, name);
  }
  else if(scalarType == SIMPL::NumericTypes::Type::Int16)
  {
    ConvertData<T, int16_t>(ptr, dims, m, attributeMatrixName, name);
  }
  else if(scalarType == SIMPL::NumericTypes::Type::UInt16)
  {
    ConvertData<T, uint16_t>(ptr, dims, m, attributeMatrixName, name);
  }
  else if(scalarType == SIMPL::NumericTypes::Type::Int32)
  {
    ConvertData<T, int32_t>(ptr, dims, m, attributeMatrixName, name);
  }
  else if(scalarType == SIMPL::NumericTypes::Type::UInt32)
  {
    ConvertData<T, uint32_t>(ptr, dims, m, attributeMatrixName, name);
  }
  else if(scalarType == SIMPL::NumericTypes::Type::Int64)
  {
    ConvertData<T, int64_t>(ptr, dims, m, attributeMatrixName, name);
  }
  else if(scalarType == SIMPL::NumericTypes::Type::UInt64)
  {
    ConvertData<T, uint64_t>(ptr, dims, m, attributeMatrixName, name);
  }
  else if(scalarType == SIMPL::NumericTypes::Type::Float)
  {
    ConvertData<T, float>(ptr, dims, m, attributeMatrixName, name);
  }
  else if(scalarType == SIMPL::NumericTypes::Type::Double)
  {
    ConvertData<T, double>(ptr, dims, m, attributeMatrixName, name);
  }
  else if(scalarType == SIMPL::NumericTypes::Type::Bool)
  {
    ConvertData<T, bool>(ptr, dims, m, attributeMatrixName, name);
  }
  else if(scalarType == SIMPL::NumericTypes::Type::SizeT)
  {
    ConvertData<T, size_t>(ptr, dims, m, attributeMatrixName, name);
  }
  else
  {
    QString ss =
        QString("Error Converting DataArray '%1/%2' from type %3 to type %4").arg(attributeMatrixName).arg(ptr->getName()).arg(static_cast<int>(ptr->getType())).arg(static_cast<int>(scalarType));
    filter->setErrorCondition(-399, ss);
    return;
  }
}
} // End Namespace Detail

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertData::ConvertData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertData::~ConvertData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_NUMERICTYPE_FP("Scalar Type", ScalarType, FilterParameter::Category::Parameter, ConvertData));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to Convert", SelectedCellArrayPath, FilterParameter::Category::RequiredArray, ConvertData, req));
  }

  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Converted Attribute Array", OutputArrayName, SelectedCellArrayPath, SelectedCellArrayPath, FilterParameter::Category::CreatedArray, ConvertData));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath(reader->readDataArrayPath("SelectedCellArrayPath", getSelectedCellArrayPath()));
  setScalarType(static_cast<SIMPL::NumericTypes::Type>(reader->readValue("ScalarType", static_cast<int>(getScalarType()))));
  setOutputArrayName(reader->readString("OutputArrayName", getOutputArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, getSelectedCellArrayPath().getDataContainerName(), false);

  QString ss;
  if(m_OutputArrayName.isEmpty())
  {
    ss = QObject::tr("The output array name must be set");
    setErrorCondition(-398, ss);
    return;
  }

  if(getInPreflight())
  {
    AttributeMatrix::Pointer cellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, m_SelectedCellArrayPath, -301);
    if(getErrorCode() < 0)
    {
      return;
    }

    IDataArray::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath(this, getSelectedCellArrayPath());
    if(getErrorCode() < 0)
    {
      return;
    }

    std::vector<size_t> dims = p->getComponentDimensions();
    size_t voxels = cellAttrMat->getNumberOfTuples();
    if(m_ScalarType == SIMPL::NumericTypes::Type::Int8)
    {
      p = Int8ArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt8)
    {
      p = UInt8ArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    else if(m_ScalarType == SIMPL::NumericTypes::Type::Int16)
    {
      p = Int16ArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt16)
    {
      p = UInt16ArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    else if(m_ScalarType == SIMPL::NumericTypes::Type::Int32)
    {
      p = Int32ArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt32)
    {
      p = UInt32ArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    else if(m_ScalarType == SIMPL::NumericTypes::Type::Int64)
    {
      p = Int64ArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    else if(m_ScalarType == SIMPL::NumericTypes::Type::UInt64)
    {
      p = UInt64ArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    else if(m_ScalarType == SIMPL::NumericTypes::Type::Float)
    {
      p = FloatArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    else if(m_ScalarType == SIMPL::NumericTypes::Type::Double)
    {
      p = DoubleArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    else if(m_ScalarType == SIMPL::NumericTypes::Type::Bool)
    {
      p = BoolArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    else if(m_ScalarType == SIMPL::NumericTypes::Type::SizeT)
    {
      p = SizeTArrayType::CreateArray(voxels, dims, m_OutputArrayName, false);
    }
    cellAttrMat->insertOrAssign(p);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedCellArrayPath.getDataContainerName());
  AttributeMatrix::Pointer am = m->getAttributeMatrix(m_SelectedCellArrayPath.getAttributeMatrixName());

  IDataArray::Pointer iArray = am->getAttributeArray(m_SelectedCellArrayPath.getDataArrayName());

  if(nullptr == iArray.get())
  {
    setErrorCondition(-90002, tr("Data Array '%1' does not exist.").arg(m_SelectedCellArrayPath.getDataArrayName()));
    return;
  }

  bool completed = false;
  CHECK_AND_CONVERT(int8_t, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)

  CHECK_AND_CONVERT(uint8_t, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(uint16_t, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(int16_t, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(uint32_t, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(int32_t, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(uint64_t, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(int64_t, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(float, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(double, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(bool, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertData::newFilterInstance(bool copyFilterParameters) const
{
  ConvertData::Pointer filter = ConvertData::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertData::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertData::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertData::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ConvertData::getUuid() const
{
  return QUuid("{f4ba5fa4-bb5c-5dd1-9429-0dd86d0ecb37}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertData::getHumanLabel() const
{
  return "Convert AttributeArray Data Type";
}

// -----------------------------------------------------------------------------
ConvertData::Pointer ConvertData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ConvertData> ConvertData::New()
{
  struct make_shared_enabler : public ConvertData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ConvertData::getNameOfClass() const
{
  return QString("ConvertData");
}

// -----------------------------------------------------------------------------
QString ConvertData::ClassName()
{
  return QString("ConvertData");
}

// -----------------------------------------------------------------------------
void ConvertData::setScalarType(SIMPL::NumericTypes::Type value)
{
  m_ScalarType = value;
}

// -----------------------------------------------------------------------------
SIMPL::NumericTypes::Type ConvertData::getScalarType() const
{
  return m_ScalarType;
}

// -----------------------------------------------------------------------------
void ConvertData::setOutputArrayName(const QString& value)
{
  m_OutputArrayName = value;
}

// -----------------------------------------------------------------------------
QString ConvertData::getOutputArrayName() const
{
  return m_OutputArrayName;
}

// -----------------------------------------------------------------------------
void ConvertData::setSelectedCellArrayPath(const DataArrayPath& value)
{
  m_SelectedCellArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ConvertData::getSelectedCellArrayPath() const
{
  return m_SelectedCellArrayPath;
}

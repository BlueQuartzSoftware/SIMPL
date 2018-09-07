/* ============================================================================
 * Copyright (c) 2018 BlueQuartz Software, LLC
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
#include "TemplateHelpers.h"

#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

using namespace TemplateHelpers;

// -----------------------------------------------------------------------------
IDataArrayWkPtr CreateNonPrereqArrayFromArrayType::operator()(AbstractFilter* f, DataArrayPath arrayPath, QVector<size_t> compDims, IDataArrayShPtr sourceArrayType)
{

  IDataArrayShPtr ptr = IDataArray::NullPointer();

  if(CanDynamicCast<FloatArrayType>()(sourceArrayType))
  {
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(f, arrayPath, 0, compDims);
  }
  else if(CanDynamicCast<DoubleArrayType>()(sourceArrayType))
  {
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(f, arrayPath, 0, compDims);
  }
  else if(CanDynamicCast<Int8ArrayType>()(sourceArrayType))
  {
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(f, arrayPath, 0, compDims);
  }
  else if(CanDynamicCast<UInt8ArrayType>()(sourceArrayType))
  {
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(f, arrayPath, 0, compDims);
  }
  else if(CanDynamicCast<Int16ArrayType>()(sourceArrayType))
  {
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter, int16_t>(f, arrayPath, 0, compDims);
  }
  else if(CanDynamicCast<UInt16ArrayType>()(sourceArrayType))
  {
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter, uint16_t>(f, arrayPath, 0, compDims);
  }
  else if(CanDynamicCast<Int32ArrayType>()(sourceArrayType))
  {
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(f, arrayPath, 0, compDims);
  }
  else if(CanDynamicCast<UInt32ArrayType>()(sourceArrayType))
  {
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(f, arrayPath, 0, compDims);
  }
  else if(CanDynamicCast<Int64ArrayType>()(sourceArrayType))
  {
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(f, arrayPath, 0, compDims);
  }
  else if(CanDynamicCast<UInt64ArrayType>()(sourceArrayType))
  {
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter, uint64_t>(f, arrayPath, 0, compDims);
  }
  else if(CanDynamicCast<BoolArrayType>()(sourceArrayType))
  {
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<BoolArrayType, AbstractFilter, uint64_t>(f, arrayPath, 0, compDims);
  }
  else
  {
    QString msg = QObject::tr("The created array '%1' is of unsupported type. The following types are supported: %3").arg(arrayPath.getDataArrayName()).arg(SIMPL::TypeNames::SupportedTypeList);
    f->setErrorCondition(Errors::UnsupportedType);
    f->notifyErrorMessage(f->getHumanLabel(), msg, f->getErrorCondition());
  }
  return ptr;
}

// -----------------------------------------------------------------------------
IDataArrayWkPtr CreateNonPrereqArrayFromTypeEnum::operator()(AbstractFilter* f, DataArrayPath arrayPath, QVector<size_t> compDims, int arrayType, double initValue)
{
  IDataArrayShPtr ptr = IDataArray::NullPointer();

  switch(arrayType)
  {
  case SIMPL::TypeEnums::Int8:
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(f, arrayPath, static_cast<int8_t>(initValue), compDims);
    break;
  case SIMPL::TypeEnums::UInt8:
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(f, arrayPath, static_cast<uint8_t>(initValue), compDims);
    break;
  case SIMPL::TypeEnums::Int16:
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter, int16_t>(f, arrayPath, static_cast<int16_t>(initValue), compDims);
    break;
  case SIMPL::TypeEnums::UInt16:
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter, uint16_t>(f, arrayPath, static_cast<uint16_t>(initValue), compDims);
    break;
  case SIMPL::TypeEnums::Int32:
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(f, arrayPath, static_cast<int32_t>(initValue), compDims);
    break;
  case SIMPL::TypeEnums::UInt32:
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(f, arrayPath, static_cast<uint32_t>(initValue), compDims);
    break;
  case SIMPL::TypeEnums::Int64:
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(f, arrayPath, static_cast<int64_t>(initValue), compDims);
    break;
  case SIMPL::TypeEnums::UInt64:
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter, uint64_t>(f, arrayPath, static_cast<uint64_t>(initValue), compDims);
    break;
  case SIMPL::TypeEnums::Float:
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(f, arrayPath, static_cast<float>(initValue), compDims);
    break;
  case SIMPL::TypeEnums::Double:
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(f, arrayPath, static_cast<double>(initValue), compDims);
    break;
  case SIMPL::TypeEnums::Bool:
    ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(f, arrayPath, static_cast<bool>(initValue), compDims);
    break;
  default:
    QString msg = QObject::tr("The created array '%1' is of unsupported type. The following types are supported: %3").arg(arrayPath.getDataArrayName()).arg(SIMPL::TypeEnums::SupportedTypeList);
    f->setErrorCondition(Errors::UnsupportedType);
    f->notifyErrorMessage(f->getHumanLabel(), msg, f->getErrorCondition());
    break;
  }
  return ptr;
}

// -----------------------------------------------------------------------------
IDataArrayShPtr CreateArrayFromArrayType::operator()(AbstractFilter* f, QVector<size_t> tupleDims, QVector<size_t> compDims, QString arrayName, bool allocate, IDataArrayShPtr sourceArrayType)
{
  IDataArrayShPtr ptr = IDataArray::NullPointer();

  if(CanDynamicCast<FloatArrayType>()(sourceArrayType))
  {
    ptr = FloatArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(CanDynamicCast<DoubleArrayType>()(sourceArrayType))
  {
    ptr = DoubleArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(CanDynamicCast<Int8ArrayType>()(sourceArrayType))
  {
    ptr = Int8ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(CanDynamicCast<UInt8ArrayType>()(sourceArrayType))
  {
    ptr = UInt8ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(CanDynamicCast<Int16ArrayType>()(sourceArrayType))
  {
    ptr = Int16ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(CanDynamicCast<UInt16ArrayType>()(sourceArrayType))
  {
    ptr = UInt16ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(CanDynamicCast<Int32ArrayType>()(sourceArrayType))
  {
    ptr = Int32ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(CanDynamicCast<UInt32ArrayType>()(sourceArrayType))
  {
    ptr = UInt32ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(CanDynamicCast<Int64ArrayType>()(sourceArrayType))
  {
    ptr = Int64ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(CanDynamicCast<UInt64ArrayType>()(sourceArrayType))
  {
    ptr = UInt64ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(CanDynamicCast<BoolArrayType>()(sourceArrayType))
  {
    ptr = BoolArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else
  {
    QString msg = QObject::tr("The created array is of unsupported type.");
    f->setErrorCondition(Errors::UnsupportedType);
    f->notifyErrorMessage(f->getHumanLabel(), msg, f->getErrorCondition());
  }
  return ptr;
}

// -----------------------------------------------------------------------------
IDataArrayShPtr CreateArrayFromType::operator()(AbstractFilter* f, size_t numTuples, QVector<size_t> compDims, QString arrayName, bool allocate, QString type)
{
  CreateArrayFromType classInstance;
  QVector<size_t> tupleDims(1, numTuples);
  return classInstance(f, tupleDims, compDims, arrayName, allocate, type);
}

// -----------------------------------------------------------------------------
IDataArrayShPtr CreateArrayFromType::operator()(AbstractFilter* f, QVector<size_t> tupleDims, QVector<size_t> compDims, QString arrayName, bool allocate, QString type)
{
  IDataArrayShPtr ptr = IDataArray::NullPointer();

  if(type.compare(SIMPL::TypeNames::Float) == 0)
  {
    ptr = FloatArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(type.compare(SIMPL::TypeNames::Double) == 0)
  {
    ptr = DoubleArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(type.compare(SIMPL::TypeNames::Int8) == 0)
  {
    ptr = Int8ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(type.compare(SIMPL::TypeNames::UInt8) == 0)
  {
    ptr = UInt8ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(type.compare(SIMPL::TypeNames::Int16) == 0)
  {
    ptr = Int16ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(type.compare(SIMPL::TypeNames::UInt16) == 0)
  {
    ptr = UInt16ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(type.compare(SIMPL::TypeNames::Int32) == 0)
  {
    ptr = Int32ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(type.compare(SIMPL::TypeNames::UInt32) == 0)
  {
    ptr = UInt32ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(type.compare(SIMPL::TypeNames::Int64) == 0)
  {
    ptr = Int64ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else if(type.compare(SIMPL::TypeNames::UInt64) == 0)
  {
    ptr = UInt64ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
  }
  else
  {
    QString msg = QObject::tr("The created array is of unsupported type.");
    f->setErrorCondition(Errors::UnsupportedType);
    f->notifyErrorMessage(f->getHumanLabel(), msg, f->getErrorCondition());
  }
  return ptr;
}

// -----------------------------------------------------------------------------
IDataArrayWkPtr GetPrereqArrayFromPath::operator()(AbstractFilter* f, DataArrayPath arrayPath, QVector<size_t>& compDims)
{
  IDataArrayShPtr retPtr = IDataArray::NullPointer();
  DataContainer::Pointer volDataCntr = f->getDataContainerArray()->template getPrereqDataContainer<AbstractFilter>(f, arrayPath.getDataContainerName(), false);
  if(f->getErrorCondition() < 0 || nullptr == volDataCntr)
  {
    QString ss = QObject::tr("The Data Container '%1' does not exist").arg(arrayPath.getDataContainerName());
    f->setErrorCondition(Errors::MissingDataContainer);
    f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
    return retPtr;
  }
  AttributeMatrix::Pointer cell_attr_matrix = volDataCntr->getPrereqAttributeMatrix(f, arrayPath.getAttributeMatrixName(), Errors::MissingAttributeMatrix);
  if(f->getErrorCondition() < 0 || nullptr == cell_attr_matrix.get())
  {
    QString ss = QObject::tr("The Attribute Matrix '%1' does not exist").arg(arrayPath.getAttributeMatrixName());
    f->setErrorCondition(Errors::MissingAttributeMatrix);
    f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
    return retPtr;
  }
  IDataArrayShPtr templ_ptr = cell_attr_matrix->getAttributeArray(arrayPath.getDataArrayName());
  if(nullptr == templ_ptr.get())
  {
    QString ss = QObject::tr("The input array '%1' was not found in the AttributeMatrix '%2'.").arg(arrayPath.getDataArrayName()).arg(arrayPath.getAttributeMatrixName());
    f->setErrorCondition(Errors::MissingArray);
    f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
    return retPtr;
  }
  if(compDims.isEmpty())
  {
    compDims = templ_ptr->getComponentDimensions();
  }

  IDataArrayShPtr i_data_array = cell_attr_matrix->getAttributeArray(arrayPath.getDataArrayName());
  if(CanDynamicCast<Int8ArrayType>()(i_data_array))
  {
    retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(f, arrayPath, compDims);
  }
  else if(CanDynamicCast<UInt8ArrayType>()(i_data_array))
  {
    retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(f, arrayPath, compDims);
  }
  else if(CanDynamicCast<Int16ArrayType>()(i_data_array))
  {
    retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter>(f, arrayPath, compDims);
  }
  else if(CanDynamicCast<UInt16ArrayType>()(i_data_array))
  {
    retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter>(f, arrayPath, compDims);
  }
  else if(CanDynamicCast<Int32ArrayType>()(i_data_array))
  {
    retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(f, arrayPath, compDims);
  }
  else if(CanDynamicCast<UInt32ArrayType>()(i_data_array))
  {
    retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(f, arrayPath, compDims);
  }
  else if(CanDynamicCast<Int64ArrayType>()(i_data_array))
  {
    retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(f, arrayPath, compDims);
  }
  else if(CanDynamicCast<UInt64ArrayType>()(i_data_array))
  {
    retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter>(f, arrayPath, compDims);
  }
  else if(CanDynamicCast<FloatArrayType>()(i_data_array))
  {
    retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(f, arrayPath, compDims);
  }
  else if(CanDynamicCast<DoubleArrayType>()(i_data_array))
  {
    retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(f, arrayPath, compDims);
  }
  else if(CanDynamicCast<BoolArrayType>()(i_data_array))
  {
    retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(f, arrayPath, compDims);
  }
  else
  {
    QString ss = QObject::tr("The input array %1 is of unsupported type '%2'. The following types are supported: %3")
                     .arg(arrayPath.getDataArrayName())
                     .arg(i_data_array->getTypeAsString())
                     .arg(SIMPL::TypeNames::SupportedTypeList);
    f->setErrorCondition(Errors::UnsupportedType);
    f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
  }
  return retPtr;
}

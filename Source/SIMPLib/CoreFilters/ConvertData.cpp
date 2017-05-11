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

#include "ConvertData.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/NumericTypeFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

namespace Detail
{
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
void ConvertData(T* ptr, QVector<size_t> dims, DataContainer::Pointer m, SIMPL::NumericTypes::Type scalarType, const QString attributeMatrixName, const QString& name)
{
  size_t voxels = ptr->getNumberOfTuples();
  size_t size = ptr->getSize();

  if(scalarType == SIMPL::NumericTypes::Type::Int8)
  {
    Int8ArrayType::Pointer p = Int8ArrayType::CreateArray(voxels, dims, name);
    m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
    for(size_t v = 0; v < size; ++v)
    {
      p->setValue(v, ptr->getValue(v));
    }
  }
  else if(scalarType == SIMPL::NumericTypes::Type::UInt8)
  {
    UInt8ArrayType::Pointer p = UInt8ArrayType::CreateArray(voxels, dims, name);
    m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
    for(size_t v = 0; v < size; ++v)
    {
      p->setValue(v, ptr->getValue(v));
    }
  }
  else if(scalarType == SIMPL::NumericTypes::Type::Int16)
  {
    Int16ArrayType::Pointer p = Int16ArrayType::CreateArray(voxels, dims, name);
    m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
    for(size_t v = 0; v < size; ++v)
    {
      p->setValue(v, ptr->getValue(v));
    }
  }
  else if(scalarType == SIMPL::NumericTypes::Type::UInt16)
  {
    UInt16ArrayType::Pointer p = UInt16ArrayType::CreateArray(voxels, dims, name);
    m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
    for(size_t v = 0; v < size; ++v)
    {
      p->setValue(v, ptr->getValue(v));
    }
  }
  else if(scalarType == SIMPL::NumericTypes::Type::Int32)
  {
    Int32ArrayType::Pointer p = Int32ArrayType::CreateArray(voxels, dims, name);
    m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
    for(size_t v = 0; v < size; ++v)
    {
      p->setValue(v, ptr->getValue(v));
    }
  }
  else if(scalarType == SIMPL::NumericTypes::Type::UInt32)
  {
    UInt32ArrayType::Pointer p = UInt32ArrayType::CreateArray(voxels, dims, name);
    m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
    for(size_t v = 0; v < size; ++v)
    {
      p->setValue(v, ptr->getValue(v));
    }
  }
  else if(scalarType == SIMPL::NumericTypes::Type::Int64)
  {
    Int64ArrayType::Pointer p = Int64ArrayType::CreateArray(voxels, dims, name);
    m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
    for(size_t v = 0; v < size; ++v)
    {
      p->setValue(v, ptr->getValue(v));
    }
  }
  else if(scalarType == SIMPL::NumericTypes::Type::UInt64)
  {
    UInt64ArrayType::Pointer p = UInt64ArrayType::CreateArray(voxels, dims, name);
    m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
    for(size_t v = 0; v < size; ++v)
    {
      p->setValue(v, ptr->getValue(v));
    }
  }
  else if(scalarType == SIMPL::NumericTypes::Type::Float)
  {
    FloatArrayType::Pointer p = FloatArrayType::CreateArray(voxels, dims, name);
    m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
    for(size_t v = 0; v < size; ++v)
    {
      p->setValue(v, ptr->getValue(v));
    }
  }
  else if(scalarType == SIMPL::NumericTypes::Type::Double)
  {
    DoubleArrayType::Pointer p = DoubleArrayType::CreateArray(voxels, dims, name);
    m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
    for(size_t v = 0; v < size; ++v)
    {
      p->setValue(v, ptr->getValue(v));
    }
  }
}
} // End Namespace Detail

#define CHECK_AND_CONVERT(Type, DataContainer, ScalarType, Array, AttributeMatrixName, OutputName)                                                                                                     \
  if(false == completed)                                                                                                                                                                               \
  {                                                                                                                                                                                                    \
    Type* Type##Ptr = Type::SafePointerDownCast(Array.get());                                                                                                                                          \
    if(nullptr != Type##Ptr)                                                                                                                                                                           \
    {                                                                                                                                                                                                  \
      QVector<size_t> dims = Array->getComponentDimensions();                                                                                                                                          \
      Detail::ConvertData<Type>(Type##Ptr, dims, DataContainer, ScalarType, AttributeMatrixName, OutputName);                                                                                          \
      completed = true;                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
  }

// Include the MOC generated file for this class
#include "moc_ConvertData.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertData::ConvertData()
: AbstractFilter()
, m_ScalarType(SIMPL::NumericTypes::Type::Int8)
, m_OutputArrayName("")
, m_SelectedCellArrayPath("", "", "")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertData::~ConvertData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_NUMERICTYPE_FP("Scalar Type", ScalarType, FilterParameter::Parameter, ConvertData));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to Convert", SelectedCellArrayPath, FilterParameter::RequiredArray, ConvertData, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("Converted Attribute Array", OutputArrayName, FilterParameter::CreatedArray, ConvertData));

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
  setErrorCondition(0);

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getSelectedCellArrayPath().getDataContainerName(), false);

  QString ss;
  if(m_OutputArrayName.isEmpty() == true)
  {
    ss = QObject::tr("The output array name must be set");
    setErrorCondition(-398);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(getInPreflight())
  {
    AttributeMatrix::Pointer cellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedCellArrayPath, -301);
    if(getErrorCondition() < 0)
    {
      return;
    }

    IDataArray::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedCellArrayPath());
    if(getErrorCondition() < 0)
    {
      return;
    }

    QVector<size_t> dims = p->getComponentDimensions();
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
    cellAttrMat->addAttributeArray(p->getName(), p);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedCellArrayPath.getDataContainerName());
  AttributeMatrix::Pointer am = m->getAttributeMatrix(m_SelectedCellArrayPath.getAttributeMatrixName());

  IDataArray::Pointer iArray = am->getAttributeArray(m_SelectedCellArrayPath.getDataArrayName());

  if (nullptr == iArray.get())
  {
    setErrorCondition(-90002);
    return;
  }

  bool completed = false;

  CHECK_AND_CONVERT(UInt8ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int8ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(UInt16ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int16ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(UInt32ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int32ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(UInt64ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int64ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(FloatArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(DoubleArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(BoolArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertData::newFilterInstance(bool copyFilterParameters)
{
  ConvertData::Pointer filter = ConvertData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertData::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertData::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertData::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertData::getGroupName()
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertData::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertData::getHumanLabel()
{
  return "Convert Attribute Data Type";
}

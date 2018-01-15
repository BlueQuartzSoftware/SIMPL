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

#include "CopyFeatureArrayToElementArray.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/SIMPLibVersion.h"

#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyFeatureArrayToElementArray::CopyFeatureArrayToElementArray()
: AbstractFilter()
, m_SelectedFeatureArrayPath("", "", "")
, m_FeatureIdsArrayPath("", "", "")
, m_CreatedArrayName("")
, m_FeatureIds(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyFeatureArrayToElementArray::~CopyFeatureArrayToElementArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToElementArray::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Data to Copy to Element Data", SelectedFeatureArrayPath, FilterParameter::RequiredArray, CopyFeatureArrayToElementArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, CopyFeatureArrayToElementArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Copied Attribute Array", CreatedArrayName, FilterParameter::CreatedArray, CopyFeatureArrayToElementArray));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToElementArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setSelectedFeatureArrayPath(reader->readDataArrayPath("SelectedFeatureArrayPath", getSelectedFeatureArrayPath()));
  setCreatedArrayName(reader->readString("CreatedArrayName", getCreatedArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToElementArray::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToElementArray::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  if(getCreatedArrayName().isEmpty() == true)
  {
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), "The new Element array name must be set", getErrorCondition());
    return;
  }

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_InArrayPtr =
      getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedFeatureArrayPath()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  DataArrayPath tempPath(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), "");
  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, tempPath, -301);

  if(getErrorCondition() < 0)
  {
    return;
  }

  tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getCreatedArrayName());
  TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, m_InArrayPtr.lock()->getComponentDimensions(), m_InArrayPtr.lock());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToElementArray::preflight()
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
template <typename T> IDataArray::Pointer copyData(IDataArray::Pointer inputData, size_t totalPoints, int32_t* featureIds)
{
  QString cellArrayName = inputData->getName();

  typename DataArray<T>::Pointer feature = std::dynamic_pointer_cast<DataArray<T>>(inputData);
  if(nullptr == feature)
  {
    return IDataArray::NullPointer();
  }

  QVector<size_t> cDims = inputData->getComponentDimensions();
  typename DataArray<T>::Pointer cell = DataArray<T>::CreateArray(totalPoints, cDims, cellArrayName);

  T* fPtr = feature->getPointer(0);
  T* cPtr = cell->getPointer(0);

  int32_t numComp = feature->getNumberOfComponents();
  int32_t featureIdx = 0;

  for(size_t i = 0; i < totalPoints; ++i)
  {
    // Get the feature id (or what ever the user has selected as their "Feature" identifier
    featureIdx = featureIds[i];
    // Now get the pointer to the start of the tuple for the Feature Array at the given Feature Id Index value
    T* fSourcePtr = fPtr + (numComp * featureIdx);
    // Now get the pointer to the start of the tuple for the Cell Array at the proper index
    T* cDestPtr = cPtr + (numComp * i);

    // Now just raw copy the bytes from the source to the destination
    ::memcpy(cDestPtr, fSourcePtr, sizeof(T) * numComp);
  }
  return cell;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToElementArray::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  // Validate that the selected InArray has tuples equal to the largest
  // Feature Id; the filter would not crash otherwise, but the user should
  // be notified of unanticipated behavior ; this cannot be done in the dataCheck since
  // we don't have acces to the data yet
  int32_t numFeatures = static_cast<int32_t>(m_InArrayPtr.lock()->getNumberOfTuples());
  bool mismatchedFeatures = false;
  int32_t largestFeature = 0;
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  for(size_t i = 0; i < totalPoints; i++)
  {
    if(m_FeatureIds[i] > largestFeature)
    {
      largestFeature = m_FeatureIds[i];
      if(largestFeature >= numFeatures)
      {
        mismatchedFeatures = true;
        break;
      }
    }
  }

  if(mismatchedFeatures == true)
  {
    QString ss = QObject::tr("The largest Feature Id (%1) in the FeatureIds array is larger than the number of Features in the InArray array (%2)").arg(largestFeature).arg(numFeatures);
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(largestFeature != (numFeatures - 1))
  {
    QString ss = QObject::tr("The number of Features in the InArray array (%1) does not match the largest Feature Id in the FeatureIds array").arg(numFeatures);
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  IDataArray::Pointer p = IDataArray::NullPointer();

  if(TemplateHelpers::CanDynamicCast<Int8ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyData<int8_t>(m_InArrayPtr.lock(), totalPoints, m_FeatureIds);
  }
  else if(TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyData<uint8_t>(m_InArrayPtr.lock(), totalPoints, m_FeatureIds);
  }
  else if(TemplateHelpers::CanDynamicCast<Int16ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyData<int16_t>(m_InArrayPtr.lock(), totalPoints, m_FeatureIds);
  }
  else if(TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyData<uint16_t>(m_InArrayPtr.lock(), totalPoints, m_FeatureIds);
  }
  else if(TemplateHelpers::CanDynamicCast<Int32ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyData<int32_t>(m_InArrayPtr.lock(), totalPoints, m_FeatureIds);
  }
  else if(TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyData<uint32_t>(m_InArrayPtr.lock(), totalPoints, m_FeatureIds);
  }
  else if(TemplateHelpers::CanDynamicCast<Int64ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyData<int64_t>(m_InArrayPtr.lock(), totalPoints, m_FeatureIds);
  }
  else if(TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyData<uint64_t>(m_InArrayPtr.lock(), totalPoints, m_FeatureIds);
  }
  else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyData<float>(m_InArrayPtr.lock(), totalPoints, m_FeatureIds);
  }
  else if(TemplateHelpers::CanDynamicCast<DoubleArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyData<double>(m_InArrayPtr.lock(), totalPoints, m_FeatureIds);
  }
  else if(TemplateHelpers::CanDynamicCast<BoolArrayType>()(m_InArrayPtr.lock()))
  {
    p = copyData<bool>(m_InArrayPtr.lock(), totalPoints, m_FeatureIds);
  }
  else
  {
    QString ss = QObject::tr("The selected array was of unsupported type. The path is %1").arg(m_SelectedFeatureArrayPath.serialize());
    setErrorCondition(-14000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(p.get() != nullptr)
  {
    p->setName(getCreatedArrayName());
    AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(getFeatureIdsArrayPath());
    am->addAttributeArray(p->getName(), p);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CopyFeatureArrayToElementArray::newFilterInstance(bool copyFilterParameters)
{
  CopyFeatureArrayToElementArray::Pointer filter = CopyFeatureArrayToElementArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyFeatureArrayToElementArray::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyFeatureArrayToElementArray::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyFeatureArrayToElementArray::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyFeatureArrayToElementArray::getGroupName()
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CopyFeatureArrayToElementArray::getUuid()
{
  return QUuid("{99836b75-144b-5126-b261-b411133b5e8a}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyFeatureArrayToElementArray::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyFeatureArrayToElementArray::getHumanLabel()
{
  return "Create Element Array from Feature Array";
}

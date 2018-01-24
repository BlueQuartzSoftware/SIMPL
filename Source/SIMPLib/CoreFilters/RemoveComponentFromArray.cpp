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

#include "RemoveComponentFromArray.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveComponentFromArray::RemoveComponentFromArray()
: AbstractFilter()
, m_SelectedArrayPath("", "", "")
, m_CompNumber(0)
, m_SaveRemovedComponent(false)
, m_NewArrayArrayName("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveComponentFromArray::~RemoveComponentFromArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveComponentFromArray::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Component Number to Remove", CompNumber, FilterParameter::Parameter, RemoveComponentFromArray));

  DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Category::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Multicomponent Attribute Array", SelectedArrayPath, FilterParameter::RequiredArray, RemoveComponentFromArray, req));

  parameters.push_back(SIMPL_NEW_STRING_FP("Removed Component Attribute Array", NewArrayArrayName, FilterParameter::CreatedArray, RemoveComponentFromArray));

  parameters.push_back(SIMPL_NEW_STRING_FP("Reduced Attribute Array", ReducedArrayArrayName, FilterParameter::CreatedArray, RemoveComponentFromArray));

  QStringList linkedProps;
  linkedProps.clear();
  linkedProps << "NewArrayArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save Removed Component in New Array", SaveRemovedComponent, FilterParameter::Parameter, RemoveComponentFromArray, linkedProps));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveComponentFromArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewArrayArrayName(reader->readString("NewArrayArrayName", getNewArrayArrayName()));
  setNewArrayArrayName(reader->readString("ReducedArrayArrayName", getReducedArrayArrayName()));
  setCompNumber(reader->readValue("CompNumber", getCompNumber()));
  setSelectedArrayPath(reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()));
  setSaveRemovedComponent(reader->readValue("SaveRemovedComponent", getSaveRemovedComponent()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveComponentFromArray::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveComponentFromArray::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  m_InArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedArrayPath());

  if(m_SaveRemovedComponent == true)
  {
    if(m_NewArrayArrayName.isEmpty() == true)
    {
      setErrorCondition(-11001);
      notifyErrorMessage(getHumanLabel(), "Removed Component array name must be set.", getErrorCondition());
      return;
    }
  }

  if(m_ReducedArrayArrayName.isEmpty() == true)
  {
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), "Reduced array name must be set.", getErrorCondition());
    return;
  }

  if(getErrorCondition() < 0)
  {
    return;
  }

  if(m_InArrayPtr.lock()->getNumberOfComponents() < 2)
  {
    setErrorCondition(-11003);
    QString ss = QObject::tr("Selected array '%1' must have more than 1 component. The number of components is %2")
                     .arg(getSelectedArrayPath().getDataArrayName())
                     .arg(m_InArrayPtr.lock()->getNumberOfComponents());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_CompNumber >= m_InArrayPtr.lock()->getNumberOfComponents())
  {
    setErrorCondition(-11004);
    QString ss = QObject::tr("Error removing component from DataArray '%3', Component to remove (%1) is greater than or equal to the number of components (%2) for array selected.")
                     .arg(m_CompNumber)
                     .arg(m_InArrayPtr.lock()->getNumberOfComponents())
                     .arg(getSelectedArrayPath().getDataArrayName());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> cDims(1, 1);
  if(m_SaveRemovedComponent == true)
  {
    DataArrayPath tempPath(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getNewArrayArrayName());
    m_NewArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, cDims, m_InArrayPtr.lock());
  }

  cDims[0] = m_InArrayPtr.lock()->getNumberOfComponents() - 1;
  DataArrayPath tempPath2(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getReducedArrayArrayName());
  m_ReducedArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath2, cDims, m_InArrayPtr.lock());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveComponentFromArray::preflight()
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
template <typename T> void extractComponent(IDataArray::Pointer inputData, IDataArray::Pointer newData, IDataArray::Pointer reducedData, int compNumber)
{
  typename DataArray<T>::Pointer inputArrayPtr = std::dynamic_pointer_cast<DataArray<T>>(inputData);
  typename DataArray<T>::Pointer newArrayPtr = std::dynamic_pointer_cast<DataArray<T>>(newData);
  typename DataArray<T>::Pointer reducedArrayPtr = std::dynamic_pointer_cast<DataArray<T>>(reducedData);

  if(nullptr == inputArrayPtr || nullptr == newArrayPtr)
  {
    return;
  }

  T* inputArray = inputArrayPtr->getPointer(0);
  T* newArray = newArrayPtr->getPointer(0);
  T* reducedArray = reducedArrayPtr->getPointer(0);

  size_t numPoints = inputArrayPtr->getNumberOfTuples();
  size_t numComps = inputArrayPtr->getNumberOfComponents();

  for(size_t i = 0; i < numPoints; i++)
  {
    for(size_t j = 0; j < numComps; j++)
    {
      if(j == compNumber)
      {
        newArray[i] = inputArray[numComps * i + j];
      }
      else if(j > compNumber)
      {
        reducedArray[(numComps - 1) * i + j - 1] = inputArray[numComps * i + j];
      }
      else if(j < compNumber)
      {
        reducedArray[(numComps - 1) * i + j] = inputArray[numComps * i + j];
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> void reduceArrayOnly(IDataArray::Pointer inputData, IDataArray::Pointer reducedData, int compNumber)
{
  typename DataArray<T>::Pointer inputArrayPtr = std::dynamic_pointer_cast<DataArray<T>>(inputData);
  typename DataArray<T>::Pointer reducedArrayPtr = std::dynamic_pointer_cast<DataArray<T>>(reducedData);

  if(nullptr == inputArrayPtr)
  {
    return;
  }

  T* inputArray = inputArrayPtr->getPointer(0);
  T* reducedArray = reducedArrayPtr->getPointer(0);

  size_t numPoints = inputArrayPtr->getNumberOfTuples();
  size_t numComps = inputArrayPtr->getNumberOfComponents();

  for(size_t i = 0; i < numPoints; i++)
  {
    for(size_t j = 0; j < numComps; j++)
    {

      if(j > compNumber)
      {
        reducedArray[(numComps - 1) * i + j - 1] = inputArray[numComps * i + j];
      }
      else if(j < compNumber)
      {
        reducedArray[(numComps - 1) * i + j] = inputArray[numComps * i + j];
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveComponentFromArray::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(m_SaveRemovedComponent == true)
  {
    EXECUTE_FUNCTION_TEMPLATE(this, extractComponent, m_InArrayPtr.lock(), m_InArrayPtr.lock(), m_NewArrayPtr.lock(), m_ReducedArrayPtr.lock(), m_CompNumber)
  }
  else if(m_SaveRemovedComponent == false)
  {
    EXECUTE_FUNCTION_TEMPLATE(this, reduceArrayOnly, m_InArrayPtr.lock(), m_InArrayPtr.lock(), m_ReducedArrayPtr.lock(), m_CompNumber)
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RemoveComponentFromArray::newFilterInstance(bool copyFilterParameters) const
{
  RemoveComponentFromArray::Pointer filter = RemoveComponentFromArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveComponentFromArray::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveComponentFromArray::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveComponentFromArray::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveComponentFromArray::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid RemoveComponentFromArray::getUuid()
{
  return QUuid("{1b4b9941-62e4-52f2-9918-15d48147ab88}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveComponentFromArray::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveComponentFromArray::getHumanLabel() const
{
  return "Remove Component From Array";
}

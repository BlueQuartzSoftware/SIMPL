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

#include "ExtractComponentAsArray.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractComponentAsArray::ExtractComponentAsArray()
: m_SelectedArrayPath("", "", "")
, m_CompNumber(0)
, m_NewArrayArrayName("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractComponentAsArray::~ExtractComponentAsArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractComponentAsArray::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Component Number to Extract", CompNumber, FilterParameter::Parameter, ExtractComponentAsArray));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Multicomponent Attribute Array", SelectedArrayPath, FilterParameter::RequiredArray, ExtractComponentAsArray, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("Scalar Attribute Array", NewArrayArrayName, FilterParameter::CreatedArray, ExtractComponentAsArray));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractComponentAsArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewArrayArrayName(reader->readString("NewArrayArrayName", getNewArrayArrayName()));
  setCompNumber(reader->readValue("CompNumber", getCompNumber()));
  setSelectedArrayPath(reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractComponentAsArray::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractComponentAsArray::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  m_InArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedArrayPath());

  if(m_NewArrayArrayName.isEmpty() == true)
  {
    setErrorCondition(-11003);
    notifyErrorMessage(getHumanLabel(), "New array name must be set.", getErrorCondition());
    return;
  }

  if(getErrorCondition() < 0)
  {
    return;
  }

  if(m_InArrayPtr.lock()->getNumberOfComponents() < 2)
  {
    QString ss = QObject::tr("Selected array '%1' must have more than 1 component. The number of components is %2")
                     .arg(getSelectedArrayPath().getDataArrayName())
                     .arg(m_InArrayPtr.lock()->getNumberOfComponents());
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_CompNumber >= m_InArrayPtr.lock()->getNumberOfComponents())
  {
    setErrorCondition(-11004);
    QString ss = QObject::tr("Error extracting component from DataArray '%3', Component to extract (%1) is greater than or equal to the number of components (%2).")
                     .arg(m_CompNumber)
                     .arg(m_InArrayPtr.lock()->getNumberOfComponents())
                     .arg(getSelectedArrayPath().getDataArrayName());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_CompNumber < 0)
  {
    setErrorCondition(-11005);
    QString ss = QObject::tr("Component to extract (%1) is a negative value and this is not allowed. Value must be Zero (0) or greater.")
                     .arg(m_CompNumber);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> cDims(1, 1);
  DataArrayPath tempPath(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getNewArrayArrayName());
  m_NewArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, cDims, m_InArrayPtr.lock());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractComponentAsArray::preflight()
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
template <typename T> void extractComponent(IDataArray::Pointer inputData, IDataArray::Pointer newData, int compNumber)
{
  typename DataArray<T>::Pointer inputArrayPtr = std::dynamic_pointer_cast<DataArray<T>>(inputData);
  typename DataArray<T>::Pointer newArrayPtr = std::dynamic_pointer_cast<DataArray<T>>(newData);

  if(nullptr == inputArrayPtr || nullptr == newArrayPtr)
  {
    return;
  }

  T* inputArray = inputArrayPtr->getPointer(0);
  T* newArray = newArrayPtr->getPointer(0);
  size_t numPoints = inputArrayPtr->getNumberOfTuples();
  size_t numComps = inputArrayPtr->getNumberOfComponents();

  for(size_t i = 0; i < numPoints; i++)
  {
    newArray[i] = inputArray[numComps * i + compNumber];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractComponentAsArray::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  EXECUTE_FUNCTION_TEMPLATE(this, extractComponent, m_InArrayPtr.lock(), m_InArrayPtr.lock(), m_NewArrayPtr.lock(), m_CompNumber)

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExtractComponentAsArray::newFilterInstance(bool copyFilterParameters) const
{
  ExtractComponentAsArray::Pointer filter = ExtractComponentAsArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractComponentAsArray::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractComponentAsArray::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractComponentAsArray::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractComponentAsArray::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ExtractComponentAsArray::getUuid()
{
  return QUuid("{79d59b85-01e8-5c4a-a6e1-3fd3e2ceffb4}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractComponentAsArray::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractComponentAsArray::getHumanLabel() const
{
  return "Extract Component as Attribute Array";
}

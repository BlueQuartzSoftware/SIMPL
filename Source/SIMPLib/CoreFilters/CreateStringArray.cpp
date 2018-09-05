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

#include "CreateStringArray.h"

#include <QtCore/QDateTime>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

/**
* @brief initializeArrayWithInts Initializes the array p with integers, either from the
* manual value entered in the filter, or with a random number.  This function does not
* check that the template type actually is an integer, so it will most likely cause
* unexpected results when passing anything other than an integer as a template parameter.
* @param p The array that will be initialized
*/
void initializeArray(IDataArray::Pointer outputArrayPtr, const QString& initializationValue)
{
  StringDataArray::Pointer array = std::dynamic_pointer_cast<StringDataArray>(outputArrayPtr);
  array->initializeWithValue(initializationValue);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateStringArray::CreateStringArray()
: m_NumberOfComponents(0)
, m_NewArray("", "", "")
, m_InitializationValue("Text")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateStringArray::~CreateStringArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateStringArray::setupFilterParameters()
{
  FilterParameterVector parameters;

  // Do not let the user change the number of components in a StringDataArray
  setNumberOfComponents(1);

  parameters.push_back(SIMPL_NEW_STRING_FP("Initialization Value", InitializationValue, FilterParameter::Parameter, CreateStringArray));
  {
    DataArrayCreationFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Created Attribute Array", NewArray, FilterParameter::CreatedArray, CreateStringArray, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateStringArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNumberOfComponents(reader->readValue("NumberOfComponents", getNumberOfComponents()));
  setNewArray(reader->readDataArrayPath("NewArray", getNewArray()));
  setInitializationValue(reader->readString("InitializationValue", getInitializationValue()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateStringArray::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateStringArray::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  if(getErrorCondition() < 0)
  {
    return;
  }

  if(getNumberOfComponents() < 0)
  {
    setErrorCondition(-8150);
    QString ss = QObject::tr("The number of components must non-negative");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if(getNumberOfComponents() == 0)
  {
    setErrorCondition(-8151);
    QString ss = QObject::tr("The number of components is Zero. This will result in an array that has no memory allocated. Are you sure you wanted to do this?");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if(!getNewArray().isValid())
  {
    setErrorCondition(-8152);
    QString ss = QObject::tr("The Created DataArrayPath is invalid. Please select the Data Container, Attribute Matrix and set an output DataArray name.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (m_InitializationValue.isEmpty())
  {
    QString ss = "Empty initialization value.";
    setErrorCondition(-5759);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  QVector<size_t> cDims(1, getNumberOfComponents());
  if(getErrorCondition() < 0)
  {
    return;
  }
  
  // Create the data array and initialize it to a placeholder value
  m_OutputArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<StringDataArray, AbstractFilter, QString>(this, getNewArray(), m_InitializationValue, cDims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateStringArray::preflight()
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
void CreateStringArray::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  initializeArray(m_OutputArrayPtr.lock(), m_InitializationValue);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateStringArray::newFilterInstance(bool copyFilterParameters) const
{
  CreateStringArray::Pointer filter = CreateStringArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateStringArray::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateStringArray::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateStringArray::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateStringArray::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CreateStringArray::getUuid()
{
  return QUuid("{e6b9a566-c5eb-5e3a-87de-7fe65d1d12b6}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateStringArray::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateStringArray::getHumanLabel() const
{
  return "Create String Array";
}

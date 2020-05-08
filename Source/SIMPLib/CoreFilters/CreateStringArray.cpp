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

#include <QtCore/QTextStream>

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
#include "SIMPLib/DataContainers/DataContainerArray.h"

enum createdPathID : RenameDataPath::DataID_t
{
  StringArrayID = 1
};

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
  FilterParameterVectorType parameters;

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
  clearErrorCode();
  clearWarningCode();

  if(getErrorCode() < 0)
  {
    return;
  }

  if(getNumberOfComponents() < 0)
  {
    QString ss = QObject::tr("The number of components must non-negative");
    setErrorCondition(-8150, ss);
  }
  if(getNumberOfComponents() == 0)
  {
    QString ss = QObject::tr("The number of components is Zero. This will result in an array that has no memory allocated. Are you sure you wanted to do this?");
    setErrorCondition(-8151, ss);
  }
  if(!getNewArray().isValid())
  {
    QString ss = QObject::tr("The Created DataArrayPath is invalid. Please select the Data Container, Attribute Matrix and set an output DataArray name.");
    setErrorCondition(-8152, ss);
  }
  if(m_InitializationValue.isEmpty())
  {
    QString ss = "Empty initialization value.";
    setErrorCondition(-5759, ss);
  }
  std::vector<size_t> cDims(1, getNumberOfComponents());
  if(getErrorCode() < 0)
  {
    return;
  }

  // Create the data array and initialize it to a placeholder value
  m_OutputArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<StringDataArray>(this, getNewArray(), m_InitializationValue, cDims, "", StringArrayID);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateStringArray::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  initializeArray(m_OutputArrayPtr.lock(), m_InitializationValue);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateStringArray::newFilterInstance(bool copyFilterParameters) const
{
  CreateStringArray::Pointer filter = CreateStringArray::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateStringArray::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateStringArray::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateStringArray::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateStringArray::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid CreateStringArray::getUuid() const
{
  return QUuid("{e6b9a566-c5eb-5e3a-87de-7fe65d1d12b6}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateStringArray::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateStringArray::getHumanLabel() const
{
  return "Create String Array";
}

// -----------------------------------------------------------------------------
CreateStringArray::Pointer CreateStringArray::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<CreateStringArray> CreateStringArray::New()
{
  struct make_shared_enabler : public CreateStringArray
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString CreateStringArray::getNameOfClass() const
{
  return QString("CreateStringArray");
}

// -----------------------------------------------------------------------------
QString CreateStringArray::ClassName()
{
  return QString("CreateStringArray");
}

// -----------------------------------------------------------------------------
void CreateStringArray::setNumberOfComponents(int value)
{
  m_NumberOfComponents = value;
}

// -----------------------------------------------------------------------------
int CreateStringArray::getNumberOfComponents() const
{
  return m_NumberOfComponents;
}

// -----------------------------------------------------------------------------
void CreateStringArray::setNewArray(const DataArrayPath& value)
{
  m_NewArray = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateStringArray::getNewArray() const
{
  return m_NewArray;
}

// -----------------------------------------------------------------------------
void CreateStringArray::setInitializationValue(const QString& value)
{
  m_InitializationValue = value;
}

// -----------------------------------------------------------------------------
QString CreateStringArray::getInitializationValue() const
{
  return m_InitializationValue;
}

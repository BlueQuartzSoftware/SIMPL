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
#include "RenameAttributeArray.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameAttributeArray::RenameAttributeArray()
: m_SelectedArrayPath("", "", "")
, m_NewArrayName("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameAttributeArray::~RenameAttributeArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  DataArraySelectionFilterParameter::RequirementType req;
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to Rename", SelectedArrayPath, FilterParameter::RequiredArray, RenameAttributeArray, req));

  parameters.push_back(SIMPL_NEW_STRING_FP("New Attribute Array Name", NewArrayName, FilterParameter::Parameter, RenameAttributeArray));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedArrayPath(reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()));
  setNewArrayName(reader->readString("NewArrayName", getNewArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(m_NewArrayName.isEmpty())
  {
    QString ss = QObject::tr("The new Attribute Array name must be set");
    setErrorCondition(-11009, ss);
    return;
  }

  QString daName = getSelectedArrayPath().getDataArrayName();

  // Update the last array name.  This is important for creating rename paths.
  if(m_LastArrayName != getNewArrayName())
  {
    DataArrayPath newArrayPath = getSelectedArrayPath();
    newArrayPath.setAttributeMatrixName(m_NewArrayName);
    DataArrayPath oldArrayPath = getSelectedArrayPath();
    oldArrayPath.setAttributeMatrixName(m_LastArrayName);
    addPathRename(oldArrayPath, newArrayPath);
    m_LastArrayName = getNewArrayName();
  }

  AttributeMatrix::Pointer attrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, getSelectedArrayPath(), -301);
  if(getErrorCode() < 0)
  {
    return;
  }

  RenameErrorCodes code = attrMat->renameAttributeArray(daName, m_NewArrayName, false);
  switch(code)
  {
  case OLD_DOES_NOT_EXIST: {
    QString ss = QObject::tr("A DataArray with the name '%1' was not found in the AttributeMatrix").arg(daName);
    setErrorCondition(-11016, ss);
    return;
  }
  case NEW_EXISTS: {
    QString ss = QObject::tr("A DataArray with the name '%1' already exists in the AttributeMatrix").arg(m_NewArrayName);
    setErrorCondition(-11017, ss);
    return;
  }
  case RenameErrorCodes::SAME_PATH: {
    QString ss = QObject::tr("The new and original DataArray names cannot be identical");
    setErrorCondition(-11018, ss);
    return;
  }
  case SUCCESS: {
    clearErrorCode();
    clearWarningCode();
    DataArrayPath newPath = getSelectedArrayPath();
    newPath.setDataArrayName(m_NewArrayName);
    addPathRename(getSelectedArrayPath(), newPath);
    return;
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck(); // calling the dataCheck will rename the array, so nothing is required here
  if(getErrorCode() < 0)
  {
    return;
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RenameAttributeArray::newFilterInstance(bool copyFilterParameters) const
{
  RenameAttributeArray::Pointer filter = RenameAttributeArray::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameAttributeArray::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameAttributeArray::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameAttributeArray::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameAttributeArray::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RenameAttributeArray::getUuid() const
{
  return QUuid("{53a5f731-2858-5e3e-bd43-8f2cf45d90ec}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameAttributeArray::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RenameAttributeArray::getHumanLabel() const
{
  return "Rename Attribute Array";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::RenameContainer RenameAttributeArray::getRenamedPaths()
{
  DataArrayPath oldPath = getSelectedArrayPath();
  DataArrayPath newPath = getSelectedArrayPath();
  newPath.setDataArrayName(getNewArrayName());

  DataArrayPath::RenameContainer container;
  DataArrayPath::RenameType rename = {oldPath, newPath};
  container.push_back(rename);

  return container;
}

// -----------------------------------------------------------------------------
RenameAttributeArray::Pointer RenameAttributeArray::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<RenameAttributeArray> RenameAttributeArray::New()
{
  struct make_shared_enabler : public RenameAttributeArray
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RenameAttributeArray::getNameOfClass() const
{
  return QString("RenameAttributeArray");
}

// -----------------------------------------------------------------------------
QString RenameAttributeArray::ClassName()
{
  return QString("RenameAttributeArray");
}

// -----------------------------------------------------------------------------
void RenameAttributeArray::setSelectedArrayPath(const DataArrayPath& value)
{
  m_SelectedArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RenameAttributeArray::getSelectedArrayPath() const
{
  return m_SelectedArrayPath;
}

// -----------------------------------------------------------------------------
void RenameAttributeArray::setNewArrayName(const QString& value)
{
  m_NewArrayName = value;
}

// -----------------------------------------------------------------------------
QString RenameAttributeArray::getNewArrayName() const
{
  return m_NewArrayName;
}

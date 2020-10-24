/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "CreateGridMontage.h"

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerGridSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateGridMontage::CreateGridMontage()
: m_MontageName("Grid Montage")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateGridMontage::~CreateGridMontage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGridMontage::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_DC_GRID_SELECTION_FP("Data Container Grid", GridSelection, FilterParameter::Parameter, CreateGridMontage));
  parameters.push_back(SIMPL_NEW_STRING_FP("Created Montage Name", MontageName, FilterParameter::CreatedArray, CreateGridMontage));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGridMontage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGridMontage::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGridMontage::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  getDataContainerArray()->createNonPrereqGridMontage(this, getMontageName(), m_GridSelection.getDimensions(), m_GridSelection.getDataContainerNames());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGridMontage::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateGridMontage::newFilterInstance(bool copyFilterParameters) const
{
  CreateGridMontage::Pointer filter = CreateGridMontage::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateGridMontage::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateGridMontage::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateGridMontage::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateGridMontage::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateGridMontage::getHumanLabel() const
{
  return "Create Grid Montage";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid CreateGridMontage::getUuid() const
{
  return QUuid("{c5bb92df-c1bb-5e57-a2bf-280303a81935}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateGridMontage::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateGridMontage::getMontageName() const
{
  return m_MontageName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGridMontage::setMontageName(const QString& value)
{
  m_MontageName = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerGrid CreateGridMontage::getGridSelection() const
{
  return m_GridSelection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGridMontage::setGridSelection(const DataContainerGrid& value)
{
  m_GridSelection = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList CreateGridMontage::getDataContainerNameList() const
{
  return m_DataContainerNameList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateGridMontage::setDataContainerNameList(const QStringList& value)
{
  m_DataContainerNameList = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateGridMontage::getNameOfClass() const
{
  return QString("CreateGridMontage");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateGridMontage::ClassName()
{
  return QString("CreateGridMontage");
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CreateGridMontage::IsTypeOf(const char* type)
{
  if(!strcmp("CreateGridMontage", type))
  {
    return 1;
  }
  return AbstractFilter::IsTypeOf(type);
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateGridMontage::Pointer CreateGridMontage::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateGridMontage::Pointer CreateGridMontage::New()
{
  Pointer sharedPtr(new(CreateGridMontage));
  return sharedPtr;
}

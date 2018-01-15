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

#include "CreateAttributeMatrix.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateAttributeMatrix::CreateAttributeMatrix()
: AbstractFilter()
, m_CreatedAttributeMatrix("", "AttributeMatrix", "")
, m_AttributeMatrixType(static_cast<int32_t>(AttributeMatrix::Type::Generic))
, m_TupleDimensions()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateAttributeMatrix::~CreateAttributeMatrix() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAttributeMatrix::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    QVector<QString> choices;
    choices.push_back("Vertex");
    choices.push_back("Edge");
    choices.push_back("Face");
    choices.push_back("Cell");
    choices.push_back("VertexFeature");
    choices.push_back("EdgeFeature");
    choices.push_back("FaceFeature");
    choices.push_back("CellFeature");
    choices.push_back("VertexEnsemble");
    choices.push_back("EdgeEnsemble");
    choices.push_back("FaceEnsemble");
    choices.push_back("CellEnsemble");
    choices.push_back("MetaData");
    choices.push_back("Generic");
    parameters.push_back(SIMPL_NEW_CHOICE_FP("Attribute Matrix Type", AttributeMatrixType, FilterParameter::Parameter, CreateAttributeMatrix, choices, false));
  }

  m_TupleDimensions.setDynamicCols(true);
  parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Tuple Dimensions", TupleDimensions, FilterParameter::Parameter, CreateAttributeMatrix));
  {
    AttributeMatrixCreationFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_CREATION_FP("Created Attribute Matrix", CreatedAttributeMatrix, FilterParameter::CreatedArray, CreateAttributeMatrix, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAttributeMatrix::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCreatedAttributeMatrix(reader->readDataArrayPath("CreatedAttributeMatrix", getCreatedAttributeMatrix()));
  setAttributeMatrixType(reader->readValue("AttributeMatrixType", getAttributeMatrixType()));
  setTupleDimensions(reader->readDynamicTableData("TupleDimensions", getTupleDimensions()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAttributeMatrix::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAttributeMatrix::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getCreatedAttributeMatrix().getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  DynamicTableData dtd = getTupleDimensions();
  std::vector<std::vector<double>> rows = dtd.getTableData();

  if(rows.size() != 1)
  {
    QString ss = QObject::tr("The number of rows of data must be 1. The data currently has %1").arg(rows.size());
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  std::vector<double> cols = rows[0]; // Get the first (and only) row of data
  QVector<size_t> tDims(cols.size(), 0);
  for(int i = 0; i < cols.size(); i++)
  {
    tDims[i] = static_cast<size_t>(cols[i]);
  }


  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCreatedAttributeMatrix().getAttributeMatrixName(), tDims, static_cast<AttributeMatrix::Type>(getAttributeMatrixType()));
  if(getErrorCondition() < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAttributeMatrix::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAttributeMatrix::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(getCancel() == true)
  {
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateAttributeMatrix::newFilterInstance(bool copyFilterParameters)
{
  CreateAttributeMatrix::Pointer filter = CreateAttributeMatrix::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAttributeMatrix::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAttributeMatrix::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAttributeMatrix::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAttributeMatrix::getGroupName()
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAttributeMatrix::getHumanLabel()
{
  return "Create Attribute Matrix";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CreateAttributeMatrix::getUuid()
{
  return QUuid("{93375ef0-7367-5372-addc-baa019b1b341}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAttributeMatrix::getSubGroupName()
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

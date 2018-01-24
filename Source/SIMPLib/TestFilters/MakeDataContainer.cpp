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

#include "MakeDataContainer.h"

#include "SIMPLib/DataArrays/StringDataArray.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"

#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MakeDataContainer::MakeDataContainer()
: AbstractFilter()
, m_DataContainerName(SIMPL::Defaults::DataContainerName)
, m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_PhaseNameArrayName("Phase")
, m_MaterialNameArrayName(SIMPL::EnsembleData::PhaseName)
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
, m_CellEulerAnglesArrayName(SIMPL::CellData::EulerAngles)
, m_CellPhasesArrayName(SIMPL::CellData::Phases)
, m_CrystalStructuresArrayName(SIMPL::EnsembleData::CrystalStructures)
, m_LatticeConstantsArrayName(SIMPL::EnsembleData::LatticeConstants)
, m_FeatureIds(nullptr)
, m_CellPhases(nullptr)
, m_CellEulerAngles(nullptr)
, m_CrystalStructures(nullptr)
, m_LatticeConstants(nullptr)
{

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MakeDataContainer::~MakeDataContainer() = default;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeDataContainer::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_STRING_FP("FeatureIds", FeatureIdsArrayName, FilterParameter::CreatedArray, MakeDataContainer));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Euler Angles", CellEulerAnglesArrayName, FilterParameter::CreatedArray, MakeDataContainer));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Phases", CellPhasesArrayName, FilterParameter::CreatedArray, MakeDataContainer));
  parameters.push_back(SIMPL_NEW_STRING_FP("Crystal Structures", CrystalStructuresArrayName, FilterParameter::CreatedArray, MakeDataContainer));
  parameters.push_back(SIMPL_NEW_STRING_FP("LatticeConstants", LatticeConstantsArrayName, FilterParameter::CreatedArray, MakeDataContainer));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeDataContainer::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeDataContainer::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeDataContainer::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  setWarningCondition(0);
  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }
  QVector<size_t> tDims(3, 64);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(getErrorCondition() < 0)
  {
    return;
  }
  //  tDims.resize(1);
  //  tDims[0] = 0;
  //  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble);
  //  if(getErrorCondition() < 0)
  //  {
  //    return;
  //  }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr =
      cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_FeatureIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // ImageGeom::Pointer image = ImageGeom::CreateGeometry("TestImageGeom");
  // image->setResolution(0.1f, 0.2f, 0.3f);
  // image->setOrigin(100.3f, 987.234f, 0.0f);
  // image->setDimensions(64, 64, 64);
  // m->setGeometry(image);

  VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(100, "TestVertexGeom", !getInPreflight());
  if(!getInPreflight())
  {
    SharedVertexList::Pointer test = vertices->getVertices();
    float* verts = test->getPointer(0);
    for(int64_t i = 0; i < vertices->getNumberOfVertices(); i++)
    {
      verts[3 * i] = float(0.1 + i);
      verts[3 * i + 1] = float(0.2 + i);
      verts[3 * i + 2] = float(0.3 + i);
    }
  }
  m->setGeometry(vertices);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeDataContainer::preflight()
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
void MakeDataContainer::execute()
{
  int err = 0;
  setErrorCondition(err);

  // Run the data check to get references to all of our data arrays initialized to the values stored in memory
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  // ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getDataContainerName())->getGeometryAs<ImageGeom>();

  // size_t index;
  // size_t iDims[3] = {0, 0, 0};
  // image->getDimensions(iDims);

  // for (size_t z=0;z<image->getZPoints();z++)
  //{
  //  for (size_t y=0;y<image->getYPoints();y++)
  //  {
  //    for (size_t x=0;x<image->getXPoints();x++)
  //    {
  //      index = (z * iDims[0] * iDims[1]) + (y * iDims[0]) + x;
  //      m_FeatureIds[index] = z + x + y;
  //    }
  //  }
  //}

  VertexGeom::Pointer verts = getDataContainerArray()->getDataContainer(getDataContainerName())->getGeometryAs<VertexGeom>();

  for(int64_t i = 0; i < verts->getNumberOfVertices(); i++)
  {
    m_FeatureIds[i] = i;
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MakeDataContainer::newFilterInstance(bool copyFilterParameters) const
{
  MakeDataContainer::Pointer filter = MakeDataContainer::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MakeDataContainer::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MakeDataContainer::getGroupName() const
{
  return SIMPL::FilterGroups::Generic;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid MakeDataContainer::getUuid()
{
  return QUuid("{9df9906c-1db6-5ecf-a85c-c4ef1a484c05}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MakeDataContainer::getSubGroupName() const
{
  return "Test";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MakeDataContainer::getHumanLabel() const
{
  return "Make DataContainer";
}

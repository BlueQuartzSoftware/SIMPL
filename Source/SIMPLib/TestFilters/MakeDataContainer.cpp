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

#include <memory>

#include "MakeDataContainer.h"

#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/DataContainers/DataContainer.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MakeDataContainer::MakeDataContainer()
: m_DataContainerName(SIMPL::Defaults::DataContainerName)
, m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_PhaseNameArrayName("Phase")
, m_MaterialNameArrayName(SIMPL::EnsembleData::PhaseName)
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
, m_CellEulerAnglesArrayName(SIMPL::CellData::EulerAngles)
, m_CellPhasesArrayName(SIMPL::CellData::Phases)
, m_CrystalStructuresArrayName(SIMPL::EnsembleData::CrystalStructures)
, m_LatticeConstantsArrayName(SIMPL::EnsembleData::LatticeConstants)
{
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
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("FeatureIds", FeatureIdsArrayName, DataContainerName, CellAttributeMatrixName, FilterParameter::CreatedArray, MakeDataContainer));
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
  clearErrorCode();
  clearWarningCode();

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer(this, getDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }
  std::vector<size_t> tDims(3, 64);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(getErrorCode() < 0)
  {
    return;
  }
  //  tDims.resize(1);
  //  tDims[0] = 0;
  //  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble);
  //  if(getErrorCode() < 0)
  //  {
  //    return;
  //  }

  std::vector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>>(this, m_FeatureIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                          /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // ImageGeom::Pointer image = ImageGeom::CreateGeometry("TestImageGeom");
  // image->setSpacing(0.1f, 0.2f, 0.3f);
  // image->setOrigin(100.3f, 987.234f, 0.0f);
  // image->setDimensions(64, 64, 64);
  // m->setGeometry(image);

  VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(100, "TestVertexGeom", !getInPreflight());
  if(!getInPreflight())
  {
    SharedVertexList::Pointer test = vertices->getVertices();
    float* verts = test->getPointer(0);
    for(size_t i = 0; i < vertices->getNumberOfVertices(); i++)
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
void MakeDataContainer::execute()
{
  clearErrorCode();
  clearWarningCode();

  // Run the data check to get references to all of our data arrays initialized to the values stored in memory
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getDataContainerName())->getGeometryAs<ImageGeom>();

  // size_t index;
  // size_t iDims[3] = {0, 0, 0};
  // std::tie(iDims[0], iDims[1], iDims[2]) = image->getDimensions();

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

  for(size_t i = 0; i < verts->getNumberOfVertices(); i++)
  {
    m_FeatureIds[i] = static_cast<int32_t>(i);
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MakeDataContainer::newFilterInstance(bool copyFilterParameters) const
{
  MakeDataContainer::Pointer filter = MakeDataContainer::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MakeDataContainer::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MakeDataContainer::getGroupName() const
{
  return SIMPL::FilterGroups::Generic;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid MakeDataContainer::getUuid() const
{
  return QUuid("{9df9906c-1db6-5ecf-a85c-c4ef1a484c05}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MakeDataContainer::getSubGroupName() const
{
  return "Test";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MakeDataContainer::getHumanLabel() const
{
  return "Make DataContainer";
}

// -----------------------------------------------------------------------------
MakeDataContainer::Pointer MakeDataContainer::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<MakeDataContainer> MakeDataContainer::New()
{
  struct make_shared_enabler : public MakeDataContainer
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::getNameOfClass() const
{
  return QString("MakeDataContainer");
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::ClassName()
{
  return QString("MakeDataContainer");
}

// -----------------------------------------------------------------------------
void MakeDataContainer::setDataContainerName(const QString& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void MakeDataContainer::setCellEnsembleAttributeMatrixName(const QString& value)
{
  m_CellEnsembleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::getCellEnsembleAttributeMatrixName() const
{
  return m_CellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void MakeDataContainer::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void MakeDataContainer::setPhaseNameArrayName(const QString& value)
{
  m_PhaseNameArrayName = value;
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::getPhaseNameArrayName() const
{
  return m_PhaseNameArrayName;
}

// -----------------------------------------------------------------------------
void MakeDataContainer::setMaterialNameArrayName(const QString& value)
{
  m_MaterialNameArrayName = value;
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::getMaterialNameArrayName() const
{
  return m_MaterialNameArrayName;
}

// -----------------------------------------------------------------------------
void MakeDataContainer::setFeatureIdsArrayName(const QString& value)
{
  m_FeatureIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::getFeatureIdsArrayName() const
{
  return m_FeatureIdsArrayName;
}

// -----------------------------------------------------------------------------
void MakeDataContainer::setCellEulerAnglesArrayName(const QString& value)
{
  m_CellEulerAnglesArrayName = value;
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::getCellEulerAnglesArrayName() const
{
  return m_CellEulerAnglesArrayName;
}

// -----------------------------------------------------------------------------
void MakeDataContainer::setCellPhasesArrayName(const QString& value)
{
  m_CellPhasesArrayName = value;
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::getCellPhasesArrayName() const
{
  return m_CellPhasesArrayName;
}

// -----------------------------------------------------------------------------
void MakeDataContainer::setCrystalStructuresArrayName(const QString& value)
{
  m_CrystalStructuresArrayName = value;
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::getCrystalStructuresArrayName() const
{
  return m_CrystalStructuresArrayName;
}

// -----------------------------------------------------------------------------
void MakeDataContainer::setLatticeConstantsArrayName(const QString& value)
{
  m_LatticeConstantsArrayName = value;
}

// -----------------------------------------------------------------------------
QString MakeDataContainer::getLatticeConstantsArrayName() const
{
  return m_LatticeConstantsArrayName;
}

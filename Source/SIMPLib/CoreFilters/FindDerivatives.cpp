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

#include "FindDerivatives.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/GeometryHelpers.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDerivatives::FindDerivatives()
: AbstractFilter()
, m_SelectedArrayPath("", "", "")
, m_DerivativesArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "Derivatives")
, m_Interpolate(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDerivatives::~FindDerivatives() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDerivatives::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    AttributeMatrix::Types amTypes;
    amTypes.push_back(AttributeMatrix::Type::Cell);
    amTypes.push_back(AttributeMatrix::Type::Face);
    amTypes.push_back(AttributeMatrix::Type::Edge);
    amTypes.push_back(AttributeMatrix::Type::Vertex);
    req.amTypes = amTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Data Array to Process", SelectedArrayPath, FilterParameter::RequiredArray, FindDerivatives, req));
  }
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Category::Unknown);
    AttributeMatrix::Types amTypes;
    amTypes.push_back(AttributeMatrix::Type::Cell);
    amTypes.push_back(AttributeMatrix::Type::Face);
    amTypes.push_back(AttributeMatrix::Type::Edge);
    amTypes.push_back(AttributeMatrix::Type::Vertex);
    req.amTypes = amTypes;
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Derivatives Array", DerivativesArrayPath, FilterParameter::CreatedArray, FindDerivatives, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDerivatives::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedArrayPath(reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()));
  setDerivativesArrayPath(reader->readDataArrayPath("DerivativesArrayPath", getDerivativesArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename DataType> void findDerivs(IDataArray::Pointer inDataPtr, DoubleArrayType::Pointer derivs, DataContainer::Pointer m, Observable* observable)
{
  typename DataArray<DataType>::Pointer inputDataPtr = std::dynamic_pointer_cast<DataArray<DataType>>(inDataPtr);
  IGeometry::Pointer geom = m->getGeometry();
  DoubleArrayType::Pointer dblInArray = DoubleArrayType::CreateArray(inputDataPtr->getNumberOfTuples(), inputDataPtr->getComponentDimensions(), "FIND_DERIVS_INTERNAL_USE_ONLY");

  size_t size = inputDataPtr->getSize();

  for(size_t i = 0; i < size; i++)
  {
    dblInArray->setValue(i, static_cast<double>(inputDataPtr->getValue(i)));
  }

  geom->findDerivatives(dblInArray, derivs, observable);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename DataType> void interpolateCellValues(IDataArray::Pointer inDataPtr, DoubleArrayType::Pointer outDataPtr, DataContainer::Pointer m)
{
  typename DataArray<DataType>::Pointer inputDataPtr = std::dynamic_pointer_cast<DataArray<DataType>>(inDataPtr);
  IGeometry::Pointer geom = m->getGeometry();
  int err = 0;

  if(geom->getElementsContainingVert().get() == nullptr)
  {
    err = geom->findElementsContainingVert();
    if(err < 0)
    {
      return;
    }
  }

  ElementDynamicList::Pointer elemsContainingVert = geom->getElementsContainingVert();
  IGeometry::Type geomType = geom->getGeometryType();

  switch(geomType)
  {
  case IGeometry::Type::Edge:
  {
    EdgeGeom::Pointer edgeGeom = m->getGeometryAs<EdgeGeom>();
    SharedVertexList::Pointer verts = edgeGeom->getVertices();
    outDataPtr->resize(edgeGeom->getNumberOfVertices());
    GeometryHelpers::Generic::AverageCellArrayValues<int64_t, DataType, uint16_t, double>(elemsContainingVert, verts, inputDataPtr, outDataPtr);
    break;
  }
  case IGeometry::Type::Triangle:
  {
    TriangleGeom::Pointer triGeom = m->getGeometryAs<TriangleGeom>();
    SharedVertexList::Pointer verts = triGeom->getVertices();
    outDataPtr->resize(triGeom->getNumberOfVertices());
    GeometryHelpers::Generic::AverageCellArrayValues<int64_t, DataType, uint16_t, double>(elemsContainingVert, verts, inputDataPtr, outDataPtr);
    break;
  }
  case IGeometry::Type::Quad:
  {
    QuadGeom::Pointer quadGeom = m->getGeometryAs<QuadGeom>();
    SharedVertexList::Pointer verts = quadGeom->getVertices();
    outDataPtr->resize(quadGeom->getNumberOfVertices());
    GeometryHelpers::Generic::AverageCellArrayValues<int64_t, DataType, uint16_t, double>(elemsContainingVert, verts, inputDataPtr, outDataPtr);
    break;
  }
  case IGeometry::Type::Tetrahedral:
  {
    TetrahedralGeom::Pointer tets = m->getGeometryAs<TetrahedralGeom>();
    SharedVertexList::Pointer verts = tets->getVertices();
    outDataPtr->resize(tets->getNumberOfVertices());
    GeometryHelpers::Generic::AverageCellArrayValues<int64_t, DataType, uint16_t, double>(elemsContainingVert, verts, inputDataPtr, outDataPtr);
    break;
  }
  default:
  {
    break;
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDerivatives::initialize()
{
  m_Interpolate = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDerivatives::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_SelectedArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || nullptr == m.get())
  {
    return;
  }
  IGeometry::Pointer geom = m->getPrereqGeometry<IGeometry>(this);
  if(getErrorCondition() < 0 || nullptr == geom.get())
  {
    return;
  }

  AttributeMatrix::Pointer inAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedArrayPath, -301);
  AttributeMatrix::Pointer destAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getDerivativesArrayPath().getAttributeMatrixName(), -301);

  if(getErrorCondition() < 0)
  {
    return;
  }

  AttributeMatrix::Type inAttrMatType = inAttrMat->getType();
  AttributeMatrix::Type destAttrMatType = destAttrMat->getType();
  IGeometry::Type geomType = geom->getGeometryType();
  QString geomName = geom->getGeometryTypeAsString();
  QString ss;

  if(geomType == IGeometry::Type::Image || geomType == IGeometry::Type::RectGrid) // validate AttributeMatrices for ImageGeom and RectGridGeom
  {
    if(inAttrMatType != AttributeMatrix::Type::Cell)
    {
      ss = QObject::tr("The Geometry type is %1, but the selected DataArray does not belong to a CellAttributeMatrix").arg(geomName);
      setErrorCondition(-11002);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    if(destAttrMatType != AttributeMatrix::Type::Cell)
    {
      ss = QObject::tr("The Geometry type is %1, but the selected destination AttributeMatrix is not a CellAttributeMatrix").arg(geomName);
      setErrorCondition(-11002);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }
  else if(geomType == IGeometry::Type::Vertex) // validate AttributeMatrices for VertexGeom
  {
    if(inAttrMatType != AttributeMatrix::Type::Vertex)
    {
      ss = QObject::tr("The Geometry type is %1, but the selected DataArray does not belong to a VertexAttributeMatrix").arg(geomName);
      setErrorCondition(-11002);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    if(destAttrMatType != AttributeMatrix::Type::Vertex)
    {
      ss = QObject::tr("The Geometry type is %1, but the selected destination AttributeMatrix is not a VertexAttributeMatrix").arg(geomName);
      setErrorCondition(-11002);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }
  else // validate AttributeMatrices for all other geometries
  {
    if(inAttrMatType == AttributeMatrix::Type::Cell || inAttrMatType == AttributeMatrix::Type::Face ||
       inAttrMatType == AttributeMatrix::Type::Edge) // need to interpolate values if the array is cell centered
    {
      m_Interpolate = true;
    }
    if(inAttrMatType != AttributeMatrix::Type::Vertex && inAttrMatType != AttributeMatrix::Type::Edge && inAttrMatType != AttributeMatrix::Type::Face &&
       inAttrMatType != AttributeMatrix::Type::Cell)
    {
      ss = QObject::tr("The Geometry type is %1, but the selected DataArray does not belong to a Cell, Face, Edge or Vertex AttributeMatrix").arg(geomName);
      setErrorCondition(-11002);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    if(geomName == SIMPL::Geometry::QuadGeometry || geomName == SIMPL::Geometry::TriangleGeometry)
    {
      if(destAttrMatType != AttributeMatrix::Type::Face)
      {
        ss = QObject::tr("The Geometry type is %1, but the selected destination Attribute Matrix is not a Face Attribute Matrix").arg(geomName);
        setErrorCondition(-11002);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
    }
    else if(geomName == SIMPL::Geometry::TetrahedralGeometry)
    {
      if(destAttrMatType != AttributeMatrix::Type::Cell)
      {
        ss = QObject::tr("The Geometry type is %1, but the selected destination Attribute Matrix is not an Cell Attribute Matrix").arg(geomName);
        setErrorCondition(-11002);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
    }
    else if(geomName == SIMPL::Geometry::EdgeGeometry)
    {
      if(destAttrMatType != AttributeMatrix::Type::Edge)
      {
        ss = QObject::tr("The Geometry type is %1, but the selected destination Attribute Matrix is not an Edge Attribute Matrix").arg(geomName);
        setErrorCondition(-11002);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
    }
  }

  m_InArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedArrayPath());
  if(getErrorCondition() < 0)
  {
    return;
  }

  int cDims = m_InArrayPtr.lock()->getNumberOfComponents();
  cDims *= 3;
  QVector<size_t> dims(1, cDims);

  m_DerivativesArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, getDerivativesArrayPath(), 0, dims);    /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_DerivativesArrayPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_DerivativesArray = m_DerivativesArrayPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDerivatives::preflight()
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
void FindDerivatives::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName());
  IGeometry::Pointer geom = m->getGeometry();

  geom->setMessagePrefix(getMessagePrefix());
  geom->setMessageTitle("Computing Derivatives");

  if(m_Interpolate)
  {
    DoubleArrayType::Pointer interpolatedValues =
        DoubleArrayType::CreateArray(m_InArrayPtr.lock()->getNumberOfTuples(), m_InArrayPtr.lock()->getComponentDimensions(), "FIND_DERIVATIVES_INTERNAL_USE_ONLY");
    EXECUTE_FUNCTION_TEMPLATE(this, interpolateCellValues, m_InArrayPtr.lock(), m_InArrayPtr.lock(), interpolatedValues, m)
    geom->findDerivatives(interpolatedValues, m_DerivativesArrayPtr.lock(), this);
  }
  else
  {
    EXECUTE_FUNCTION_TEMPLATE(this, findDerivs, m_InArrayPtr.lock(), m_InArrayPtr.lock(), m_DerivativesArrayPtr.lock(), m, this);
  }

  geom->setMessagePrefix("");
  geom->setMessageTitle("");

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindDerivatives::newFilterInstance(bool copyFilterParameters)
{
  FindDerivatives::Pointer filter = FindDerivatives::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDerivatives::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDerivatives::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDerivatives::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDerivatives::getGroupName()
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid FindDerivatives::getUuid()
{
  return QUuid("{8ec1fc8e-6484-5412-a898-8079986c0a26}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDerivatives::getSubGroupName()
{
  return SIMPL::FilterSubGroups::GeometryFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDerivatives::getHumanLabel()
{
  return "Find Derivatives";
}

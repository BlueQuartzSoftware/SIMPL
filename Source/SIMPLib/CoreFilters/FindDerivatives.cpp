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

#include <QtCore/QTextStream>

#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/GeometryHelpers.h"
#include "SIMPLib/Geometry/HexahedralGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/Messages/AbstractMessageHandler.h"
#include "SIMPLib/Messages/GenericProgressMessage.h"
#include "SIMPLib/Messages/GenericStatusMessage.h"
#include "SIMPLib/Messages/GenericErrorMessage.h"
#include "SIMPLib/Messages/GenericWarningMessage.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

/**
 * @brief This message handler is used by the FindDerivatives filter to re-emit incoming generic
 * messages from geometry classes as filter messages.  It also prepends text to status messages to show that it is
 * computing derivatives.
 */
class FilterMessageHandler : public AbstractMessageHandler
{
public:
  explicit FilterMessageHandler(FindDerivatives* filter)
  : m_Filter(filter)
  {
  }

  /**
   * @brief Re-emits incoming GenericProgressMessages as FilterProgressMessages.
   */
  void processMessage(const GenericProgressMessage* msg) const override
  {
    emit m_Filter->notifyProgressMessage(msg->getProgressValue(), msg->getMessageText());
  }

  /**
   * @brief Re-emits incoming GenericStatusMessages as FilterStatusMessages.  Prepends text to the
   * message text that explains that we are computing the derivatives
   */
  void processMessage(const GenericStatusMessage* msg) const override
  {
    QString messageText = QObject::tr("Computing Derivatives || %1").arg(msg->getMessageText());
    emit m_Filter->notifyStatusMessage(messageText);
  }

  /**
   * @brief Re-emits incoming GenericErrorMessages as FilterErrorMessages.
   */
  void processMessage(const GenericErrorMessage* msg) const override
  {
    emit m_Filter->setErrorCondition(msg->getCode(), msg->getMessageText());
  }

  /**
   * @brief Re-emits incoming GenericWarningMessages as FilterWarningMessages.
   */
  void processMessage(const GenericWarningMessage* msg) const override
  {
    emit m_Filter->setWarningCondition(msg->getCode(), msg->getMessageText());
  }

private:
  FindDerivatives* m_Filter = nullptr;
};

enum createdPathID : RenameDataPath::DataID_t
{
  DerivativesArrayID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDerivatives::FindDerivatives()
: m_SelectedArrayPath("", "", "")
, m_DerivativesArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "Derivatives")
, m_Interpolate(false)
{
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
  FilterParameterVectorType parameters;
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
template <typename DataType>
void findDerivs(IDataArray::Pointer inDataPtr, DoubleArrayType::Pointer derivs, DataContainer::Pointer m, Observable* observable)
{
  typename DataArray<DataType>::Pointer inputDataPtr = std::dynamic_pointer_cast<DataArray<DataType>>(inDataPtr);
  IGeometry::Pointer geom = m->getGeometry();
  DoubleArrayType::Pointer dblInArray = DoubleArrayType::CreateArray(inputDataPtr->getNumberOfTuples(), inputDataPtr->getComponentDimensions(), "FIND_DERIVS_INTERNAL_USE_ONLY", true);

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
template <typename DataType>
void interpolateCellValues(IDataArray::Pointer inDataPtr, DoubleArrayType::Pointer outDataPtr, DataContainer::Pointer m)
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
  case IGeometry::Type::Edge: {
    EdgeGeom::Pointer edgeGeom = m->getGeometryAs<EdgeGeom>();
    SharedVertexList::Pointer verts = edgeGeom->getVertices();
    outDataPtr->resizeTuples(edgeGeom->getNumberOfVertices());
    GeometryHelpers::Generic::AverageCellArrayValues<size_t, DataType, uint16_t, double>(elemsContainingVert, verts, inputDataPtr, outDataPtr);
    break;
  }
  case IGeometry::Type::Triangle: {
    TriangleGeom::Pointer triGeom = m->getGeometryAs<TriangleGeom>();
    SharedVertexList::Pointer verts = triGeom->getVertices();
    outDataPtr->resizeTuples(triGeom->getNumberOfVertices());
    GeometryHelpers::Generic::AverageCellArrayValues<size_t, DataType, uint16_t, double>(elemsContainingVert, verts, inputDataPtr, outDataPtr);
    break;
  }
  case IGeometry::Type::Quad: {
    QuadGeom::Pointer quadGeom = m->getGeometryAs<QuadGeom>();
    SharedVertexList::Pointer verts = quadGeom->getVertices();
    outDataPtr->resizeTuples(quadGeom->getNumberOfVertices());
    GeometryHelpers::Generic::AverageCellArrayValues<size_t, DataType, uint16_t, double>(elemsContainingVert, verts, inputDataPtr, outDataPtr);
    break;
  }
  case IGeometry::Type::Tetrahedral: {
    TetrahedralGeom::Pointer tets = m->getGeometryAs<TetrahedralGeom>();
    SharedVertexList::Pointer verts = tets->getVertices();
    outDataPtr->resizeTuples(tets->getNumberOfVertices());
    GeometryHelpers::Generic::AverageCellArrayValues<size_t, DataType, uint16_t, double>(elemsContainingVert, verts, inputDataPtr, outDataPtr);
    break;
  }
  case IGeometry::Type::Hexahedral: {
    HexahedralGeom::Pointer hexas = m->getGeometryAs<HexahedralGeom>();
    SharedVertexList::Pointer verts = hexas->getVertices();
    outDataPtr->resizeTuples(hexas->getNumberOfVertices());
    GeometryHelpers::Generic::AverageCellArrayValues<size_t, DataType, uint16_t, double>(elemsContainingVert, verts, inputDataPtr, outDataPtr);
    break;
  }
  default: {
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
  clearErrorCode();
  clearWarningCode();
  initialize();
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, m_SelectedArrayPath.getDataContainerName(), false);
  if(getErrorCode() < 0 || nullptr == m.get())
  {
    return;
  }
  IGeometry::Pointer geom = m->getPrereqGeometry<IGeometry>(this);
  if(getErrorCode() < 0 || nullptr == geom.get())
  {
    return;
  }

  AttributeMatrix::Pointer inAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, m_SelectedArrayPath, -301);
  AttributeMatrix::Pointer destAttrMat = m->getPrereqAttributeMatrix(this, getDerivativesArrayPath().getAttributeMatrixName(), -301);

  if(getErrorCode() < 0)
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
      setErrorCondition(-11002, ss);
      return;
    }
    if(destAttrMatType != AttributeMatrix::Type::Cell)
    {
      ss = QObject::tr("The Geometry type is %1, but the selected destination AttributeMatrix is not a CellAttributeMatrix").arg(geomName);
      setErrorCondition(-11002, ss);
    }
  }
  else if(geomType == IGeometry::Type::Vertex) // validate AttributeMatrices for VertexGeom
  {
    if(inAttrMatType != AttributeMatrix::Type::Vertex)
    {
      ss = QObject::tr("The Geometry type is %1, but the selected DataArray does not belong to a VertexAttributeMatrix").arg(geomName);
      setErrorCondition(-11002, ss);
    }
    if(destAttrMatType != AttributeMatrix::Type::Vertex)
    {
      ss = QObject::tr("The Geometry type is %1, but the selected destination AttributeMatrix is not a VertexAttributeMatrix").arg(geomName);
      setErrorCondition(-11002, ss);
    }
  }
  else // validate AttributeMatrices for all other geometries
  {
    if(inAttrMatType == AttributeMatrix::Type::Cell || inAttrMatType == AttributeMatrix::Type::Face ||
       inAttrMatType == AttributeMatrix::Type::Edge) // need to interpolate values if the array is cell centered
    {
      m_Interpolate = true;
    }
    if(inAttrMatType != AttributeMatrix::Type::Vertex && inAttrMatType != AttributeMatrix::Type::Edge && inAttrMatType != AttributeMatrix::Type::Face && inAttrMatType != AttributeMatrix::Type::Cell)
    {
      ss = QObject::tr("The Geometry type is %1, but the selected DataArray does not belong to a Cell, Face, Edge or Vertex AttributeMatrix").arg(geomName);
      setErrorCondition(-11002, ss);
    }
    if(geomName == SIMPL::Geometry::QuadGeometry || geomName == SIMPL::Geometry::TriangleGeometry)
    {
      if(destAttrMatType != AttributeMatrix::Type::Face)
      {
        ss = QObject::tr("The Geometry type is %1, but the selected destination Attribute Matrix is not a Face Attribute Matrix").arg(geomName);
        setErrorCondition(-11002, ss);
      }
    }
    else if(geomName == SIMPL::Geometry::TetrahedralGeometry || geomName == SIMPL::Geometry::HexahedralGeometry)
    {
      if(destAttrMatType != AttributeMatrix::Type::Cell)
      {
        ss = QObject::tr("The Geometry type is %1, but the selected destination Attribute Matrix is not an Cell Attribute Matrix").arg(geomName);
        setErrorCondition(-11002, ss);
      }
    }
    else if(geomName == SIMPL::Geometry::EdgeGeometry)
    {
      if(destAttrMatType != AttributeMatrix::Type::Edge)
      {
        ss = QObject::tr("The Geometry type is %1, but the selected destination Attribute Matrix is not an Edge Attribute Matrix").arg(geomName);
        setErrorCondition(-11002, ss);
      }
    }
  }

  m_InArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, getSelectedArrayPath());
  if(getErrorCode() < 0)
  {
    return;
  }

  int cDims = m_InArrayPtr.lock()->getNumberOfComponents();
  cDims *= 3;
  std::vector<size_t> dims(1, cDims);

  m_DerivativesArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>>(this, getDerivativesArrayPath(), 0, dims, "",
                                                                                                   DerivativesArrayID); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_DerivativesArrayPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_DerivativesArray = m_DerivativesArrayPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDerivatives::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName());
  IGeometry::Pointer geom = m->getGeometry();

  if(m_Interpolate)
  {
    DoubleArrayType::Pointer interpolatedValues =
        DoubleArrayType::CreateArray(m_InArrayPtr.lock()->getNumberOfTuples(), m_InArrayPtr.lock()->getComponentDimensions(), "FIND_DERIVATIVES_INTERNAL_USE_ONLY", true);
    EXECUTE_FUNCTION_TEMPLATE(this, interpolateCellValues, m_InArrayPtr.lock(), m_InArrayPtr.lock(), interpolatedValues, m)

    geom->findDerivatives(interpolatedValues, m_DerivativesArrayPtr.lock(), this);
  }
  else
  {
    EXECUTE_FUNCTION_TEMPLATE(this, findDerivs, m_InArrayPtr.lock(), m_InArrayPtr.lock(), m_DerivativesArrayPtr.lock(), m, this);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDerivatives::processDerivativesMessage(const AbstractMessage::Pointer& msg)
{
  FilterMessageHandler msgHandler(this);
  msg->visit(&msgHandler);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindDerivatives::newFilterInstance(bool copyFilterParameters) const
{
  FindDerivatives::Pointer filter = FindDerivatives::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindDerivatives::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindDerivatives::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindDerivatives::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindDerivatives::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindDerivatives::getUuid() const
{
  return QUuid("{8ec1fc8e-6484-5412-a898-8079986c0a26}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindDerivatives::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GeometryFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindDerivatives::getHumanLabel() const
{
  return "Find Derivatives";
}

// -----------------------------------------------------------------------------
FindDerivatives::Pointer FindDerivatives::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindDerivatives> FindDerivatives::New()
{
  struct make_shared_enabler : public FindDerivatives
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindDerivatives::getNameOfClass() const
{
  return QString("FindDerivatives");
}

// -----------------------------------------------------------------------------
QString FindDerivatives::ClassName()
{
  return QString("FindDerivatives");
}

// -----------------------------------------------------------------------------
void FindDerivatives::setSelectedArrayPath(const DataArrayPath& value)
{
  m_SelectedArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindDerivatives::getSelectedArrayPath() const
{
  return m_SelectedArrayPath;
}

// -----------------------------------------------------------------------------
void FindDerivatives::setDerivativesArrayPath(const DataArrayPath& value)
{
  m_DerivativesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindDerivatives::getDerivativesArrayPath() const
{
  return m_DerivativesArrayPath;
}

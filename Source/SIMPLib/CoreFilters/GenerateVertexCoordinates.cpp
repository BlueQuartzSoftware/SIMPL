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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "GenerateVertexCoordinates.h"

#include <array>
#include <numeric>

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID31 = 31
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateVertexCoordinates::GenerateVertexCoordinates()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateVertexCoordinates::~GenerateVertexCoordinates() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateVertexCoordinates::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateVertexCoordinates::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    req.dcGeometryTypes = {IGeometry::Type::Image, IGeometry::Type::RectGrid};
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container with Input Geometry", SelectedDataContainerName, FilterParameter::Category::RequiredArray, GenerateVertexCoordinates, req));
  }

  DataArrayCreationFilterParameter::RequirementType req;
  req.amTypes = {AttributeMatrix::Type::Cell};
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Created Vertex Coordinates", CoordinateArrayPath, FilterParameter::Category::CreatedArray, GenerateVertexCoordinates, req, {0}));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateVertexCoordinates::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(m_SelectedDataContainerName.isEmpty())
  {
    QString ss = QObject::tr("The name of the input DataContainer is empty. Please specify a value.");
    setErrorCondition(-2004, ss);
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer(this, getSelectedDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }

  IGeometry::Pointer fromGeometry = dc->getGeometry();
  if(nullptr == fromGeometry)
  {
    QString ss = QObject::tr("Selected input DataContainer must contain either an Image Geometry or RectLinearGrid Geometry. The DataContainer did not contain a Geometry object.");
    setErrorCondition(-2008, ss);
    return;
  }
  IGeometry::Type geomType = fromGeometry->getGeometryType();
  if(IGeometry::Type::Image == geomType || IGeometry::Type::RectGrid == geomType)
  {
    m_CoordinatesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType>(this, getCoordinateArrayPath(), 0, {3}, "", DataArrayID31);
  }
  else
  {
    QString ss = QObject::tr("Data Container's Geometry type must be either an Image Geometry or RectLinearGrid Geometry. The Geomerty is of type %1").arg(fromGeometry->getGeometryTypeAsString());
    setErrorCondition(-2010, ss);
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateVertexCoordinates::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  IGeometryGrid::Pointer sourceGeometry = getDataContainerArray()->getDataContainer(getSelectedDataContainerName())->getGeometryAs<IGeometryGrid>();

  std::array<float, 3> coords = {0.0f, 0.0f, 0.0f};

  SizeVec3Type dims = sourceGeometry->getDimensions();
  size_t cellCount = std::accumulate(dims.begin(), dims.end(), static_cast<size_t>(1), std::multiplies<size_t>());

  // Use the APIs from the IGeometryGrid to get the XYZ coord for the center of each cell and then set that into the
  // the new VertexGeometry
  for(size_t idx = 0; idx < cellCount; idx++)
  {
    sourceGeometry->getCoords(idx, coords.data());
    m_CoordinatesPtr->setTuple(idx, coords.data());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateVertexCoordinates::newFilterInstance(bool copyFilterParameters) const
{
  GenerateVertexCoordinates::Pointer filter = GenerateVertexCoordinates::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateVertexCoordinates::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateVertexCoordinates::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateVertexCoordinates::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateVertexCoordinates::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateVertexCoordinates::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateVertexCoordinates::getHumanLabel() const
{
  return "Generate Cell Center Coords";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GenerateVertexCoordinates::getUuid() const
{
  return QUuid("{657a79de-7d60-5d9d-9f41-80abb62a3158}");
}

// -----------------------------------------------------------------------------
GenerateVertexCoordinates::Pointer GenerateVertexCoordinates::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GenerateVertexCoordinates> GenerateVertexCoordinates::New()
{
  struct make_shared_enabler : public GenerateVertexCoordinates
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GenerateVertexCoordinates::getNameOfClass() const
{
  return QString("GenerateVertexCoordinates");
}

// -----------------------------------------------------------------------------
QString GenerateVertexCoordinates::ClassName()
{
  return QString("GenerateVertexCoordinates");
}

// -----------------------------------------------------------------------------
void GenerateVertexCoordinates::setSelectedDataContainerName(const DataArrayPath& value)
{
  m_SelectedDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateVertexCoordinates::getSelectedDataContainerName() const
{
  return m_SelectedDataContainerName;
}

// -----------------------------------------------------------------------------
void GenerateVertexCoordinates::setCoordinateArrayPath(const DataArrayPath& value)
{
  m_CoordinateArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateVertexCoordinates::getCoordinateArrayPath() const
{
  return m_CoordinateArrayPath;
}

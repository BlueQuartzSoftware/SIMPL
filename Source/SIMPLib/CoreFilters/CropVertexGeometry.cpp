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
#include "CropVertexGeometry.h"

#include <cassert>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropVertexGeometry::CropVertexGeometry()
: m_DataContainerName(SIMPL::Defaults::VertexDataContainerName)
, m_CroppedDataContainerName("CroppedDataContainer")
, m_XMin(0)
, m_YMin(0)
, m_ZMin(0)
, m_XMax(0)
, m_YMax(0)
, m_ZMax(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropVertexGeometry::~CropVertexGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVertexGeometry::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  DataContainerSelectionFilterParameter::RequirementType req;
  IGeometry::Types reqGeom = {IGeometry::Type::Vertex};
  req.dcGeometryTypes = reqGeom;
  parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Vertex Geometry to Crop", DataContainerName, FilterParameter::RequiredArray, CropVertexGeometry, req));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("X Min", XMin, FilterParameter::Parameter, CropVertexGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Y Min", YMin, FilterParameter::Parameter, CropVertexGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Z Min", ZMin, FilterParameter::Parameter, CropVertexGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("X Max", XMax, FilterParameter::Parameter, CropVertexGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Y Max", YMax, FilterParameter::Parameter, CropVertexGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Z Max", ZMax, FilterParameter::Parameter, CropVertexGeometry));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Cropped Data Container", CroppedDataContainerName, FilterParameter::CreatedArray, CropVertexGeometry));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVertexGeometry::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readDataArrayPath("DataContainerName", getDataContainerName()));
  setXMin(reader->readValue("XMin", getXMin()));
  setYMin(reader->readValue("YMin", getYMin()));
  setZMin(reader->readValue("ZMin", getZMin()));
  setXMax(reader->readValue("XMax", getXMax()));
  setYMax(reader->readValue("YMax", getYMax()));
  setZMax(reader->readValue("ZMax", getZMax()));
  setCroppedDataContainerName(reader->readDataArrayPath("CroppedDataContainerName", getCroppedDataContainerName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVertexGeometry::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
  m_AttrMatList.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVertexGeometry::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();

  getDataContainerArray()->getPrereqGeometryFromDataContainer<VertexGeom>(this, getDataContainerName());

  if(getXMax() < getXMin())
  {
    QString ss = QObject::tr("X Max (%1) less than X Min (%2)").arg(getXMax()).arg(getXMin());
    setErrorCondition(-5550, ss);
  }
  if(getYMax() < getYMin())
  {
    QString ss = QObject::tr("Y Max (%1) less than Y Min (%2)").arg(getYMax()).arg(getYMin());
    setErrorCondition(-5550, ss);
  }
  if(getZMax() < getZMin())
  {
    QString ss = QObject::tr("Z Max (%1) less than Z Min (%2)").arg(getZMax()).arg(getZMin());
    setErrorCondition(-5550, ss);
  }

  DataContainer::Pointer dc = getDataContainerArray()->createNonPrereqDataContainer(this, getCroppedDataContainerName(), DataContainerID);

  if(getErrorCode() < 0)
  {
    return;
  }

  VertexGeom::Pointer crop = VertexGeom::CreateGeometry(0, SIMPL::Geometry::VertexGeometry, !getInPreflight());
  dc->setGeometry(crop);

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  m_AttrMatList = m->getAttributeMatrixNames();
  std::vector<size_t> tDims(1, 0);
  QList<QString> tempDataArrayList;
  DataArrayPath tempPath;
  AttributeMatrix::Type tempAttrMatType = AttributeMatrix::Type::Vertex;

  if(getErrorCode() < 0)
  {
    return;
  }

  for(auto&& attr_mat : m_AttrMatList)
  {
    AttributeMatrix::Pointer tmpAttrMat = m->getPrereqAttributeMatrix(this, attr_mat, -301);
    if(getErrorCode() >= 0)
    {
      tempAttrMatType = tmpAttrMat->getType();
      if(tempAttrMatType != AttributeMatrix::Type::Vertex)
      {
        AttributeMatrix::Pointer attrMat = tmpAttrMat->deepCopy(getInPreflight());
        dc->addOrReplaceAttributeMatrix(attrMat);
      }
      else
      {
        dc->createNonPrereqAttributeMatrix(this, tmpAttrMat->getName(), tDims, AttributeMatrix::Type::Vertex);
        tempDataArrayList = tmpAttrMat->getAttributeArrayNames();
        for(auto&& data_array : tempDataArrayList)
        {
          tempPath.update(getCroppedDataContainerName().getDataContainerName(), tmpAttrMat->getName(), data_array);
          IDataArray::Pointer tmpDataArray = tmpAttrMat->getPrereqIDataArray(this, data_array, -90002);
          if(getErrorCode() >= 0)
          {
            std::vector<size_t> cDims = tmpDataArray->getComponentDimensions();
            TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, cDims, tmpDataArray);
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void copyDataToCroppedGeometry(IDataArray::Pointer inDataPtr, IDataArray::Pointer outDataPtr, std::vector<int64_t>& croppedPoints)
{
  typename DataArray<T>::Pointer inputDataPtr = std::dynamic_pointer_cast<DataArray<T>>(inDataPtr);
  T* inputData = static_cast<T*>(inputDataPtr->getPointer(0));
  typename DataArray<T>::Pointer croppedDataPtr = std::dynamic_pointer_cast<DataArray<T>>(outDataPtr);
  T* croppedData = static_cast<T*>(croppedDataPtr->getPointer(0));

  size_t nComps = inDataPtr->getNumberOfComponents();
  size_t tmpIndex = 0;
  size_t ptrIndex = 0;

  for(std::vector<int64_t>::size_type i = 0; i < croppedPoints.size(); i++)
  {
    for(size_t d = 0; d < nComps; d++)
    {
      tmpIndex = nComps * i + d;
      ptrIndex = nComps * croppedPoints[i] + d;
      croppedData[tmpIndex] = inputData[ptrIndex];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVertexGeometry::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getCroppedDataContainerName());
  VertexGeom::Pointer vertices = getDataContainerArray()->getDataContainer(getDataContainerName())->getGeometryAs<VertexGeom>();
  int64_t numVerts = vertices->getNumberOfVertices();
  float* allVerts = vertices->getVertexPointer(0);
  std::vector<int64_t> croppedPoints;
  croppedPoints.reserve(numVerts);

  for(int64_t i = 0; i < numVerts; i++)
  {
    if(getCancel())
    {
      return;
    }
    if(allVerts[3 * i + 0] >= m_XMin && allVerts[3 * i + 0] <= m_XMax && allVerts[3 * i + 1] >= m_YMin && allVerts[3 * i + 1] <= m_YMax && allVerts[3 * i + 2] >= m_ZMin &&
       allVerts[3 * i + 2] <= m_ZMax)
    {
      croppedPoints.push_back(i);
    }
  }

  croppedPoints.shrink_to_fit();

  VertexGeom::Pointer crop = dc->getGeometryAs<VertexGeom>();
  crop->resizeVertexList(croppedPoints.size());
  float coords[3] = {0.0f, 0.0f, 0.0f};

  for(std::list<int64_t>::size_type i = 0; i < croppedPoints.size(); i++)
  {
    if(getCancel())
    {
      return;
    }
    vertices->getCoords(croppedPoints[i], coords);
    crop->setCoords(i, coords);
  }

  std::vector<size_t> tDims(1, croppedPoints.size());

  for(auto&& attr_mat : m_AttrMatList)
  {
    AttributeMatrix::Pointer tmpAttrMat = dc->getPrereqAttributeMatrix(this, attr_mat, -301);
    if(getErrorCode() >= 0)
    {
      AttributeMatrix::Type tempAttrMatType = tmpAttrMat->getType();
      if(tempAttrMatType == AttributeMatrix::Type::Vertex)
      {
        tmpAttrMat->resizeAttributeArrays(tDims);
        QList<QString> srcDataArrays = tmpAttrMat->getAttributeArrayNames();
        AttributeMatrix::Pointer srcAttrMat = m->getAttributeMatrix(tmpAttrMat->getName());
        assert(srcAttrMat);

        for(auto&& data_array : srcDataArrays)
        {
          IDataArray::Pointer src = srcAttrMat->getAttributeArray(data_array);
          IDataArray::Pointer dest = tmpAttrMat->getAttributeArray(data_array);

          assert(src);
          assert(dest);
          assert(src->getNumberOfComponents() == dest->getNumberOfComponents());

          EXECUTE_FUNCTION_TEMPLATE(this, copyDataToCroppedGeometry, src, src, dest, croppedPoints)
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CropVertexGeometry::newFilterInstance(bool copyFilterParameters) const
{
  CropVertexGeometry::Pointer filter = CropVertexGeometry::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropVertexGeometry::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropVertexGeometry::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropVertexGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropVertexGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid CropVertexGeometry::getUuid() const
{
  return QUuid("{f28cbf07-f15a-53ca-8c7f-b41a11dae6cc}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropVertexGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CropCutFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CropVertexGeometry::getHumanLabel() const
{
  return "Crop Geometry (Vertex)";
}

// -----------------------------------------------------------------------------
CropVertexGeometry::Pointer CropVertexGeometry::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<CropVertexGeometry> CropVertexGeometry::New()
{
  struct make_shared_enabler : public CropVertexGeometry
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString CropVertexGeometry::getNameOfClass() const
{
  return QString("CropVertexGeometry");
}

// -----------------------------------------------------------------------------
QString CropVertexGeometry::ClassName()
{
  return QString("CropVertexGeometry");
}

// -----------------------------------------------------------------------------
void CropVertexGeometry::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CropVertexGeometry::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void CropVertexGeometry::setCroppedDataContainerName(const DataArrayPath& value)
{
  m_CroppedDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CropVertexGeometry::getCroppedDataContainerName() const
{
  return m_CroppedDataContainerName;
}

// -----------------------------------------------------------------------------
void CropVertexGeometry::setXMin(float value)
{
  m_XMin = value;
}

// -----------------------------------------------------------------------------
float CropVertexGeometry::getXMin() const
{
  return m_XMin;
}

// -----------------------------------------------------------------------------
void CropVertexGeometry::setYMin(float value)
{
  m_YMin = value;
}

// -----------------------------------------------------------------------------
float CropVertexGeometry::getYMin() const
{
  return m_YMin;
}

// -----------------------------------------------------------------------------
void CropVertexGeometry::setZMin(float value)
{
  m_ZMin = value;
}

// -----------------------------------------------------------------------------
float CropVertexGeometry::getZMin() const
{
  return m_ZMin;
}

// -----------------------------------------------------------------------------
void CropVertexGeometry::setXMax(float value)
{
  m_XMax = value;
}

// -----------------------------------------------------------------------------
float CropVertexGeometry::getXMax() const
{
  return m_XMax;
}

// -----------------------------------------------------------------------------
void CropVertexGeometry::setYMax(float value)
{
  m_YMax = value;
}

// -----------------------------------------------------------------------------
float CropVertexGeometry::getYMax() const
{
  return m_YMax;
}

// -----------------------------------------------------------------------------
void CropVertexGeometry::setZMax(float value)
{
  m_ZMax = value;
}

// -----------------------------------------------------------------------------
float CropVertexGeometry::getZMax() const
{
  return m_ZMax;
}

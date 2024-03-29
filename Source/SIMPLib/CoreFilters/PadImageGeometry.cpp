/* ============================================================================
 * Copyright (c) 2022-2022 BlueQuartz Software, LLC
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PadImageGeometry.h"

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/CoreFilters/Algorithms/PadImageGeometryImpl.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec2FilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/task_group.h>
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PadImageGeometry::PadImageGeometry()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PadImageGeometry::~PadImageGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PadImageGeometry::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PadImageGeometry::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INT_VEC2_FP("X Min & Max Padding (in voxels)", XMinMax, FilterParameter::Category::Parameter, PadImageGeometry));
  parameters.push_back(SIMPL_NEW_INT_VEC2_FP("Y Min & Max Padding (in voxels)", YMinMax, FilterParameter::Category::Parameter, PadImageGeometry));
  parameters.push_back(SIMPL_NEW_INT_VEC2_FP("Z Min & Max Padding (in voxels)", ZMinMax, FilterParameter::Category::Parameter, PadImageGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Default Padding Value", DefaultFillValue, FilterParameter::Category::Parameter, PadImageGeometry));

  parameters.push_back(SIMPL_NEW_BOOL_FP("Update Origin", UpdateOrigin, FilterParameter::Category::Parameter, PadImageGeometry));

  PreflightUpdatedValueFilterParameter::Pointer param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Old Geometry Information", OldGeometryInformation, FilterParameter::Category::Parameter, PadImageGeometry);
  param->setReadOnly(true);
  parameters.push_back(param);

  param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("New Geometry Information", NewGeometryInformation, FilterParameter::Category::Parameter, PadImageGeometry);
  param->setReadOnly(true);
  parameters.push_back(param);

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    req.dcGeometryTypes = {IGeometry::Type::Image};
    req.amTypes = {AttributeMatrix::Type::Any};
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Attribute Matrix", AttributeMatrixPath, FilterParameter::Category::RequiredArray, PadImageGeometry, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PadImageGeometry::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(m_XMinMax.getX() < 0 || m_XMinMax.getY() < 0)
  {
    QString ss = QObject::tr("X Min & Max: The values cannot be negative.");
    setErrorCondition(ErrorCodes::X_VALUES_NEGATIVE, ss);
    return;
  }

  if(m_YMinMax.getX() < 0 || m_YMinMax.getY() < 0)
  {
    QString ss = QObject::tr("Y Min & Max: The values cannot be negative.");
    setErrorCondition(ErrorCodes::Y_VALUES_NEGATIVE, ss);
    return;
  }

  if(m_ZMinMax.getX() < 0 || m_ZMinMax.getY() < 0)
  {
    QString ss = QObject::tr("Z Min & Max: The values cannot be negative.");
    setErrorCondition(ErrorCodes::Z_VALUES_NEGATIVE, ss);
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer(this, m_AttributeMatrixPath.getDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer imageGeom = dc->getGeometryAs<ImageGeom>();
  if(imageGeom == IGeometry::NullPointer())
  {
    QString ss = QObject::tr("The data container '%1', containing the cell attribute matrix '%2', must have an image geometry.")
                     .arg(m_AttributeMatrixPath.getDataContainerName(), m_AttributeMatrixPath.getAttributeMatrixName());
    setErrorCondition(ErrorCodes::MISSING_IMG_GEOMETRY, ss);
    return;
  }

  AttributeMatrix::Pointer attrMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, m_AttributeMatrixPath, 1);
  if(getErrorCode() < 0)
  {
    return;
  }

  m_OldAttrMatrix = attrMatrix;
  m_OldGeometry = imageGeom;

  SizeVec3Type newDims = imageGeom->getDimensions();
  newDims[0] += (m_XMinMax.getX() + m_XMinMax.getY());
  newDims[1] += (m_YMinMax.getX() + m_YMinMax.getY());
  newDims[2] += (m_ZMinMax.getX() + m_ZMinMax.getY());

  FloatVec3Type newOrigin = imageGeom->getOrigin();
  if(m_UpdateOrigin)
  {
    newOrigin[0] -= m_XMinMax.getX();
    newOrigin[1] -= m_YMinMax.getX();
    newOrigin[2] -= m_ZMinMax.getX();
  }

  ImageGeom::Pointer newGeometry = ImageGeom::CreateGeometry(imageGeom->getName());
  newGeometry->setDimensions(newDims);
  newGeometry->setSpacing(m_OldGeometry->getSpacing());
  newGeometry->setOrigin(newOrigin);
  dc->setGeometry(newGeometry);

  size_t totalPoints = newGeometry->getNumberOfElements();
  AttributeMatrix::Pointer newAttrMatrix = AttributeMatrix::New(newGeometry->getDimensions().toContainer<std::vector<size_t>>(), attrMatrix->getName(), attrMatrix->getType());

  QList<QString> voxelArrayNames = attrMatrix->getAttributeArrayNames();
  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = attrMatrix->getAttributeArray(*iter);
    IDataArray::Pointer data = p->createNewArray(totalPoints, p->getComponentDimensions(), p->getName(), !getInPreflight());
    newAttrMatrix->insertOrAssign(data);
  }
  dc->removeAttributeMatrix(attrMatrix->getName());
  dc->addOrReplaceAttributeMatrix(newAttrMatrix);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PadImageGeometry::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_AttributeMatrixPath.getDataContainerName());
  ImageGeom::Pointer imageGeom = dc->getGeometryAs<ImageGeom>();
  AttributeMatrix::Pointer attrMatrix = dc->getAttributeMatrix(m_AttributeMatrixPath.getAttributeMatrixName());
  QList<QString> voxelArrayNames = attrMatrix->getAttributeArrayNames();
  IntVec3Type minPadding = {m_XMinMax[0], m_YMinMax[0], m_ZMinMax[0]};

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  std::shared_ptr<tbb::task_group> g(new tbb::task_group);
#endif

  for(const QString& name : voxelArrayNames)
  {
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    g->run(PadImageGeometryImpl(this, *m_OldAttrMatrix, *attrMatrix, name, m_DefaultFillValue, minPadding));
#else
    PadImageGeometryImpl(this, *m_OldAttrMatrix, *attrMatrix, name, m_DefaultFillValue, minPadding)();
#endif
  }
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  // This will spill over if the number of arrays to process does not divide evenly by the number of threads.
  g->wait();
#endif

  // Clean up old geometry and attribute matrix
  m_OldAttrMatrix.reset();
  m_OldGeometry.reset();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PadImageGeometry::newFilterInstance(bool copyFilterParameters) const
{
  PadImageGeometry::Pointer filter = PadImageGeometry::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PadImageGeometry::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PadImageGeometry::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PadImageGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PadImageGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PadImageGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PadImageGeometry::getHumanLabel() const
{
  return "Pad Geometry (Image)";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid PadImageGeometry::getUuid() const
{
  return QUuid("{c0ac6c9d-c130-5055-a69b-2f4011846ff0}");
}

// -----------------------------------------------------------------------------
PadImageGeometry::Pointer PadImageGeometry::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<PadImageGeometry> PadImageGeometry::New()
{
  struct make_shared_enabler : public PadImageGeometry
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString PadImageGeometry::getNameOfClass() const
{
  return QString("PadImageGeometry");
}

// -----------------------------------------------------------------------------
QString PadImageGeometry::ClassName()
{
  return QString("PadImageGeometry");
}

// -----------------------------------------------------------------------------
void PadImageGeometry::setXMinMax(const IntVec2Type& value)
{
  m_XMinMax = value;
}

// -----------------------------------------------------------------------------
IntVec2Type PadImageGeometry::getXMinMax() const
{
  return m_XMinMax;
}

// -----------------------------------------------------------------------------
void PadImageGeometry::setYMinMax(const IntVec2Type& value)
{
  m_YMinMax = value;
}

// -----------------------------------------------------------------------------
IntVec2Type PadImageGeometry::getYMinMax() const
{
  return m_YMinMax;
}

// -----------------------------------------------------------------------------
void PadImageGeometry::setZMinMax(const IntVec2Type& value)
{
  m_ZMinMax = value;
}

// -----------------------------------------------------------------------------
IntVec2Type PadImageGeometry::getZMinMax() const
{
  return m_ZMinMax;
}

// -----------------------------------------------------------------------------
void PadImageGeometry::setDefaultFillValue(const int& value)
{
  m_DefaultFillValue = value;
}

// -----------------------------------------------------------------------------
int PadImageGeometry::getDefaultFillValue() const
{
  return m_DefaultFillValue;
}

// -----------------------------------------------------------------------------
void PadImageGeometry::setUpdateOrigin(const bool& value)
{
  m_UpdateOrigin = value;
}

// -----------------------------------------------------------------------------
bool PadImageGeometry::getUpdateOrigin() const
{
  return m_UpdateOrigin;
}

// -----------------------------------------------------------------------------
QString PadImageGeometry::getOldGeometryInformation()
{
  QString desc = QString("Please select a Data Container that has an Image Geometry");
  DataContainerArray::Pointer dca = getDataContainerArray();
  if(nullptr != dca)
  {
    DataContainer::Pointer m = dca->getDataContainer(getAttributeMatrixPath().getDataContainerName());
    if(nullptr != m)
    {
      ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
      if(nullptr != image)
      {
        desc.clear();

        FloatVec3Type oldOrigin = m_OldGeometry->getOrigin();
        FloatVec3Type oldSpacing = m_OldGeometry->getSpacing();
        SizeVec3Type oldDims = m_OldGeometry->getDimensions();
        IGeometry::LengthUnit lengthUnits = image->getUnits();
        QString lengthUnitStr = IGeometry::LengthUnitToString(lengthUnits);
        if(lengthUnits == IGeometry::LengthUnit::Unspecified)
        {
          lengthUnitStr.append(" Units");
        }
        float xRangeMax = oldOrigin[0] + (oldDims[0] * oldSpacing[0]);
        float xDelta = oldDims[0] * oldSpacing[0];
        float yRangeMax = oldOrigin[1] + (oldDims[1] * oldSpacing[1]);
        float yDelta = oldDims[1] * oldSpacing[1];
        float zRangeMax = oldOrigin[2] + (oldDims[2] * oldSpacing[2]);
        float zDelta = oldDims[2] * oldSpacing[2];

        desc.append(QObject::tr("X Range: %1 to %2 [%5] (Delta: %3 [%5]) 0-%4 Voxels\n")
                        .arg(QString::number(oldOrigin[0]), QString::number(xRangeMax), QString::number(xDelta), QString::number(oldDims[0] - 1), lengthUnitStr));
        desc.append(QObject::tr("Y Range: %1 to %2 [%5] (Delta: %3 [%5]) 0-%4 Voxels\n")
                        .arg(QString::number(oldOrigin[1]), QString::number(yRangeMax), QString::number(yDelta), QString::number(oldDims[1] - 1), lengthUnitStr));
        desc.append(QObject::tr("Z Range: %1 to %2 [%5] (Delta: %3 [%5]) 0-%4 Voxels\n")
                        .arg(QString::number(oldOrigin[2]), QString::number(zRangeMax), QString::number(zDelta), QString::number(oldDims[2] - 1), lengthUnitStr));
      }
    }
  }
  return desc;
}

// -----------------------------------------------------------------------------
QString PadImageGeometry::getNewGeometryInformation()
{
  QString desc = QString("Please select a Data Container that has an Image Geometry");
  DataContainerArray::Pointer dca = getDataContainerArray();
  if(nullptr != dca)
  {
    DataContainer::Pointer m = dca->getDataContainer(getAttributeMatrixPath().getDataContainerName());
    if(nullptr != m)
    {
      ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
      if(nullptr != image)
      {
        desc.clear();

        FloatVec3Type newOrigin = image->getOrigin();
        FloatVec3Type newSpacing = image->getSpacing();
        SizeVec3Type newDims = image->getDimensions();
        IGeometry::LengthUnit lengthUnits = image->getUnits();
        QString lengthUnitStr = IGeometry::LengthUnitToString(lengthUnits);
        if(lengthUnits == IGeometry::LengthUnit::Unspecified)
        {
          lengthUnitStr.append(" Units");
        }
        float xRangeMax = newOrigin[0] + (newDims[0] * newSpacing[0]);
        float xDelta = newDims[0] * newSpacing[0];
        float yRangeMax = newOrigin[1] + (newDims[1] * newSpacing[1]);
        float yDelta = newDims[1] * newSpacing[1];
        float zRangeMax = newOrigin[2] + (newDims[2] * newSpacing[2]);
        float zDelta = newDims[2] * newSpacing[2];

        desc.append(QObject::tr("X Range: %1 to %2 [%5] (Delta: %3 [%5]) 0-%4 Voxels\n")
                        .arg(QString::number(newOrigin[0]), QString::number(xRangeMax), QString::number(xDelta), QString::number(newDims[0] - 1), lengthUnitStr));
        desc.append(QObject::tr("Y Range: %1 to %2 [%5] (Delta: %3 [%5]) 0-%4 Voxels\n")
                        .arg(QString::number(newOrigin[1]), QString::number(yRangeMax), QString::number(yDelta), QString::number(newDims[1] - 1), lengthUnitStr));
        desc.append(QObject::tr("Z Range: %1 to %2 [%5] (Delta: %3 [%5]) 0-%4 Voxels\n")
                        .arg(QString::number(newOrigin[2]), QString::number(zRangeMax), QString::number(zDelta), QString::number(newDims[2] - 1), lengthUnitStr));
      }
    }
  }
  return desc;
}

// -----------------------------------------------------------------------------
void PadImageGeometry::setAttributeMatrixPath(const DataArrayPath& value)
{
  m_AttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath PadImageGeometry::getAttributeMatrixPath() const
{
  return m_AttributeMatrixPath;
}

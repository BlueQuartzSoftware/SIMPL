/*
 * Your License or Copyright can go here
 */

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

  PreflightUpdatedValueFilterParameter::Pointer param =
      SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Old Geometry Size in Length Units", OldGeometryDimensions, FilterParameter::Category::Parameter, PadImageGeometry);
  param->setReadOnly(true);
  parameters.push_back(param);

  param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("New Geometry Size in Length Units", NewGeometryDimensions, FilterParameter::Category::Parameter, PadImageGeometry);
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
QString PadImageGeometry::getOldGeometryDimensions()
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
        QTextStream ss(&desc);
        FloatVec3Type oldOrigin = m_OldGeometry->getOrigin();
        FloatVec3Type oldSpacing = m_OldGeometry->getSpacing();
        SizeVec3Type oldDims = m_OldGeometry->getDimensions();
        ss << "X Range: " << oldOrigin[0] << " to " << (oldOrigin[0] + (oldDims[0] * oldSpacing[0])) << " (Delta: " << (oldDims[0] * oldSpacing[0]) << ") " << 0 << "-" << oldDims[0] - 1
           << " Voxels\n";
        ss << "Y Range: " << oldOrigin[1] << " to " << (oldOrigin[1] + (oldDims[1] * oldSpacing[1])) << " (Delta: " << (oldDims[1] * oldSpacing[1]) << ") " << 0 << "-" << oldDims[1] - 1
           << " Voxels\n";
        ss << "Z Range: " << oldOrigin[2] << " to " << (oldOrigin[2] + (oldDims[2] * oldSpacing[2])) << " (Delta: " << (oldDims[2] * oldSpacing[2]) << ") " << 0 << "-" << oldDims[2] - 1
           << " Voxels\n";
      }
    }
  }
  return desc;
}

// -----------------------------------------------------------------------------
QString PadImageGeometry::getNewGeometryDimensions()
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
        QTextStream ss(&desc);
        FloatVec3Type newOrigin = image->getOrigin();
        FloatVec3Type newSpacing = image->getSpacing();
        SizeVec3Type newDims = image->getDimensions();
        ss << "X Range: " << newOrigin[0] << " to " << (newOrigin[0] + (newDims[0] * newSpacing[0])) << " (Delta: " << (newDims[0] * newSpacing[0]) << ") " << 0 << "-" << newDims[0] - 1
           << " Voxels\n";
        ss << "Y Range: " << newOrigin[1] << " to " << (newOrigin[1] + (newDims[1] * newSpacing[1])) << " (Delta: " << (newDims[1] * newSpacing[1]) << ") " << 0 << "-" << newDims[1] - 1
           << " Voxels\n";
        ss << "Z Range: " << newOrigin[2] << " to " << (newOrigin[2] + (newDims[2] * newSpacing[2])) << " (Delta: " << (newDims[2] * newSpacing[2]) << ") " << 0 << "-" << newDims[2] - 1
           << " Voxels\n";
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

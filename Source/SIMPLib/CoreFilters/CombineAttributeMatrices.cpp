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
#include "CombineAttributeMatrices.h"

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CombineAttributeMatrices::CombineAttributeMatrices()
: m_FirstAttributeMatrixPath("", "", "")
, m_SecondAttributeMatrixPath("", "", "")
, m_FirstIndexArrayPath("", "", "")
, m_SecondIndexArrayPath("", "", "")
, m_CombinedAttributeMatrixName("")
, m_NewIndexArrayName("")
, m_FirstIndex(nullptr)
, m_SecondIndex(nullptr)
, m_NewIndex(nullptr)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CombineAttributeMatrices::~CombineAttributeMatrices() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeMatrices::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Feature/Ensemble Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    IGeometry::Types dcGeomTypes;
    AttributeMatrix::Types amTypes;
    amTypes.push_back(AttributeMatrix::Type::CellEnsemble);
    amTypes.push_back(AttributeMatrix::Type::CellFeature);
    amTypes.push_back(AttributeMatrix::Type::EdgeEnsemble);
    amTypes.push_back(AttributeMatrix::Type::EdgeFeature);
    amTypes.push_back(AttributeMatrix::Type::FaceEnsemble);
    amTypes.push_back(AttributeMatrix::Type::FaceFeature);
    amTypes.push_back(AttributeMatrix::Type::VertexEnsemble);
    amTypes.push_back(AttributeMatrix::Type::VertexFeature);
    dcGeomTypes.push_back(IGeometry::Type::Any);
    req.amTypes = amTypes;
    req.dcGeometryTypes = dcGeomTypes;
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("First Feature/Ensemble Attribute Matrix", FirstAttributeMatrixPath, FilterParameter::RequiredArray, CombineAttributeMatrices, req));
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Second Feature/Ensemble Attribute Matrix", SecondAttributeMatrixPath, FilterParameter::RequiredArray, CombineAttributeMatrices, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    IGeometry::Types dcGeomTypes;
    req.dcGeometryTypes = dcGeomTypes;
    AttributeMatrix::Types amTypes;
    amTypes.push_back(AttributeMatrix::Type::Cell);
    amTypes.push_back(AttributeMatrix::Type::CellFeature);
    amTypes.push_back(AttributeMatrix::Type::Edge);
    amTypes.push_back(AttributeMatrix::Type::EdgeFeature);
    amTypes.push_back(AttributeMatrix::Type::Face);
    amTypes.push_back(AttributeMatrix::Type::FaceFeature);
    amTypes.push_back(AttributeMatrix::Type::Vertex);
    amTypes.push_back(AttributeMatrix::Type::VertexFeature);
    QVector<QString> daTypes;
    daTypes.push_back(SIMPL::TypeNames::Int32);
    QVector<QVector<size_t>> compDims;
    compDims.resize(1);
    compDims[0].resize(1);
    compDims[0][0] = 1;
    req.dcGeometryTypes = dcGeomTypes;
    req.amTypes = amTypes;
    req.daTypes = daTypes;
    req.componentDimensions = compDims;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("First Index Array", FirstIndexArrayPath, FilterParameter::RequiredArray, CombineAttributeMatrices, req));
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Second Index Array", SecondIndexArrayPath, FilterParameter::RequiredArray, CombineAttributeMatrices, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell/Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("New Index Array", NewIndexArrayName, FilterParameter::CreatedArray, CombineAttributeMatrices));
  parameters.push_back(SeparatorFilterParameter::New("Feature/Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Combined Attribute Matrix", CombinedAttributeMatrixName, FilterParameter::CreatedArray, CombineAttributeMatrices));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeMatrices::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFirstAttributeMatrixPath(reader->readDataArrayPath("FirstAttributeMatrixPath", getFirstAttributeMatrixPath()));
  setSecondAttributeMatrixPath(reader->readDataArrayPath("SecondAttributeMatrixPath", getSecondAttributeMatrixPath()));
  setCombinedAttributeMatrixName(reader->readString("CombinedAttributeMatrixName", getCombinedAttributeMatrixName()));
  setFirstIndexArrayPath(reader->readDataArrayPath("FirstIndexArrayPath", getFirstIndexArrayPath()));
  setSecondIndexArrayPath(reader->readDataArrayPath("SecondIndexArrayPath", getSecondIndexArrayPath()));
  setNewIndexArrayName(reader->readString("NewIndexArrayName", getNewIndexArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeMatrices::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeMatrices::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, getFirstAttributeMatrixPath().getDataContainerName(), false);
  if(getErrorCondition() < 0 || nullptr == m.get())
  {
    return;
  }

  if(getFirstAttributeMatrixPath().getDataContainerName().compare(getSecondAttributeMatrixPath().getDataContainerName()) != 0)
  {
    QString ss = QObject::tr("The selected attribute matrices must be in the same data container and currently are not");
    setErrorCondition(-5557);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getFirstAttributeMatrixPath().getAttributeMatrixName().compare(getSecondAttributeMatrixPath().getAttributeMatrixName()) == 0)
  {
    QString ss = QObject::tr("The selected attribute matrices must be different and currently are the same");
    setErrorCondition(-5558);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  AttributeMatrix::Pointer firstAttrMat = m->getPrereqAttributeMatrix(this, getFirstAttributeMatrixPath().getAttributeMatrixName(), -301);
  AttributeMatrix::Pointer secondAttrMat = m->getPrereqAttributeMatrix(this, getSecondAttributeMatrixPath().getAttributeMatrixName(), -301);
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(firstAttrMat->getType() != secondAttrMat->getType())
  {
    QString ss = QObject::tr("The selected attribute matrices must be of the same type (ie Feature) and currently are not");
    setErrorCondition(-5559);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getErrorCondition() < 0)
  {
    return;
  }

  // Note that the minus 1 in the totalTuples calculation is to account for the fact that the zeroth tuple in the two attribute matrices should only be counted once, not twice.
  // All Feature or Ensemble AMs should start from 1 and the zeroth tuple can be combined in the two AMs
  size_t totalTuples = firstAttrMat->getNumberOfTuples() + secondAttrMat->getNumberOfTuples() - 1;
  QVector<size_t> tDims(1, totalTuples);
  m->createNonPrereqAttributeMatrix(this, getCombinedAttributeMatrixName(), tDims, firstAttrMat->getType());
  if(getErrorCondition() < 0)
  {
    return;
  }
  AttributeMatrix::Pointer combinedAttrMat = m->getAttributeMatrix(getCombinedAttributeMatrixName());

  QVector<size_t> cDims(1, 1);
  m_FirstIndexPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFirstIndexArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FirstIndexPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FirstIndex = m_FirstIndexPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0)
  {
    return;
  }

  m_SecondIndexPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSecondIndexArrayPath(),
                                                                                                         cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SecondIndexPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SecondIndex = m_SecondIndexPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0)
  {
    return;
  }

  // Create arrays on the reference grid to hold data present on the sampling grid
  QList<QString> fArrayNames = firstAttrMat->getAttributeArrayNames();
  for(QList<QString>::iterator iter = fArrayNames.begin(); iter != fArrayNames.end(); ++iter)
  {
    tempPath.update(getFirstAttributeMatrixPath().getDataContainerName(), getCombinedAttributeMatrixName(), *iter);
    IDataArray::Pointer tmpDataArray = firstAttrMat->getPrereqIDataArray<IDataArray, AbstractFilter>(this, *iter, -90001);
    if(getErrorCondition() >= 0)
    {
      QVector<size_t> cDims = tmpDataArray->getComponentDimensions();
      TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, cDims, tmpDataArray);
    }
  }
  QList<QString> sArrayNames = secondAttrMat->getAttributeArrayNames();
  for(QList<QString>::iterator iter = sArrayNames.begin(); iter != sArrayNames.end(); ++iter)
  {
    tempPath.update(getSecondAttributeMatrixPath().getDataContainerName(), getCombinedAttributeMatrixName(), *iter);
    IDataArray::Pointer tmpDataArray = secondAttrMat->getPrereqIDataArray<IDataArray, AbstractFilter>(this, *iter, -90001);
    if(getErrorCondition() >= 0)
    {
      if(fArrayNames.contains(*iter) == false)
      {
        QVector<size_t> cDims = tmpDataArray->getComponentDimensions();
        TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, cDims, tmpDataArray);
      }
    }
  }

  tempPath.update(getFirstIndexArrayPath().getDataContainerName(), getFirstIndexArrayPath().getAttributeMatrixName(), getNewIndexArrayName());
  m_NewIndexPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0,
                                                                                                                     cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NewIndexPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NewIndex = m_NewIndexPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeMatrices::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

template <typename T> void copyData(IDataArray::Pointer fromData, IDataArray::Pointer toData, size_t location)
{
  typename DataArray<T>::Pointer fData = std::dynamic_pointer_cast<DataArray<T>>(fromData);
  typename DataArray<T>::Pointer tData = std::dynamic_pointer_cast<DataArray<T>>(toData);

  // only wanting to grab data from tuple 1 to numTuples of the fromData array,since the zeroth slot is a placeholder the first AM should already have
  T* src = fData->getPointer(1 * fromData->getNumberOfComponents());
  T* dest = tData->getPointer(location * toData->getNumberOfComponents());
  size_t bytes = sizeof(T) * (fromData->getNumberOfTuples() - 1) * fromData->getNumberOfComponents();
  ::memcpy(dest, src, bytes);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeMatrices::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFirstAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer firstAttrMat = m->getAttributeMatrix(getFirstAttributeMatrixPath().getAttributeMatrixName());
  AttributeMatrix::Pointer secondAttrMat = m->getAttributeMatrix(getSecondAttributeMatrixPath().getAttributeMatrixName());
  AttributeMatrix::Pointer combinedAttrMat = m->getAttributeMatrix(getCombinedAttributeMatrixName());
  size_t firstAttrMatNumTuples = firstAttrMat->getNumberOfTuples();

  size_t totalTuples1 = m_SecondIndexPtr.lock()->getNumberOfTuples();
  size_t totalTuples2 = m_SecondIndexPtr.lock()->getNumberOfTuples();
  for(size_t i = 0; i < totalTuples1; i++)
  {
    if(m_FirstIndex[i] > 0)
    {
      m_NewIndex[i] = m_FirstIndex[i];
    }
  }
  for(size_t i = 0; i < totalTuples2; i++)
  {
    // subtract 1 from the index plus numTuples because the second index should be shifted to account for the zeroth tuple (all AMs above element start at tuple 1)
    if(m_SecondIndex[i] > 0 && m_NewIndex[i] == 0)
    {
      m_NewIndex[i] = m_SecondIndex[i] + firstAttrMatNumTuples - 1;
    }
    else if(m_SecondIndex[i] > 0 && m_NewIndex[i] != 0)
    {
      QString ss = QObject::tr("When copying the indices, the indices of the two attribute matrices overlapped.  The index of the first attribute matrix was kept.");
      notifyWarningMessage(getHumanLabel(), ss, -111);
    }
  }

  QList<QString> arrayNames = firstAttrMat->getAttributeArrayNames();
  size_t location = 0;
  for(QList<QString>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    IDataArray::Pointer fromDataArray = firstAttrMat->getAttributeArray(*iter);
    IDataArray::Pointer toDataArray = combinedAttrMat->getAttributeArray(*iter);
    EXECUTE_FUNCTION_TEMPLATE(this, copyData, fromDataArray, fromDataArray, toDataArray, location);
  }

  arrayNames.clear();
  arrayNames = secondAttrMat->getAttributeArrayNames();
  location = firstAttrMatNumTuples;
  for(QList<QString>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    IDataArray::Pointer fromDataArray = secondAttrMat->getAttributeArray(*iter);
    IDataArray::Pointer toDataArray = combinedAttrMat->getAttributeArray(*iter);
    EXECUTE_FUNCTION_TEMPLATE(this, copyData, fromDataArray, fromDataArray, toDataArray, location);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CombineAttributeMatrices::newFilterInstance(bool copyFilterParameters) const
{
  CombineAttributeMatrices::Pointer filter = CombineAttributeMatrices::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CombineAttributeMatrices::getUuid()
{
  return QUuid("{334034e9-405f-51a3-9c3c-8d9c955835d9}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getHumanLabel() const
{
  return "Combine Feature/Ensemble Attribute Matrices";
}

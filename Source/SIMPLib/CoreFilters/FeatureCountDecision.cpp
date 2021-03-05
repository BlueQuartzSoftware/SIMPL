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

#include "FeatureCountDecision.h"

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureCountDecision::FeatureCountDecision()

    = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureCountDecision::~FeatureCountDecision() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureCountDecision::setupFilterParameters()
{
  FilterParameterVectorType parameters = getFilterParameters();
  parameters.push_back(SeparatorFilterParameter::Create("Cell Ensemble Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::Category::RequiredArray, FeatureCountDecision, req));
  }
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Maximum Number of Features", MaxGrains, FilterParameter::Category::Parameter, FeatureCountDecision, 0));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureCountDecision::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  AbstractDecisionFilter::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setMaxGrains(reader->readValue("MaxGrains", getMaxGrains()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureCountDecision::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureCountDecision::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom>(this, getFeatureIdsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 1);

  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getFeatureIdsArrayPath(), cDims);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureCountDecision::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // Assumes a SINGLE Phase. This WILL BREAK if there is more than 1 phase.
  qDebug() << "MaxGrains: " << m_MaxGrains;
  qDebug() << "FeatureIds: " << m_FeatureIds[1];

  bool dm = true;

  if(m_MaxGrains < m_FeatureIds[1])
  {
    dm = false;
  }

  Q_EMIT decisionMade(dm);
  Q_EMIT targetValue(m_FeatureIds[1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureCountDecision::extractProperties(const QJsonDocument& jsonDoc)
{
  QJsonObject json = jsonDoc.object();
  QJsonValue jvalue = json.value(this->getNameOfClass());
  if(jvalue.isUndefined())
  {
    return;
  }

  {
    QJsonValue propValue = jvalue.toObject()["MaxGrains"];
    if(!propValue.isUndefined())
    {
      setMaxGrains(propValue.toInt());
    }
  }

  {
    QJsonValue propValue = jvalue.toObject()["FeatureIdsArrayPath"];
    if(!propValue.isUndefined())
    {
      QJsonObject jObj = propValue.toObject();
      m_FeatureIdsArrayPath.readJson(jObj);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FeatureCountDecision::newFilterInstance(bool copyFilterParameters) const
{
  FeatureCountDecision::Pointer filter = FeatureCountDecision::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FeatureCountDecision::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FeatureCountDecision::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FeatureCountDecision::getUuid() const
{
  return QUuid("{64d1df13-17a2-56a2-90a5-4dfda442b144}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FeatureCountDecision::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FeatureCountDecision::getHumanLabel() const
{
  return "Feature Count Decision";
}

// -----------------------------------------------------------------------------
FeatureCountDecision::Pointer FeatureCountDecision::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
FeatureCountDecision::Pointer FeatureCountDecision::New()
{
  Pointer sharedPtr(new(FeatureCountDecision));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString FeatureCountDecision::getNameOfClass() const
{
  return QString("FeatureCountDecision");
}

// -----------------------------------------------------------------------------
QString FeatureCountDecision::ClassName()
{
  return QString("FeatureCountDecision");
}

// -----------------------------------------------------------------------------
void FeatureCountDecision::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FeatureCountDecision::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FeatureCountDecision::setMaxGrains(int value)
{
  m_MaxGrains = value;
}

// -----------------------------------------------------------------------------
int FeatureCountDecision::getMaxGrains() const
{
  return m_MaxGrains;
}

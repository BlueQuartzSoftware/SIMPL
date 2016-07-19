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

#include "DataArrayCreationFilterParameter.h"
#include "SIMPLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::DataArrayCreationFilterParameter() :
  FilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::~DataArrayCreationFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::Pointer DataArrayCreationFilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const DataArrayPath& defaultValue, Category category, const RequirementType req, SetterCallbackType setterCallback,
                                                                                GetterCallbackType getterCallback, int groupIndex)
{

  DataArrayCreationFilterParameter::Pointer ptr = DataArrayCreationFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setDefaultGeometryTypes(req.dcGeometryTypes);
  ptr->setDefaultAttributeMatrixTypes(req.amTypes);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayCreationFilterParameter::getWidgetType()
{
  return QString("DataArrayCreationWidget");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::RequirementType DataArrayCreationFilterParameter::CreateRequirement(uint32_t attributeMatrixObjectType)
{
  DataArrayCreationFilterParameter::RequirementType req;
  QVector<unsigned int> amTypes;
  if(attributeMatrixObjectType == SIMPL::AttributeMatrixObjectType::Element)
  {
    amTypes.push_back(SIMPL::AttributeMatrixType::Cell);
    amTypes.push_back(SIMPL::AttributeMatrixType::Face);
    amTypes.push_back(SIMPL::AttributeMatrixType::Edge);
    amTypes.push_back(SIMPL::AttributeMatrixType::Vertex);
  }
  else if(attributeMatrixObjectType == SIMPL::AttributeMatrixObjectType::Feature)
  {
    amTypes.push_back(SIMPL::AttributeMatrixType::CellFeature);
    amTypes.push_back(SIMPL::AttributeMatrixType::FaceFeature);
    amTypes.push_back(SIMPL::AttributeMatrixType::EdgeFeature);
    amTypes.push_back(SIMPL::AttributeMatrixType::VertexFeature);
  }
  else if(attributeMatrixObjectType == SIMPL::AttributeMatrixObjectType::Ensemble)
  {
    amTypes.push_back(SIMPL::AttributeMatrixType::CellEnsemble);
    amTypes.push_back(SIMPL::AttributeMatrixType::FaceEnsemble);
    amTypes.push_back(SIMPL::AttributeMatrixType::EdgeEnsemble);
    amTypes.push_back(SIMPL::AttributeMatrixType::VertexEnsemble);
  }
  req.amTypes = amTypes;

  return req;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::RequirementType DataArrayCreationFilterParameter::CreateRequirement(uint32_t attributeMatrixType,
    uint32_t geometryType)
{
  DataArrayCreationFilterParameter::RequirementType req;
  if(SIMPL::Defaults::AnyAttributeMatrix != attributeMatrixType)
  {
    req.amTypes = QVector<unsigned int>(1, attributeMatrixType);;
  }
  if(SIMPL::Defaults::AnyGeometry != geometryType)
  {
    req.dcGeometryTypes = QVector<uint32_t>(1, geometryType);
  }
  return req;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationFilterParameter::readJson(const QJsonObject &json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(!jsonValue.isUndefined() )
  {
    QJsonObject obj = jsonValue.toObject();
    DataArrayPath dap;
    dap.readJson(obj);
    m_SetterCallback(dap);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationFilterParameter::writeJson(QJsonObject &json)
{
  DataArrayPath dap = m_GetterCallback();
  QJsonObject obj;
  dap.writeJson(obj);
  json[getPropertyName()] = obj;
}

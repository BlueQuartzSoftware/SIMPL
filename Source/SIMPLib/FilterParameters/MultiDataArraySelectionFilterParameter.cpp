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

#include "MultiDataArraySelectionFilterParameter.h"

#include <QtCore/QJsonArray>

#include "SIMPLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionFilterParameter::MultiDataArraySelectionFilterParameter()
: FilterParameter()
, m_DefaultPaths(QVector<DataArrayPath>())
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionFilterParameter::~MultiDataArraySelectionFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionFilterParameter::Pointer MultiDataArraySelectionFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QVector<DataArrayPath>& defaultValue,
                                                                                            Category category, SetterCallbackType setterCallback, GetterCallbackType getterCallback,
                                                                                            const RequirementType req, int groupIndex)
{

  MultiDataArraySelectionFilterParameter::Pointer ptr = MultiDataArraySelectionFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setDefaultGeometryTypes(req.dcGeometryTypes);
  ptr->setDefaultAttributeMatrixTypes(req.amTypes);
  ptr->setDefaultAttributeArrayTypes(req.daTypes);
  ptr->setDefaultComponentDimensions(req.componentDimensions);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiDataArraySelectionFilterParameter::getWidgetType()
{
  return QString("MultiDataArraySelectionWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionFilterParameter::RequirementType MultiDataArraySelectionFilterParameter::CreateCategoryRequirement(const QString& primitiveType, size_t allowedCompDim,
                                                                                                                          AttributeMatrix::Category attributeMatrixCategory)
{
  typedef QVector<size_t> QVectorOfSizeType;
  MultiDataArraySelectionFilterParameter::RequirementType req;
  AttributeMatrix::Types amTypes;
  if(attributeMatrixCategory == AttributeMatrix::Category::Element)
  {
    amTypes.push_back(AttributeMatrix::Type::Cell);
    amTypes.push_back(AttributeMatrix::Type::Face);
    amTypes.push_back(AttributeMatrix::Type::Edge);
    amTypes.push_back(AttributeMatrix::Type::Vertex);
  }
  else if(attributeMatrixCategory == AttributeMatrix::Category::Feature)
  {
    amTypes.push_back(AttributeMatrix::Type::CellFeature);
    amTypes.push_back(AttributeMatrix::Type::FaceFeature);
    amTypes.push_back(AttributeMatrix::Type::EdgeFeature);
    amTypes.push_back(AttributeMatrix::Type::VertexFeature);
  }
  else if(attributeMatrixCategory == AttributeMatrix::Category::Ensemble)
  {
    amTypes.push_back(AttributeMatrix::Type::CellEnsemble);
    amTypes.push_back(AttributeMatrix::Type::FaceEnsemble);
    amTypes.push_back(AttributeMatrix::Type::EdgeEnsemble);
    amTypes.push_back(AttributeMatrix::Type::VertexEnsemble);
  }
  req.amTypes = amTypes;
  if(primitiveType.compare(SIMPL::Defaults::AnyPrimitive) != 0)
  {
    req.daTypes = QVector<QString>(1, primitiveType);
  }
  if(SIMPL::Defaults::AnyComponentSize != allowedCompDim)
  {
    req.componentDimensions = QVector<QVectorOfSizeType>(1, QVectorOfSizeType(1, allowedCompDim));
  }
  //  if(IGeometry::Type::Unknown != geometryType)
  //  {
  //    req.dcGeometryTypes = IGeometry::Types(1, geometryType);
  //  }
  return req;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionFilterParameter::RequirementType MultiDataArraySelectionFilterParameter::CreateRequirement(const QString& primitiveType, size_t allowedCompDim, AttributeMatrix::Type attributeMatrixType,
                                                                                                                  IGeometry::Type geometryType)
{
  typedef QVector<size_t> QVectorOfSizeType;
  MultiDataArraySelectionFilterParameter::RequirementType req;
  if(primitiveType.compare(SIMPL::Defaults::AnyPrimitive) != 0)
  {
    req.daTypes = QVector<QString>(1, primitiveType);
  }
  if(SIMPL::Defaults::AnyComponentSize != allowedCompDim)
  {
    req.componentDimensions = QVector<QVectorOfSizeType>(1, QVectorOfSizeType(1, allowedCompDim));
  }
  if(AttributeMatrix::Type::Any != attributeMatrixType)
  {
    QVector<AttributeMatrix::Type> amTypes(1, attributeMatrixType);
    req.amTypes = amTypes;
  }
  if(IGeometry::Type::Any != geometryType)
  {
    req.dcGeometryTypes = IGeometry::Types(1, geometryType);
  }
  return req;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonArray arrayObj = jsonValue.toArray();
    QVector<DataArrayPath> dapVec;
    for(int i = 0; i < arrayObj.size(); i++)
    {
      QJsonObject obj = arrayObj.at(i).toObject();
      DataArrayPath dap;
      dap.readJson(obj);
      dapVec.push_back(dap);
    }

    m_SetterCallback(dapVec);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionFilterParameter::writeJson(QJsonObject& json)
{
  if (m_GetterCallback)
  {
    QVector<DataArrayPath> dapVec = m_GetterCallback();
    QJsonArray arrayObj;

    for(int i = 0; i < dapVec.size(); i++)
    {
      DataArrayPath dap = dapVec[i];
      QJsonObject obj;
      dap.writeJson(obj);
      arrayObj.push_back(obj);
    }

    json[getPropertyName()] = arrayObj;
  }
}

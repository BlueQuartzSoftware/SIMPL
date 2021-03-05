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
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::DataArrayCreationFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::~DataArrayCreationFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::Pointer DataArrayCreationFilterParameter::Create(const QString& humanLabel, const QString& propertyName, const DataArrayPath& defaultValue, Category category,
                                                                                   const SetterCallbackType& setterCallback, const GetterCallbackType& getterCallback, RequirementType req,
                                                                                   int groupIndex)
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
QString DataArrayCreationFilterParameter::getWidgetType() const
{
  return QString("DataArrayCreationWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::RequirementType DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Category attributeMatrixObjectType)
{
  DataArrayCreationFilterParameter::RequirementType req;
  AttributeMatrix::Types amTypes;
  if(attributeMatrixObjectType == AttributeMatrix::Category::Element)
  {
    amTypes.push_back(AttributeMatrix::Type::Cell);
    amTypes.push_back(AttributeMatrix::Type::Face);
    amTypes.push_back(AttributeMatrix::Type::Edge);
    amTypes.push_back(AttributeMatrix::Type::Vertex);
  }
  else if(attributeMatrixObjectType == AttributeMatrix::Category::Feature)
  {
    amTypes.push_back(AttributeMatrix::Type::CellFeature);
    amTypes.push_back(AttributeMatrix::Type::FaceFeature);
    amTypes.push_back(AttributeMatrix::Type::EdgeFeature);
    amTypes.push_back(AttributeMatrix::Type::VertexFeature);
  }
  else if(attributeMatrixObjectType == AttributeMatrix::Category::Ensemble)
  {
    amTypes.push_back(AttributeMatrix::Type::CellEnsemble);
    amTypes.push_back(AttributeMatrix::Type::FaceEnsemble);
    amTypes.push_back(AttributeMatrix::Type::EdgeEnsemble);
    amTypes.push_back(AttributeMatrix::Type::VertexEnsemble);
  }
  req.amTypes = amTypes;

  return req;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::RequirementType DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type attributeMatrixType, IGeometry::Type geometryType)
{
  DataArrayCreationFilterParameter::RequirementType req;
  if(AttributeMatrix::Type::Any != attributeMatrixType)
  {
    req.amTypes = AttributeMatrix::Types(1, attributeMatrixType);
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
void DataArrayCreationFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
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
void DataArrayCreationFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    DataArrayPath dap = m_GetterCallback();
    QJsonObject obj;
    dap.writeJson(obj);
    json[getPropertyName()] = obj;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationFilterParameter::dataArrayPathRenamed(AbstractFilter* filter, const DataArrayPath::RenameType& renamePath)
{
  QVariant var = filter->property(qPrintable(getPropertyName()));
  if(var.isValid() && var.canConvert<DataArrayPath>())
  {
    DataArrayPath path = var.value<DataArrayPath>();
    if(path.updatePath(renamePath))
    {
      if(m_SetterCallback)
      {
        m_SetterCallback(path);
      }
      Q_EMIT filter->dataArrayPathUpdated(getPropertyName(), renamePath);
    }
  }
}

// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::Pointer DataArrayCreationFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::Pointer DataArrayCreationFilterParameter::New()
{
  Pointer sharedPtr(new(DataArrayCreationFilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString DataArrayCreationFilterParameter::getNameOfClass() const
{
  return QString("DataArrayCreationFilterParameter");
}

// -----------------------------------------------------------------------------
QString DataArrayCreationFilterParameter::ClassName()
{
  return QString("DataArrayCreationFilterParameter");
}

// -----------------------------------------------------------------------------
void DataArrayCreationFilterParameter::setDefaultGeometryTypes(const IGeometry::Types& value)
{
  m_DefaultGeometryTypes = value;
}

// -----------------------------------------------------------------------------
IGeometry::Types DataArrayCreationFilterParameter::getDefaultGeometryTypes() const
{
  return m_DefaultGeometryTypes;
}

// -----------------------------------------------------------------------------
void DataArrayCreationFilterParameter::setDefaultAttributeMatrixTypes(const AttributeMatrix::Types& value)
{
  m_DefaultAttributeMatrixTypes = value;
}

// -----------------------------------------------------------------------------
AttributeMatrix::Types DataArrayCreationFilterParameter::getDefaultAttributeMatrixTypes() const
{
  return m_DefaultAttributeMatrixTypes;
}

// -----------------------------------------------------------------------------
void DataArrayCreationFilterParameter::setSetterCallback(const DataArrayCreationFilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::SetterCallbackType DataArrayCreationFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void DataArrayCreationFilterParameter::setGetterCallback(const DataArrayCreationFilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
DataArrayCreationFilterParameter::GetterCallbackType DataArrayCreationFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

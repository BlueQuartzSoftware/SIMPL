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

#include "DataArraySelectionFilterParameter.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArraySelectionFilterParameter::DataArraySelectionFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArraySelectionFilterParameter::~DataArraySelectionFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArraySelectionFilterParameter::Pointer DataArraySelectionFilterParameter::New(const QString& humanLabel, const QString& propertyName, const DataArrayPath& defaultValue, Category category,
                                                                                  const SetterCallbackType& setterCallback, const GetterCallbackType& getterCallback, RequirementType req,
                                                                                  int groupIndex)
{

  DataArraySelectionFilterParameter::Pointer ptr = DataArraySelectionFilterParameter::New();
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
QString DataArraySelectionFilterParameter::getWidgetType() const
{
  return QString("DataArraySelectionWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArraySelectionFilterParameter::RequirementType DataArraySelectionFilterParameter::CreateCategoryRequirement(const QString& primitiveType, size_t allowedCompDim,
                                                                                                                AttributeMatrix::Category attributeMatrixCategory)
{
  using SizeTVectorType = std::vector<size_t>;
  DataArraySelectionFilterParameter::RequirementType req;
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
    req.componentDimensions = std::vector<SizeTVectorType>(1, SizeTVectorType(1, allowedCompDim));
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
DataArraySelectionFilterParameter::RequirementType DataArraySelectionFilterParameter::CreateRequirement(const QString& primitiveType, size_t allowedCompDim, AttributeMatrix::Type attributeMatrixType,
                                                                                                        IGeometry::Type geometryType)
{
  using SizeTVectorType = std::vector<size_t>;
  DataArraySelectionFilterParameter::RequirementType req;
  if(primitiveType.compare(SIMPL::Defaults::AnyPrimitive) != 0)
  {
    req.daTypes = QVector<QString>(1, primitiveType);
  }
  if(SIMPL::Defaults::AnyComponentSize != allowedCompDim)
  {
    req.componentDimensions = std::vector<SizeTVectorType>(1, SizeTVectorType(1, allowedCompDim));
  }
  if(AttributeMatrix::Type::Any != attributeMatrixType)
  {
    AttributeMatrix::Types amTypes(1, attributeMatrixType);
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
DataArraySelectionFilterParameter::RequirementType DataArraySelectionFilterParameter::CreateRequirement(const QVector<QString>& primitiveTypes, size_t allowedCompDim,
                                                                                                        AttributeMatrix::Type attributeMatrixType, IGeometry::Type geometryType)
{
  using SizeTVectorType = std::vector<size_t>;
  DataArraySelectionFilterParameter::RequirementType req;

  for(const auto& type : primitiveTypes)
  {
    if(type.compare(SIMPL::Defaults::AnyPrimitive) != 0)
    {
      req.daTypes.push_back(type);
    }
  }

  if(SIMPL::Defaults::AnyComponentSize != allowedCompDim)
  {
    req.componentDimensions = std::vector<SizeTVectorType>(1, SizeTVectorType(1, allowedCompDim));
  }
  if(AttributeMatrix::Type::Any != attributeMatrixType)
  {
    AttributeMatrix::Types amTypes(1, attributeMatrixType);
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
void DataArraySelectionFilterParameter::readJson(const QJsonObject& json)
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
void DataArraySelectionFilterParameter::writeJson(QJsonObject& json)
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
DataArraySelectionFilterParameter::RequirementType DataArraySelectionFilterParameter::getRequirements()
{
  RequirementType reqs;
  reqs.dcGeometryTypes = getDefaultGeometryTypes();
  reqs.amTypes = getDefaultAttributeMatrixTypes();
  reqs.daTypes = getDefaultAttributeArrayTypes();
  reqs.componentDimensions = getDefaultComponentDimensions();

  return reqs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionFilterParameter::dataArrayPathRenamed(AbstractFilter* filter, const DataArrayPath::RenameType& renamePath)
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
      emit filter->dataArrayPathUpdated(getPropertyName(), renamePath);
    }
  }
}

// -----------------------------------------------------------------------------
DataArraySelectionFilterParameter::Pointer DataArraySelectionFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
DataArraySelectionFilterParameter::Pointer DataArraySelectionFilterParameter::New()
{
  Pointer sharedPtr(new(DataArraySelectionFilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString DataArraySelectionFilterParameter::getNameOfClass() const
{
  return QString("DataArraySelectionFilterParameter");
}

// -----------------------------------------------------------------------------
QString DataArraySelectionFilterParameter::ClassName()
{
  return QString("DataArraySelectionFilterParameter");
}

// -----------------------------------------------------------------------------
void DataArraySelectionFilterParameter::setDefaultGeometryTypes(const IGeometry::Types& value)
{
  m_DefaultGeometryTypes = value;
}

// -----------------------------------------------------------------------------
IGeometry::Types DataArraySelectionFilterParameter::getDefaultGeometryTypes() const
{
  return m_DefaultGeometryTypes;
}

// -----------------------------------------------------------------------------
void DataArraySelectionFilterParameter::setDefaultAttributeMatrixTypes(const AttributeMatrix::Types& value)
{
  m_DefaultAttributeMatrixTypes = value;
}

// -----------------------------------------------------------------------------
AttributeMatrix::Types DataArraySelectionFilterParameter::getDefaultAttributeMatrixTypes() const
{
  return m_DefaultAttributeMatrixTypes;
}

// -----------------------------------------------------------------------------
void DataArraySelectionFilterParameter::setDefaultAttributeArrayTypes(const QVector<QString>& value)
{
  m_DefaultAttributeArrayTypes = value;
}

// -----------------------------------------------------------------------------
QVector<QString> DataArraySelectionFilterParameter::getDefaultAttributeArrayTypes() const
{
  return m_DefaultAttributeArrayTypes;
}

// -----------------------------------------------------------------------------
void DataArraySelectionFilterParameter::setDefaultComponentDimensions(const std::vector<std::vector<size_t>>& value)
{
  m_DefaultComponentDimensions = value;
}

// -----------------------------------------------------------------------------
std::vector<std::vector<size_t>> DataArraySelectionFilterParameter::getDefaultComponentDimensions() const
{
  return m_DefaultComponentDimensions;
}

// -----------------------------------------------------------------------------
void DataArraySelectionFilterParameter::setSetterCallback(const DataArraySelectionFilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
DataArraySelectionFilterParameter::SetterCallbackType DataArraySelectionFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void DataArraySelectionFilterParameter::setGetterCallback(const DataArraySelectionFilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
DataArraySelectionFilterParameter::GetterCallbackType DataArraySelectionFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

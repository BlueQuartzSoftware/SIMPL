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

#include "MultiDataContainerSelectionFilterParameter.h"

#include <QtCore/QJsonArray>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataContainerSelectionFilterParameter::MultiDataContainerSelectionFilterParameter()
: m_DefaultNames(QStringList())
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataContainerSelectionFilterParameter::~MultiDataContainerSelectionFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataContainerSelectionFilterParameter::Pointer MultiDataContainerSelectionFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QStringList& defaultValue,
                                                                                                    Category category, const SetterCallbackType& setterCallback,
                                                                                                    const GetterCallbackType& getterCallback, const RequirementType& req, int groupIndex)
{

  MultiDataContainerSelectionFilterParameter::Pointer ptr = MultiDataContainerSelectionFilterParameter::New();
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
QString MultiDataContainerSelectionFilterParameter::getWidgetType() const
{
  return QString("MultiDataContainerSelectionWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataContainerSelectionFilterParameter::RequirementType MultiDataContainerSelectionFilterParameter::CreateCategoryRequirement(const QString& primitiveType, size_t allowedCompDim,
                                                                                                                                  AttributeMatrix::Category attributeMatrixCategory)
{
  using SizeTVectorType = std::vector<size_t>;
  MultiDataContainerSelectionFilterParameter::RequirementType req;
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
MultiDataContainerSelectionFilterParameter::RequirementType MultiDataContainerSelectionFilterParameter::CreateRequirement(const QString& primitiveType, size_t allowedCompDim,
                                                                                                                          AttributeMatrix::Type attributeMatrixType, IGeometry::Type geometryType)
{
  using SizeTVectorType = std::vector<size_t>;
  MultiDataContainerSelectionFilterParameter::RequirementType req;
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
void MultiDataContainerSelectionFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonArray arrayObj = jsonValue.toArray();
    QStringList dcList;
    for(const auto& obj : arrayObj)
    {
      QString dcName = obj.toString();
      dcList.push_back(dcName);
    }

    m_SetterCallback(dcList);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    QStringList dcList = m_GetterCallback();
    QJsonArray arrayObj;

    for(int i = 0; i < dcList.size(); i++)
    {
      QString dcName = dcList[i];
      arrayObj.push_back(dcName);
    }

    json[getPropertyName()] = arrayObj;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataContainerSelectionFilterParameter::dataArrayPathRenamed(AbstractFilter* filter, const DataArrayPath::RenameType& renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  QStringList dcList = m_GetterCallback();
  int count = dcList.size();
  bool updated = false;

  for(int i = 0; i < count; i++)
  {
    if(dcList[i] == oldPath.getDataContainerName())
    {
      dcList[i] = newPath.getDataContainerName();
      updated = true;
    }
  }

  m_SetterCallback(dcList);
  emit filter->dataArrayPathUpdated(getPropertyName(), renamePath);
}

// -----------------------------------------------------------------------------
MultiDataContainerSelectionFilterParameter::Pointer MultiDataContainerSelectionFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
MultiDataContainerSelectionFilterParameter::Pointer MultiDataContainerSelectionFilterParameter::New()
{
  Pointer sharedPtr(new(MultiDataContainerSelectionFilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString MultiDataContainerSelectionFilterParameter::getNameOfClass() const
{
  return QString("MultiDataContainerSelectionFilterParameter");
}

// -----------------------------------------------------------------------------
QString MultiDataContainerSelectionFilterParameter::ClassName()
{
  return QString("MultiDataContainerSelectionFilterParameter");
}

// -----------------------------------------------------------------------------
void MultiDataContainerSelectionFilterParameter::setDefaultNames(const QStringList& value)
{
  m_DefaultNames = value;
}

// -----------------------------------------------------------------------------
QStringList MultiDataContainerSelectionFilterParameter::getDefaultNames() const
{
  return m_DefaultNames;
}

// -----------------------------------------------------------------------------
void MultiDataContainerSelectionFilterParameter::setDefaultGeometryTypes(const IGeometry::Types& value)
{
  m_DefaultGeometryTypes = value;
}

// -----------------------------------------------------------------------------
IGeometry::Types MultiDataContainerSelectionFilterParameter::getDefaultGeometryTypes() const
{
  return m_DefaultGeometryTypes;
}

// -----------------------------------------------------------------------------
void MultiDataContainerSelectionFilterParameter::setDefaultAttributeMatrixTypes(const QVector<AttributeMatrix::Type>& value)
{
  m_DefaultAttributeMatrixTypes = value;
}

// -----------------------------------------------------------------------------
QVector<AttributeMatrix::Type> MultiDataContainerSelectionFilterParameter::getDefaultAttributeMatrixTypes() const
{
  return m_DefaultAttributeMatrixTypes;
}

// -----------------------------------------------------------------------------
void MultiDataContainerSelectionFilterParameter::setDefaultAttributeArrayTypes(const QVector<QString>& value)
{
  m_DefaultAttributeArrayTypes = value;
}

// -----------------------------------------------------------------------------
QVector<QString> MultiDataContainerSelectionFilterParameter::getDefaultAttributeArrayTypes() const
{
  return m_DefaultAttributeArrayTypes;
}

// -----------------------------------------------------------------------------
void MultiDataContainerSelectionFilterParameter::setDefaultComponentDimensions(const std::vector<std::vector<size_t>>& value)
{
  m_DefaultComponentDimensions = value;
}

// -----------------------------------------------------------------------------
std::vector<std::vector<size_t>> MultiDataContainerSelectionFilterParameter::getDefaultComponentDimensions() const
{
  return m_DefaultComponentDimensions;
}

// -----------------------------------------------------------------------------
void MultiDataContainerSelectionFilterParameter::setSetterCallback(const MultiDataContainerSelectionFilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
MultiDataContainerSelectionFilterParameter::SetterCallbackType MultiDataContainerSelectionFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void MultiDataContainerSelectionFilterParameter::setGetterCallback(const MultiDataContainerSelectionFilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
MultiDataContainerSelectionFilterParameter::GetterCallbackType MultiDataContainerSelectionFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

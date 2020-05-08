/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5280
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "LinkedPathCreationFilterParameter.h"

#include <stdexcept>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::ILinkedPath* LinkedPathCreationFilterParameter::CreateLinkedPath(LinkedDataPath::GetterCallbackType callback, DataArrayPathHelper::DataType dataType)
{
  return new LinkedDataPath(callback, dataType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::ILinkedPath* LinkedPathCreationFilterParameter::CreateLinkedPath(LinkedStringPath::GetterCallbackType dcCallback)
{
  return new LinkedStringPath(dcCallback);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::ILinkedPath* LinkedPathCreationFilterParameter::CreateLinkedPath(LinkedStringPath::GetterCallbackType dcCallback, LinkedStringPath::GetterCallbackType amCallback)
{
  return new LinkedStringPath(dcCallback, amCallback);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::ILinkedPath* LinkedPathCreationFilterParameter::CreateLinkedPath(LinkedMixedPath::PathGetterCallbackType dcCallback,
                                                                                                    LinkedMixedPath::PathGetterCallbackType amCallback)
{
  return new LinkedMixedPath(dcCallback, amCallback);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::ILinkedPath* LinkedPathCreationFilterParameter::CreateLinkedPath(LinkedMixedPath::PathGetterCallbackType dcCallback,
                                                                                                    LinkedMixedPath::StringGetterCallbackType amCallback)
{
  return new LinkedMixedPath(dcCallback, amCallback);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::ILinkedPath* LinkedPathCreationFilterParameter::CreateLinkedPath(LinkedMixedPath::PathGetterCallbackType pathCallback)
{
  return new LinkedDataPath(pathCallback, DataArrayPathHelper::DataType::DataContainer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::ILinkedPath* LinkedPathCreationFilterParameter::CreateLinkedPath(LinkedAdvComparisonPath::GetterCallbackType callback)
{
  return new LinkedAdvComparisonPath(callback);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::LinkedPathCreationFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::~LinkedPathCreationFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::Pointer LinkedPathCreationFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QString& defaultValue, Category category,
                                                                                  const SetterCallbackType& setterCallback, const GetterCallbackType& getterCallback, ILinkedPath* linkedPath,
                                                                                  int groupIndex)
{

  LinkedPathCreationFilterParameter::Pointer ptr = LinkedPathCreationFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setDefaultValue(defaultValue);
  ptr->setLinkedPath(linkedPath);
  ptr->setCategory(category);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString LinkedPathCreationFilterParameter::getWidgetType() const
{
  return QString("LinkedPathCreationWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedPathCreationFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(jsonValue.isUndefined())
  {
    jsonValue = json[getLegacyPropertyName()];
  }
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    m_SetterCallback(jsonValue.toString(""));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedPathCreationFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    json[getPropertyName()] = m_GetterCallback();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::LinkedStringPath::LinkedStringPath(GetterCallbackType getDc)
: dcGetter(getDc)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::LinkedStringPath::LinkedStringPath(GetterCallbackType getDc, GetterCallbackType getAm)
: dcGetter(getDc)
, amGetter(getAm)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::LinkedMixedPath::LinkedMixedPath(PathGetterCallbackType getDc, StringGetterCallbackType getAm)
: dcGetter(getDc)
, amGetter(getAm)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::LinkedMixedPath::LinkedMixedPath(PathGetterCallbackType dc, PathGetterCallbackType am)
: dcGetter(dc)
, amPathGetter(am)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::LinkedDataPath::LinkedDataPath(GetterCallbackType getPath)
: pathGetter(getPath)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::LinkedDataPath::LinkedDataPath(GetterCallbackType getPath, DataArrayPathHelper::DataType targetLevel)
: pathGetter(getPath)
, m_TargetPathType(targetLevel)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::LinkedAdvComparisonPath::LinkedAdvComparisonPath(GetterCallbackType advComp)
: compGetter(advComp)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath LinkedPathCreationFilterParameter::LinkedStringPath::generatePath()
{
  DataArrayPath linkedPath;
  // Use std::function operator bool to check if this is a callable function
  if(dcGetter)
  {
    linkedPath.setDataContainerName(dcGetter());
    if(amGetter)
    {
      linkedPath.setAttributeMatrixName(amGetter());
    }
  }
  return linkedPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath LinkedPathCreationFilterParameter::LinkedMixedPath::generatePath()
{
  DataArrayPath linkedPath;
  // Use std::function operator bool to check if this is a callable function
  if(dcGetter && amGetter)
  {
    linkedPath.setDataContainerName(dcGetter().getDataContainerName());
    linkedPath.setAttributeMatrixName(amGetter());
  }
  else if(dcGetter && amPathGetter)
  {
    linkedPath.setDataContainerName(dcGetter().getDataContainerName());
    linkedPath.setAttributeMatrixName(amPathGetter().getAttributeMatrixName());
  }
  // else
  //{
  //  throw std::invalid_argument("Invalid linked AttributeMatrix path");
  //}
  return linkedPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath LinkedPathCreationFilterParameter::LinkedDataPath::generatePath()
{
  if(!pathGetter)
  {
    return DataArrayPath();
  }
  DataArrayPath linkedPath = pathGetter();
  switch(m_TargetPathType)
  {
  case DataArrayPathHelper::DataType::DataContainer:
    return DataArrayPath(linkedPath.getDataContainerName(), "", "");
  case DataArrayPathHelper::DataType::AttributeMatrix:
    return DataArrayPath(linkedPath.getDataContainerName(), linkedPath.getAttributeMatrixName(), "");
  case DataArrayPathHelper::DataType::DataArray:
    throw std::domain_error("LinkedPathCreationFilterParameter cannot target a child path underneath DataArray");
  case DataArrayPathHelper::DataType::None:
    return linkedPath;
  }
  return DataArrayPath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath LinkedPathCreationFilterParameter::LinkedAdvComparisonPath::generatePath()
{
  DataArrayPath linkedPath;
  // Use std::function operator bool to check if this is a callable function
  if(compGetter)
  {
    linkedPath.setDataContainerName(compGetter().getDataContainerName());
    linkedPath.setAttributeMatrixName(compGetter().getAttributeMatrixName());
  }
  // else
  //{
  //  throw std::invalid_argument("Invalid linked AttributeMatrix path");
  //}
  return linkedPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedPathCreationFilterParameter::setLinkedPath(ILinkedPath* linkedPath)
{
  delete m_LinkedPath;
  m_LinkedPath = linkedPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath LinkedPathCreationFilterParameter::getLinkedDataArrayPath() const
{
  if(m_LinkedPath)
  {
    return m_LinkedPath->generatePath();
  }
  return DataArrayPath("", "", "");
}

// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::Pointer LinkedPathCreationFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::Pointer LinkedPathCreationFilterParameter::New()
{
  Pointer sharedPtr(new(LinkedPathCreationFilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString LinkedPathCreationFilterParameter::getNameOfClass() const
{
  return QString("LinkedPathCreationFilterParameter");
}

// -----------------------------------------------------------------------------
QString LinkedPathCreationFilterParameter::ClassName()
{
  return QString("LinkedPathCreationFilterParameter");
}

// -----------------------------------------------------------------------------
void LinkedPathCreationFilterParameter::setSetterCallback(const LinkedPathCreationFilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::SetterCallbackType LinkedPathCreationFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void LinkedPathCreationFilterParameter::setGetterCallback(const LinkedPathCreationFilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
LinkedPathCreationFilterParameter::GetterCallbackType LinkedPathCreationFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}

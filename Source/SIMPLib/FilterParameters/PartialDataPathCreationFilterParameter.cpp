/* ============================================================================
* Copyright (c) 2009-2019 BlueQuartz Software, LLC
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

#include "PartialDataPathCreationFilterParameter.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Filtering/AbstractFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PartialDataPathCreationFilterParameter::PartialDataPathCreationFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PartialDataPathCreationFilterParameter::~PartialDataPathCreationFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PartialDataPathCreationFilterParameter::Pointer PartialDataPathCreationFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QString& defaultValue, Category category,
  SetterCallbackType setterCallback, GetterCallbackType getterCallback, ILinkedPath* linkedPath, int groupIndex)
{
  PartialDataPathCreationFilterParameter::Pointer ptr = PartialDataPathCreationFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setLinkedPath(linkedPath);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PartialDataPathCreationFilterParameter::setLinkedPath(ILinkedPath* linkedPath)
{
  m_LinkedPath = std::make_unique<ILinkedPath>(linkedPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath PartialDataPathCreationFilterParameter::LinkedStringPath::generatePath() const
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
DataArrayPath PartialDataPathCreationFilterParameter::LinkedDataPath::generatePath() const
{
  if(pathGetter)
  {
    return pathGetter();
  }
  return DataArrayPath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PartialDataPathCreationFilterParameter::getWidgetType() const
{
  return QString("PartialDataPathCreationWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath PartialDataPathCreationFilterParameter::getParentPath() const
{
  if(nullptr == m_LinkedPath)
  {
    return DataArrayPath();
  }
  return m_LinkedPath->generatePath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath PartialDataPathCreationFilterParameter::getDataArrayPath() const
{
  DataArrayPath path = getParentPath();
  QString value = getGetterCallback()();
  switch(path.getDataType())
  {
  case DataArrayPath::DataType::None:
    path.setDataContainerName(value);
    break;
  case DataArrayPath::DataType::DataContainer:
    path.setAttributeMatrixName(value);
    break;
  case DataArrayPath::DataType::AttributeMatrix:
    path.setDataArrayName(value);
    break;
  case DataArrayPath::DataType::DataArray:
    throw std::domain_error("Linked path is complete.  Cannot create a child for an IDataArray.");
    break;
  }
  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PartialDataPathCreationFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonObject obj = jsonValue.toObject();
    QString name = jsonValue.toString();
    m_SetterCallback(name);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PartialDataPathCreationFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    QString name = m_GetterCallback();
    json[getPropertyName()] = name;
  }
}

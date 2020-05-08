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
#include "CopyObject.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1,
  AttributeMatrixID,
  DataArrayID
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyObject::CopyObject()
: m_ObjectToCopy(0)
, m_DataContainerToCopy("")
, m_AttributeMatrixToCopy("", "", "")
, m_AttributeArrayToCopy("", "", "")
, m_CopiedObjectName("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyObject::~CopyObject() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyObject::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Object Type to Copy");
    parameter->setPropertyName("ObjectToCopy");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(CopyObject, this, ObjectToCopy));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(CopyObject, this, ObjectToCopy));
    QVector<QString> choices;
    choices.push_back("Data Container");
    choices.push_back("Attribute Matrix");
    choices.push_back("Attribute Array");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "DataContainerToCopy"
                << "AttributeMatrixToCopy"
                << "AttributeArrayToCopy";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container to Copy", DataContainerToCopy, FilterParameter::RequiredArray, CopyObject, req, 0));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Attribute Matrix to Copy", AttributeMatrixToCopy, FilterParameter::RequiredArray, CopyObject, req, 1));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to Copy", AttributeArrayToCopy, FilterParameter::RequiredArray, CopyObject, req, 2));
  }
  parameters.push_back(SIMPL_NEW_STRING_FP("Copied Object Name", CopiedObjectName, FilterParameter::CreatedArray, CopyObject));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyObject::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setObjectToCopy(reader->readValue("ObjectToCopy", getObjectToCopy()));
  setDataContainerToCopy(reader->readDataArrayPath("DataContainerToCopy", getDataContainerToCopy()));
  setAttributeMatrixToCopy(reader->readDataArrayPath("AttributeMatrixToCopy", getAttributeMatrixToCopy()));
  setAttributeArrayToCopy(reader->readDataArrayPath("AttributeArrayToCopy", getAttributeArrayToCopy()));
  setCopiedObjectName(reader->readString("CopiedObjectName", getCopiedObjectName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyObject::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyObject::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(getCopiedObjectName().isEmpty())
  {
    QString ss = QObject::tr("The copied object name must be set");
    setErrorCondition(-11001, ss);
  }

  switch(getObjectToCopy())
  {
  // Copy Data Container
  case 0: {
    if(getDataContainerArray()->doesDataContainerExist(getCopiedObjectName()))
    {
      QString ss = QObject::tr("A Data Container already exists with the name %1").arg(getCopiedObjectName());
      setErrorCondition(-11001, ss);
    }

    DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerToCopy());

    if(getErrorCode() < 0)
    {
      return;
    }

    DataContainer::Pointer dcCopy = m->deepCopy(getInPreflight());
    dcCopy->setName(getCopiedObjectName());
    getDataContainerArray()->addOrReplaceDataContainer(dcCopy);
    RenameDataPath::AlertFilterCreatedPath(this, DataContainerID, dcCopy->getDataArrayPath());

    break;
  }
  // Copy Attribute Matrix
  case 1: {
    DataArrayPath path(getAttributeMatrixToCopy().getDataContainerName(), getCopiedObjectName(), "");
    if(getDataContainerArray()->doesAttributeMatrixExist(path))
    {
      QString ss = QObject::tr("An Attribute Matrix already exists with the name %1").arg(getCopiedObjectName());
      setErrorCondition(-11001, ss);
    }

    AttributeMatrix::Pointer attrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, getAttributeMatrixToCopy(), -301);

    if(getErrorCode() < 0)
    {
      return;
    }

    AttributeMatrix::Pointer attrMatCopy = attrMat->deepCopy(getInPreflight());
    attrMatCopy->setName(getCopiedObjectName());
    getDataContainerArray()->getDataContainer(getAttributeMatrixToCopy().getDataContainerName())->addOrReplaceAttributeMatrix(attrMatCopy);
    RenameDataPath::AlertFilterCreatedPath(this, AttributeMatrixID, attrMatCopy->getDataArrayPath());

    break;
  }
  // Copy Attribute Array
  case 2: {
    DataArrayPath path(getAttributeArrayToCopy().getDataContainerName(), getAttributeArrayToCopy().getAttributeMatrixName(), getCopiedObjectName());
    if(getDataContainerArray()->doesAttributeArrayExist(path))
    {
      QString ss = QObject::tr("An Attribute Array already exists with the name %1").arg(getCopiedObjectName());
      setErrorCondition(-11001, ss);
    }

    IDataArray::Pointer array = getDataContainerArray()->getPrereqIDataArrayFromPath(this, getAttributeArrayToCopy());

    if(getErrorCode() < 0)
    {
      return;
    }

    IDataArray::Pointer arrayCopy = array->deepCopy(getInPreflight());
    arrayCopy->setName(getCopiedObjectName());
    getDataContainerArray()->getDataContainer(getAttributeArrayToCopy().getDataContainerName())->getAttributeMatrix(getAttributeArrayToCopy().getAttributeMatrixName())->insertOrAssign(arrayCopy);
    RenameDataPath::AlertFilterCreatedPath(this, DataArrayID, arrayCopy->getDataArrayPath());

    break;
  }
  default: {
    break;
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyObject::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CopyObject::newFilterInstance(bool copyFilterParameters) const
{
  CopyObject::Pointer filter = CopyObject::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CopyObject::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CopyObject::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CopyObject::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CopyObject::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid CopyObject::getUuid() const
{
  return QUuid("{088ef69b-ca98-51a9-97ac-369862015d71}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CopyObject::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CopyObject::getHumanLabel() const
{
  return "Copy Object";
}

// -----------------------------------------------------------------------------
CopyObject::Pointer CopyObject::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<CopyObject> CopyObject::New()
{
  struct make_shared_enabler : public CopyObject
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString CopyObject::getNameOfClass() const
{
  return QString("CopyObject");
}

// -----------------------------------------------------------------------------
QString CopyObject::ClassName()
{
  return QString("CopyObject");
}

// -----------------------------------------------------------------------------
void CopyObject::setObjectToCopy(int value)
{
  m_ObjectToCopy = value;
}

// -----------------------------------------------------------------------------
int CopyObject::getObjectToCopy() const
{
  return m_ObjectToCopy;
}

// -----------------------------------------------------------------------------
void CopyObject::setDataContainerToCopy(const DataArrayPath& value)
{
  m_DataContainerToCopy = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CopyObject::getDataContainerToCopy() const
{
  return m_DataContainerToCopy;
}

// -----------------------------------------------------------------------------
void CopyObject::setAttributeMatrixToCopy(const DataArrayPath& value)
{
  m_AttributeMatrixToCopy = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CopyObject::getAttributeMatrixToCopy() const
{
  return m_AttributeMatrixToCopy;
}

// -----------------------------------------------------------------------------
void CopyObject::setAttributeArrayToCopy(const DataArrayPath& value)
{
  m_AttributeArrayToCopy = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CopyObject::getAttributeArrayToCopy() const
{
  return m_AttributeArrayToCopy;
}

// -----------------------------------------------------------------------------
void CopyObject::setCopiedObjectName(const QString& value)
{
  m_CopiedObjectName = value;
}

// -----------------------------------------------------------------------------
QString CopyObject::getCopiedObjectName() const
{
  return m_CopiedObjectName;
}

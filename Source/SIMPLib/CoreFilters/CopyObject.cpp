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

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

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
  FilterParameterVector parameters;
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
  setDataContainerToCopy(reader->readString("DataContainerToCopy", getDataContainerToCopy()));
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
  setErrorCondition(0);
  setWarningCondition(0);

  if(getCopiedObjectName().isEmpty())
  {
    setErrorCondition(-11001);
    QString ss = QObject::tr("The copied object name must be set");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  switch(getObjectToCopy())
  {
  // Copy Data Container
  case 0:
  {
    if(getDataContainerArray()->doesDataContainerExist(getCopiedObjectName()))
    {
      setErrorCondition(-11001);
      QString ss = QObject::tr("A Data Container already exists with the name %1").arg(getCopiedObjectName());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerToCopy());

    if(getErrorCondition() < 0)
    {
      return;
    }

    DataContainer::Pointer dcCopy = m->deepCopy(getInPreflight());
    dcCopy->setName(getCopiedObjectName());
    getDataContainerArray()->addDataContainer(dcCopy);

    break;
  }
  // Copy Attribute Matrix
  case 1:
  {
    DataArrayPath path(getAttributeMatrixToCopy().getDataContainerName(), getCopiedObjectName(), "");
    if(getDataContainerArray()->doesAttributeMatrixExist(path))
    {
      setErrorCondition(-11001);
      QString ss = QObject::tr("An Attribute Matrix already exists with the name %1").arg(getCopiedObjectName());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    AttributeMatrix::Pointer attrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getAttributeMatrixToCopy(), -301);

    if(getErrorCondition() < 0)
    {
      return;
    }

    AttributeMatrix::Pointer attrMatCopy = attrMat->deepCopy(getInPreflight());
    attrMatCopy->setName(getCopiedObjectName());
    getDataContainerArray()->getDataContainer(getAttributeMatrixToCopy().getDataContainerName())->addAttributeMatrix(getCopiedObjectName(), attrMatCopy);

    break;
  }
  // Copy Attribute Array
  case 2:
  {
    DataArrayPath path(getAttributeArrayToCopy().getDataContainerName(), getAttributeArrayToCopy().getAttributeMatrixName(), getCopiedObjectName());
    if(getDataContainerArray()->doesAttributeArrayExist(path))
    {
      setErrorCondition(-11001);
      QString ss = QObject::tr("An Attribute Array already exists with the name %1").arg(getCopiedObjectName());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    IDataArray::Pointer array = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getAttributeArrayToCopy());

    if(getErrorCondition() < 0)
    {
      return;
    }

    IDataArray::Pointer arrayCopy = array->deepCopy(getInPreflight());
    arrayCopy->setName(getCopiedObjectName());
    getDataContainerArray()
        ->getDataContainer(getAttributeArrayToCopy().getDataContainerName())
        ->getAttributeMatrix(getAttributeArrayToCopy().getAttributeMatrixName())
        ->addAttributeArray(getCopiedObjectName(), arrayCopy);

    break;
  }
  default:
  {
    break;
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyObject::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyObject::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
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
const QString CopyObject::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyObject::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyObject::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyObject::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CopyObject::getUuid()
{
  return QUuid("{088ef69b-ca98-51a9-97ac-369862015d71}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyObject::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyObject::getHumanLabel() const
{
  return "Copy Object";
}

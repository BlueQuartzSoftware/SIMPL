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
#include "MoveData.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

namespace
{
const int32_t k_MoveAttributeMatrix = 0;
const int32_t k_MoveDataArray = 1;
} // namespace

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveData::MoveData()
: m_WhatToMove(k_MoveAttributeMatrix)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveData::~MoveData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveData::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  QStringList linkedProps;
  linkedProps << "DataContainerDestination"
              << "AttributeMatrixSource"
              << "AttributeMatrixDestination"
              << "DataArraySource";
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Object to Move");
    parameter->setPropertyName("WhatToMove");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(MoveData, this, WhatToMove));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(MoveData, this, WhatToMove));

    parameter->setDefaultValue(getWhatToMove()); // Just set the first index
    QVector<QString> choices;
    choices.push_back("Attribute Matrix");
    choices.push_back("Attribute Array");
    parameter->setChoices(choices);
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Attribute Matrix Source", AttributeMatrixSource, FilterParameter::RequiredArray, MoveData, req, 0));
  }
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container Destination", DataContainerDestination, FilterParameter::RequiredArray, MoveData, req, 0));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array Source", DataArraySource, FilterParameter::RequiredArray, MoveData, req, 1));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Attribute Matrix Destination", AttributeMatrixDestination, FilterParameter::RequiredArray, MoveData, req, 1));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setWhatToMove(reader->readValue("WhatToMove", getWhatToMove()));
  setDataContainerDestination(reader->readDataArrayPath("DataContainerDestination", getDataContainerDestination()));
  setAttributeMatrixSource(reader->readDataArrayPath("AttributeMatrixSource", getAttributeMatrixSource()));
  setAttributeMatrixDestination(reader->readDataArrayPath("AttributeMatrixDestination", getAttributeMatrixDestination()));
  setDataArraySource(reader->readDataArrayPath("DataArraySource", getDataArraySource()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveData::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath amSrcPath = getAttributeMatrixSource();
  DataArrayPath amDestPath = getAttributeMatrixDestination();
  DataArrayPath daSrcPath = getDataArraySource();

  if(getWhatToMove() == k_MoveAttributeMatrix)
  {
    DataContainer::Pointer amDestDataContainer = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerDestination());
    DataContainer::Pointer amSrcDataContainer = getDataContainerArray()->getPrereqDataContainer(this, amSrcPath.getDataContainerName());
    AttributeMatrix::Pointer amSrcAttributeMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, amSrcPath, -301);

    if(getErrorCode() < 0)
    {
      return;
    }

    if(amSrcDataContainer->getName() == amDestDataContainer->getName())
    {
      QString ss = QObject::tr("The source and destination Data Container are the same.  Is this what you meant to do?");
      setWarningCondition(-11018, ss);
      return;
    }

    amDestDataContainer->addOrReplaceAttributeMatrix(amSrcAttributeMatrix);
    // amSrcDataContainer->removeAttributeMatrix(amSrcAttributeMatrix->getName());
    addPathRename(amSrcPath, amSrcAttributeMatrix->getDataArrayPath());
  }
  else if(getWhatToMove() == k_MoveDataArray)
  {
    AttributeMatrix::Pointer daSrcAttributeMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, daSrcPath, -301);
    AttributeMatrix::Pointer daDestAttributeMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, amDestPath, -301);
    IDataArray::Pointer daSrcDataArray = getDataContainerArray()->getPrereqIDataArrayFromPath(this, daSrcPath);

    if(getErrorCode() < 0)
    {
      return;
    }

    if(daDestAttributeMatrix->getNumberOfTuples() != daSrcDataArray->getNumberOfTuples())
    {
      QString ss = QObject::tr("The number of tuples of source Attribute Array (%1) and destination Attribute Matrix (%2) do not match")
                       .arg(daSrcDataArray->getNumberOfTuples())
                       .arg(daDestAttributeMatrix->getNumberOfTuples());
      setErrorCondition(-11019, ss);
      return;
    }
    if(amSrcPath == amDestPath)
    {
      QString ss = QObject::tr("The source and destination Attribute Matrix are the same.  Is this what you meant to do?");
      setWarningCondition(-11020, ss);
      return;
    }

    daDestAttributeMatrix->insertOrAssign(daSrcDataArray);
    // daSrcAttributeMatrix->removeAttributeArray(daSrcPath.getDataArrayName());
    addPathRename(daSrcPath, daSrcDataArray->getDataArrayPath());
  }
  else
  {
    QString ss = QObject::tr("Neither an Attribute Matrix nor an Attribute Array was selected to be moved");
    setErrorCondition(-11021, ss);
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveData::execute()
{
  clearErrorCode();
  clearWarningCode();
  // Simply running the preflight will do what we need it to.
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MoveData::newFilterInstance(bool copyFilterParameters) const
{
  MoveData::Pointer filter = MoveData::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveData::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveData::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveData::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid MoveData::getUuid() const
{
  return QUuid("{fe2cbe09-8ae1-5bea-9397-fd5741091fdb}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveData::getHumanLabel() const
{
  return "Move Data";
}

// -----------------------------------------------------------------------------
MoveData::Pointer MoveData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<MoveData> MoveData::New()
{
  struct make_shared_enabler : public MoveData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString MoveData::getNameOfClass() const
{
  return QString("MoveData");
}

// -----------------------------------------------------------------------------
QString MoveData::ClassName()
{
  return QString("MoveData");
}

// -----------------------------------------------------------------------------
void MoveData::setWhatToMove(int value)
{
  m_WhatToMove = value;
}

// -----------------------------------------------------------------------------
int MoveData::getWhatToMove() const
{
  return m_WhatToMove;
}

// -----------------------------------------------------------------------------
void MoveData::setDataContainerDestination(const DataArrayPath& value)
{
  m_DataContainerDestination = value;
}

// -----------------------------------------------------------------------------
DataArrayPath MoveData::getDataContainerDestination() const
{
  return m_DataContainerDestination;
}

// -----------------------------------------------------------------------------
void MoveData::setAttributeMatrixSource(const DataArrayPath& value)
{
  m_AttributeMatrixSource = value;
}

// -----------------------------------------------------------------------------
DataArrayPath MoveData::getAttributeMatrixSource() const
{
  return m_AttributeMatrixSource;
}

// -----------------------------------------------------------------------------
void MoveData::setAttributeMatrixDestination(const DataArrayPath& value)
{
  m_AttributeMatrixDestination = value;
}

// -----------------------------------------------------------------------------
DataArrayPath MoveData::getAttributeMatrixDestination() const
{
  return m_AttributeMatrixDestination;
}

// -----------------------------------------------------------------------------
void MoveData::setDataArraySource(const DataArrayPath& value)
{
  m_DataArraySource = value;
}

// -----------------------------------------------------------------------------
DataArrayPath MoveData::getDataArraySource() const
{
  return m_DataArraySource;
}

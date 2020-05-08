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
#include "MoveMultiData.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiAttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

namespace
{
const int32_t k_MoveAttributeMatrix = 0;
const int32_t k_MoveMultiDataArray = 1;
} // namespace

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveMultiData::MoveMultiData()
: m_WhatToMove(k_MoveAttributeMatrix)
, m_DataContainerDestination("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveMultiData::~MoveMultiData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveMultiData::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  QStringList linkedProps;
  linkedProps << "DataContainerDestination"
              << "AttributeMatrixSources"
              << "AttributeMatrixDestination"
              << "DataArraySources";
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Object to Move");
    parameter->setPropertyName("WhatToMove");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(MoveMultiData, this, WhatToMove));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(MoveMultiData, this, WhatToMove));

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
    MultiAttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_MAM_SELECTION_FP("Attribute Matrix Sources", AttributeMatrixSources, FilterParameter::RequiredArray, MoveMultiData, req, 0));
  }
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container Destination", DataContainerDestination, FilterParameter::RequiredArray, MoveMultiData, req, 0));
  }
  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Attribute Array Sources", DataArraySources, FilterParameter::RequiredArray, MoveMultiData, req, 1));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Attribute Matrix Destination", AttributeMatrixDestination, FilterParameter::RequiredArray, MoveMultiData, req, 1));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveMultiData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setWhatToMove(reader->readValue("WhatToMove", getWhatToMove()));
  setDataContainerDestination(reader->readDataArrayPath("DataContainerDestination", getDataContainerDestination()));
  setAttributeMatrixSources(reader->readDataArrayPathVector("AttributeMatrixSource", getAttributeMatrixSources()));
  setAttributeMatrixDestination(reader->readDataArrayPath("AttributeMatrixDestination", getAttributeMatrixDestination()));
  setDataArraySources(reader->readDataArrayPathVector("DataArraySources", getDataArraySources()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveMultiData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveMultiData::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  QVector<DataArrayPath> amSrcPaths = getAttributeMatrixSources();
  DataArrayPath amDestPath = getAttributeMatrixDestination();
  QVector<DataArrayPath> daSrcPaths = getDataArraySources();

  if(getWhatToMove() == k_MoveAttributeMatrix)
  {
    DataContainer::Pointer amDestDataContainer = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerDestination());

    // Repeat for each AttributeMatrix
    for(int i = 0; i < amSrcPaths.size(); i++)
    {
      DataContainer::Pointer amSrcDataContainer = getDataContainerArray()->getPrereqDataContainer(this, amSrcPaths[i].getDataContainerName());
      AttributeMatrix::Pointer amSrcAttributeMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, amSrcPaths[i], -301);

      if(getErrorCode() < 0)
      {
        return;
      }

      // Source and Destination match
      if(amSrcDataContainer->getName() == amDestDataContainer->getName())
      {
        QString ss = QObject::tr("The source and destination Data Container are the same.  Is this what you meant to do?");
        setWarningCondition(-11018, ss);
        return;
      }

      // Move Attribute Matrix
      amDestDataContainer->addOrReplaceAttributeMatrix(amSrcAttributeMatrix);
      // amSrcDataContainer->removeAttributeMatrix(amSrcAttributeMatrix->getName());
      addPathRename(amSrcPaths[i], amSrcAttributeMatrix->getDataArrayPath());
    }
  }
  else if(getWhatToMove() == k_MoveMultiDataArray)
  {
    AttributeMatrix::Pointer daDestAttributeMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, amDestPath, -301);

    // Repeat for each DataArray
    for(int i = 0; i < daSrcPaths.size(); i++)
    {
      AttributeMatrix::Pointer daSrcAttributeMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, daSrcPaths[i], -301);
      IDataArray::Pointer daSrcDataArray = getDataContainerArray()->getPrereqIDataArrayFromPath(this, daSrcPaths[i]);

      if(getErrorCode() < 0)
      {
        return;
      }

      // Number of tuples does not match
      if(daDestAttributeMatrix->getNumberOfTuples() != daSrcDataArray->getNumberOfTuples())
      {
        QString ss = QObject::tr("The number of tuples of source Attribute Array (%1) and destination Attribute Matrix (%2) do not match")
                         .arg(daSrcDataArray->getNumberOfTuples())
                         .arg(daDestAttributeMatrix->getNumberOfTuples());
        setErrorCondition(-11019, ss);
        return;
      }
      // Source and Destination match
      if(daSrcPaths[i].hasSameAttributeMatrix(amDestPath))
      {
        QString ss = QObject::tr("The source and destination Attribute Matrix are the same.  Is this what you meant to do?");
        setWarningCondition(-11020, ss);
        return;
      }

      // Move Array
      daDestAttributeMatrix->insertOrAssign(daSrcDataArray);
      // daSrcAttributeMatrix->removeAttributeArray(daSrcPaths[i].getDataArrayName());
      addPathRename(daSrcPaths[i], daSrcDataArray->getDataArrayPath());
    }
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
void MoveMultiData::execute()
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
AbstractFilter::Pointer MoveMultiData::newFilterInstance(bool copyFilterParameters) const
{
  MoveMultiData::Pointer filter = MoveMultiData::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveMultiData::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveMultiData::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveMultiData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveMultiData::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid MoveMultiData::getUuid() const
{
  return QUuid("{e3702900-a6c1-59e1-9180-b57557a7b193}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveMultiData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MoveMultiData::getHumanLabel() const
{
  return "Move Multi Data";
}

// -----------------------------------------------------------------------------
MoveMultiData::Pointer MoveMultiData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<MoveMultiData> MoveMultiData::New()
{
  struct make_shared_enabler : public MoveMultiData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString MoveMultiData::getNameOfClass() const
{
  return QString("MoveMultiData");
}

// -----------------------------------------------------------------------------
QString MoveMultiData::ClassName()
{
  return QString("MoveMultiData");
}

// -----------------------------------------------------------------------------
void MoveMultiData::setWhatToMove(int value)
{
  m_WhatToMove = value;
}

// -----------------------------------------------------------------------------
int MoveMultiData::getWhatToMove() const
{
  return m_WhatToMove;
}

// -----------------------------------------------------------------------------
void MoveMultiData::setDataContainerDestination(const DataArrayPath& value)
{
  m_DataContainerDestination = value;
}

// -----------------------------------------------------------------------------
DataArrayPath MoveMultiData::getDataContainerDestination() const
{
  return m_DataContainerDestination;
}

// -----------------------------------------------------------------------------
void MoveMultiData::setAttributeMatrixSources(const QVector<DataArrayPath>& value)
{
  m_AttributeMatrixSources = value;
}

// -----------------------------------------------------------------------------
QVector<DataArrayPath> MoveMultiData::getAttributeMatrixSources() const
{
  return m_AttributeMatrixSources;
}

// -----------------------------------------------------------------------------
void MoveMultiData::setAttributeMatrixDestination(const DataArrayPath& value)
{
  m_AttributeMatrixDestination = value;
}

// -----------------------------------------------------------------------------
DataArrayPath MoveMultiData::getAttributeMatrixDestination() const
{
  return m_AttributeMatrixDestination;
}

// -----------------------------------------------------------------------------
void MoveMultiData::setDataArraySources(const QVector<DataArrayPath>& value)
{
  m_DataArraySources = value;
}

// -----------------------------------------------------------------------------
QVector<DataArrayPath> MoveMultiData::getDataArraySources() const
{
  return m_DataArraySources;
}

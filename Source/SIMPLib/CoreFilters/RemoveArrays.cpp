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

#include "RemoveArrays.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerArrayProxyFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveArrays::RemoveArrays()
: AbstractFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveArrays::~RemoveArrays() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    DataContainerArrayProxyFilterParameter::Pointer parameter = DataContainerArrayProxyFilterParameter::New();
    parameter->setHumanLabel("Objects to Delete");
    parameter->setPropertyName("DataArraysToRemove");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(RemoveArrays, this, DataArraysToRemove));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(RemoveArrays, this, DataArraysToRemove));

    parameter->setDefaultFlagValue(Qt::Unchecked);
    parameter->setCategory(FilterParameter::RequiredArray);
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  DataContainerArrayProxy proxy = reader->readDataContainerArrayProxy("DataArraysToRemove", getDataArraysToRemove());
  setDataArraysToRemove(proxy);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  markSelectionsForDeletion(getDataContainerArray().get(), Qt::Checked);

  // Remove all the arrays, but only if they are CHECKED. This is opposite of the default
  removeSelectionsFromDataContainerArray(getDataContainerArray().get(), Qt::Checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::markSelectionsForDeletion(DataContainerArray* dca, Qt::CheckState state)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::removeSelectionsFromDataContainerArray(DataContainerArray* dca, Qt::CheckState state)
{
  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = m_DataArraysToRemove.dataContainers.values();
  QMutableListIterator<DataContainerProxy> containerIter(containers);
  QStringList dcList;
  while(containerIter.hasNext())
  {
    DataContainerProxy dcProxy = containerIter.next();
    dcList.push_back(dcProxy.name);
    DataContainer::Pointer dcItem = dca->getPrereqDataContainer<AbstractFilter>(this, dcProxy.name);
    if(getErrorCondition() < 0 || dcItem.get() == nullptr)
    {
      continue;
    }

    // Check to see if the DataContainer is checked, if it is checked then we remove the entire DataContainer from
    // the DataContainerArray
    if(dcProxy.flag == state)
    {
      dca->removeDataContainer(dcProxy.name); // Remove it out
      continue;                               // Continue to the next DataContainer
    }
    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.attributeMatricies;
    QMutableMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext())
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      AttributeMatrix::Pointer amItem = dcItem->getAttributeMatrix(amName);
      // assert(amItem.get() != nullptr);
      if(amItem.get() == nullptr)
      {
        setErrorCondition(-11008);
        QString ss = QObject::tr("The AttributeMatrix '%1' could not be removed because it was not found in DataContainer '%2'").arg(amName).arg(dcProxy.name);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        continue;
      }
      AttributeMatrixProxy attrProxy = attrMatsIter.value();
      // Check to see if this AttributeMatrix is checked, if not then remove it from the DataContainer and go to the next loop
      if(attrProxy.flag == state)
      {
        dcItem->removeAttributeMatrix(amName);
        continue;
      }
      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QMap<QString, DataArrayProxy>& dataArrays = attrProxy.dataArrays;
      QMutableMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
      while(dataArraysIter.hasNext())
      {
        dataArraysIter.next();
        QString daName = dataArraysIter.key();
        IDataArray::Pointer daItem = amItem->getAttributeArray(daName);
        if(daItem.get() == nullptr)
        {
          setWarningCondition(-11009);
          QString ss = QObject::tr("%1/%2/%3 was not found. This could be due to another filter removing the array.").arg(dcProxy.name).arg(amName).arg(daName);
          notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
          continue;
        }
        DataArrayProxy daProxy = dataArraysIter.value();
        // Check to see if the user selected this item
        if(daProxy.flag == state)
        {
          amItem->removeAttributeArray(daName);
          continue;
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  // Simply running the preflight will do what we need it to.
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
AbstractFilter::Pointer RemoveArrays::newFilterInstance(bool copyFilterParameters) const
{
  RemoveArrays::Pointer filter = RemoveArrays::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveArrays::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveArrays::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveArrays::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveArrays::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid RemoveArrays::getUuid()
{
  return QUuid("{7b1c8f46-90dd-584a-b3ba-34e16958a7d0}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveArrays::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveArrays::getHumanLabel() const
{
  return "Delete Data";
}

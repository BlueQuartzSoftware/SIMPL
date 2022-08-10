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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "MultiThresholdObjects.h"

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ComparisonSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/ScalarTypeFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Filtering/ThresholdFilterHelper.h"

enum createdPathID : RenameDataPath::DataID_t
{
  ThresholdArrayID = 1
};

namespace Detail
{
template <typename T>
void updateDestinationArray(const BoolArrayType& thresholdArray, IDataArray::Pointer iDestinationArray)
{
  typename DataArray<T>::Pointer destinationArray = std::dynamic_pointer_cast<DataArray<T>>(iDestinationArray);
  size_t thresholdSize = thresholdArray.getNumberOfTuples();
  T* dest = destinationArray->getPointer(0);

  for(int64_t p = 0; p < thresholdSize; p++)
  {
    if constexpr(std::is_same<T, bool>::value)
    {
      // Boolean case
      if(!dest[p] || !thresholdArray[p])
      {
        dest[p] = false;
      }
    }
    else if(dest[p] == static_cast<T>(0) || !thresholdArray[p])
    {
      // Non-boolean case
      dest[p] = static_cast<T>(0);
    }
  }
}
} // namespace Detail

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiThresholdObjects::MultiThresholdObjects() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiThresholdObjects::~MultiThresholdObjects() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    ComparisonSelectionFilterParameter::Pointer parameter = ComparisonSelectionFilterParameter::New();
    parameter->setHumanLabel("Select Arrays to Threshold");
    parameter->setPropertyName("SelectedThresholds");

    parameter->setShowOperators(true);
    parameter->setCategory(FilterParameter::Category::Parameter);
    parameter->setSetterCallback(SIMPL_BIND_SETTER(MultiThresholdObjects, this, SelectedThresholds));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(MultiThresholdObjects, this, SelectedThresholds));
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_SCALARTYPE_FP("Output Scalar Type", ScalarType, FilterParameter::Category::Parameter, MultiThresholdObjects));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output Attribute Array", DestinationArrayName, FilterParameter::Category::CreatedArray, MultiThresholdObjects));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDestinationArrayName(reader->readString("DestinationArrayName", getDestinationArrayName()));
  setSelectedThresholds(reader->readComparisonInputs("SelectedThresholds", getSelectedThresholds()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(m_SelectedThresholds.size() == 0)
  {
    setErrorCondition(-12000, "You must add at least 1 threshold value.");
  }
  else
  {
    int32_t count = m_SelectedThresholds.size();
    QSet<QString> dcSet;
    QSet<QString> amSet;

    // Loop through each selected threshold item which will have the complete path and check that path
    for(int32_t i = 0; i < count; i++)
    {
      ComparisonInput_t comp = m_SelectedThresholds[i];
      dcSet.insert(comp.dataContainerName);
      amSet.insert(comp.attributeMatrixName);
    }

    // Enforce that right now all the arrays MUST come from the same data container and attribute matrix
    if(dcSet.size() != 1)
    {
      QString ss = QObject::tr("Threshold selections must come from the same DataContainer. %1 were selected").arg(dcSet.size());
      setErrorCondition(-13090, ss);
    }
    if(amSet.size() != 1)
    {
      QString ss = QObject::tr("Threshold selections must come from the same AttributeMatrix. %1 were selected").arg(amSet.size());
      setErrorCondition(-13091, ss);
    }

    ComparisonInput_t comp = m_SelectedThresholds[0];
    std::vector<size_t> cDims(1, 1);
    DataArrayPath tempPath(comp.dataContainerName, comp.attributeMatrixName, getDestinationArrayName());

    m_DestinationPtr = TemplateHelpers::CreateNonPrereqArrayFromTypeEnum()(this, tempPath, cDims, static_cast<int>(getScalarType()), 1, ThresholdArrayID);
    if(getErrorCode() < 0)
    {
      return;
    }

    // Do not allow non-scalar arrays
    for(size_t i = 0; i < m_SelectedThresholds.size(); ++i)
    {
      ComparisonInput_t comp = m_SelectedThresholds[i];
      tempPath.update(comp.dataContainerName, comp.attributeMatrixName, comp.attributeArrayName);
      IDataArray::Pointer inputData = getDataContainerArray()->getPrereqIDataArrayFromPath(this, tempPath);
      if(getErrorCode() >= 0)
      {
        cDims = inputData->getComponentDimensions();
        int32_t numComp = static_cast<int32_t>(cDims[0]);
        for(int32_t d = 1; d < cDims.size(); d++)
        {
          numComp *= cDims[d];
        }
        if(numComp > 1)
        {
          QString ss = QObject::tr("Selected array '%1' is not a scalar array").arg(m_SelectedThresholds[i].attributeArrayName);
          setErrorCondition(-11003, ss);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // Get the first comparison object
  ComparisonInput_t& comp_0 = m_SelectedThresholds[0];
  // Get the names of the Data Container and AttributeMatrix for later
  QString dcName = comp_0.dataContainerName;
  QString amName = comp_0.attributeMatrixName;

  DataContainerArray::Pointer dca = getDataContainerArray();
  DataContainer::Pointer m = dca->getDataContainer(dcName);
  AttributeMatrix::Pointer am = m->getAttributeMatrix(amName);

  // Get the total number of tuples, create and initialize an array to use for these results
  int64_t totalTuples = static_cast<int64_t>(am->getNumberOfTuples());

  // At least one threshold value is required
  if(m_SelectedThresholds.size() == 0)
  {
    QString ss = QObject::tr("Error Executing threshold filter. There are no specified values to threshold against");
    setErrorCondition(-13001, ss);
    return;
  }

  // Loop on the Comparison objects updating our final result array as we go
  for(int32_t i = 0; i < m_SelectedThresholds.size(); ++i)
  {
    // Create and initialize an array to use for these results
    BoolArrayType::Pointer thresholdArrayPtr = BoolArrayType::CreateArray(totalTuples, std::string("_INTERNAL_USE_ONLY_TEMP"), true);
    // Initialize with False
    thresholdArrayPtr->initializeWithZeros();

    ComparisonInput_t& compRef = m_SelectedThresholds[i];

    ThresholdFilterHelper filter(static_cast<SIMPL::Comparison::Enumeration>(compRef.compOperator), compRef.compValue, thresholdArrayPtr.get());

    int32_t err = filter.execute(m->getAttributeMatrix(amName)->getAttributeArray(compRef.attributeArrayName), thresholdArrayPtr.get());
    if(err < 0)
    {
      DataArrayPath tempPath(compRef.dataContainerName, compRef.attributeMatrixName, compRef.attributeArrayName);
      QString ss = QObject::tr("Error Executing threshold filter on array. The path is %1").arg(tempPath.serialize());
      setErrorCondition(-13002, ss);
      return;
    }

    switch(m_ScalarType)
    {
    case SIMPL::ScalarTypes::Type::Int8:
      Detail::updateDestinationArray<int8_t>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    case SIMPL::ScalarTypes::Type::Int16:
      Detail::updateDestinationArray<int16_t>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    case SIMPL::ScalarTypes::Type::Int32:
      Detail::updateDestinationArray<int32_t>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    case SIMPL::ScalarTypes::Type::Int64:
      Detail::updateDestinationArray<int64_t>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    case SIMPL::ScalarTypes::Type::UInt8:
      Detail::updateDestinationArray<uint8_t>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    case SIMPL::ScalarTypes::Type::UInt16:
      Detail::updateDestinationArray<uint16_t>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    case SIMPL::ScalarTypes::Type::UInt32:
      Detail::updateDestinationArray<uint32_t>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    case SIMPL::ScalarTypes::Type::UInt64:
      Detail::updateDestinationArray<uint64_t>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    case SIMPL::ScalarTypes::Type::Float:
      Detail::updateDestinationArray<float>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    case SIMPL::ScalarTypes::Type::Double:
      Detail::updateDestinationArray<double>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    case SIMPL::ScalarTypes::Type::Bool:
      Detail::updateDestinationArray<bool>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    case SIMPL::ScalarTypes::Type::SizeT:
      Detail::updateDestinationArray<size_t>(*thresholdArrayPtr, m_DestinationPtr.lock());
      break;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MultiThresholdObjects::newFilterInstance(bool copyFilterParameters) const
{
  MultiThresholdObjects::Pointer filter = MultiThresholdObjects::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiThresholdObjects::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiThresholdObjects::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiThresholdObjects::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiThresholdObjects::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid MultiThresholdObjects::getUuid() const
{
  return QUuid("{014b7300-cf36-5ede-a751-5faf9b119dae}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiThresholdObjects::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ThresholdFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiThresholdObjects::getHumanLabel() const
{
  return "Threshold Objects";
}

// -----------------------------------------------------------------------------
MultiThresholdObjects::Pointer MultiThresholdObjects::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<MultiThresholdObjects> MultiThresholdObjects::New()
{
  struct make_shared_enabler : public MultiThresholdObjects
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString MultiThresholdObjects::getNameOfClass() const
{
  return QString("MultiThresholdObjects");
}

// -----------------------------------------------------------------------------
QString MultiThresholdObjects::ClassName()
{
  return QString("MultiThresholdObjects");
}

// -----------------------------------------------------------------------------
void MultiThresholdObjects::setDestinationArrayName(const QString& value)
{
  m_DestinationArrayName = value;
}

// -----------------------------------------------------------------------------
QString MultiThresholdObjects::getDestinationArrayName() const
{
  return m_DestinationArrayName;
}

// -----------------------------------------------------------------------------
void MultiThresholdObjects::setSelectedThresholds(const ComparisonInputs& value)
{
  m_SelectedThresholds = value;
}

// -----------------------------------------------------------------------------
ComparisonInputs MultiThresholdObjects::getSelectedThresholds() const
{
  return m_SelectedThresholds;
}

// -----------------------------------------------------------------------------
void MultiThresholdObjects::setScalarType(SIMPL::ScalarTypes::Type value)
{
  m_ScalarType = value;
}

// -----------------------------------------------------------------------------
SIMPL::ScalarTypes::Type MultiThresholdObjects::getScalarType() const
{
  return m_ScalarType;
}

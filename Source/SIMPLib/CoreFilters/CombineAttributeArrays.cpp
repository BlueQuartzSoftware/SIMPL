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

#include "CombineAttributeArrays.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

/**
 * @brief The CombineAttributeArraysTemplatePrivate class is a templated private implementation that deals with
 * combining the various input arrays into one contiguous array
 */
template <typename DataType> class CombineAttributeArraysTemplatePrivate
{
public:
  typedef DataArray<DataType> DataArrayType;

  CombineAttributeArraysTemplatePrivate() = default;

  virtual ~CombineAttributeArraysTemplatePrivate() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool operator()(QVector<IDataArray::WeakPointer> p)
  {
    return (std::dynamic_pointer_cast<DataArrayType>(p[0].lock()).get() != nullptr);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Execute(CombineAttributeArrays* filter, QVector<IDataArray::WeakPointer> inputIDataArrays, IDataArray::Pointer outputIDataArray)
  {
    typename DataArrayType::Pointer outputDataPtr = std::dynamic_pointer_cast<DataArrayType>(outputIDataArray);

    std::vector<DataType*> inputArrays;
    int32_t numArrays = inputIDataArrays.size();

    for(int32_t i = 0; i < numArrays; i++)
    {
      DataType* inputData = (std::dynamic_pointer_cast<DataArrayType>(inputIDataArrays.at(i).lock()))->getPointer(0);
      inputArrays.push_back(inputData);
    }
    DataType* outputData = static_cast<DataType*>(outputDataPtr->getPointer(0));

    size_t numTuples = inputIDataArrays[0].lock()->getNumberOfTuples();
    int32_t stackedDims = outputIDataArray.get()->getNumberOfComponents();
    size_t arrayOffset = 0;
    int32_t numDims = 0;

    if(filter->getNormalizeData())
    {
      std::vector<DataType> maxVals(stackedDims, std::numeric_limits<DataType>::lowest());
      std::vector<DataType> minVals(stackedDims, std::numeric_limits<DataType>::max());

      for(int32_t i = 0; i < numArrays; i++)
      {
        numDims = inputIDataArrays[i].lock()->getNumberOfComponents();
        if(i > 0)
        {
          arrayOffset += inputIDataArrays[i - 1].lock()->getNumberOfComponents();
        }
        for(size_t j = 0; j < numTuples; j++)
        {
          for(int32_t k = 0; k < numDims; k++)
          {
            if(inputArrays[i][numDims * j + k] > maxVals[arrayOffset + k])
            {
              maxVals[arrayOffset + k] = inputArrays[i][numDims * j + k];
            }
            if(inputArrays[i][numDims * j + k] < minVals[arrayOffset + k])
            {
              minVals[arrayOffset + k] = inputArrays[i][numDims * j + k];
            }
          }
        }
      }

      arrayOffset = 0;

      for(size_t i = 0; i < numTuples; i++)
      {
        for(int32_t j = 0; j < numArrays; j++)
        {
          numDims = inputIDataArrays[j].lock()->getNumberOfComponents();
          if(j > 0)
          {
            arrayOffset += inputIDataArrays[j - 1].lock()->getNumberOfComponents();
          }
          for(int32_t k = 0; k < numDims; k++)
          {
            if(maxVals[arrayOffset + k] == minVals[arrayOffset + k])
            {
              outputData[stackedDims * i + (arrayOffset) + k] = static_cast<DataType>(0);
            }
            else
            {
              outputData[stackedDims * i + (arrayOffset) + k] = (inputArrays[j][numDims * i + k] - minVals[arrayOffset + k]) / (maxVals[arrayOffset + k] - minVals[arrayOffset + k]);
            }
          }
        }
        arrayOffset = 0;
      }
    }
    else
    {
      for(size_t i = 0; i < numTuples; i++)
      {
        for(int32_t j = 0; j < numArrays; j++)
        {
          numDims = inputIDataArrays[j].lock()->getNumberOfComponents();
          if(j > 0)
          {
            arrayOffset += inputIDataArrays[j - 1].lock()->getNumberOfComponents();
          }
          for(int32_t k = 0; k < numDims; k++)
          {
            outputData[stackedDims * i + (arrayOffset) + k] = inputArrays[j][numDims * i + k];
          }
        }
        arrayOffset = 0;
      }
    }
  }

private:
  CombineAttributeArraysTemplatePrivate(const CombineAttributeArraysTemplatePrivate&); // Copy Constructor Not Implemented
  void operator=(const CombineAttributeArraysTemplatePrivate&);                        // Move assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CombineAttributeArrays::CombineAttributeArrays()
: m_SelectedDataArrayPaths(QVector<DataArrayPath>())
, m_StackedDataArrayName(SIMPL::GeneralData::CombinedData)
, m_NormalizeData(false)
, m_MoveValues(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CombineAttributeArrays::~CombineAttributeArrays() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeArrays::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_BOOL_FP("Normalize Data", NormalizeData, FilterParameter::Parameter, CombineAttributeArrays));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Move Data", MoveValues, FilterParameter::Parameter, CombineAttributeArrays));
  {
    MultiDataArraySelectionFilterParameter::RequirementType req =
        MultiDataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Attribute Arrays to Combine", SelectedDataArrayPaths, FilterParameter::RequiredArray, CombineAttributeArrays, req));
  }
  parameters.push_back(SIMPL_NEW_STRING_FP("Combined Data", StackedDataArrayName, FilterParameter::CreatedArray, CombineAttributeArrays));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeArrays::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedDataArrayPaths(reader->readDataArrayPathVector("SelectedDataArrayPaths", getSelectedDataArrayPaths()));
  setStackedDataArrayName(reader->readString("StackedDataArrayName", getStackedDataArrayName()));
  setNormalizeData(reader->readValue("NormalizeData", getNormalizeData()));
  setMoveValues(reader->readValue("MoveValues", getMoveValues()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename DataType> void verifyArrayList(AbstractFilter* filter, QVector<IDataArray::WeakPointer> ptrArray)
{
  for(QVector<IDataArray::WeakPointer>::iterator it = ptrArray.begin(); it != ptrArray.end(); ++it)
  {
    if(!TemplateHelpers::CanDynamicCast<DataArray<DataType>>()((*it).lock()))
    {
      filter->setErrorCondition(-90000);
      filter->notifyErrorMessage(filter->getHumanLabel(), "Selected Attribute Arrays must all be of the same type", filter->getErrorCondition());
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeArrays::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeArrays::dataCheck()
{
  m_SelectedWeakPtrVector.clear();

  setErrorCondition(0);
  setWarningCondition(0);

  if(getSelectedDataArrayPaths().size() < 2)
  {
    setErrorCondition(-11001);
    QString ss = QObject::tr("At least two Attribute Array must be selected");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<DataArrayPath> paths = getSelectedDataArrayPaths();

  if(DataArrayPath::ValidateVector(paths) == false)
  {
    setErrorCondition(-11002);
    QString ss = QObject::tr("There are Attribute Arrays selected that are not contained in the same Attribute Matrix. All selected Attribute Arrays must belong to the same Attribute Matrix");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  int32_t totalComps = 0;

  for(int32_t i = 0; i < paths.count(); i++)
  {
    DataArrayPath path = paths.at(i);
    IDataArray::WeakPointer ptr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, path);
    if(nullptr != ptr.lock())
    {
      m_SelectedWeakPtrVector.push_back(ptr);
      int32_t numComps = ptr.lock()->getNumberOfComponents();
      totalComps += numComps;
    }
  }
  if(! m_SelectedWeakPtrVector.isEmpty() )
  {
    EXECUTE_FUNCTION_TEMPLATE(this, verifyArrayList, m_SelectedWeakPtrVector[0].lock(), this, m_SelectedWeakPtrVector)
  }
  if(getErrorCondition() < 0)
  {
    return;
  }

  QVector<size_t> cDims(1, totalComps);

  DataArrayPath tempPath(getSelectedDataArrayPaths()[0].getDataContainerName(), getSelectedDataArrayPaths()[0].getAttributeMatrixName(), getStackedDataArrayName());
  m_StackedDataPtr = TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, cDims, m_SelectedWeakPtrVector[0].lock());

  if(getMoveValues() && getInPreflight())
  {
    QVector<DataArrayPath> paths = getSelectedDataArrayPaths();
    for(DataArrayPath path : paths)
    {
      AttributeMatrix::Pointer attrMat = getDataContainerArray()->getAttributeMatrix(path);
      attrMat->removeAttributeArray(path.getDataArrayName());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeArrays::preflight()
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
void CombineAttributeArrays::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  EXECUTE_TEMPLATE(this, CombineAttributeArraysTemplatePrivate, m_SelectedWeakPtrVector, this, m_SelectedWeakPtrVector, m_StackedDataPtr.lock())

  if(getMoveValues())
  {
    QVector<DataArrayPath> paths = getSelectedDataArrayPaths();
    for(DataArrayPath path : paths)
    {
      AttributeMatrix::Pointer attrMat = getDataContainerArray()->getAttributeMatrix(path);
      attrMat->removeAttributeArray(path.getDataArrayName());
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CombineAttributeArrays::newFilterInstance(bool copyFilterParameters) const
{
  CombineAttributeArrays::Pointer filter = CombineAttributeArrays::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeArrays::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeArrays::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeArrays::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeArrays::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CombineAttributeArrays::getUuid()
{
  return QUuid("{a6b50fb0-eb7c-5d9b-9691-825d6a4fe772}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeArrays::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeArrays::getHumanLabel() const
{
  return "Combine Attribute Arrays";
}

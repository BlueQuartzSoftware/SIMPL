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
#include "SplitAttributeArray.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataArrays/IDataArray.h"

enum createdPathID : RenameDataPath::DataID_t
{
  SplitArrayID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SplitAttributeArray::SplitAttributeArray()
: m_InputArrayPath("", "", "")
, m_SplitArraysSuffix("Component")
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SplitAttributeArray::~SplitAttributeArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SplitAttributeArray::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  DataArraySelectionFilterParameter::RequirementType dasReq =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Any, IGeometry::Type::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Multicomponent Attribute Array", InputArrayPath, FilterParameter::RequiredArray, SplitAttributeArray, dasReq));
  parameters.push_back(SIMPL_NEW_STRING_FP("Postfix", SplitArraysSuffix, FilterParameter::Parameter, SplitAttributeArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SplitAttributeArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputArrayPath(reader->readDataArrayPath("InputArrayPath", getInputArrayPath()));
  setSplitArraysSuffix(reader->readString("SplitArraysSuffix", getSplitArraysSuffix()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SplitAttributeArray::initialize()
{
  m_SplitArraysPtrVector.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SplitAttributeArray::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();

  m_InputArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, getInputArrayPath());

  if(getErrorCode() < 0)
  {
    return;
  }

  if(m_InputArrayPtr.lock())
  {
    int32_t numComps = m_InputArrayPtr.lock()->getNumberOfComponents();
    if(numComps <= 1)
    {
      QString ss = QObject::tr("Selected Attribute Array must have more than 1 component");
      setErrorCondition(-11000, ss);
      return;
    }

    std::vector<size_t> cDims(1, 1);

    for(int32_t i = 0; i < numComps; i++)
    {
      QString arrayName = getInputArrayPath().getDataArrayName() + getSplitArraysSuffix() + QString::number(i);
      DataArrayPath path(getInputArrayPath().getDataContainerName(), getInputArrayPath().getAttributeMatrixName(), arrayName);
      IDataArray::WeakPointer ptr = TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, path, cDims, m_InputArrayPtr.lock(), SplitArrayID + i);
      if(getErrorCode() >= 0)
      {
        m_SplitArraysPtrVector.push_back(ptr.lock());
      }
      else
      {
        QString ss = QObject::tr("Unable to create an Attribute Array for component %1 in the selected multicomponent Attribute Array").arg(i);
        setErrorCondition(-11051, ss);
      }
    }

    if(numComps != m_SplitArraysPtrVector.size())
    {
      QString ss = QObject::tr("The number of created arrays %1 does not match the number of components %2").arg(m_SplitArraysPtrVector.size()).arg(numComps);
      setErrorCondition(-11001, ss);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void splitMulticomponentArray(IDataArray::Pointer inputArray, std::vector<IDataArray::Pointer>& splitArrays)
{
  typename DataArray<T>::Pointer inputPtr = std::dynamic_pointer_cast<DataArray<T>>(inputArray);
  T* iPtr = inputPtr->getPointer(0);
  std::vector<T*> downcastPtrs;

  for(auto&& ptr : splitArrays)
  {
    auto tmp = std::dynamic_pointer_cast<DataArray<T>>(ptr);
    T* tmpPtr = tmp->getPointer(0);
    downcastPtrs.push_back(tmpPtr);
  }

  size_t numTuples = inputPtr->getNumberOfTuples();
  int32_t numComps = inputPtr->getNumberOfComponents();

  for(size_t i = 0; i < numTuples; i++)
  {
    for(int32_t j = 0; j < numComps; j++)
    {
      downcastPtrs[j][i] = iPtr[numComps * i + j];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SplitAttributeArray::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  EXECUTE_FUNCTION_TEMPLATE(this, splitMulticomponentArray, m_InputArrayPtr.lock(), m_InputArrayPtr.lock(), m_SplitArraysPtrVector)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SplitAttributeArray::newFilterInstance(bool copyFilterParameters) const
{
  SplitAttributeArray::Pointer filter = SplitAttributeArray::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SplitAttributeArray::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SplitAttributeArray::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SplitAttributeArray::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SplitAttributeArray::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid SplitAttributeArray::getUuid() const
{
  return QUuid("{5ecf77f4-a38a-52ab-b4f6-0fb8a9c5cb9c}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SplitAttributeArray::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SplitAttributeArray::getHumanLabel() const
{
  return "Split Multicomponent Attribute Array";
}

// -----------------------------------------------------------------------------
SplitAttributeArray::Pointer SplitAttributeArray::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<SplitAttributeArray> SplitAttributeArray::New()
{
  struct make_shared_enabler : public SplitAttributeArray
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString SplitAttributeArray::getNameOfClass() const
{
  return QString("SplitAttributeArray");
}

// -----------------------------------------------------------------------------
QString SplitAttributeArray::ClassName()
{
  return QString("SplitAttributeArray");
}

// -----------------------------------------------------------------------------
void SplitAttributeArray::setInputArrayPath(const DataArrayPath& value)
{
  m_InputArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath SplitAttributeArray::getInputArrayPath() const
{
  return m_InputArrayPath;
}

// -----------------------------------------------------------------------------
void SplitAttributeArray::setSplitArraysSuffix(const QString& value)
{
  m_SplitArraysSuffix = value;
}

// -----------------------------------------------------------------------------
QString SplitAttributeArray::getSplitArraysSuffix() const
{
  return m_SplitArraysSuffix;
}

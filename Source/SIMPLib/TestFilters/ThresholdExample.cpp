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

#include <memory>

#include "ThresholdExample.h"

#include "SIMPLib/FilterParameters/ComparisonSelectionFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThresholdExample::ThresholdExample()
: m_DataContainerName(SIMPL::Defaults::DataContainerName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThresholdExample::~ThresholdExample() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  /* To Compare Arrays like a threshold filter */
  {
    ComparisonSelectionFilterParameter::Pointer parameter = ComparisonSelectionFilterParameter::New();
    parameter->setHumanLabel("Voxel Cell Arrays to Threshold");
    parameter->setPropertyName("CellComparisonInputs");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(ThresholdExample, this, CellComparisonInputs));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(ThresholdExample, this, CellComparisonInputs));

    parameter->setShowOperators(true);
    parameters.push_back(parameter);
  }
#if 0
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonSelectionFilterParameter::Pointer parameter = ComparisonSelectionFilterParameter::New();
    parameter->setHumanLabel("Voxel Feature Arrays to Threshold");
    parameter->setPropertyName("FeatureComparisonInputs");

    //parameter->setValueType("QVector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonSelectionFilterParameter::Pointer parameter = ComparisonSelectionFilterParameter::New();
    parameter->setHumanLabel("Voxel Ensemble Arrays to Threshold");
    parameter->setPropertyName("EnsembleComparisonInputs");

    //parameter->setValueType("QVector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonSelectionFilterParameter::Pointer parameter = ComparisonSelectionFilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Point Arrays to Threshold");
    parameter->setPropertyName("PointComparisonInputs");

    //parameter->setValueType("QVector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonSelectionFilterParameter::Pointer parameter = ComparisonSelectionFilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Face Arrays to Threshold");
    parameter->setPropertyName("FaceComparisonInputs");

    //parameter->setValueType("QVector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonSelectionFilterParameter::Pointer parameter = ComparisonSelectionFilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Edge Arrays to Threshold");
    parameter->setPropertyName("EdgeComparisonInputs");

    //parameter->setValueType("QVector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }
#endif
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellComparisonInputs(reader->readComparisonInputs("CellComparisonInputs", getCellComparisonInputs()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerName(), false);
  if(getErrorCode() < 0 || nullptr == m)
  {
    return;
  }
  /* Example code for preflighting looking for a valid string for the output file
   * but not necessarily the fact that the file exists: Example code to make sure
   * we have something in a string before proceeding.*/
  /*
  if (m_OutputFile.isEmpty() == true)
  {
    ss << "The output file must be set before executing this filter.";
    setErrorCondition(-1, ss.str());
  }
  */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::execute()
{
  dataCheck();
  /* Place all your code to execute your filter here. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ThresholdExample::newFilterInstance(bool copyFilterParameters) const
{
  /*
   * CellComparisonInputs
   * FeatureComparisonInputs
   * EnsembleComparisonInputs
   * PointComparisonInputs
   * FaceComparisonInputs
   * EdgeComparisonInputs
   */
  ThresholdExample::Pointer filter = ThresholdExample::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ThresholdExample::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ThresholdExample::getGroupName() const
{
  return SIMPL::FilterGroups::Generic;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ThresholdExample::getUuid() const
{
  return QUuid("{bd013013-ba0f-52be-a7b7-187665c92a9e}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ThresholdExample::getSubGroupName() const
{
  return "Test";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ThresholdExample::getHumanLabel() const
{
  return "Threshold Example";
}

// -----------------------------------------------------------------------------
ThresholdExample::Pointer ThresholdExample::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ThresholdExample> ThresholdExample::New()
{
  struct make_shared_enabler : public ThresholdExample
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ThresholdExample::getNameOfClass() const
{
  return QString("ThresholdExample");
}

// -----------------------------------------------------------------------------
QString ThresholdExample::ClassName()
{
  return QString("ThresholdExample");
}

// -----------------------------------------------------------------------------
void ThresholdExample::setDataContainerName(const QString& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
QString ThresholdExample::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void ThresholdExample::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ThresholdExample::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ThresholdExample::setCellComparisonInputs(const ComparisonInputs& value)
{
  m_CellComparisonInputs = value;
}

// -----------------------------------------------------------------------------
ComparisonInputs ThresholdExample::getCellComparisonInputs() const
{
  return m_CellComparisonInputs;
}

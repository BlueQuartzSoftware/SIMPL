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

#include "GenericExample.h"

#include <QtCore/QDebug>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/AxisAngleFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/CalculatorFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/ComparisonSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerArrayProxyFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/InputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/MontageSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/RangeFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/ShapeTypeSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericExample::GenericExample()
: m_StlFilePrefix("Some Prefix")
, m_ShowPrefix(false)
, m_MaxIterations(0)
, m_MisorientationTolerance(0)
, m_PhaseCount(1)
, m_InputFile("/Some/Path/file.cpp")
, m_InputPath("/Some/Path")
, m_OutputFile("/Some/Path/Out.bin")
, m_OutputPath("/Some/Path")
, m_WriteAlignmentShifts(false)
, m_ConversionType(0)
, m_FeatureIdsArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "Garbly Gook")
, m_AttributeMatrixPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::NewCellFeatureAttributeMatrixName, "")
, m_DataContainerName(SIMPL::Defaults::StatsGenerator)
, m_CreatedDataArray(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::EulerColor)
, m_Bool1(false)
, m_Bool2(false)
, m_AlgorithmSelection(0)
, m_DistanceMetric(1)
{
  m_Dimensions[0] = 0;
  m_Dimensions[1] = 0;
  m_Dimensions[2] = 0;

  m_Origin[0] = 0.0;
  m_Origin[1] = 0.0;
  m_Origin[2] = 0.0;

  StackFileListInfo flInfo;
  flInfo.PaddingDigits = 2;
  flInfo.Ordering = 0;
  flInfo.StartIndex = 0;
  flInfo.EndIndex = 1;
  flInfo.InputPath = "";
  flInfo.FilePrefix = "prefix_";
  flInfo.FileSuffix = "_suffix";
  flInfo.FileExtension = ".png";
  setInputFileListInfo(flInfo);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericExample::~GenericExample() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  /* Place all your option initialization code here */
  /* For String input use this code */
  parameters.push_back(SIMPL_NEW_STRING_FP("STL Output Prefix", StlFilePrefix, FilterParameter::Parameter, GenericExample));

  /*   For an output file use this code*/
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output File", OutputFile, FilterParameter::Parameter, GenericExample));
  /*   For an output path use this code*/
  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path", OutputPath, FilterParameter::Parameter, GenericExample));
  /*   For a simple true/false boolean use this code*/
  parameters.push_back(SIMPL_NEW_BOOL_FP("Write Alignment Shift File", WriteAlignmentShifts, FilterParameter::Parameter, GenericExample));

  parameters.push_back(SeparatorFilterParameter::New("Choice Example", FilterParameter::Parameter));

  parameters.push_back(SIMPL_NEW_FILELISTINFO_FP("Input File List", InputFileListInfo, FilterParameter::Parameter, GenericExample));

  {
    QVector<QString> choices;
    choices.push_back("Beta");
    choices.push_back("Lognormal");
    choices.push_back("Power");
    parameters.push_back(SIMPL_NEW_CHOICE_FP("Size Distribution Fit Type", SizeDistributionFitType, FilterParameter::Parameter, GenericExample, choices, false));
  }

  {
    QVector<QString> choices;
    choices.push_back("CellData");
    choices.push_back("CellEnsemble");
    choices.push_back("FeatureIds");
    parameters.push_back(SIMPL_NEW_COMP_SEL_FP("Select Arrays to Threshold", SelectedThresholds, FilterParameter::Parameter, GenericExample, choices, true));
  }

  /*   For presenting a set of choices to the user use this code*/
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Conversion Type");
    parameter->setPropertyName("ConversionType");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(GenericExample, this, ConversionType));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(GenericExample, this, ConversionType));

    ////parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Degrees To Radians");
    choices.push_back("Radians To Degrees");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Estimated Primary Features", EstimatedPrimaryFeatures, FilterParameter::Parameter, GenericExample));

  /* Display a group of 3 text boxes to collect 3 integer values */
  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Dimensions (XYZ)", Dimensions, FilterParameter::Parameter, GenericExample));
  parameters.push_back(SIMPL_NEW_INT_VEC2_FP("Range (XY)", Range, FilterParameter::Parameter, GenericExample));

  parameters.push_back(SIMPL_NEW_RANGE_FP("Initialization Range", InitRange, FilterParameter::Parameter, GenericExample));

  parameters.push_back(SIMPL_NEW_SecondO_POLY_FP("Second Order A Coefficients", SecondOrderACoeff, FilterParameter::Parameter, GenericExample, 0));

  parameters.push_back(SIMPL_NEW_ThirdO_POLY_FP("Third Order A Coefficients", ThirdOrderACoeff, FilterParameter::Parameter, GenericExample, 1));

  parameters.push_back(SIMPL_NEW_SHAPETYPE_SELECTION_FP("Shape Types", ShapeTypeData, FilterParameter::CreatedArray, GenericExample, "PhaseCount", "InputPhaseTypesArrayPath"));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::Parameter, GenericExample, req));
  }

  {
    DataContainerArrayProxy proxy;
    parameters.push_back(SIMPL_NEW_DCA_PROXY_FP("Array to Select", DcaProxy, FilterParameter::Parameter, GenericExample, proxy, Qt::Checked));
  }

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Created Data Container", CreatedDataContainer, FilterParameter::CreatedArray, GenericExample));

  {
    QStringList linkedProps;
    linkedProps << "Bool2"
                << "Double2";
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Bool1", Bool1, FilterParameter::Parameter, GenericExample, linkedProps));
    parameters.push_back(SIMPL_NEW_DOUBLE_FP("Double 2", Double2, FilterParameter::Parameter, GenericExample));
  }

  parameters.push_back(SIMPL_NEW_DYN_CHOICE_FP("X Coordinate Array", SelectedXCoordArrayName, FilterParameter::CreatedArray, GenericExample, "DataArrayList"));

  {
    QStringList linkedProps;
    linkedProps << "AttributeMatrixPath";
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Bool2", Bool2, FilterParameter::Parameter, GenericExample, linkedProps));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Attribute Matrix", AttributeMatrixPath, FilterParameter::Parameter, GenericExample, req));
  }
  {
    AttributeMatrixCreationFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_CREATION_FP("Created Attribute Matrix", CreatedAttributeMatrix, FilterParameter::CreatedArray, GenericExample, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Linked Combo Box Example (1)", FilterParameter::Parameter));
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Select Distance Metric");
    parameter->setPropertyName("DistanceMetric");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(GenericExample, this, DistanceMetric));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(GenericExample, this, DistanceMetric));

    parameter->setDefaultValue(getDistanceMetric()); // Just set the first index

    QVector<QString> choices;
    choices.push_back("Alt Choice 0");
    choices.push_back("Alt Choice 1");
    choices.push_back("Alt Choice 2");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "MaxIterations"
                << "MisorientationTolerance"
                << "InputFile"
                << "InputPath";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);

    /*  For an Integer use this code*/
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Max Iterations", MaxIterations, FilterParameter::Parameter, GenericExample, 0)); /*  For a Floating point value use this code*/
    parameters.push_back(SIMPL_NEW_DOUBLE_FP("Misorientation Tolerance", MisorientationTolerance, FilterParameter::Parameter, GenericExample, 1));
    /*   For an input file use this code*/
    parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, GenericExample, "*.txt", "", 1));
    /*   For an input path use this code*/
    parameters.push_back(SIMPL_NEW_INPUT_PATH_FP("Input Path", InputPath, FilterParameter::Parameter, GenericExample, "*.txt", "", 2));
  }

  //------------------

  parameters.push_back(SeparatorFilterParameter::New("Linked Combo Box Example (2)", FilterParameter::Parameter));

  /*   For presenting a set of choices to the user use this code*/
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Select Algorithm");
    parameter->setPropertyName("AlgorithmSelection");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(GenericExample, this, AlgorithmSelection));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(GenericExample, this, AlgorithmSelection));

    parameter->setDefaultValue(getAlgorithmSelection()); // Just set the first index

    QVector<QString> choices;
    choices.push_back("Choice 0");
    choices.push_back("Choice 1");
    choices.push_back("Choice 2");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "CreatedDataArray"
                << "Origin"
                << "CrystalSymmetryRotations"
                << "DataContainerName";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);

    {
      DataArrayCreationFilterParameter::RequirementType req;
      parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Created Data Array", CreatedDataArray, FilterParameter::Parameter, GenericExample, req, 0));
    }
    /* Display a group of 3 text boxes to collect 3 float values */
    parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, GenericExample, 1));

    /* Display the AxisAngleWidget to collect Axis-Angle pairs from the user */
    parameters.push_back(SIMPL_NEW_AXISANGLE_FP("Crystal Rotations", CrystalSymmetryRotations, FilterParameter::Parameter, GenericExample, 2));

    parameters.push_back(SIMPL_NEW_FourthO_POLY_FP("Fourth Order A Coefficients", FourthOrderACoeff, FilterParameter::Parameter, GenericExample, 2));

    {
      DataContainerSelectionFilterParameter::RequirementType req;
      parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container", DataContainerName, FilterParameter::Parameter, GenericExample, req, 2));
    }
  }
  QVector<DataArrayPath> paths;
  paths.push_back(DataArrayPath("StatsGeneratorDataContainer", "CellEnsembleData", "CrystalStructures"));
  paths.push_back(DataArrayPath("StatsGeneratorDataContainer", "CellEnsembleData", "Statistics"));

  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Multi Data Array Test", SelectedMultiArrayPaths, FilterParameter::Parameter, GenericExample, req, 0));
  }

  parameters.push_back(SeparatorFilterParameter::New("Montage Selection Example", FilterParameter::Parameter));
  parameters.push_back(SIMPL_NEW_MONTAGE_SELECTION_FP("Montage Selection", MontageSelection, FilterParameter::Parameter, GenericExample));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{

  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setSelectedMultiArrayPaths(reader->readDataArrayPathVector("SelectedMultiArrayPaths", getSelectedMultiArrayPaths()));
  setStlFilePrefix(reader->readString("StlFilePrefix", getStlFilePrefix()));
  setMaxIterations(reader->readValue("MaxIterations", getMaxIterations()));
  setMisorientationTolerance(reader->readValue("MisorientationTolerance", getMisorientationTolerance()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  setInputPath(reader->readString("InputPath", getInputPath()));
  setOutputFile(reader->readString("OutputFile", getOutputFile()));
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setWriteAlignmentShifts(reader->readValue("WriteAlignmentShifts", getWriteAlignmentShifts()));
  setConversionType(reader->readValue("ConversionType", getConversionType()));
  setDimensions(reader->readIntVec3("Dimensions", getDimensions()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::readFilterParameters(QJsonObject& obj)
{
  AbstractFilter::readFilterParameters(obj);
  setInputFile(obj["InputFile"].toString());
  setInputPath(obj["InputPath"].toString());
  setOutputFile(obj["OutputFile"].toString());
  setOutputPath(obj["OutputPath"].toString());
}

// FP: Check why these values are not connected to a filter parameter!

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::writeFilterParameters(QJsonObject& obj) const
{
  AbstractFilter::writeFilterParameters(obj);
  obj["InputFile"] = getInputFile();
  obj["InputPath"] = getInputPath();
  obj["OutputFile"] = getOutputFile();
  obj["OutputPath"] = getOutputPath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::dataCheck()
{
  DataArrayPath tempPath;
  clearErrorCode();
  clearWarningCode();

  FileSystemPathHelper::CheckOutputFile(this, "Output File Name", getOutputFile(), true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void computeEulerAngle(float pq, float* eulerAngle)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::execute()
{
  clearErrorCode();
  clearWarningCode();

  // Run the data check to get references to all of our data arrays initialized to the values stored in memory
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  qDebug() << "Feature Ids: " << getFeatureIdsArrayPath().getDataArrayName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenericExample::newFilterInstance(bool copyFilterParameters) const
{
  GenericExample::Pointer filter = GenericExample::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenericExample::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenericExample::getGroupName() const
{
  return SIMPL::FilterGroups::Generic;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GenericExample::getUuid() const
{
  return QUuid("{b1b9da5c-4ad8-5b61-9615-2a3e17b38970}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenericExample::getSubGroupName() const
{
  return "Test";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenericExample::getHumanLabel() const
{
  return "Generic Example";
}

// -----------------------------------------------------------------------------
GenericExample::Pointer GenericExample::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GenericExample> GenericExample::New()
{
  struct make_shared_enabler : public GenericExample
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GenericExample::getNameOfClass() const
{
  return QString("GenericExample");
}

// -----------------------------------------------------------------------------
QString GenericExample::ClassName()
{
  return QString("GenericExample");
}

// -----------------------------------------------------------------------------
void GenericExample::setStlFilePrefix(const QString& value)
{
  m_StlFilePrefix = value;
}

// -----------------------------------------------------------------------------
QString GenericExample::getStlFilePrefix() const
{
  return m_StlFilePrefix;
}

// -----------------------------------------------------------------------------
void GenericExample::setShowPrefix(bool value)
{
  m_ShowPrefix = value;
}

// -----------------------------------------------------------------------------
bool GenericExample::getShowPrefix() const
{
  return m_ShowPrefix;
}

// -----------------------------------------------------------------------------
void GenericExample::setMaxIterations(int value)
{
  m_MaxIterations = value;
}

// -----------------------------------------------------------------------------
int GenericExample::getMaxIterations() const
{
  return m_MaxIterations;
}

// -----------------------------------------------------------------------------
void GenericExample::setSecondOrderACoeff(const Float2ndOrderPoly_t& value)
{
  m_SecondOrderACoeff = value;
}

// -----------------------------------------------------------------------------
Float2ndOrderPoly_t GenericExample::getSecondOrderACoeff() const
{
  return m_SecondOrderACoeff;
}

// -----------------------------------------------------------------------------
void GenericExample::setThirdOrderACoeff(const Float3rdOrderPoly_t& value)
{
  m_ThirdOrderACoeff = value;
}

// -----------------------------------------------------------------------------
Float3rdOrderPoly_t GenericExample::getThirdOrderACoeff() const
{
  return m_ThirdOrderACoeff;
}

// -----------------------------------------------------------------------------
void GenericExample::setFourthOrderACoeff(const Float4thOrderPoly_t& value)
{
  m_FourthOrderACoeff = value;
}

// -----------------------------------------------------------------------------
Float4thOrderPoly_t GenericExample::getFourthOrderACoeff() const
{
  return m_FourthOrderACoeff;
}

// -----------------------------------------------------------------------------
void GenericExample::setMisorientationTolerance(double value)
{
  m_MisorientationTolerance = value;
}

// -----------------------------------------------------------------------------
double GenericExample::getMisorientationTolerance() const
{
  return m_MisorientationTolerance;
}

// -----------------------------------------------------------------------------
void GenericExample::setInputPhaseTypesArrayPath(const DataArrayPath& value)
{
  m_InputPhaseTypesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenericExample::getInputPhaseTypesArrayPath() const
{
  return m_InputPhaseTypesArrayPath;
}

// -----------------------------------------------------------------------------
void GenericExample::setShapeTypeData(const ShapeType::Types& value)
{
  m_ShapeTypeData = value;
}

// -----------------------------------------------------------------------------
ShapeType::Types GenericExample::getShapeTypeData() const
{
  return m_ShapeTypeData;
}

// -----------------------------------------------------------------------------
void GenericExample::setPhaseCount(int value)
{
  m_PhaseCount = value;
}

// -----------------------------------------------------------------------------
int GenericExample::getPhaseCount() const
{
  return m_PhaseCount;
}

// -----------------------------------------------------------------------------
void GenericExample::setInitRange(const FPRangePair& value)
{
  m_InitRange = value;
}

// -----------------------------------------------------------------------------
FPRangePair GenericExample::getInitRange() const
{
  return m_InitRange;
}

// -----------------------------------------------------------------------------
void GenericExample::setEstimatedPrimaryFeatures(const QString& value)
{
  m_EstimatedPrimaryFeatures = value;
}

// -----------------------------------------------------------------------------
QString GenericExample::getEstimatedPrimaryFeatures() const
{
  return m_EstimatedPrimaryFeatures;
}

// -----------------------------------------------------------------------------
void GenericExample::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString GenericExample::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void GenericExample::setInputFileListInfo(const StackFileListInfo& value)
{
  m_InputFileListInfo = value;
}

// -----------------------------------------------------------------------------
StackFileListInfo GenericExample::getInputFileListInfo() const
{
  return m_InputFileListInfo;
}

// -----------------------------------------------------------------------------
void GenericExample::setSelectedXCoordArrayName(const QString& value)
{
  m_SelectedXCoordArrayName = value;
}

// -----------------------------------------------------------------------------
QString GenericExample::getSelectedXCoordArrayName() const
{
  return m_SelectedXCoordArrayName;
}

// -----------------------------------------------------------------------------
void GenericExample::setDataArrayList(const QStringList& value)
{
  m_DataArrayList = value;
}

// -----------------------------------------------------------------------------
QStringList GenericExample::getDataArrayList() const
{
  return m_DataArrayList;
}

// -----------------------------------------------------------------------------
void GenericExample::setCreatedDataContainer(const DataArrayPath& value)
{
  m_CreatedDataContainer = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenericExample::getCreatedDataContainer() const
{
  return m_CreatedDataContainer;
}

// -----------------------------------------------------------------------------
void GenericExample::setDcaProxy(const DataContainerArrayProxy& value)
{
  m_DcaProxy = value;
}

// -----------------------------------------------------------------------------
DataContainerArrayProxy GenericExample::getDcaProxy() const
{
  return m_DcaProxy;
}

// -----------------------------------------------------------------------------
void GenericExample::setInputPath(const QString& value)
{
  m_InputPath = value;
}

// -----------------------------------------------------------------------------
QString GenericExample::getInputPath() const
{
  return m_InputPath;
}

// -----------------------------------------------------------------------------
void GenericExample::setOutputFile(const QString& value)
{
  m_OutputFile = value;
}

// -----------------------------------------------------------------------------
QString GenericExample::getOutputFile() const
{
  return m_OutputFile;
}

// -----------------------------------------------------------------------------
void GenericExample::setOutputPath(const QString& value)
{
  m_OutputPath = value;
}

// -----------------------------------------------------------------------------
QString GenericExample::getOutputPath() const
{
  return m_OutputPath;
}

// -----------------------------------------------------------------------------
void GenericExample::setSelectedMultiArrayPaths(const QVector<DataArrayPath>& value)
{
  m_SelectedMultiArrayPaths = value;
}

// -----------------------------------------------------------------------------
QVector<DataArrayPath> GenericExample::getSelectedMultiArrayPaths() const
{
  return m_SelectedMultiArrayPaths;
}

// -----------------------------------------------------------------------------
void GenericExample::setWriteAlignmentShifts(bool value)
{
  m_WriteAlignmentShifts = value;
}

// -----------------------------------------------------------------------------
bool GenericExample::getWriteAlignmentShifts() const
{
  return m_WriteAlignmentShifts;
}

// -----------------------------------------------------------------------------
void GenericExample::setConversionType(int value)
{
  m_ConversionType = value;
}

// -----------------------------------------------------------------------------
int GenericExample::getConversionType() const
{
  return m_ConversionType;
}

// -----------------------------------------------------------------------------
void GenericExample::setDimensions(const IntVec3Type& value)
{
  m_Dimensions = value;
}

// -----------------------------------------------------------------------------
IntVec3Type GenericExample::getDimensions() const
{
  return m_Dimensions;
}

// -----------------------------------------------------------------------------
void GenericExample::setRange(const IntVec2Type& value)
{
  m_Range = value;
}

// -----------------------------------------------------------------------------
IntVec2Type GenericExample::getRange() const
{
  return m_Range;
}

// -----------------------------------------------------------------------------
void GenericExample::setOrigin(const FloatVec3Type& value)
{
  m_Origin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type GenericExample::getOrigin() const
{
  return m_Origin;
}

// -----------------------------------------------------------------------------
void GenericExample::setCrystalSymmetryRotations(const AxisAngleInput_t& value)
{
  m_CrystalSymmetryRotations = value;
}

// -----------------------------------------------------------------------------
AxisAngleInput_t GenericExample::getCrystalSymmetryRotations() const
{
  return m_CrystalSymmetryRotations;
}

// -----------------------------------------------------------------------------
void GenericExample::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenericExample::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void GenericExample::setAttributeMatrixPath(const DataArrayPath& value)
{
  m_AttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenericExample::getAttributeMatrixPath() const
{
  return m_AttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void GenericExample::setCreatedAttributeMatrix(const DataArrayPath& value)
{
  m_CreatedAttributeMatrix = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenericExample::getCreatedAttributeMatrix() const
{
  return m_CreatedAttributeMatrix;
}

// -----------------------------------------------------------------------------
void GenericExample::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenericExample::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void GenericExample::setSizeDistributionFitType(int value)
{
  m_SizeDistributionFitType = value;
}

// -----------------------------------------------------------------------------
int GenericExample::getSizeDistributionFitType() const
{
  return m_SizeDistributionFitType;
}

// -----------------------------------------------------------------------------
void GenericExample::setSelectedThresholds(const ComparisonInputs& value)
{
  m_SelectedThresholds = value;
}

// -----------------------------------------------------------------------------
ComparisonInputs GenericExample::getSelectedThresholds() const
{
  return m_SelectedThresholds;
}

// -----------------------------------------------------------------------------
void GenericExample::setCalcExpression(const QString& value)
{
  m_CalcExpression = value;
}

// -----------------------------------------------------------------------------
QString GenericExample::getCalcExpression() const
{
  return m_CalcExpression;
}

// -----------------------------------------------------------------------------
void GenericExample::setCreatedDataArray(const DataArrayPath& value)
{
  m_CreatedDataArray = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenericExample::getCreatedDataArray() const
{
  return m_CreatedDataArray;
}

// -----------------------------------------------------------------------------
void GenericExample::setBool1(bool value)
{
  m_Bool1 = value;
}

// -----------------------------------------------------------------------------
bool GenericExample::getBool1() const
{
  return m_Bool1;
}

// -----------------------------------------------------------------------------
void GenericExample::setDouble2(double value)
{
  m_Double2 = value;
}

// -----------------------------------------------------------------------------
double GenericExample::getDouble2() const
{
  return m_Double2;
}

// -----------------------------------------------------------------------------
void GenericExample::setBool2(bool value)
{
  m_Bool2 = value;
}

// -----------------------------------------------------------------------------
bool GenericExample::getBool2() const
{
  return m_Bool2;
}

// -----------------------------------------------------------------------------
void GenericExample::setAlgorithmSelection(int value)
{
  m_AlgorithmSelection = value;
}

// -----------------------------------------------------------------------------
int GenericExample::getAlgorithmSelection() const
{
  return m_AlgorithmSelection;
}

// -----------------------------------------------------------------------------
void GenericExample::setDistanceMetric(int value)
{
  m_DistanceMetric = value;
}

// -----------------------------------------------------------------------------
int GenericExample::getDistanceMetric() const
{
  return m_DistanceMetric;
}

// -----------------------------------------------------------------------------
void GenericExample::setMontageSelection(const MontageSelection& value)
{
  m_MontageSelection = value;
}

// -----------------------------------------------------------------------------
MontageSelection GenericExample::getMontageSelection() const
{
  return m_MontageSelection;
}

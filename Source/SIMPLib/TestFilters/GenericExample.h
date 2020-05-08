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

#pragma once

#include <memory>

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/ShapeType.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/FilterParameters/AxisAngleFilterParameter.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/FourthOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec2FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/MontageSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SecondOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/ThirdOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/RangeFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/ComparisonInputs.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"

/**
 * @class GenericExample GenericExample.h ExamplePlugin/Code/ExamplePluginFilters/GenericExample.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class SIMPLib_EXPORT GenericExample : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(GenericExample SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(GenericExample)
  PYB11_FILTER_NEW_MACRO(GenericExample)
  PYB11_PROPERTY(QString StlFilePrefix READ getStlFilePrefix WRITE setStlFilePrefix)
  PYB11_PROPERTY(bool ShowPrefix READ getShowPrefix WRITE setShowPrefix)
  PYB11_PROPERTY(int MaxIterations READ getMaxIterations WRITE setMaxIterations)
  PYB11_PROPERTY(Float2ndOrderPoly_t SecondOrderACoeff READ getSecondOrderACoeff WRITE setSecondOrderACoeff)
  PYB11_PROPERTY(Float3rdOrderPoly_t ThirdOrderACoeff READ getThirdOrderACoeff WRITE setThirdOrderACoeff)
  PYB11_PROPERTY(Float4thOrderPoly_t FourthOrderACoeff READ getFourthOrderACoeff WRITE setFourthOrderACoeff)
  PYB11_PROPERTY(double MisorientationTolerance READ getMisorientationTolerance WRITE setMisorientationTolerance)
  PYB11_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)
  PYB11_PROPERTY(ShapeType::Types ShapeTypeData READ getShapeTypeData WRITE setShapeTypeData)
  PYB11_PROPERTY(int PhaseCount READ getPhaseCount WRITE setPhaseCount)
  PYB11_PROPERTY(FPRangePair InitRange READ getInitRange WRITE setInitRange)
  PYB11_PROPERTY(QString EstimatedPrimaryFeatures READ getEstimatedPrimaryFeatures WRITE setEstimatedPrimaryFeatures)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(StackFileListInfo InputFileListInfo READ getInputFileListInfo WRITE setInputFileListInfo)
  PYB11_PROPERTY(QString SelectedXCoordArrayName READ getSelectedXCoordArrayName WRITE setSelectedXCoordArrayName)
  PYB11_PROPERTY(QStringList DataArrayList READ getDataArrayList WRITE setDataArrayList)
  PYB11_PROPERTY(DataArrayPath CreatedDataContainer READ getCreatedDataContainer WRITE setCreatedDataContainer)
  PYB11_PROPERTY(DataContainerArrayProxy DcaProxy READ getDcaProxy WRITE setDcaProxy)
  PYB11_PROPERTY(QString InputPath READ getInputPath WRITE setInputPath)
  PYB11_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)
  PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
  PYB11_PROPERTY(QVector<DataArrayPath> SelectedMultiArrayPaths READ getSelectedMultiArrayPaths WRITE setSelectedMultiArrayPaths)
  PYB11_PROPERTY(bool WriteAlignmentShifts READ getWriteAlignmentShifts WRITE setWriteAlignmentShifts)
  PYB11_PROPERTY(int ConversionType READ getConversionType WRITE setConversionType)
  PYB11_PROPERTY(IntVec3Type Dimensions READ getDimensions WRITE setDimensions)
  PYB11_PROPERTY(IntVec2Type Range READ getRange WRITE setRange)
  PYB11_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)
  PYB11_PROPERTY(AxisAngleInput_t CrystalSymmetryRotations READ getCrystalSymmetryRotations WRITE setCrystalSymmetryRotations)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath AttributeMatrixPath READ getAttributeMatrixPath WRITE setAttributeMatrixPath)
  PYB11_PROPERTY(DataArrayPath CreatedAttributeMatrix READ getCreatedAttributeMatrix WRITE setCreatedAttributeMatrix)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(int SizeDistributionFitType READ getSizeDistributionFitType WRITE setSizeDistributionFitType)
  PYB11_PROPERTY(ComparisonInputs SelectedThresholds READ getSelectedThresholds WRITE setSelectedThresholds)
  PYB11_PROPERTY(QString CalcExpression READ getCalcExpression WRITE setCalcExpression)
  PYB11_PROPERTY(DataArrayPath CreatedDataArray READ getCreatedDataArray WRITE setCreatedDataArray)
  PYB11_PROPERTY(bool Bool1 READ getBool1 WRITE setBool1)
  PYB11_PROPERTY(double Double2 READ getDouble2 WRITE setDouble2)
  PYB11_PROPERTY(bool Bool2 READ getBool2 WRITE setBool2)
  PYB11_PROPERTY(int AlgorithmSelection READ getAlgorithmSelection WRITE setAlgorithmSelection)
  PYB11_PROPERTY(int DistanceMetric READ getDistanceMetric WRITE setDistanceMetric)
  PYB11_PROPERTY(MontageSelection MontageSelection READ getMontageSelection WRITE setMontageSelection)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = GenericExample;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;

  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for GenericExample
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GenericExample
   */
  static QString ClassName();

  ~GenericExample() override;

  /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
  /**
   * @brief Setter property for StlFilePrefix
   */
  void setStlFilePrefix(const QString& value);
  /**
   * @brief Getter property for StlFilePrefix
   * @return Value of StlFilePrefix
   */
  QString getStlFilePrefix() const;

  Q_PROPERTY(QString StlFilePrefix READ getStlFilePrefix WRITE setStlFilePrefix)

  /**
   * @brief Setter property for ShowPrefix
   */
  void setShowPrefix(bool value);
  /**
   * @brief Getter property for ShowPrefix
   * @return Value of ShowPrefix
   */
  bool getShowPrefix() const;

  Q_PROPERTY(bool ShowPrefix READ getShowPrefix WRITE setShowPrefix)

  /**
   * @brief Setter property for MaxIterations
   */
  void setMaxIterations(int value);
  /**
   * @brief Getter property for MaxIterations
   * @return Value of MaxIterations
   */
  int getMaxIterations() const;

  Q_PROPERTY(int MaxIterations READ getMaxIterations WRITE setMaxIterations)

  /**
   * @brief Setter property for SecondOrderACoeff
   */
  void setSecondOrderACoeff(const Float2ndOrderPoly_t& value);
  /**
   * @brief Getter property for SecondOrderACoeff
   * @return Value of SecondOrderACoeff
   */
  Float2ndOrderPoly_t getSecondOrderACoeff() const;

  Q_PROPERTY(Float2ndOrderPoly_t SecondOrderACoeff READ getSecondOrderACoeff WRITE setSecondOrderACoeff)

  /**
   * @brief Setter property for ThirdOrderACoeff
   */
  void setThirdOrderACoeff(const Float3rdOrderPoly_t& value);
  /**
   * @brief Getter property for ThirdOrderACoeff
   * @return Value of ThirdOrderACoeff
   */
  Float3rdOrderPoly_t getThirdOrderACoeff() const;

  Q_PROPERTY(Float3rdOrderPoly_t ThirdOrderACoeff READ getThirdOrderACoeff WRITE setThirdOrderACoeff)

  /**
   * @brief Setter property for FourthOrderACoeff
   */
  void setFourthOrderACoeff(const Float4thOrderPoly_t& value);
  /**
   * @brief Getter property for FourthOrderACoeff
   * @return Value of FourthOrderACoeff
   */
  Float4thOrderPoly_t getFourthOrderACoeff() const;

  Q_PROPERTY(Float4thOrderPoly_t FourthOrderACoeff READ getFourthOrderACoeff WRITE setFourthOrderACoeff)

  /**
   * @brief Setter property for MisorientationTolerance
   */
  void setMisorientationTolerance(double value);
  /**
   * @brief Getter property for MisorientationTolerance
   * @return Value of MisorientationTolerance
   */
  double getMisorientationTolerance() const;

  Q_PROPERTY(double MisorientationTolerance READ getMisorientationTolerance WRITE setMisorientationTolerance)

  /**
   * @brief Setter property for InputPhaseTypesArrayPath
   */
  void setInputPhaseTypesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for InputPhaseTypesArrayPath
   * @return Value of InputPhaseTypesArrayPath
   */
  DataArrayPath getInputPhaseTypesArrayPath() const;

  Q_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)

  /**
   * @brief Setter property for ShapeTypeData
   */
  void setShapeTypeData(const ShapeType::Types& value);
  /**
   * @brief Getter property for ShapeTypeData
   * @return Value of ShapeTypeData
   */
  ShapeType::Types getShapeTypeData() const;

  Q_PROPERTY(ShapeType::Types ShapeTypeData READ getShapeTypeData WRITE setShapeTypeData)

  /**
   * @brief Setter property for PhaseCount
   */
  void setPhaseCount(int value);
  /**
   * @brief Getter property for PhaseCount
   * @return Value of PhaseCount
   */
  int getPhaseCount() const;

  Q_PROPERTY(int PhaseCount READ getPhaseCount WRITE setPhaseCount)

  /**
   * @brief Setter property for InitRange
   */
  void setInitRange(const FPRangePair& value);
  /**
   * @brief Getter property for InitRange
   * @return Value of InitRange
   */
  FPRangePair getInitRange() const;

  Q_PROPERTY(FPRangePair InitRange READ getInitRange WRITE setInitRange)

  /**
   * @brief Setter property for EstimatedPrimaryFeatures
   */
  void setEstimatedPrimaryFeatures(const QString& value);
  /**
   * @brief Getter property for EstimatedPrimaryFeatures
   * @return Value of EstimatedPrimaryFeatures
   */
  QString getEstimatedPrimaryFeatures() const;

  Q_PROPERTY(QString EstimatedPrimaryFeatures READ getEstimatedPrimaryFeatures WRITE setEstimatedPrimaryFeatures)

  /**
   * @brief Setter property for InputFile
   */
  void setInputFile(const QString& value);
  /**
   * @brief Getter property for InputFile
   * @return Value of InputFile
   */
  QString getInputFile() const;

  Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

  /**
   * @brief Setter property for InputFileListInfo
   */
  void setInputFileListInfo(const StackFileListInfo& value);
  /**
   * @brief Getter property for InputFileListInfo
   * @return Value of InputFileListInfo
   */
  StackFileListInfo getInputFileListInfo() const;

  Q_PROPERTY(StackFileListInfo InputFileListInfo READ getInputFileListInfo WRITE setInputFileListInfo)

  /**
   * @brief Setter property for SelectedXCoordArrayName
   */
  void setSelectedXCoordArrayName(const QString& value);
  /**
   * @brief Getter property for SelectedXCoordArrayName
   * @return Value of SelectedXCoordArrayName
   */
  QString getSelectedXCoordArrayName() const;

  Q_PROPERTY(QString SelectedXCoordArrayName READ getSelectedXCoordArrayName WRITE setSelectedXCoordArrayName)

  /**
   * @brief Setter property for DataArrayList
   */
  void setDataArrayList(const QStringList& value);
  /**
   * @brief Getter property for DataArrayList
   * @return Value of DataArrayList
   */
  QStringList getDataArrayList() const;

  Q_PROPERTY(QStringList DataArrayList READ getDataArrayList WRITE setDataArrayList)

  /**
   * @brief Setter property for CreatedDataContainer
   */
  void setCreatedDataContainer(const DataArrayPath& value);
  /**
   * @brief Getter property for CreatedDataContainer
   * @return Value of CreatedDataContainer
   */
  DataArrayPath getCreatedDataContainer() const;

  Q_PROPERTY(DataArrayPath CreatedDataContainer READ getCreatedDataContainer WRITE setCreatedDataContainer)

  /**
   * @brief Setter property for DcaProxy
   */
  void setDcaProxy(const DataContainerArrayProxy& value);
  /**
   * @brief Getter property for DcaProxy
   * @return Value of DcaProxy
   */
  DataContainerArrayProxy getDcaProxy() const;

  Q_PROPERTY(DataContainerArrayProxy DcaProxy READ getDcaProxy WRITE setDcaProxy)

  /**
   * @brief Setter property for InputPath
   */
  void setInputPath(const QString& value);
  /**
   * @brief Getter property for InputPath
   * @return Value of InputPath
   */
  QString getInputPath() const;

  Q_PROPERTY(QString InputPath READ getInputPath WRITE setInputPath)

  /**
   * @brief Setter property for OutputFile
   */
  void setOutputFile(const QString& value);
  /**
   * @brief Getter property for OutputFile
   * @return Value of OutputFile
   */
  QString getOutputFile() const;

  Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)

  /**
   * @brief Setter property for OutputPath
   */
  void setOutputPath(const QString& value);
  /**
   * @brief Getter property for OutputPath
   * @return Value of OutputPath
   */
  QString getOutputPath() const;

  Q_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)

  /**
   * @brief Setter property for SelectedMultiArrayPaths
   */
  void setSelectedMultiArrayPaths(const QVector<DataArrayPath>& value);
  /**
   * @brief Getter property for SelectedMultiArrayPaths
   * @return Value of SelectedMultiArrayPaths
   */
  QVector<DataArrayPath> getSelectedMultiArrayPaths() const;

  Q_PROPERTY(QVector<DataArrayPath> SelectedMultiArrayPaths READ getSelectedMultiArrayPaths WRITE setSelectedMultiArrayPaths)

  /**
   * @brief Setter property for WriteAlignmentShifts
   */
  void setWriteAlignmentShifts(bool value);
  /**
   * @brief Getter property for WriteAlignmentShifts
   * @return Value of WriteAlignmentShifts
   */
  bool getWriteAlignmentShifts() const;

  Q_PROPERTY(bool WriteAlignmentShifts READ getWriteAlignmentShifts WRITE setWriteAlignmentShifts)

  /**
   * @brief Setter property for ConversionType
   */
  void setConversionType(int value);
  /**
   * @brief Getter property for ConversionType
   * @return Value of ConversionType
   */
  int getConversionType() const;

  Q_PROPERTY(int ConversionType READ getConversionType WRITE setConversionType)

  /**
   * @brief Setter property for Dimensions
   */
  void setDimensions(const IntVec3Type& value);
  /**
   * @brief Getter property for Dimensions
   * @return Value of Dimensions
   */
  IntVec3Type getDimensions() const;

  Q_PROPERTY(IntVec3Type Dimensions READ getDimensions WRITE setDimensions)

  /**
   * @brief Setter property for Range
   */
  void setRange(const IntVec2Type& value);
  /**
   * @brief Getter property for Range
   * @return Value of Range
   */
  IntVec2Type getRange() const;

  Q_PROPERTY(IntVec2Type Range READ getRange WRITE setRange)

  /**
   * @brief Setter property for Origin
   */
  void setOrigin(const FloatVec3Type& value);
  /**
   * @brief Getter property for Origin
   * @return Value of Origin
   */
  FloatVec3Type getOrigin() const;

  Q_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)

  /**
   * @brief Setter property for CrystalSymmetryRotations
   */
  void setCrystalSymmetryRotations(const AxisAngleInput_t& value);
  /**
   * @brief Getter property for CrystalSymmetryRotations
   * @return Value of CrystalSymmetryRotations
   */
  AxisAngleInput_t getCrystalSymmetryRotations() const;

  Q_PROPERTY(AxisAngleInput_t CrystalSymmetryRotations READ getCrystalSymmetryRotations WRITE setCrystalSymmetryRotations)

  /**
   * @brief Setter property for FeatureIdsArrayPath
   */
  void setFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureIdsArrayPath
   * @return Value of FeatureIdsArrayPath
   */
  DataArrayPath getFeatureIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  /**
   * @brief Setter property for AttributeMatrixPath
   */
  void setAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AttributeMatrixPath
   * @return Value of AttributeMatrixPath
   */
  DataArrayPath getAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath AttributeMatrixPath READ getAttributeMatrixPath WRITE setAttributeMatrixPath)

  /**
   * @brief Setter property for CreatedAttributeMatrix
   */
  void setCreatedAttributeMatrix(const DataArrayPath& value);
  /**
   * @brief Getter property for CreatedAttributeMatrix
   * @return Value of CreatedAttributeMatrix
   */
  DataArrayPath getCreatedAttributeMatrix() const;

  Q_PROPERTY(DataArrayPath CreatedAttributeMatrix READ getCreatedAttributeMatrix WRITE setCreatedAttributeMatrix)

  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  DataArrayPath getDataContainerName() const;

  Q_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)

  /**
   * @brief Setter property for SizeDistributionFitType
   */
  void setSizeDistributionFitType(int value);
  /**
   * @brief Getter property for SizeDistributionFitType
   * @return Value of SizeDistributionFitType
   */
  int getSizeDistributionFitType() const;

  Q_PROPERTY(int SizeDistributionFitType READ getSizeDistributionFitType WRITE setSizeDistributionFitType)

  /**
   * @brief Setter property for SelectedThresholds
   */
  void setSelectedThresholds(const ComparisonInputs& value);
  /**
   * @brief Getter property for SelectedThresholds
   * @return Value of SelectedThresholds
   */
  ComparisonInputs getSelectedThresholds() const;

  Q_PROPERTY(ComparisonInputs SelectedThresholds READ getSelectedThresholds WRITE setSelectedThresholds)

  /**
   * @brief Setter property for CalcExpression
   */
  void setCalcExpression(const QString& value);
  /**
   * @brief Getter property for CalcExpression
   * @return Value of CalcExpression
   */
  QString getCalcExpression() const;

  Q_PROPERTY(QString CalcExpression READ getCalcExpression WRITE setCalcExpression)

  /**
   * @brief Setter property for CreatedDataArray
   */
  void setCreatedDataArray(const DataArrayPath& value);
  /**
   * @brief Getter property for CreatedDataArray
   * @return Value of CreatedDataArray
   */
  DataArrayPath getCreatedDataArray() const;

  Q_PROPERTY(DataArrayPath CreatedDataArray READ getCreatedDataArray WRITE setCreatedDataArray)

  /**
   * @brief Setter property for Bool1
   */
  void setBool1(bool value);
  /**
   * @brief Getter property for Bool1
   * @return Value of Bool1
   */
  bool getBool1() const;

  Q_PROPERTY(bool Bool1 READ getBool1 WRITE setBool1)

  /**
   * @brief Setter property for Double2
   */
  void setDouble2(double value);
  /**
   * @brief Getter property for Double2
   * @return Value of Double2
   */
  double getDouble2() const;

  Q_PROPERTY(double Double2 READ getDouble2 WRITE setDouble2)

  /**
   * @brief Setter property for Bool2
   */
  void setBool2(bool value);
  /**
   * @brief Getter property for Bool2
   * @return Value of Bool2
   */
  bool getBool2() const;

  Q_PROPERTY(bool Bool2 READ getBool2 WRITE setBool2)

  /**
   * @brief Setter property for AlgorithmSelection
   */
  void setAlgorithmSelection(int value);
  /**
   * @brief Getter property for AlgorithmSelection
   * @return Value of AlgorithmSelection
   */
  int getAlgorithmSelection() const;

  Q_PROPERTY(int AlgorithmSelection READ getAlgorithmSelection WRITE setAlgorithmSelection)

  /**
   * @brief Setter property for DistanceMetric
   */
  void setDistanceMetric(int value);
  /**
   * @brief Getter property for DistanceMetric
   * @return Value of DistanceMetric
   */
  int getDistanceMetric() const;

  Q_PROPERTY(int DistanceMetric READ getDistanceMetric WRITE setDistanceMetric)

  /**
   * @brief Setter property for MontageSelection
   */
  void setMontageSelection(const MontageSelection& value);
  /**
   * @brief Getter property for MontageSelection
   * @return Value of MontageSelection
   */
  MontageSelection getMontageSelection() const;

  Q_PROPERTY(MontageSelection MontageSelection READ getMontageSelection WRITE setMontageSelection)

  QString getCompiledLibraryName() const override;
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;
  QString getGroupName() const override;

  /**
   * @brief This returns a string that is displayed in the GUI. It should be readable
   * and understandable by humans.
   */
  QString getHumanLabel() const override;

  /**
   * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
   * a subgroup. It should be readable and understandable by humans.
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief This method will instantiate all the end user settable options/parameters
   * for this filter
   */
  void setupFilterParameters() override;

  /**
   * @brief This method will read the options from a file
   * @param reader The reader that is used to read the options from a file
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief readFilterParametersFromJson Reads the filter parameters from a file
   * @param reader Reader that is used to read the parameters from a file
   */
  void readFilterParameters(QJsonObject& obj) override;

  /**
   * @brief writeFilterParametersToJson Writes the filter parameters to a file
   * @param root The root json object
   */
  void writeFilterParameters(QJsonObject& obj) const override;

  /**
   * @brief Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  GenericExample();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

public:
  GenericExample(const GenericExample&) = delete;            // Copy Constructor Not Implemented
  GenericExample(GenericExample&&) = delete;                 // Move Constructor Not Implemented
  GenericExample& operator=(const GenericExample&) = delete; // Copy Assignment Not Implemented
  GenericExample& operator=(GenericExample&&) = delete;      // Move Assignment Not Implemented

private:
  QString m_StlFilePrefix = {};
  bool m_ShowPrefix = {};
  int m_MaxIterations = {};
  Float2ndOrderPoly_t m_SecondOrderACoeff = {};
  Float3rdOrderPoly_t m_ThirdOrderACoeff = {};
  Float4thOrderPoly_t m_FourthOrderACoeff = {};
  double m_MisorientationTolerance = {};
  DataArrayPath m_InputPhaseTypesArrayPath = {};
  ShapeType::Types m_ShapeTypeData = {};
  int m_PhaseCount = {};
  FPRangePair m_InitRange = {};
  QString m_EstimatedPrimaryFeatures = {};
  QString m_InputFile = {};
  StackFileListInfo m_InputFileListInfo = {};
  QString m_SelectedXCoordArrayName = {};
  QStringList m_DataArrayList = {};
  DataArrayPath m_CreatedDataContainer = {};
  DataContainerArrayProxy m_DcaProxy = {};
  QString m_InputPath = {};
  QString m_OutputFile = {};
  QString m_OutputPath = {};
  QVector<DataArrayPath> m_SelectedMultiArrayPaths = {};
  bool m_WriteAlignmentShifts = {};
  int m_ConversionType = {};
  IntVec3Type m_Dimensions = {};
  IntVec2Type m_Range = {};
  FloatVec3Type m_Origin = {};
  AxisAngleInput_t m_CrystalSymmetryRotations = {};
  DataArrayPath m_FeatureIdsArrayPath = {};
  DataArrayPath m_AttributeMatrixPath = {};
  DataArrayPath m_CreatedAttributeMatrix = {};
  DataArrayPath m_DataContainerName = {};
  int m_SizeDistributionFitType = {};
  ComparisonInputs m_SelectedThresholds = {};
  QString m_CalcExpression = {};
  DataArrayPath m_CreatedDataArray = {};
  bool m_Bool1 = {};
  double m_Double2 = {};
  bool m_Bool2 = {};
  int m_AlgorithmSelection = {};
  int m_DistanceMetric = {};
  MontageSelection m_MontageSelection = {};
};

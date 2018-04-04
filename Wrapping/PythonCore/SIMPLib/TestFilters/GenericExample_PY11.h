#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class GenericExample located at
 * SIMPLib/TestFilters/GenericExample.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.182
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/TestFilters/GenericExample.h"

/**
 * @brief This defines a C++11 alias so loading the file by itself into an IDE
 * will allow the proper code completion for a wrapped std::shared_ptr<> class.
 */
#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T> using PySharedPtrClass = py::class_<T, std::shared_ptr<T>>;
#endif

/**
 * @brief This allows filters declared in a plugin to be wrapped
 */
#ifndef PyAbstractFilterSharedPtrClass_TEMPLATE
#define PyAbstractFilterSharedPtrClass_TEMPLATE
template <typename T> using PyAbstractFilterSharedPtrClass = py::class_<T, AbstractFilter, std::shared_ptr<T>>;
#endif

/**
 * @brief
 * @param m The Python module
 * @param parent The parent class of the wrapped class
 * @return A std::shared_ptr<T> wrapped instance of the wrapped class properly
 * initialized.
 */
PySharedPtrClass<GenericExample> pybind11_init_SIMPLib_GenericExample(py::module& m
#ifdef simpl_EXPORTS
                                                                      ,
                                                                      PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<GenericExample> instance(m, "GenericExample", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<GenericExample> instance(m, "GenericExample");
#endif

  instance
      /* Property accessors for StlFilePrefix */
      .def_property("StlFilePrefix", &GenericExample::getStlFilePrefix, &GenericExample::setStlFilePrefix)
      /* Property accessors for ShowPrefix */
      .def_property("ShowPrefix", &GenericExample::getShowPrefix, &GenericExample::setShowPrefix)
      /* Property accessors for MaxIterations */
      .def_property("MaxIterations", &GenericExample::getMaxIterations, &GenericExample::setMaxIterations)
      /* Property accessors for SecondOrderACoeff */
      .def_property("SecondOrderACoeff", &GenericExample::getSecondOrderACoeff, &GenericExample::setSecondOrderACoeff)
      /* Property accessors for ThirdOrderACoeff */
      .def_property("ThirdOrderACoeff", &GenericExample::getThirdOrderACoeff, &GenericExample::setThirdOrderACoeff)
      /* Property accessors for FourthOrderACoeff */
      .def_property("FourthOrderACoeff", &GenericExample::getFourthOrderACoeff, &GenericExample::setFourthOrderACoeff)
      /* Property accessors for MisorientationTolerance */
      .def_property("MisorientationTolerance", &GenericExample::getMisorientationTolerance, &GenericExample::setMisorientationTolerance)
      /* Property accessors for InputPhaseTypesArrayPath */
      .def_property("InputPhaseTypesArrayPath", &GenericExample::getInputPhaseTypesArrayPath, &GenericExample::setInputPhaseTypesArrayPath)
      /* Property accessors for ShapeTypeData */
      .def_property("ShapeTypeData", &GenericExample::getShapeTypeData, &GenericExample::setShapeTypeData)
      /* Property accessors for PhaseCount */
      .def_property("PhaseCount", &GenericExample::getPhaseCount, &GenericExample::setPhaseCount)
      /* Property accessors for InitRange */
      .def_property("InitRange", &GenericExample::getInitRange, &GenericExample::setInitRange)
      /* Property accessors for EstimatedPrimaryFeatures */
      .def_property("EstimatedPrimaryFeatures", &GenericExample::getEstimatedPrimaryFeatures, &GenericExample::setEstimatedPrimaryFeatures)
      /* Property accessors for InputFile */
      .def_property("InputFile", &GenericExample::getInputFile, &GenericExample::setInputFile)
      /* Property accessors for InputFileListInfo */
      .def_property("InputFileListInfo", &GenericExample::getInputFileListInfo, &GenericExample::setInputFileListInfo)
      /* Property accessors for SelectedXCoordArrayName */
      .def_property("SelectedXCoordArrayName", &GenericExample::getSelectedXCoordArrayName, &GenericExample::setSelectedXCoordArrayName)
      /* Property accessors for DataArrayList */
      .def_property("DataArrayList", &GenericExample::getDataArrayList, &GenericExample::setDataArrayList)
      /* Property accessors for CreatedDataContainer */
      .def_property("CreatedDataContainer", &GenericExample::getCreatedDataContainer, &GenericExample::setCreatedDataContainer)
      /* Property accessors for DcaProxy */
      .def_property("DcaProxy", &GenericExample::getDcaProxy, &GenericExample::setDcaProxy)
      /* Property accessors for InputPath */
      .def_property("InputPath", &GenericExample::getInputPath, &GenericExample::setInputPath)
      /* Property accessors for OutputFile */
      .def_property("OutputFile", &GenericExample::getOutputFile, &GenericExample::setOutputFile)
      /* Property accessors for OutputPath */
      .def_property("OutputPath", &GenericExample::getOutputPath, &GenericExample::setOutputPath)
      /* Property accessors for SelectedMultiArrayPaths */
      .def_property("SelectedMultiArrayPaths", &GenericExample::getSelectedMultiArrayPaths, &GenericExample::setSelectedMultiArrayPaths)
      /* Property accessors for WriteAlignmentShifts */
      .def_property("WriteAlignmentShifts", &GenericExample::getWriteAlignmentShifts, &GenericExample::setWriteAlignmentShifts)
      /* Property accessors for ConversionType */
      .def_property("ConversionType", &GenericExample::getConversionType, &GenericExample::setConversionType)
      /* Property accessors for Dimensions */
      .def_property("Dimensions", &GenericExample::getDimensions, &GenericExample::setDimensions)
      /* Property accessors for Origin */
      .def_property("Origin", &GenericExample::getOrigin, &GenericExample::setOrigin)
      /* Property accessors for CrystalSymmetryRotations */
      .def_property("CrystalSymmetryRotations", &GenericExample::getCrystalSymmetryRotations, &GenericExample::setCrystalSymmetryRotations)
      /* Property accessors for FeatureIdsArrayPath */
      .def_property("FeatureIdsArrayPath", &GenericExample::getFeatureIdsArrayPath, &GenericExample::setFeatureIdsArrayPath)
      /* Property accessors for AttributeMatrixPath */
      .def_property("AttributeMatrixPath", &GenericExample::getAttributeMatrixPath, &GenericExample::setAttributeMatrixPath)
      /* Property accessors for CreatedAttributeMatrix */
      .def_property("CreatedAttributeMatrix", &GenericExample::getCreatedAttributeMatrix, &GenericExample::setCreatedAttributeMatrix)
      /* Property accessors for DataContainerName */
      .def_property("DataContainerName", &GenericExample::getDataContainerName, &GenericExample::setDataContainerName)
      /* Property accessors for SizeDistributionFitType */
      .def_property("SizeDistributionFitType", &GenericExample::getSizeDistributionFitType, &GenericExample::setSizeDistributionFitType)
      /* Property accessors for SelectedThresholds */
      .def_property("SelectedThresholds", &GenericExample::getSelectedThresholds, &GenericExample::setSelectedThresholds)
      /* Property accessors for CalcExpression */
      .def_property("CalcExpression", &GenericExample::getCalcExpression, &GenericExample::setCalcExpression)
      /* Property accessors for CreatedDataArray */
      .def_property("CreatedDataArray", &GenericExample::getCreatedDataArray, &GenericExample::setCreatedDataArray)
      /* Property accessors for Bool1 */
      .def_property("Bool1", &GenericExample::getBool1, &GenericExample::setBool1)
      /* Property accessors for Double2 */
      .def_property("Double2", &GenericExample::getDouble2, &GenericExample::setDouble2)
      /* Property accessors for Bool2 */
      .def_property("Bool2", &GenericExample::getBool2, &GenericExample::setBool2)
      /* Property accessors for AlgorithmSelection */
      .def_property("AlgorithmSelection", &GenericExample::getAlgorithmSelection, &GenericExample::setAlgorithmSelection)
      /* Property accessors for DistanceMetric */
      .def_property("DistanceMetric", &GenericExample::getDistanceMetric, &GenericExample::setDistanceMetric);

  /* Return the instance */
  return instance;
}

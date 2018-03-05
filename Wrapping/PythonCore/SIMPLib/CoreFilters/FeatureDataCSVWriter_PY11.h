#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class FeatureDataCSVWriter located at
* SIMPLib/CoreFilters/FeatureDataCSVWriter.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/FeatureDataCSVWriter.h"

/**
* @brief This defines a C++11 alias so loading the file by itself into an IDE
* will allow the proper code completion for a wrapped std::shared_ptr<> class.
*/
#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T>
using PySharedPtrClass = py::class_<T, std::shared_ptr<T> >;
#endif

/**
* @brief
* @param m The Python module
* @param parent The parent class of the wrapped class
* @return A std::shared_ptr<T> wrapped instance of the wrapped class properly 
* initialized.
*/
PySharedPtrClass<FeatureDataCSVWriter> pybind11_init_SIMPLib_FeatureDataCSVWriter(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<FeatureDataCSVWriter> instance(m, "FeatureDataCSVWriter", parent);
  instance
  .def(py::init([]()
    { 
      return FeatureDataCSVWriter::New();
    }
))
  .def_static("New", &FeatureDataCSVWriter::New)
  /* Property accessors for CellFeatureAttributeMatrixPath */
  .def_property("CellFeatureAttributeMatrixPath", &FeatureDataCSVWriter::getCellFeatureAttributeMatrixPath, &FeatureDataCSVWriter::setCellFeatureAttributeMatrixPath)
  /* Property accessors for FeatureDataFile */
  .def_property("FeatureDataFile", &FeatureDataCSVWriter::getFeatureDataFile, &FeatureDataCSVWriter::setFeatureDataFile)
  /* Property accessors for WriteNeighborListData */
  .def_property("WriteNeighborListData", &FeatureDataCSVWriter::getWriteNeighborListData, &FeatureDataCSVWriter::setWriteNeighborListData)
  /* Property accessors for DelimiterChoice */
  .def_property("DelimiterChoice", &FeatureDataCSVWriter::getDelimiterChoice, &FeatureDataCSVWriter::setDelimiterChoice)
  /* Property accessors for WriteNumFeaturesLine */
  .def_property("WriteNumFeaturesLine", &FeatureDataCSVWriter::getWriteNumFeaturesLine, &FeatureDataCSVWriter::setWriteNumFeaturesLine)
  ;
  return instance;
}



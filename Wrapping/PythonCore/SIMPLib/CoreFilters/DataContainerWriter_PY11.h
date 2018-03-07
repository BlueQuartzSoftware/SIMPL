#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class DataContainerWriter located at
* SIMPLib/CoreFilters/DataContainerWriter.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/DataContainerWriter.h"

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
PySharedPtrClass<DataContainerWriter> pybind11_init_simpl_DataContainerWriter(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<DataContainerWriter> instance(m, "DataContainerWriter", parent);
  instance
  .def(py::init([]()
    { 
      return DataContainerWriter::New();
    }
))
  .def_static("New", &DataContainerWriter::New)
  /* Property accessors for OutputFile */
  .def_property("OutputFile", &DataContainerWriter::getOutputFile, &DataContainerWriter::setOutputFile)
  /* Property accessors for WriteXdmfFile */
  .def_property("WriteXdmfFile", &DataContainerWriter::getWriteXdmfFile, &DataContainerWriter::setWriteXdmfFile)
  /* Property accessors for WriteTimeSeries */
  .def_property("WriteTimeSeries", &DataContainerWriter::getWriteTimeSeries, &DataContainerWriter::setWriteTimeSeries)
  /* Property accessors for OutputFile */
  .def_property("OutputFile", &DataContainerWriter::getOutputFile, &DataContainerWriter::setOutputFile)
  /* Property accessors for WriteXdmfFile */
  .def_property("WriteXdmfFile", &DataContainerWriter::getWriteXdmfFile, &DataContainerWriter::setWriteXdmfFile)
  /* Property accessors for WriteTimeSeries */
  .def_property("WriteTimeSeries", &DataContainerWriter::getWriteTimeSeries, &DataContainerWriter::setWriteTimeSeries)
  ;
  return instance;
}



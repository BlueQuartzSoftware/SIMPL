#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class WriteASCIIData located at
* SIMPLib/CoreFilters/WriteASCIIData.h. 
* The Python bindings are created using Pybind11.
* @date Tue Mar 6 17:07:40 2018
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/WriteASCIIData.h"

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
PySharedPtrClass<WriteASCIIData> pybind11_init_SIMPLib_WriteASCIIData(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<WriteASCIIData> instance(m, "WriteASCIIData", parent);
  instance
  .def(py::init([]()
    { 
      return WriteASCIIData::New();
    }
))
  .def_static("New", &WriteASCIIData::New)
  /* Property accessors for SelectedDataArrayPaths */
  .def_property("SelectedDataArrayPaths", &WriteASCIIData::getSelectedDataArrayPaths, &WriteASCIIData::setSelectedDataArrayPaths)
  /* Property accessors for OutputPath */
  .def_property("OutputPath", &WriteASCIIData::getOutputPath, &WriteASCIIData::setOutputPath)
  /* Property accessors for Delimiter */
  .def_property("Delimiter", &WriteASCIIData::getDelimiter, &WriteASCIIData::setDelimiter)
  /* Property accessors for FileExtension */
  .def_property("FileExtension", &WriteASCIIData::getFileExtension, &WriteASCIIData::setFileExtension)
  /* Property accessors for MaxValPerLine */
  .def_property("MaxValPerLine", &WriteASCIIData::getMaxValPerLine, &WriteASCIIData::setMaxValPerLine)
  ;
  return instance;
}



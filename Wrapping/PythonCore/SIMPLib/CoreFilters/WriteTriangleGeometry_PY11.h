#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class WriteTriangleGeometry located at
* SIMPLib/CoreFilters/WriteTriangleGeometry.h. 
* The Python bindings are created using Pybind11.
* @date Tue Mar 6 17:07:40 2018
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/WriteTriangleGeometry.h"

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
PySharedPtrClass<WriteTriangleGeometry> pybind11_init_SIMPLib_WriteTriangleGeometry(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<WriteTriangleGeometry> instance(m, "WriteTriangleGeometry", parent);
  instance
  .def(py::init([]()
    { 
      return WriteTriangleGeometry::New();
    }
))
  .def_static("New", &WriteTriangleGeometry::New)
  /* Property accessors for DataContainerSelection */
  .def_property("DataContainerSelection", &WriteTriangleGeometry::getDataContainerSelection, &WriteTriangleGeometry::setDataContainerSelection)
  /* Property accessors for OutputNodesFile */
  .def_property("OutputNodesFile", &WriteTriangleGeometry::getOutputNodesFile, &WriteTriangleGeometry::setOutputNodesFile)
  /* Property accessors for OutputTrianglesFile */
  .def_property("OutputTrianglesFile", &WriteTriangleGeometry::getOutputTrianglesFile, &WriteTriangleGeometry::setOutputTrianglesFile)
  ;
  return instance;
}



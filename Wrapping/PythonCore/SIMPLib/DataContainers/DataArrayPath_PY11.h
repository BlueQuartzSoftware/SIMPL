#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class DataArrayPath located at
* SIMPLib/DataContainers/DataArrayPath.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/DataContainers/DataArrayPath.h"


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
* @return A std::shared_ptr<T> wrapped instance of the wrapped class properly 
* initialized.
*/
PySharedPtrClass<DataArrayPath> pybind11_init_SIMPLib_DataArrayPath(py::module &m)
{
  PySharedPtrClass<DataArrayPath> instance(m, "DataArrayPath");
  instance
  .def(py::init<DataArrayPath>())
  .def(py::init<DataArrayPath const &>())
  ;
  return instance;
}



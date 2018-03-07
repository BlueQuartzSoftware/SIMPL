#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class DataContainerArrayProxy located at
* SIMPLib/DataContainers/DataContainerArrayProxy.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"


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
PySharedPtrClass<DataContainerArrayProxy> pybind11_init_simpl_DataContainerArrayProxy(py::module &m)
{
  PySharedPtrClass<DataContainerArrayProxy> instance(m, "DataContainerArrayProxy");
  instance
  .def(py::init<DataContainerArrayProxy>())
  .def(py::init<DataContainerArrayProxy const &>())
  /* Number of costructors: 0*/
  ;
  return instance;
}



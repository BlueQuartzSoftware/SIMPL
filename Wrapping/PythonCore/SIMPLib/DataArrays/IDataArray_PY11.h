#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class IDataArray located at
* SIMPLib/DataArrays/IDataArray.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/DataArrays/IDataArray.h"


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
PySharedPtrClass<IDataArray> pybind11_init_simpl_IDataArray(py::module &m)
{
  PySharedPtrClass<IDataArray> instance(m, "IDataArray");
  instance
  /* Property accessors for Name */
  .def_property("Name", &IDataArray::getName, &IDataArray::setName)
  ;
  return instance;
}



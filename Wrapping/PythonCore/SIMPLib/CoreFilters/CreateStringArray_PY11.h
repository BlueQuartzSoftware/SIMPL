#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class CreateStringArray located at
* SIMPLib/CoreFilters/CreateStringArray.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/CreateStringArray.h"

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
PySharedPtrClass<CreateStringArray> pybind11_init_SIMPLib_CreateStringArray(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<CreateStringArray> instance(m, "CreateStringArray", parent);
  instance
  .def(py::init([]()
    { 
      return CreateStringArray::New();
    }
))
  .def_static("New", &CreateStringArray::New)
  /* Property accessors for NumberOfComponents */
  .def_property("NumberOfComponents", &CreateStringArray::getNumberOfComponents, &CreateStringArray::setNumberOfComponents)
  /* Property accessors for NewArray */
  .def_property("NewArray", &CreateStringArray::getNewArray, &CreateStringArray::setNewArray)
  /* Property accessors for InitializationValue */
  .def_property("InitializationValue", &CreateStringArray::getInitializationValue, &CreateStringArray::setInitializationValue)
  ;
  return instance;
}



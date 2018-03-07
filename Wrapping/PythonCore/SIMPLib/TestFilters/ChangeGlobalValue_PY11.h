#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class ChangeGlobalValue located at
* SIMPLib/TestFilters/ChangeGlobalValue.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/TestFilters/ChangeGlobalValue.h"

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
PySharedPtrClass<ChangeGlobalValue> pybind11_init_SIMPLib_ChangeGlobalValue(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<ChangeGlobalValue> instance(m, "ChangeGlobalValue", parent);
  instance
  .def(py::init([]()
    { 
      return ChangeGlobalValue::New();
    }
))
  .def_static("New", &ChangeGlobalValue::New)
  /* Property accessors for Value */
  .def_property("Value", &ChangeGlobalValue::getValue, &ChangeGlobalValue::setValue)
  /* Property accessors for GlobalValue */
  .def_property("GlobalValue", &ChangeGlobalValue::getGlobalValue, &ChangeGlobalValue::setGlobalValue)
  ;
  return instance;
}



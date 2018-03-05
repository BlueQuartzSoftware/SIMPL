#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class ConditionalSetValue located at
* SIMPLib/CoreFilters/ConditionalSetValue.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/ConditionalSetValue.h"

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
PySharedPtrClass<ConditionalSetValue> pybind11_init_SIMPLib_ConditionalSetValue(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<ConditionalSetValue> instance(m, "ConditionalSetValue", parent);
  instance
  .def(py::init([]()
    { 
      return ConditionalSetValue::New();
    }
))
  .def_static("New", &ConditionalSetValue::New)
  /* Property accessors for SelectedArrayPath */
  .def_property("SelectedArrayPath", &ConditionalSetValue::getSelectedArrayPath, &ConditionalSetValue::setSelectedArrayPath)
  /* Property accessors for ConditionalArrayPath */
  .def_property("ConditionalArrayPath", &ConditionalSetValue::getConditionalArrayPath, &ConditionalSetValue::setConditionalArrayPath)
  /* Property accessors for ReplaceValue */
  .def_property("ReplaceValue", &ConditionalSetValue::getReplaceValue, &ConditionalSetValue::setReplaceValue)
  ;
  return instance;
}



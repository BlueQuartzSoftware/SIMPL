#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class ReplaceValueInArray located at
* SIMPLib/CoreFilters/ReplaceValueInArray.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/ReplaceValueInArray.h"

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
PySharedPtrClass<ReplaceValueInArray> pybind11_init_SIMPLib_ReplaceValueInArray(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<ReplaceValueInArray> instance(m, "ReplaceValueInArray", parent);
  instance
  .def(py::init([]()
    { 
      return ReplaceValueInArray::New();
    }
))
  .def_static("New", &ReplaceValueInArray::New)
  /* Property accessors for SelectedArray */
  .def_property("SelectedArray", &ReplaceValueInArray::getSelectedArray, &ReplaceValueInArray::setSelectedArray)
  /* Property accessors for RemoveValue */
  .def_property("RemoveValue", &ReplaceValueInArray::getRemoveValue, &ReplaceValueInArray::setRemoveValue)
  /* Property accessors for ReplaceValue */
  .def_property("ReplaceValue", &ReplaceValueInArray::getReplaceValue, &ReplaceValueInArray::setReplaceValue)
  ;
  return instance;
}



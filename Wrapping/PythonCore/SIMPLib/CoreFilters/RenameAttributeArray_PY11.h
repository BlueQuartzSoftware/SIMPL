#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class RenameAttributeArray located at
* SIMPLib/CoreFilters/RenameAttributeArray.h. 
* The Python bindings are created using Pybind11.
* @date Tue Mar 6 17:07:40 2018
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/RenameAttributeArray.h"

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
PySharedPtrClass<RenameAttributeArray> pybind11_init_SIMPLib_RenameAttributeArray(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<RenameAttributeArray> instance(m, "RenameAttributeArray", parent);
  instance
  .def(py::init([]()
    { 
      return RenameAttributeArray::New();
    }
))
  .def_static("New", &RenameAttributeArray::New)
  /* Property accessors for SelectedArrayPath */
  .def_property("SelectedArrayPath", &RenameAttributeArray::getSelectedArrayPath, &RenameAttributeArray::setSelectedArrayPath)
  /* Property accessors for NewArrayName */
  .def_property("NewArrayName", &RenameAttributeArray::getNewArrayName, &RenameAttributeArray::setNewArrayName)
  ;
  return instance;
}



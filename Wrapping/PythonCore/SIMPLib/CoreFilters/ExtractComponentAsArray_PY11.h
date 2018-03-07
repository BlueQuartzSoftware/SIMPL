#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class ExtractComponentAsArray located at
* SIMPLib/CoreFilters/ExtractComponentAsArray.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/ExtractComponentAsArray.h"

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
PySharedPtrClass<ExtractComponentAsArray> pybind11_init_SIMPLib_ExtractComponentAsArray(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<ExtractComponentAsArray> instance(m, "ExtractComponentAsArray", parent);
  instance
  .def(py::init([]()
    { 
      return ExtractComponentAsArray::New();
    }
))
  .def_static("New", &ExtractComponentAsArray::New)
  /* Property accessors for SelectedArrayPath */
  .def_property("SelectedArrayPath", &ExtractComponentAsArray::getSelectedArrayPath, &ExtractComponentAsArray::setSelectedArrayPath)
  /* Property accessors for CompNumber */
  .def_property("CompNumber", &ExtractComponentAsArray::getCompNumber, &ExtractComponentAsArray::setCompNumber)
  /* Property accessors for NewArrayArrayName */
  .def_property("NewArrayArrayName", &ExtractComponentAsArray::getNewArrayArrayName, &ExtractComponentAsArray::setNewArrayArrayName)
  ;
  return instance;
}



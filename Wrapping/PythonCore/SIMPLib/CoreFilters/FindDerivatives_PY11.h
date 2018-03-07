#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class FindDerivatives located at
* SIMPLib/CoreFilters/FindDerivatives.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/FindDerivatives.h"

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
PySharedPtrClass<FindDerivatives> pybind11_init_simpl_FindDerivatives(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<FindDerivatives> instance(m, "FindDerivatives", parent);
  instance
  .def(py::init([]()
    { 
      return FindDerivatives::New();
    }
))
  .def_static("New", &FindDerivatives::New)
  /* Property accessors for SelectedArrayPath */
  .def_property("SelectedArrayPath", &FindDerivatives::getSelectedArrayPath, &FindDerivatives::setSelectedArrayPath)
  /* Property accessors for DerivativesArrayPath */
  .def_property("DerivativesArrayPath", &FindDerivatives::getDerivativesArrayPath, &FindDerivatives::setDerivativesArrayPath)
  ;
  return instance;
}



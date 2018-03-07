#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class ErrorWarningFilter located at
* SIMPLib/TestFilters/ErrorWarningFilter.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/TestFilters/ErrorWarningFilter.h"

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
PySharedPtrClass<ErrorWarningFilter> pybind11_init_SIMPLib_ErrorWarningFilter(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<ErrorWarningFilter> instance(m, "ErrorWarningFilter", parent);
  instance
  .def(py::init([]()
    { 
      return ErrorWarningFilter::New();
    }
))
  .def_static("New", &ErrorWarningFilter::New)
  /* Property accessors for PreflightWarning */
  .def_property("PreflightWarning", &ErrorWarningFilter::getPreflightWarning, &ErrorWarningFilter::setPreflightWarning)
  /* Property accessors for PreflightError */
  .def_property("PreflightError", &ErrorWarningFilter::getPreflightError, &ErrorWarningFilter::setPreflightError)
  /* Property accessors for ExecuteWarning */
  .def_property("ExecuteWarning", &ErrorWarningFilter::getExecuteWarning, &ErrorWarningFilter::setExecuteWarning)
  /* Property accessors for ExecuteError */
  .def_property("ExecuteError", &ErrorWarningFilter::getExecuteError, &ErrorWarningFilter::setExecuteError)
  /* Property accessors for PropertyError */
  .def_property("PropertyError", &ErrorWarningFilter::getPropertyError, &ErrorWarningFilter::setPropertyError)
  ;
  return instance;
}



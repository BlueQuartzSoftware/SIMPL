#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class ErrorWarningFilter located at
 * SIMPLib/TestFilters/ErrorWarningFilter.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.179
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
template <typename T> using PySharedPtrClass = py::class_<T, std::shared_ptr<T>>;
#endif

/**
 * @brief This allows filters declared in a plugin to be wrapped
 */
#ifndef PyAbstractFilterSharedPtrClass_TEMPLATE
#define PyAbstractFilterSharedPtrClass_TEMPLATE
template <typename T> using PyAbstractFilterSharedPtrClass = py::class_<T, AbstractFilter, std::shared_ptr<T>>;
#endif

/**
 * @brief
 * @param m The Python module
 * @param parent The parent class of the wrapped class
 * @return A std::shared_ptr<T> wrapped instance of the wrapped class properly
 * initialized.
 */
PySharedPtrClass<ErrorWarningFilter> pybind11_init_SIMPLib_ErrorWarningFilter(py::module& m
#ifdef simpl_EXPORTS
                                                                              ,
                                                                              PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<ErrorWarningFilter> instance(m, "ErrorWarningFilter", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<ErrorWarningFilter> instance(m, "ErrorWarningFilter");
#endif

  instance
      /* Property accessors for PreflightWarning */
      .def_property("PreflightWarning", &ErrorWarningFilter::getPreflightWarning, &ErrorWarningFilter::setPreflightWarning)
      /* Property accessors for PreflightError */
      .def_property("PreflightError", &ErrorWarningFilter::getPreflightError, &ErrorWarningFilter::setPreflightError)
      /* Property accessors for ExecuteWarning */
      .def_property("ExecuteWarning", &ErrorWarningFilter::getExecuteWarning, &ErrorWarningFilter::setExecuteWarning)
      /* Property accessors for ExecuteError */
      .def_property("ExecuteError", &ErrorWarningFilter::getExecuteError, &ErrorWarningFilter::setExecuteError)
      /* Property accessors for PropertyError */
      .def_property("PropertyError", &ErrorWarningFilter::getPropertyError, &ErrorWarningFilter::setPropertyError);

  /* Return the instance */
  return instance;
}

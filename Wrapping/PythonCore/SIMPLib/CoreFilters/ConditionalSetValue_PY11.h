#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class ConditionalSetValue located at
 * SIMPLib/CoreFilters/ConditionalSetValue.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.090
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
PySharedPtrClass<ConditionalSetValue> pybind11_init_SIMPLib_ConditionalSetValue(py::module& m
#ifdef simpl_EXPORTS
                                                                                ,
                                                                                PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<ConditionalSetValue> instance(m, "ConditionalSetValue", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<ConditionalSetValue> instance(m, "ConditionalSetValue");
#endif

  instance
      /* Property accessors for SelectedArrayPath */
      .def_property("SelectedArrayPath", &ConditionalSetValue::getSelectedArrayPath, &ConditionalSetValue::setSelectedArrayPath)
      /* Property accessors for ConditionalArrayPath */
      .def_property("ConditionalArrayPath", &ConditionalSetValue::getConditionalArrayPath, &ConditionalSetValue::setConditionalArrayPath)
      /* Property accessors for ReplaceValue */
      .def_property("ReplaceValue", &ConditionalSetValue::getReplaceValue, &ConditionalSetValue::setReplaceValue);

  /* Return the instance */
  return instance;
}

#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class ArraySelectionExample located at
 * SIMPLib/TestFilters/ArraySelectionExample.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.177
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/TestFilters/ArraySelectionExample.h"

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
PySharedPtrClass<ArraySelectionExample> pybind11_init_SIMPLib_ArraySelectionExample(py::module& m
#ifdef simpl_EXPORTS
                                                                                    ,
                                                                                    PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<ArraySelectionExample> instance(m, "ArraySelectionExample", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<ArraySelectionExample> instance(m, "ArraySelectionExample");
#endif

  instance
      /* Property accessors for DataContainerArrayProxy */
      .def_property("DataContainerArrayProxy", &ArraySelectionExample::getDataContainerArrayProxy, &ArraySelectionExample::setDataContainerArrayProxy);

  /* Return the instance */
  return instance;
}

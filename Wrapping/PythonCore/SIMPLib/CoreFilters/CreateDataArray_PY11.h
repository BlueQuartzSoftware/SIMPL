#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class CreateDataArray located at
 * SIMPLib/CoreFilters/CreateDataArray.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.093
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/CreateDataArray.h"

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
PySharedPtrClass<CreateDataArray> pybind11_init_SIMPLib_CreateDataArray(py::module& m
#ifdef simpl_EXPORTS
                                                                        ,
                                                                        PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<CreateDataArray> instance(m, "CreateDataArray", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<CreateDataArray> instance(m, "CreateDataArray");
#endif

  instance
      /* Property accessors for ScalarType */
      .def_property("ScalarType", &CreateDataArray::getScalarType, &CreateDataArray::setScalarType)
      /* Property accessors for NumberOfComponents */
      .def_property("NumberOfComponents", &CreateDataArray::getNumberOfComponents, &CreateDataArray::setNumberOfComponents)
      /* Property accessors for NewArray */
      .def_property("NewArray", &CreateDataArray::getNewArray, &CreateDataArray::setNewArray)
      /* Property accessors for InitializationType */
      .def_property("InitializationType", &CreateDataArray::getInitializationType, &CreateDataArray::setInitializationType)
      /* Property accessors for InitializationValue */
      .def_property("InitializationValue", &CreateDataArray::getInitializationValue, &CreateDataArray::setInitializationValue)
      /* Property accessors for InitializationRange */
      .def_property("InitializationRange", &CreateDataArray::getInitializationRange, &CreateDataArray::setInitializationRange);

  /* Return the instance */
  return instance;
}

#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class ArrayCalculator located at
 * SIMPLib/CoreFilters/ArrayCalculator.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.086
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/ArrayCalculator.h"

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
PySharedPtrClass<ArrayCalculator> pybind11_init_SIMPLib_ArrayCalculator(py::module& m
#ifdef simpl_EXPORTS
                                                                        ,
                                                                        PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<ArrayCalculator> instance(m, "ArrayCalculator", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<ArrayCalculator> instance(m, "ArrayCalculator");
#endif

  instance
      /* Property accessors for SelectedAttributeMatrix */
      .def_property("SelectedAttributeMatrix", &ArrayCalculator::getSelectedAttributeMatrix, &ArrayCalculator::setSelectedAttributeMatrix)
      /* Property accessors for InfixEquation */
      .def_property("InfixEquation", &ArrayCalculator::getInfixEquation, &ArrayCalculator::setInfixEquation)
      /* Property accessors for CalculatedArray */
      .def_property("CalculatedArray", &ArrayCalculator::getCalculatedArray, &ArrayCalculator::setCalculatedArray)
      /* Property accessors for Units */
      .def_property("Units", &ArrayCalculator::getUnits, &ArrayCalculator::setUnits);

  /* Return the instance */
  return instance;
}

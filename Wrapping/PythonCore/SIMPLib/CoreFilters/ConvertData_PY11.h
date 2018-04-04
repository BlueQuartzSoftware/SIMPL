#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class ConvertData located at
 * SIMPLib/CoreFilters/ConvertData.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.090
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/ConvertData.h"

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
PySharedPtrClass<ConvertData> pybind11_init_SIMPLib_ConvertData(py::module& m
#ifdef simpl_EXPORTS
                                                                ,
                                                                PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<ConvertData> instance(m, "ConvertData", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<ConvertData> instance(m, "ConvertData");
#endif

  instance
      /* Property accessors for ScalarType */
      .def_property("ScalarType", &ConvertData::getScalarType, &ConvertData::setScalarType)
      /* Property accessors for OutputArrayName */
      .def_property("OutputArrayName", &ConvertData::getOutputArrayName, &ConvertData::setOutputArrayName)
      /* Property accessors for SelectedCellArrayPath */
      .def_property("SelectedCellArrayPath", &ConvertData::getSelectedCellArrayPath, &ConvertData::setSelectedCellArrayPath);

  /* Return the instance */
  return instance;
}

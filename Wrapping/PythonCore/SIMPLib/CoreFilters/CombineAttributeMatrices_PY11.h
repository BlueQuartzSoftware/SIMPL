#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class CombineAttributeMatrices located at
 * SIMPLib/CoreFilters/CombineAttributeMatrices.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.089
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/CombineAttributeMatrices.h"

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
PySharedPtrClass<CombineAttributeMatrices> pybind11_init_SIMPLib_CombineAttributeMatrices(py::module& m
#ifdef simpl_EXPORTS
                                                                                          ,
                                                                                          PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<CombineAttributeMatrices> instance(m, "CombineAttributeMatrices", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<CombineAttributeMatrices> instance(m, "CombineAttributeMatrices");
#endif

  instance
      /* Property accessors for FirstAttributeMatrixPath */
      .def_property("FirstAttributeMatrixPath", &CombineAttributeMatrices::getFirstAttributeMatrixPath, &CombineAttributeMatrices::setFirstAttributeMatrixPath)
      /* Property accessors for SecondAttributeMatrixPath */
      .def_property("SecondAttributeMatrixPath", &CombineAttributeMatrices::getSecondAttributeMatrixPath, &CombineAttributeMatrices::setSecondAttributeMatrixPath)
      /* Property accessors for FirstIndexArrayPath */
      .def_property("FirstIndexArrayPath", &CombineAttributeMatrices::getFirstIndexArrayPath, &CombineAttributeMatrices::setFirstIndexArrayPath)
      /* Property accessors for SecondIndexArrayPath */
      .def_property("SecondIndexArrayPath", &CombineAttributeMatrices::getSecondIndexArrayPath, &CombineAttributeMatrices::setSecondIndexArrayPath)
      /* Property accessors for CombinedAttributeMatrixName */
      .def_property("CombinedAttributeMatrixName", &CombineAttributeMatrices::getCombinedAttributeMatrixName, &CombineAttributeMatrices::setCombinedAttributeMatrixName)
      /* Property accessors for NewIndexArrayName */
      .def_property("NewIndexArrayName", &CombineAttributeMatrices::getNewIndexArrayName, &CombineAttributeMatrices::setNewIndexArrayName);

  /* Return the instance */
  return instance;
}

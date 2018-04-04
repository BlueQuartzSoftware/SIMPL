#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class RenameAttributeMatrix located at
 * SIMPLib/CoreFilters/RenameAttributeMatrix.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.112
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/RenameAttributeMatrix.h"

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
PySharedPtrClass<RenameAttributeMatrix> pybind11_init_SIMPLib_RenameAttributeMatrix(py::module& m
#ifdef simpl_EXPORTS
                                                                                    ,
                                                                                    PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<RenameAttributeMatrix> instance(m, "RenameAttributeMatrix", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<RenameAttributeMatrix> instance(m, "RenameAttributeMatrix");
#endif

  instance
      /* Property accessors for SelectedAttributeMatrixPath */
      .def_property("SelectedAttributeMatrixPath", &RenameAttributeMatrix::getSelectedAttributeMatrixPath, &RenameAttributeMatrix::setSelectedAttributeMatrixPath)
      /* Property accessors for NewAttributeMatrix */
      .def_property("NewAttributeMatrix", &RenameAttributeMatrix::getNewAttributeMatrix, &RenameAttributeMatrix::setNewAttributeMatrix);

  /* Return the instance */
  return instance;
}

#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class WriteTriangleGeometry located at
 * SIMPLib/CoreFilters/WriteTriangleGeometry.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.116
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/WriteTriangleGeometry.h"

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
PySharedPtrClass<WriteTriangleGeometry> pybind11_init_SIMPLib_WriteTriangleGeometry(py::module& m
#ifdef simpl_EXPORTS
                                                                                    ,
                                                                                    PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<WriteTriangleGeometry> instance(m, "WriteTriangleGeometry", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<WriteTriangleGeometry> instance(m, "WriteTriangleGeometry");
#endif

  instance
      /* Property accessors for DataContainerSelection */
      .def_property("DataContainerSelection", &WriteTriangleGeometry::getDataContainerSelection, &WriteTriangleGeometry::setDataContainerSelection)
      /* Property accessors for OutputNodesFile */
      .def_property("OutputNodesFile", &WriteTriangleGeometry::getOutputNodesFile, &WriteTriangleGeometry::setOutputNodesFile)
      /* Property accessors for OutputTrianglesFile */
      .def_property("OutputTrianglesFile", &WriteTriangleGeometry::getOutputTrianglesFile, &WriteTriangleGeometry::setOutputTrianglesFile);

  /* Return the instance */
  return instance;
}

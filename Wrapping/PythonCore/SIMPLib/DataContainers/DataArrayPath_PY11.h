#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class DataArrayPath located at
 * SIMPLib/DataContainers/DataArrayPath.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.123
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/DataContainers/DataArrayPath.h"

/**
 * @brief This defines a C++11 alias so loading the file by itself into an IDE
 * will allow the proper code completion for a wrapped std::shared_ptr<> class.
 */
#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T> using PySharedPtrClass = py::class_<T, std::shared_ptr<T>>;
#endif

/**
 * @brief
 * @param m The Python module
 * @return A std::shared_ptr<T> wrapped instance of the wrapped class properly
 * initialized.
 */
PySharedPtrClass<DataArrayPath> pybind11_init_SIMPLib_DataArrayPath(py::module& m)
{
  PySharedPtrClass<DataArrayPath> instance(m, "DataArrayPath");
  instance.def(py::init<DataArrayPath>())
      .def(py::init<DataArrayPath const&>())
      /* Number of costructors: 1*/
      .def(py::init<const QString&, const QString&, const QString&>())
      /* Property accessors for DataContainerName */
      .def_property("DataContainerName", &DataArrayPath::getDataContainerName, &DataArrayPath::setDataContainerName)
      /* Property accessors for AttributeMatrixName */
      .def_property("AttributeMatrixName", &DataArrayPath::getAttributeMatrixName, &DataArrayPath::setAttributeMatrixName)
      /* Property accessors for DataArrayName */
      .def_property("DataArrayName", &DataArrayPath::getDataArrayName, &DataArrayPath::setDataArrayName)
      /* Class instance method isEmpty */
      .def("isEmpty", &DataArrayPath::isEmpty)
      /* Class instance method isValid */
      .def("isValid", &DataArrayPath::isValid)
      /* Class instance method update */
      .def("update", &DataArrayPath::update, py::arg("dcName"), py::arg("amName"), py::arg("daName"));

  /* Return the instance */
  return instance;
}

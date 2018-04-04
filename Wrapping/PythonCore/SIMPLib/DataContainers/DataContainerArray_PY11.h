#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class DataContainerArray located at
 * SIMPLib/DataContainers/DataContainerArray.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.126
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/DataContainers/DataContainerArray.h"

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
 * @return A std::shared_ptr<T> wrapped instance of the wrapped class properly
 * initialized.
 */
PySharedPtrClass<DataContainerArray> pybind11_init_SIMPLib_DataContainerArray(py::module& m)
{
  PySharedPtrClass<DataContainerArray> instance(m, "DataContainerArray");
  instance.def(py::init([]() { return DataContainerArray::New(); }))
      .def_static("New", &DataContainerArray::New)
      /* Class instance method addDataContainer */
      .def("addDataContainer", &DataContainerArray::addDataContainer, py::arg("data_container"))
      /* Class instance method doesDataContainerExist */
      .def("doesDataContainerExist", &DataContainerArray::doesDataContainerExist, py::arg("name"))
      /* Class instance method removeDataContainer */
      .def("removeDataContainer", &DataContainerArray::removeDataContainer, py::arg("name"))
      /* Class instance method renameDataContainer */
      .def("renameDataContainer", &DataContainerArray::renameDataContainer, py::arg("oldName"), py::arg("newName"))
      /* Class instance method clearDataContainers */
      .def("clearDataContainers", &DataContainerArray::clearDataContainers)
      /* Class instance method getNumDataContainers */
      .def("getNumDataContainers", &DataContainerArray::getNumDataContainers)
      /* Class instance method duplicateDataContainer */
      .def("duplicateDataContainer", &DataContainerArray::duplicateDataContainer, py::arg("oldName,"), py::arg("newName"))
      /* Class instance method getAttributeMatrix */
      .def("getAttributeMatrix", &DataContainerArray::getAttributeMatrix, py::arg("dataArrayPath"))
      /* Class instance method doesAttributeMatrixExist */
      .def("doesAttributeMatrixExist", &DataContainerArray::doesAttributeMatrixExist, py::arg("dataArrayPath"))
      /* Class instance method doesAttributeArrayExist */
      .def("doesAttributeArrayExist", &DataContainerArray::doesAttributeArrayExist, py::arg("dataArrayPath"));

  /* Return the instance */
  return instance;
}

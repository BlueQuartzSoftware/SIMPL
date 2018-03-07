#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class DataContainer located at
* SIMPLib/DataContainers/DataContainer.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/DataContainers/DataContainer.h"


/**
* @brief This defines a C++11 alias so loading the file by itself into an IDE
* will allow the proper code completion for a wrapped std::shared_ptr<> class.
*/
#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T>
using PySharedPtrClass = py::class_<T, std::shared_ptr<T> >;
#endif

/**
* @brief
* @param m The Python module
* @return A std::shared_ptr<T> wrapped instance of the wrapped class properly 
* initialized.
*/
PySharedPtrClass<DataContainer> pybind11_init_SIMPLib_DataContainer(py::module &m)
{
  PySharedPtrClass<DataContainer> instance(m, "DataContainer");
  instance
  .def(py::init([](QString var_0) {
      return DataContainer::New(var_0);
    }))
  .def_static("New", &DataContainer::New)
  /* Property accessors for Name */
  .def_property("Name", &DataContainer::getName, &DataContainer::setName)
  /* Class instance method getInfoString */
  .def("getInfoString", &DataContainer::getInfoString, 
        py::arg("InfoStringFormat")
      )
  /* Class instance method addAttributeMatrix */
  .def("addAttributeMatrix", &DataContainer::addAttributeMatrix, 
        py::arg("Name"), 
        py::arg("AttributeMatrix")
      )
  /* Class instance method renameAttributeMatrix */
  .def("renameAttributeMatrix", &DataContainer::renameAttributeMatrix, 
        py::arg("OldName"), 
        py::arg("NewName"), 
        py::arg("OverWrite")
      )
  /* Class instance method doesAttributeMatrixExist */
  .def("doesAttributeMatrixExist", &DataContainer::doesAttributeMatrixExist, 
        py::arg("Name")
      )
  ;
  return instance;
}



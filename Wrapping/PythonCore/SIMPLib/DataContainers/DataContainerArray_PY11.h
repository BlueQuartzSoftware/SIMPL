#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class DataContainerArray located at
* SIMPLib/DataContainers/DataContainerArray.h. 
* The Python bindings are created using Pybind11.
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
template <typename T>
using PySharedPtrClass = py::class_<T, std::shared_ptr<T> >;
#endif

/**
* @brief
* @param m The Python module
* @return A std::shared_ptr<T> wrapped instance of the wrapped class properly 
* initialized.
*/
PySharedPtrClass<DataContainerArray> pybind11_init_SIMPLib_DataContainerArray(py::module &m)
{
  PySharedPtrClass<DataContainerArray> instance(m, "DataContainerArray");
  instance
  .def(py::init([]()
    { 
      return DataContainerArray::New();
    }
))
  .def_static("New", &DataContainerArray::New)
  /* Class instance method addDataContainer */
  .def("addDataContainer", &DataContainerArray::addDataContainer, 
        py::arg("data_container")
      )
  ;
  return instance;
}



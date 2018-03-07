#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class CreateDataContainer located at
* SIMPLib/CoreFilters/CreateDataContainer.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/CreateDataContainer.h"

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
* @param parent The parent class of the wrapped class
* @return A std::shared_ptr<T> wrapped instance of the wrapped class properly 
* initialized.
*/
PySharedPtrClass<CreateDataContainer> pybind11_init_simpl_CreateDataContainer(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<CreateDataContainer> instance(m, "CreateDataContainer", parent);
  instance
  .def(py::init([]()
    { 
      return CreateDataContainer::New();
    }
))
  .def_static("New", &CreateDataContainer::New)
  /* Property accessors for CreatedDataContainer */
  .def_property("CreatedDataContainer", &CreateDataContainer::getCreatedDataContainer, &CreateDataContainer::setCreatedDataContainer)
  ;
  return instance;
}



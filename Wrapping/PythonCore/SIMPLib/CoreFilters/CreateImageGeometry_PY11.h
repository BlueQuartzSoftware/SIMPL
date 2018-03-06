#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class CreateImageGeometry located at
* SIMPLib/CoreFilters/CreateImageGeometry.h. 
* The Python bindings are created using Pybind11.
* @date Tue Mar 6 17:07:40 2018
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/CreateImageGeometry.h"

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
PySharedPtrClass<CreateImageGeometry> pybind11_init_SIMPLib_CreateImageGeometry(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<CreateImageGeometry> instance(m, "CreateImageGeometry", parent);
  instance
  .def(py::init([]()
    { 
      return CreateImageGeometry::New();
    }
))
  .def_static("New", &CreateImageGeometry::New)
  /* Property accessors for SelectedDataContainer */
  .def_property("SelectedDataContainer", &CreateImageGeometry::getSelectedDataContainer, &CreateImageGeometry::setSelectedDataContainer)
  /* Property accessors for Dimensions */
  .def_property("Dimensions", &CreateImageGeometry::getDimensions, &CreateImageGeometry::setDimensions)
  /* Property accessors for Origin */
  .def_property("Origin", &CreateImageGeometry::getOrigin, &CreateImageGeometry::setOrigin)
  /* Property accessors for Resolution */
  .def_property("Resolution", &CreateImageGeometry::getResolution, &CreateImageGeometry::setResolution)
  ;
  return instance;
}



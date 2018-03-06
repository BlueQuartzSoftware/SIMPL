#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class RequiredZThickness located at
* SIMPLib/CoreFilters/RequiredZThickness.h. 
* The Python bindings are created using Pybind11.
* @date Tue Mar 6 17:07:40 2018
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/RequiredZThickness.h"

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
PySharedPtrClass<RequiredZThickness> pybind11_init_SIMPLib_RequiredZThickness(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<RequiredZThickness> instance(m, "RequiredZThickness", parent);
  instance
  .def(py::init([]()
    { 
      return RequiredZThickness::New();
    }
))
  .def_static("New", &RequiredZThickness::New)
  /* Property accessors for DataContainerSelection */
  .def_property("DataContainerSelection", &RequiredZThickness::getDataContainerSelection, &RequiredZThickness::setDataContainerSelection)
  /* Property accessors for NumZVoxels */
  .def_property("NumZVoxels", &RequiredZThickness::getNumZVoxels, &RequiredZThickness::setNumZVoxels)
  /* Property accessors for PreflightCheck */
  .def_property("PreflightCheck", &RequiredZThickness::getPreflightCheck, &RequiredZThickness::setPreflightCheck)
  ;
  return instance;
}



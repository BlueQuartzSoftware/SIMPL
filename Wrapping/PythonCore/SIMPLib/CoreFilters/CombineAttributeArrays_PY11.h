#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class CombineAttributeArrays located at
* SIMPLib/CoreFilters/CombineAttributeArrays.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/CombineAttributeArrays.h"

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
PySharedPtrClass<CombineAttributeArrays> pybind11_init_simpl_CombineAttributeArrays(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<CombineAttributeArrays> instance(m, "CombineAttributeArrays", parent);
  instance
  .def(py::init([]()
    { 
      return CombineAttributeArrays::New();
    }
))
  .def_static("New", &CombineAttributeArrays::New)
  /* Property accessors for SelectedDataArrayPaths */
  .def_property("SelectedDataArrayPaths", &CombineAttributeArrays::getSelectedDataArrayPaths, &CombineAttributeArrays::setSelectedDataArrayPaths)
  /* Property accessors for StackedDataArrayName */
  .def_property("StackedDataArrayName", &CombineAttributeArrays::getStackedDataArrayName, &CombineAttributeArrays::setStackedDataArrayName)
  /* Property accessors for NormalizeData */
  .def_property("NormalizeData", &CombineAttributeArrays::getNormalizeData, &CombineAttributeArrays::setNormalizeData)
  ;
  return instance;
}



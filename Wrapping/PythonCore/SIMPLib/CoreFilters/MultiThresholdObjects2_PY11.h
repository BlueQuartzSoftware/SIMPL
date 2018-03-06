#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class MultiThresholdObjects2 located at
* SIMPLib/CoreFilters/MultiThresholdObjects2.h. 
* The Python bindings are created using Pybind11.
* @date Tue Mar 6 17:07:40 2018
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/MultiThresholdObjects2.h"

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
PySharedPtrClass<MultiThresholdObjects2> pybind11_init_SIMPLib_MultiThresholdObjects2(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<MultiThresholdObjects2> instance(m, "MultiThresholdObjects2", parent);
  instance
  .def(py::init([]()
    { 
      return MultiThresholdObjects2::New();
    }
))
  .def_static("New", &MultiThresholdObjects2::New)
  /* Property accessors for DestinationArrayName */
  .def_property("DestinationArrayName", &MultiThresholdObjects2::getDestinationArrayName, &MultiThresholdObjects2::setDestinationArrayName)
  /* Property accessors for SelectedThresholds */
  .def_property("SelectedThresholds", &MultiThresholdObjects2::getSelectedThresholds, &MultiThresholdObjects2::setSelectedThresholds)
  ;
  return instance;
}



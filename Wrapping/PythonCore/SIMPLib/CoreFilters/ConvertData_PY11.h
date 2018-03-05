#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class ConvertData located at
* SIMPLib/CoreFilters/ConvertData.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/ConvertData.h"

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
PySharedPtrClass<ConvertData> pybind11_init_SIMPLib_ConvertData(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<ConvertData> instance(m, "ConvertData", parent);
  instance
  .def(py::init([]()
    { 
      return ConvertData::New();
    }
))
  .def_static("New", &ConvertData::New)
  /* Property accessors for ScalarType */
  .def_property("ScalarType", &ConvertData::getScalarType, &ConvertData::setScalarType)
  /* Property accessors for OutputArrayName */
  .def_property("OutputArrayName", &ConvertData::getOutputArrayName, &ConvertData::setOutputArrayName)
  /* Property accessors for SelectedCellArrayPath */
  .def_property("SelectedCellArrayPath", &ConvertData::getSelectedCellArrayPath, &ConvertData::setSelectedCellArrayPath)
  ;
  return instance;
}



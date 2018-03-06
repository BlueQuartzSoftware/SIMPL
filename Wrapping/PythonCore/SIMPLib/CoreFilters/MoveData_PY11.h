#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class MoveData located at
* SIMPLib/CoreFilters/MoveData.h. 
* The Python bindings are created using Pybind11.
* @date Tue Mar 6 17:07:40 2018
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/MoveData.h"

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
PySharedPtrClass<MoveData> pybind11_init_SIMPLib_MoveData(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<MoveData> instance(m, "MoveData", parent);
  instance
  .def(py::init([]()
    { 
      return MoveData::New();
    }
))
  .def_static("New", &MoveData::New)
  /* Property accessors for WhatToMove */
  .def_property("WhatToMove", &MoveData::getWhatToMove, &MoveData::setWhatToMove)
  /* Property accessors for DataContainerDestination */
  .def_property("DataContainerDestination", &MoveData::getDataContainerDestination, &MoveData::setDataContainerDestination)
  /* Property accessors for AttributeMatrixSource */
  .def_property("AttributeMatrixSource", &MoveData::getAttributeMatrixSource, &MoveData::setAttributeMatrixSource)
  /* Property accessors for AttributeMatrixDestination */
  .def_property("AttributeMatrixDestination", &MoveData::getAttributeMatrixDestination, &MoveData::setAttributeMatrixDestination)
  /* Property accessors for DataArraySource */
  .def_property("DataArraySource", &MoveData::getDataArraySource, &MoveData::setDataArraySource)
  ;
  return instance;
}



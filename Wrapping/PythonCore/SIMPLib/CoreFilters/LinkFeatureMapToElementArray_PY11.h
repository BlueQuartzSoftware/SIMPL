#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class LinkFeatureMapToElementArray located at
* SIMPLib/CoreFilters/LinkFeatureMapToElementArray.h. 
* The Python bindings are created using Pybind11.
* @date Tue Mar 6 17:07:40 2018
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/LinkFeatureMapToElementArray.h"

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
PySharedPtrClass<LinkFeatureMapToElementArray> pybind11_init_SIMPLib_LinkFeatureMapToElementArray(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<LinkFeatureMapToElementArray> instance(m, "LinkFeatureMapToElementArray", parent);
  instance
  .def(py::init([]()
    { 
      return LinkFeatureMapToElementArray::New();
    }
))
  .def_static("New", &LinkFeatureMapToElementArray::New)
  /* Property accessors for CellFeatureAttributeMatrixName */
  .def_property("CellFeatureAttributeMatrixName", &LinkFeatureMapToElementArray::getCellFeatureAttributeMatrixName, &LinkFeatureMapToElementArray::setCellFeatureAttributeMatrixName)
  /* Property accessors for SelectedCellArrayPath */
  .def_property("SelectedCellArrayPath", &LinkFeatureMapToElementArray::getSelectedCellArrayPath, &LinkFeatureMapToElementArray::setSelectedCellArrayPath)
  /* Property accessors for ActiveArrayName */
  .def_property("ActiveArrayName", &LinkFeatureMapToElementArray::getActiveArrayName, &LinkFeatureMapToElementArray::setActiveArrayName)
  ;
  return instance;
}



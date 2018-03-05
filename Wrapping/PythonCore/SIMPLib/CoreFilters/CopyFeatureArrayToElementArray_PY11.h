#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class CopyFeatureArrayToElementArray located at
* SIMPLib/CoreFilters/CopyFeatureArrayToElementArray.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/CopyFeatureArrayToElementArray.h"

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
PySharedPtrClass<CopyFeatureArrayToElementArray> pybind11_init_SIMPLib_CopyFeatureArrayToElementArray(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<CopyFeatureArrayToElementArray> instance(m, "CopyFeatureArrayToElementArray", parent);
  instance
  .def(py::init([]()
    { 
      return CopyFeatureArrayToElementArray::New();
    }
))
  .def_static("New", &CopyFeatureArrayToElementArray::New)
  /* Property accessors for SelectedFeatureArrayPath */
  .def_property("SelectedFeatureArrayPath", &CopyFeatureArrayToElementArray::getSelectedFeatureArrayPath, &CopyFeatureArrayToElementArray::setSelectedFeatureArrayPath)
  /* Property accessors for FeatureIdsArrayPath */
  .def_property("FeatureIdsArrayPath", &CopyFeatureArrayToElementArray::getFeatureIdsArrayPath, &CopyFeatureArrayToElementArray::setFeatureIdsArrayPath)
  /* Property accessors for CreatedArrayName */
  .def_property("CreatedArrayName", &CopyFeatureArrayToElementArray::getCreatedArrayName, &CopyFeatureArrayToElementArray::setCreatedArrayName)
  ;
  return instance;
}



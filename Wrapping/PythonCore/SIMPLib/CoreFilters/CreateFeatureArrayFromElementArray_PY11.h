#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class CreateFeatureArrayFromElementArray located at
* SIMPLib/CoreFilters/CreateFeatureArrayFromElementArray.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/CreateFeatureArrayFromElementArray.h"

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
PySharedPtrClass<CreateFeatureArrayFromElementArray> pybind11_init_SIMPLib_CreateFeatureArrayFromElementArray(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<CreateFeatureArrayFromElementArray> instance(m, "CreateFeatureArrayFromElementArray", parent);
  instance
  .def(py::init([]()
    { 
      return CreateFeatureArrayFromElementArray::New();
    }
))
  .def_static("New", &CreateFeatureArrayFromElementArray::New)
  /* Property accessors for CellFeatureAttributeMatrixName */
  .def_property("CellFeatureAttributeMatrixName", &CreateFeatureArrayFromElementArray::getCellFeatureAttributeMatrixName, &CreateFeatureArrayFromElementArray::setCellFeatureAttributeMatrixName)
  /* Property accessors for SelectedCellArrayPath */
  .def_property("SelectedCellArrayPath", &CreateFeatureArrayFromElementArray::getSelectedCellArrayPath, &CreateFeatureArrayFromElementArray::setSelectedCellArrayPath)
  /* Property accessors for CreatedArrayName */
  .def_property("CreatedArrayName", &CreateFeatureArrayFromElementArray::getCreatedArrayName, &CreateFeatureArrayFromElementArray::setCreatedArrayName)
  /* Property accessors for FeatureIdsArrayPath */
  .def_property("FeatureIdsArrayPath", &CreateFeatureArrayFromElementArray::getFeatureIdsArrayPath, &CreateFeatureArrayFromElementArray::setFeatureIdsArrayPath)
  ;
  return instance;
}



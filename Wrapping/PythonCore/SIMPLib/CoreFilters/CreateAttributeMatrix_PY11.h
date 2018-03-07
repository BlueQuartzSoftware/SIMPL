#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class CreateAttributeMatrix located at
* SIMPLib/CoreFilters/CreateAttributeMatrix.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/CreateAttributeMatrix.h"

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
PySharedPtrClass<CreateAttributeMatrix> pybind11_init_simpl_CreateAttributeMatrix(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<CreateAttributeMatrix> instance(m, "CreateAttributeMatrix", parent);
  instance
  .def(py::init([]()
    { 
      return CreateAttributeMatrix::New();
    }
))
  .def_static("New", &CreateAttributeMatrix::New)
  /* Property accessors for CreatedAttributeMatrix */
  .def_property("CreatedAttributeMatrix", &CreateAttributeMatrix::getCreatedAttributeMatrix, &CreateAttributeMatrix::setCreatedAttributeMatrix)
  /* Property accessors for AttributeMatrixType */
  .def_property("AttributeMatrixType", &CreateAttributeMatrix::getAttributeMatrixType, &CreateAttributeMatrix::setAttributeMatrixType)
  /* Property accessors for TupleDimensions */
  .def_property("TupleDimensions", &CreateAttributeMatrix::getTupleDimensions, &CreateAttributeMatrix::setTupleDimensions)
  ;
  return instance;
}



#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class ImportAsciDataArray located at
* SIMPLib/CoreFilters/ImportAsciDataArray.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/ImportAsciDataArray.h"

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
PySharedPtrClass<ImportAsciDataArray> pybind11_init_SIMPLib_ImportAsciDataArray(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<ImportAsciDataArray> instance(m, "ImportAsciDataArray", parent);
  instance
  .def(py::init([]()
    { 
      return ImportAsciDataArray::New();
    }
))
  .def_static("New", &ImportAsciDataArray::New)
  /* Property accessors for CreatedAttributeArrayPath */
  .def_property("CreatedAttributeArrayPath", &ImportAsciDataArray::getCreatedAttributeArrayPath, &ImportAsciDataArray::setCreatedAttributeArrayPath)
  /* Property accessors for ScalarType */
  .def_property("ScalarType", &ImportAsciDataArray::getScalarType, &ImportAsciDataArray::setScalarType)
  /* Property accessors for NumberOfComponents */
  .def_property("NumberOfComponents", &ImportAsciDataArray::getNumberOfComponents, &ImportAsciDataArray::setNumberOfComponents)
  /* Property accessors for SkipHeaderLines */
  .def_property("SkipHeaderLines", &ImportAsciDataArray::getSkipHeaderLines, &ImportAsciDataArray::setSkipHeaderLines)
  /* Property accessors for InputFile */
  .def_property("InputFile", &ImportAsciDataArray::getInputFile, &ImportAsciDataArray::setInputFile)
  /* Property accessors for Delimiter */
  .def_property("Delimiter", &ImportAsciDataArray::getDelimiter, &ImportAsciDataArray::setDelimiter)
  ;
  return instance;
}



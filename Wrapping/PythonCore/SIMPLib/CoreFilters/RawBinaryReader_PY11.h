#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class RawBinaryReader located at
* SIMPLib/CoreFilters/RawBinaryReader.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/RawBinaryReader.h"

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
PySharedPtrClass<RawBinaryReader> pybind11_init_SIMPLib_RawBinaryReader(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<RawBinaryReader> instance(m, "RawBinaryReader", parent);
  instance
  .def(py::init([]()
    { 
      return RawBinaryReader::New();
    }
))
  .def_static("New", &RawBinaryReader::New)
  /* Property accessors for CreatedAttributeArrayPath */
  .def_property("CreatedAttributeArrayPath", &RawBinaryReader::getCreatedAttributeArrayPath, &RawBinaryReader::setCreatedAttributeArrayPath)
  /* Property accessors for ScalarType */
  .def_property("ScalarType", &RawBinaryReader::getScalarType, &RawBinaryReader::setScalarType)
  /* Property accessors for Endian */
  .def_property("Endian", &RawBinaryReader::getEndian, &RawBinaryReader::setEndian)
  /* Property accessors for NumberOfComponents */
  .def_property("NumberOfComponents", &RawBinaryReader::getNumberOfComponents, &RawBinaryReader::setNumberOfComponents)
  /* Property accessors for SkipHeaderBytes */
  .def_property("SkipHeaderBytes", &RawBinaryReader::getSkipHeaderBytes, &RawBinaryReader::setSkipHeaderBytes)
  /* Property accessors for InputFile */
  .def_property("InputFile", &RawBinaryReader::getInputFile, &RawBinaryReader::setInputFile)
  ;
  return instance;
}



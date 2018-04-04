#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class RawBinaryReader located at
 * SIMPLib/CoreFilters/RawBinaryReader.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.109
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
template <typename T> using PySharedPtrClass = py::class_<T, std::shared_ptr<T>>;
#endif

/**
 * @brief This allows filters declared in a plugin to be wrapped
 */
#ifndef PyAbstractFilterSharedPtrClass_TEMPLATE
#define PyAbstractFilterSharedPtrClass_TEMPLATE
template <typename T> using PyAbstractFilterSharedPtrClass = py::class_<T, AbstractFilter, std::shared_ptr<T>>;
#endif

/**
 * @brief
 * @param m The Python module
 * @param parent The parent class of the wrapped class
 * @return A std::shared_ptr<T> wrapped instance of the wrapped class properly
 * initialized.
 */
PySharedPtrClass<RawBinaryReader> pybind11_init_SIMPLib_RawBinaryReader(py::module& m
#ifdef simpl_EXPORTS
                                                                        ,
                                                                        PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<RawBinaryReader> instance(m, "RawBinaryReader", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<RawBinaryReader> instance(m, "RawBinaryReader");
#endif

  instance
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
      .def_property("InputFile", &RawBinaryReader::getInputFile, &RawBinaryReader::setInputFile);

  /* Return the instance */
  return instance;
}

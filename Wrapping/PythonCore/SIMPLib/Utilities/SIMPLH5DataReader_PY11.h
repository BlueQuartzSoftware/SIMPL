#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class SIMPLH5DataReader located at
* SIMPLib/Utilities/SIMPLH5DataReader.h. 
* The Python bindings are created using Pybind11.
* @date Tue Mar 6 17:07:40 2018
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/Utilities/SIMPLH5DataReader.h"


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
* @return A std::shared_ptr<T> wrapped instance of the wrapped class properly 
* initialized.
*/
PySharedPtrClass<SIMPLH5DataReader> pybind11_init_SIMPLib_SIMPLH5DataReader(py::module &m)
{
  PySharedPtrClass<SIMPLH5DataReader> instance(m, "SIMPLH5DataReader");
  instance
  .def(py::init([]()
    { 
      return SIMPLH5DataReader::New();
    }
))
  .def_static("New", &SIMPLH5DataReader::New)
  /* Class instance method openFile */
  .def("openFile", &SIMPLH5DataReader::openFile, 
        py::arg("filePath")
      )
  /* Class instance method closeFile */
  .def("closeFile", &SIMPLH5DataReader::closeFile)
  /* Class instance method readDataContainerArrayStructure */
  .def("readDataContainerArrayStructure", &SIMPLH5DataReader::readDataContainerArrayStructure, 
        py::arg("SIMPLH5DataReaderRequirements"), 
        py::arg("err")
      )
  ;
  return instance;
}



#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class DataContainerReader located at
* SIMPLib/CoreFilters/DataContainerReader.h. 
* The Python bindings are created using Pybind11.
* @date Tue Mar 6 17:07:40 2018
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/DataContainerReader.h"

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
PySharedPtrClass<DataContainerReader> pybind11_init_SIMPLib_DataContainerReader(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<DataContainerReader> instance(m, "DataContainerReader", parent);
  instance
  .def(py::init([]()
    { 
      return DataContainerReader::New();
    }
))
  .def_static("New", &DataContainerReader::New)
  /* Property accessors for InputFile */
  .def_property("InputFile", &DataContainerReader::getInputFile, &DataContainerReader::setInputFile)
  /* Property accessors for OverwriteExistingDataContainers */
  .def_property("OverwriteExistingDataContainers", &DataContainerReader::getOverwriteExistingDataContainers, &DataContainerReader::setOverwriteExistingDataContainers)
  /* Property accessors for LastFileRead */
  .def_property("LastFileRead", &DataContainerReader::getLastFileRead, &DataContainerReader::setLastFileRead)
  /* Property accessors for LastRead */
  .def_property("LastRead", &DataContainerReader::getLastRead, &DataContainerReader::setLastRead)
  /* Property accessors for InputFileDataContainerArrayProxy */
  .def_property("InputFileDataContainerArrayProxy", &DataContainerReader::getInputFileDataContainerArrayProxy, &DataContainerReader::setInputFileDataContainerArrayProxy)
  /* Property accessors for InputFile */
  .def_property("InputFile", &DataContainerReader::getInputFile, &DataContainerReader::setInputFile)
  /* Property accessors for OverwriteExistingDataContainers */
  .def_property("OverwriteExistingDataContainers", &DataContainerReader::getOverwriteExistingDataContainers, &DataContainerReader::setOverwriteExistingDataContainers)
  /* Property accessors for LastFileRead */
  .def_property("LastFileRead", &DataContainerReader::getLastFileRead, &DataContainerReader::setLastFileRead)
  /* Property accessors for LastRead */
  .def_property("LastRead", &DataContainerReader::getLastRead, &DataContainerReader::setLastRead)
  /* Property accessors for InputFileDataContainerArrayProxy */
  .def_property("InputFileDataContainerArrayProxy", &DataContainerReader::getInputFileDataContainerArrayProxy, &DataContainerReader::setInputFileDataContainerArrayProxy)
  /* Class instance method readDataContainerArrayStructure */
  .def("readDataContainerArrayStructure", &DataContainerReader::readDataContainerArrayStructure, 
        py::arg("path")
      )
  ;
  return instance;
}



#ifndef pybind_DataContainerReader_H_
#define pybind_DataContainerReader_H_

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class DataContainerReader located at
* SIMPLib/CoreFilters/DataContainerReader.h. 
* The Python bindings are created using Pybind11.
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
* @return A std::shared_ptr<T> wrapped insance of the wrapped class properly 
* initialized.
*/
PySharedPtrClass<DataContainerReader> pybind11_init_SIMPLib_DataContainerReader(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<DataContainerReader> instance(m, "DataContainerReader", parent);
  instance
  .def(py::init([](){ return DataContainerReader::New();}))
  .def_static("New", &DataContainerReader::New)
  /* Property accessors for InputFile */
  .def("getInputFile", &DataContainerReader::getInputFile)
  .def("setInputFile", &DataContainerReader::setInputFile, py::arg("InputFile"))
  /* Property accessors for OverwriteExistingDataContainers */
  .def("getOverwriteExistingDataContainers", &DataContainerReader::getOverwriteExistingDataContainers)
  .def("setOverwriteExistingDataContainers", &DataContainerReader::setOverwriteExistingDataContainers, py::arg("OverwriteExistingDataContainers"))
  /* Property accessors for LastFileRead */
  .def("getLastFileRead", &DataContainerReader::getLastFileRead)
  .def("setLastFileRead", &DataContainerReader::setLastFileRead, py::arg("LastFileRead"))
  /* Property accessors for LastRead */
  .def("getLastRead", &DataContainerReader::getLastRead)
  .def("setLastRead", &DataContainerReader::setLastRead, py::arg("LastRead"))
  /* Property accessors for InputFileDataContainerArrayProxy */
  .def("getInputFileDataContainerArrayProxy", &DataContainerReader::getInputFileDataContainerArrayProxy)
  .def("setInputFileDataContainerArrayProxy", &DataContainerReader::setInputFileDataContainerArrayProxy, py::arg("InputFileDataContainerArrayProxy"))
  /* Class instance method readDataContainerArrayStructure */
  .def("readDataContainerArrayStructure", &DataContainerReader::readDataContainerArrayStructure, 
        py::arg("path")
      )
  ;
  return instance;
}


#endif /* pybind_DataContainerReader_H_  */

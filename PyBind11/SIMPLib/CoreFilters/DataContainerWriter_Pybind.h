#ifndef pybind_DataContainerWriter_H_
#define pybind_DataContainerWriter_H_

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class DataContainerWriter located at
* SIMPLib/CoreFilters/DataContainerWriter.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/DataContainerWriter.h"

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
PySharedPtrClass<DataContainerWriter> pybind11_init_SIMPLib_DataContainerWriter(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<DataContainerWriter> instance(m, "DataContainerWriter", parent);
  instance
  .def(py::init([](){ return DataContainerWriter::New();}))
  .def_static("New", &DataContainerWriter::New)
  /* Property accessors for OutputFile */
  .def("getOutputFile", &DataContainerWriter::getOutputFile)
  .def("setOutputFile", &DataContainerWriter::setOutputFile, py::arg("OutputFile"))
  /* Property accessors for WriteXdmfFile */
  .def("getWriteXdmfFile", &DataContainerWriter::getWriteXdmfFile)
  .def("setWriteXdmfFile", &DataContainerWriter::setWriteXdmfFile, py::arg("WriteXdmfFile"))
  /* Property accessors for WriteTimeSeries */
  .def("getWriteTimeSeries", &DataContainerWriter::getWriteTimeSeries)
  .def("setWriteTimeSeries", &DataContainerWriter::setWriteTimeSeries, py::arg("WriteTimeSeries"))
  ;
  return instance;
}


#endif /* pybind_DataContainerWriter_H_  */

#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class DataContainerReader located at
 * SIMPLib/CoreFilters/DataContainerReader.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.096
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
PySharedPtrClass<DataContainerReader> pybind11_init_SIMPLib_DataContainerReader(py::module& m
#ifdef simpl_EXPORTS
                                                                                ,
                                                                                PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<DataContainerReader> instance(m, "DataContainerReader", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<DataContainerReader> instance(m, "DataContainerReader");
#endif

  instance
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
      .def("readDataContainerArrayStructure", &DataContainerReader::readDataContainerArrayStructure, py::arg("path"));

  /* Return the instance */
  return instance;
}

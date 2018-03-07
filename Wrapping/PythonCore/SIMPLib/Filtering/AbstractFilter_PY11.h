#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class AbstractFilter located at
* SIMPLib/Filtering/AbstractFilter.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/Filtering/AbstractFilter.h"


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
PySharedPtrClass<AbstractFilter> pybind11_init_simpl_AbstractFilter(py::module &m)
{
  PySharedPtrClass<AbstractFilter> instance(m, "AbstractFilter");
  instance
  .def(py::init([]()
    { 
      return AbstractFilter::New();
    }
))
  .def_static("New", &AbstractFilter::New)
  /* Read Only Property for GroupName */
  .def_property_readonly("GroupName", &AbstractFilter::getGroupName)
  /* Read Only Property for SubGroupName */
  .def_property_readonly("SubGroupName", &AbstractFilter::getSubGroupName)
  /* Read Only Property for HumanLabel */
  .def_property_readonly("HumanLabel", &AbstractFilter::getHumanLabel)
  /* Read Only Property for FilterVersion */
  .def_property_readonly("FilterVersion", &AbstractFilter::getFilterVersion)
  /* Read Only Property for CompiledLibraryName */
  .def_property_readonly("CompiledLibraryName", &AbstractFilter::getCompiledLibraryName)
  /* Property accessors for Cancel */
  .def_property("Cancel", &AbstractFilter::getCancel, &AbstractFilter::setCancel)
  /* Property accessors for Enabled */
  .def_property("Enabled", &AbstractFilter::getEnabled, &AbstractFilter::setEnabled)
  /* Property accessors for MessagePrefix */
  .def_property("MessagePrefix", &AbstractFilter::getMessagePrefix, &AbstractFilter::setMessagePrefix)
  /* Property accessors for ErrorCondition */
  .def_property("ErrorCondition", &AbstractFilter::getErrorCondition, &AbstractFilter::setErrorCondition)
  /* Property accessors for WarningCondition */
  .def_property("WarningCondition", &AbstractFilter::getWarningCondition, &AbstractFilter::setWarningCondition)
  /* Property accessors for InPreflight */
  .def_property("InPreflight", &AbstractFilter::getInPreflight, &AbstractFilter::setInPreflight)
  /* Property accessors for PipelineIndex */
  .def_property("PipelineIndex", &AbstractFilter::getPipelineIndex, &AbstractFilter::setPipelineIndex)
  /* Class instance method generateHtmlSummary */
  .def("generateHtmlSummary", &AbstractFilter::generateHtmlSummary)
  /* Class instance method execute */
  .def("execute", &AbstractFilter::execute)
  /* Class instance method preflight */
  .def("preflight", &AbstractFilter::preflight)
  /* Class instance method setDataContainerArray */
  .def("setDataContainerArray", &AbstractFilter::setDataContainerArray)
  ;
  return instance;
}



#ifndef pybind_AbstractFilter_H_
#define pybind_AbstractFilter_H_


#include <pybind11/pybind11.h>

#include "SIMPLib/Filtering/AbstractFilter.h"

namespace py = pybind11;

#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T>
using PySharedPtrClass = py::class_<T, std::shared_ptr<T> >;
#endif

PySharedPtrClass<AbstractFilter> pybind11_init_SIMPLib_AbstractFilter(py::module &m)
{
  PySharedPtrClass<AbstractFilter> instance(m, "AbstractFilter");
  instance
  .def(py::init([](){ return AbstractFilter::New();}))
  .def_static("New", &AbstractFilter::New)
  /* Property accessors for GroupName */
  .def("getGroupName", &AbstractFilter::getGroupName)
  /* Property accessors for SubGroupName */
  .def("getSubGroupName", &AbstractFilter::getSubGroupName)
  /* Property accessors for HumanLabel */
  .def("getHumanLabel", &AbstractFilter::getHumanLabel)
  /* Property accessors for FilterVersion */
  .def("getFilterVersion", &AbstractFilter::getFilterVersion)
  /* Property accessors for CompiledLibraryName */
  .def("getCompiledLibraryName", &AbstractFilter::getCompiledLibraryName)
  /* Property accessors for Cancel */
  .def("getCancel", &AbstractFilter::getCancel)
  .def("setCancel", &AbstractFilter::setCancel, py::arg("Cancel"))
  /* Property accessors for Enabled */
  .def("getEnabled", &AbstractFilter::getEnabled)
  .def("setEnabled", &AbstractFilter::setEnabled, py::arg("Enabled"))
  /* Property accessors for MessagePrefix */
  .def("getMessagePrefix", &AbstractFilter::getMessagePrefix)
  .def("setMessagePrefix", &AbstractFilter::setMessagePrefix, py::arg("MessagePrefix"))
  /* Property accessors for ErrorCondition */
  .def("getErrorCondition", &AbstractFilter::getErrorCondition)
  .def("setErrorCondition", &AbstractFilter::setErrorCondition, py::arg("ErrorCondition"))
  /* Property accessors for WarningCondition */
  .def("getWarningCondition", &AbstractFilter::getWarningCondition)
  .def("setWarningCondition", &AbstractFilter::setWarningCondition, py::arg("WarningCondition"))
  /* Property accessors for InPreflight */
  .def("getInPreflight", &AbstractFilter::getInPreflight)
  .def("setInPreflight", &AbstractFilter::setInPreflight, py::arg("InPreflight"))
  /* Property accessors for PipelineIndex */
  .def("getPipelineIndex", &AbstractFilter::getPipelineIndex)
  .def("setPipelineIndex", &AbstractFilter::setPipelineIndex, py::arg("PipelineIndex"))
  /* Class instance method generateHtmlSummary */
  .def("generateHtmlSummary", &AbstractFilter::generateHtmlSummary)
  /* Class instance method execute */
  .def("execute", &AbstractFilter::execute)
  /* Class instance method preflight */
  .def("preflight", &AbstractFilter::preflight)
  ;
  return instance;
}


#endif /* pybind_AbstractFilter_H_  */

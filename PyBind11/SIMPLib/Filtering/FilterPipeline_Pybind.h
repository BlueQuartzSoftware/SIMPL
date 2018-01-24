#ifndef pybind_FilterPipeline_H_
#define pybind_FilterPipeline_H_


#include <pybind11/pybind11.h>

#include "SIMPLib/Filtering/FilterPipeline.h"

namespace py = pybind11;

#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T>
using PySharedPtrClass = py::class_<T, std::shared_ptr<T> >;
#endif

PySharedPtrClass<FilterPipeline> pybind11_init_SIMPLib_FilterPipeline(py::module &m)
{
  PySharedPtrClass<FilterPipeline> instance(m, "FilterPipeline");
  instance
  .def(py::init([](){ return FilterPipeline::New();}))
  .def_static("New", &FilterPipeline::New)
  /* Property accessors for ErrorCondition */
  .def("getErrorCondition", &FilterPipeline::getErrorCondition)
  .def("setErrorCondition", &FilterPipeline::setErrorCondition, py::arg("ErrorCondition"))
  /* Property accessors for CurrentFilter */
  .def("getCurrentFilter", &FilterPipeline::getCurrentFilter)
  .def("setCurrentFilter", &FilterPipeline::setCurrentFilter, py::arg("CurrentFilter"))
  /* Property accessors for Cancel */
  .def("getCancel", &FilterPipeline::getCancel)
  .def("setCancel", &FilterPipeline::setCancel, py::arg("Cancel"))
  /* Property accessors for Name */
  .def("getName", &FilterPipeline::getName)
  .def("setName", &FilterPipeline::setName, py::arg("Name"))
  /* Class instance method execute */
  .def("execute", &FilterPipeline::execute)
  /* Class instance method preflightPipeline */
  .def("preflightPipeline", &FilterPipeline::preflightPipeline)
  /* Class instance method pushFront */
  .def("pushFront", &FilterPipeline::pushFront, py::arg("AbstractFilter"))
  /* Class instance method pushBack */
  .def("pushBack", &FilterPipeline::pushBack, py::arg("AbstractFilter"))
  /* Class instance method popFront */
  .def("popFront", &FilterPipeline::popFront)
  /* Class instance method popBack */
  .def("popBack", &FilterPipeline::popBack)
  /* Class instance method clear */
  .def("clear", &FilterPipeline::clear)
  /* Class instance method size */
  .def("size", &FilterPipeline::size)
  /* Class instance method empty */
  .def("empty", &FilterPipeline::empty)
  ;
  return instance;
}


#endif /* pybind_FilterPipeline_H_  */

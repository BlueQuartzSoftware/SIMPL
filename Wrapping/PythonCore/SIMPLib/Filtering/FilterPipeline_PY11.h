#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class FilterPipeline located at
* SIMPLib/Filtering/FilterPipeline.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/Filtering/FilterPipeline.h"


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
PySharedPtrClass<FilterPipeline> pybind11_init_SIMPLib_FilterPipeline(py::module &m)
{
  PySharedPtrClass<FilterPipeline> instance(m, "FilterPipeline");
  instance
  .def(py::init([]()
    { 
      return FilterPipeline::New();
    }
))
  .def_static("New", &FilterPipeline::New)
  /* Property accessors for ErrorCondition */
  .def_property("ErrorCondition", &FilterPipeline::getErrorCondition, &FilterPipeline::setErrorCondition)
  /* Property accessors for CurrentFilter */
  .def_property("CurrentFilter", &FilterPipeline::getCurrentFilter, &FilterPipeline::setCurrentFilter)
  /* Property accessors for Cancel */
  .def_property("Cancel", &FilterPipeline::getCancel, &FilterPipeline::setCancel)
  /* Property accessors for Name */
  .def_property("Name", &FilterPipeline::getName, &FilterPipeline::setName)
  /* Class instance method run */
  .def("run", &FilterPipeline::run)
  /* Class instance method preflightPipeline */
  .def("preflightPipeline", &FilterPipeline::preflightPipeline)
  /* Class instance method pushFront */
  .def("pushFront", &FilterPipeline::pushFront, 
        py::arg("AbstractFilter")
      )
  /* Class instance method pushBack */
  .def("pushBack", &FilterPipeline::pushBack, 
        py::arg("AbstractFilter")
      )
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



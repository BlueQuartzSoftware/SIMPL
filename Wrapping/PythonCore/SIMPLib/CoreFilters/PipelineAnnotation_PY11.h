#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class PipelineAnnotation located at
* SIMPLib/CoreFilters/PipelineAnnotation.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/PipelineAnnotation.h"

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
PySharedPtrClass<PipelineAnnotation> pybind11_init_simpl_PipelineAnnotation(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<PipelineAnnotation> instance(m, "PipelineAnnotation", parent);
  instance
  .def(py::init([]()
    { 
      return PipelineAnnotation::New();
    }
))
  .def_static("New", &PipelineAnnotation::New)
  /* Property accessors for Summary */
  .def_property("Summary", &PipelineAnnotation::getSummary, &PipelineAnnotation::setSummary)
  ;
  return instance;
}



#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class FeatureCountDecision located at
* SIMPLib/CoreFilters/FeatureCountDecision.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/FeatureCountDecision.h"

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
PySharedPtrClass<FeatureCountDecision> pybind11_init_SIMPLib_FeatureCountDecision(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<FeatureCountDecision> instance(m, "FeatureCountDecision", parent);
  instance
  .def(py::init([]()
    { 
      return FeatureCountDecision::New();
    }
))
  .def_static("New", &FeatureCountDecision::New)
  /* Property accessors for FeatureIdsArrayPath */
  .def_property("FeatureIdsArrayPath", &FeatureCountDecision::getFeatureIdsArrayPath, &FeatureCountDecision::setFeatureIdsArrayPath)
  /* Property accessors for MaxGrains */
  .def_property("MaxGrains", &FeatureCountDecision::getMaxGrains, &FeatureCountDecision::setMaxGrains)
  ;
  return instance;
}



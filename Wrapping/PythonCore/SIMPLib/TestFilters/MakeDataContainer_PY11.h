#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class MakeDataContainer located at
* SIMPLib/TestFilters/MakeDataContainer.h. 
* The Python bindings are created using Pybind11.
* @date Tue Mar 6 17:07:40 2018
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/TestFilters/MakeDataContainer.h"

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
PySharedPtrClass<MakeDataContainer> pybind11_init_SIMPLib_MakeDataContainer(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<MakeDataContainer> instance(m, "MakeDataContainer", parent);
  instance
  .def(py::init([]()
    { 
      return MakeDataContainer::New();
    }
))
  .def_static("New", &MakeDataContainer::New)
  /* Property accessors for FeatureIdsArrayName */
  .def_property("FeatureIdsArrayName", &MakeDataContainer::getFeatureIdsArrayName, &MakeDataContainer::setFeatureIdsArrayName)
  /* Property accessors for CellEulerAnglesArrayName */
  .def_property("CellEulerAnglesArrayName", &MakeDataContainer::getCellEulerAnglesArrayName, &MakeDataContainer::setCellEulerAnglesArrayName)
  /* Property accessors for CellPhasesArrayName */
  .def_property("CellPhasesArrayName", &MakeDataContainer::getCellPhasesArrayName, &MakeDataContainer::setCellPhasesArrayName)
  /* Property accessors for CrystalStructuresArrayName */
  .def_property("CrystalStructuresArrayName", &MakeDataContainer::getCrystalStructuresArrayName, &MakeDataContainer::setCrystalStructuresArrayName)
  /* Property accessors for LatticeConstantsArrayName */
  .def_property("LatticeConstantsArrayName", &MakeDataContainer::getLatticeConstantsArrayName, &MakeDataContainer::setLatticeConstantsArrayName)
  ;
  return instance;
}



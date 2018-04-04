#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class DynamicTableExample located at
 * SIMPLib/TestFilters/DynamicTableExample.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.178
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/TestFilters/DynamicTableExample.h"

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
PySharedPtrClass<DynamicTableExample> pybind11_init_SIMPLib_DynamicTableExample(py::module& m
#ifdef simpl_EXPORTS
                                                                                ,
                                                                                PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<DynamicTableExample> instance(m, "DynamicTableExample", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<DynamicTableExample> instance(m, "DynamicTableExample");
#endif

  instance
      /* Property accessors for DynamicData1 */
      .def_property("DynamicData1", &DynamicTableExample::getDynamicData1, &DynamicTableExample::setDynamicData1)
      /* Property accessors for DynamicData2 */
      .def_property("DynamicData2", &DynamicTableExample::getDynamicData2, &DynamicTableExample::setDynamicData2)
      /* Property accessors for DynamicData3 */
      .def_property("DynamicData3", &DynamicTableExample::getDynamicData3, &DynamicTableExample::setDynamicData3)
      /* Property accessors for DynamicData4 */
      .def_property("DynamicData4", &DynamicTableExample::getDynamicData4, &DynamicTableExample::setDynamicData4)
      /* Property accessors for DynamicData5 */
      .def_property("DynamicData5", &DynamicTableExample::getDynamicData5, &DynamicTableExample::setDynamicData5);

  /* Return the instance */
  return instance;
}

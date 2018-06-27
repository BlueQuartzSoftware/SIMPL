/* This file is auto-genereated from a template file. If you want changes
 * then edit the template file.
 */

#include <pybind11/pybind11.h>

#include <utility>

#include <QtCore/QString>

namespace py = pybind11;

#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T> using PySharedPtrClass = py::class_<T, std::shared_ptr<T>>;
#endif


/******************************************************************************
 * This is a list of all the SIMPL headers that are needed/wrapped by Pybind11
 ******************************************************************************/
@HEADER_PATH@

/**
 * @brief PYBIND11_MODULE This section declares our python module, its name and
 * what classes are available within the module.
 *
 * @param parentModule The parent Python Module that this module will become a part of.
 */
void Init_@LIB_NAME@(py::module& parentModule)
{ 
  /* NOTE: It is important that the submodule variable is "mod" as that is what
  * the CodeScraper program uses. DO NOT CHANGE IT.
  */

  py::module mod = parentModule.def_submodule("@LIB_NAME@", "  Python wrapping for @FULL_LIB_NAME@");
  mod.doc() = "Python wrapping for @LIB_NAME@";

  /* Init codes for classes in the Module */
  @MODULE_INIT_CODE@
}

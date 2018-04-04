#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class PostSlackMessage located at
 * SIMPLib/CoreFilters/PostSlackMessage.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.109
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/PostSlackMessage.h"

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
PySharedPtrClass<PostSlackMessage> pybind11_init_SIMPLib_PostSlackMessage(py::module& m
#ifdef simpl_EXPORTS
                                                                          ,
                                                                          PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<PostSlackMessage> instance(m, "PostSlackMessage", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<PostSlackMessage> instance(m, "PostSlackMessage");
#endif

  instance
      /* Property accessors for SlackUser */
      .def_property("SlackUser", &PostSlackMessage::getSlackUser, &PostSlackMessage::setSlackUser)
      /* Property accessors for SlackUrl */
      .def_property("SlackUrl", &PostSlackMessage::getSlackUrl, &PostSlackMessage::setSlackUrl)
      /* Property accessors for SlackMessage */
      .def_property("SlackMessage", &PostSlackMessage::getSlackMessage, &PostSlackMessage::setSlackMessage)
      /* Property accessors for WarningsAsError */
      .def_property("WarningsAsError", &PostSlackMessage::getWarningsAsError, &PostSlackMessage::setWarningsAsError);

  /* Return the instance */
  return instance;
}

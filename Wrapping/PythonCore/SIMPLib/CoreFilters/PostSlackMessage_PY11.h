#pragma once

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class PostSlackMessage located at
* SIMPLib/CoreFilters/PostSlackMessage.h. 
* The Python bindings are created using Pybind11.
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
PySharedPtrClass<PostSlackMessage> pybind11_init_SIMPLib_PostSlackMessage(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<PostSlackMessage> instance(m, "PostSlackMessage", parent);
  instance
  .def(py::init([]()
    { 
      return PostSlackMessage::New();
    }
))
  .def_static("New", &PostSlackMessage::New)
  /* Property accessors for SlackUser */
  .def_property("SlackUser", &PostSlackMessage::getSlackUser, &PostSlackMessage::setSlackUser)
  /* Property accessors for SlackUrl */
  .def_property("SlackUrl", &PostSlackMessage::getSlackUrl, &PostSlackMessage::setSlackUrl)
  /* Property accessors for SlackMessage */
  .def_property("SlackMessage", &PostSlackMessage::getSlackMessage, &PostSlackMessage::setSlackMessage)
  /* Property accessors for WarningsAsError */
  .def_property("WarningsAsError", &PostSlackMessage::getWarningsAsError, &PostSlackMessage::setWarningsAsError)
  ;
  return instance;
}



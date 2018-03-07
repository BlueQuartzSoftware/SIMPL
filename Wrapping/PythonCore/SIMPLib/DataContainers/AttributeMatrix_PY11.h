#pragma once

/**
* @brief This header file is genererated from a program. Make changes with caution.
* 
* 
* This header file wraps the SIMPL class AttributeMatrix located at
* SIMPLib/DataContainers/AttributeMatrix.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/DataContainers/AttributeMatrix.h"


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
PySharedPtrClass<AttributeMatrix> pybind11_init_SIMPLib_AttributeMatrix(py::module &m)
{
  PySharedPtrClass<AttributeMatrix> instance(m, "AttributeMatrix");
  instance
  .def(py::init([](std::vector<size_t> var_0, QString var_1, AttributeMatrix::Type var_2) {
      return AttributeMatrix::Create(var_0, var_1, var_2);
    }))
  .def_static("Create", &AttributeMatrix::Create)
  /* Property accessors for Name */
  .def_property("Name", &AttributeMatrix::getName, &AttributeMatrix::setName)
  /* Class instance method addAttributeArray */
  .def("addAttributeArray", &AttributeMatrix::addAttributeArray, 
        py::arg("Name"), 
        py::arg("Array")
      )
  /* Class instance method doesAttributeArrayExist */
  .def("doesAttributeArrayExist", &AttributeMatrix::doesAttributeArrayExist, 
        py::arg("Name")
      )
  /* Class instance method removeAttributeArray */
  .def("removeAttributeArray", &AttributeMatrix::removeAttributeArray, 
        py::arg("Name")
      )
  /* Class instance method renameAttributeArray */
  .def("renameAttributeArray", &AttributeMatrix::renameAttributeArray, 
        py::arg("OldName"), 
        py::arg("NewName"), 
        py::arg("OverWrite")
      )
  ;


  /* Enumeration code for AttributeMatrix::Category ******************/
  py::enum_<AttributeMatrix::Category>(instance, "Category")
    .value("Element", AttributeMatrix::Category::Element)
    .value("Feature", AttributeMatrix::Category::Feature)
    .value("Ensemble", AttributeMatrix::Category::Ensemble)
    .value("Any", AttributeMatrix::Category::Any)
    .value("Unknown", AttributeMatrix::Category::Unknown)
    .export_values();


  /* Enumeration code for AttributeMatrix::Type ******************/
  py::enum_<AttributeMatrix::Type>(instance, "Type")
    .value("Vertex", AttributeMatrix::Type::Vertex)
    .value("Edge", AttributeMatrix::Type::Edge)
    .value("Face", AttributeMatrix::Type::Face)
    .value("Cell", AttributeMatrix::Type::Cell)
    .value("VertexFeature", AttributeMatrix::Type::VertexFeature)
    .value("EdgeFeature", AttributeMatrix::Type::EdgeFeature)
    .value("FaceFeature", AttributeMatrix::Type::FaceFeature)
    .value("CellFeature", AttributeMatrix::Type::CellFeature)
    .value("VertexEnsemble", AttributeMatrix::Type::VertexEnsemble)
    .value("EdgeEnsemble", AttributeMatrix::Type::EdgeEnsemble)
    .value("FaceEnsemble", AttributeMatrix::Type::FaceEnsemble)
    .value("CellEnsemble", AttributeMatrix::Type::CellEnsemble)
    .value("MetaData", AttributeMatrix::Type::MetaData)
    .value("Generic", AttributeMatrix::Type::Generic)
    .value("Unknown", AttributeMatrix::Type::Unknown)
    .value("Any", AttributeMatrix::Type::Any)
    .export_values();
  return instance;
}



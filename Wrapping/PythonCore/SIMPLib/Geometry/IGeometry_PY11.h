#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class IGeometry located at
 * SIMPLib/Geometry/IGeometry.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.159
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/Geometry/IGeometry.h"

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
 * @return A std::shared_ptr<T> wrapped instance of the wrapped class properly
 * initialized.
 */
PySharedPtrClass<IGeometry> pybind11_init_SIMPLib_IGeometry(py::module& m)
{
  PySharedPtrClass<IGeometry> instance(m, "IGeometry");
  instance
      /* Property accessors for Name */
      .def_property("Name", &IGeometry::getName, &IGeometry::setName)
      /* Property accessors for MessagePrefix */
      .def_property("MessagePrefix", &IGeometry::getMessagePrefix, &IGeometry::setMessagePrefix)
      /* Property accessors for MessageTitle */
      .def_property("MessageTitle", &IGeometry::getMessageTitle, &IGeometry::setMessageTitle)
      /* Property accessors for MessageLabel */
      .def_property("MessageLabel", &IGeometry::getMessageLabel, &IGeometry::setMessageLabel)
      /* Property accessors for SpatialDimensionality */
      .def_property("SpatialDimensionality", &IGeometry::getSpatialDimensionality, &IGeometry::setSpatialDimensionality)
      /* Class instance method getGeometryType */
      .def("getGeometryType", &IGeometry::getGeometryType)
      /* Class instance method getGeometryTypeAsString */
      .def("getGeometryTypeAsString", &IGeometry::getGeometryTypeAsString)
      /* Class instance method getInfoString */
      .def("getInfoString", &IGeometry::getInfoString, py::arg("formatType"))
      /* Class instance method getXdmfGridType */
      .def("getXdmfGridType", &IGeometry::getXdmfGridType)
      /* Class instance method getUnitDimensionality */
      .def("getUnitDimensionality", &IGeometry::getUnitDimensionality)
      /* Class instance method deepCopy */
      .def("deepCopy", &IGeometry::deepCopy, py::arg("forceNoAllocate"))
      /* Class instance method initializeWithZeros */
      .def("initializeWithZeros", &IGeometry::initializeWithZeros)
      /* Class instance method addAttributeMatrix */
      .def("addAttributeMatrix", &IGeometry::addAttributeMatrix, py::arg("Name"), py::arg("AttributeMatrix"))
      /* Class instance method getAttributeMatrix */
      .def("getAttributeMatrix", &IGeometry::getAttributeMatrix, py::arg("Name"))
      /* Class instance method removeAttributeMatrix */
      .def("removeAttributeMatrix", &IGeometry::removeAttributeMatrix, py::arg("Name"));

  /* Enumeration code for IGeometry::Type ******************/
  py::enum_<IGeometry::Type>(instance, "Type")
      .value("Image", IGeometry::Type::Image)
      .value("RectGrid", IGeometry::Type::RectGrid)
      .value("Vertex", IGeometry::Type::Vertex)
      .value("Edge", IGeometry::Type::Edge)
      .value("Triangle", IGeometry::Type::Triangle)
      .value("Quad", IGeometry::Type::Quad)
      .value("Tetrahedral", IGeometry::Type::Tetrahedral)
      .value("Unknown", IGeometry::Type::Unknown)
      .value("Any", IGeometry::Type::Any)
      .export_values();

  /* Enumeration code for IGeometry::VtkCellType ******************/
  py::enum_<IGeometry::VtkCellType>(instance, "VtkCellType")
      .value("Image", IGeometry::VtkCellType::Image)
      .value("RectGrid", IGeometry::VtkCellType::RectGrid)
      .value("Vertex", IGeometry::VtkCellType::Vertex)
      .value("Edge", IGeometry::VtkCellType::Edge)
      .value("Triangle", IGeometry::VtkCellType::Triangle)
      .value("Quad", IGeometry::VtkCellType::Quad)
      .value("Tetrahedral", IGeometry::VtkCellType::Tetrahedral)
      .value("Unknown", IGeometry::VtkCellType::Unknown)
      .value("Any", IGeometry::VtkCellType::Any)
      .export_values();

  /* Return the instance */
  return instance;
}

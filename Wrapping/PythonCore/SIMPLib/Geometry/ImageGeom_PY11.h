#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class ImageGeom located at
 * SIMPLib/Geometry/ImageGeom.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.161
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/Geometry/ImageGeom.h"

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
PySharedPtrClass<ImageGeom> pybind11_init_SIMPLib_ImageGeom(py::module& m
#ifdef simpl_EXPORTS
                                                            ,
                                                            PySharedPtrClass<IGeometryGrid>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<ImageGeom> instance(m, "ImageGeom", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<ImageGeom> instance(m, "ImageGeom");
#endif

  instance.def(py::init([]() { return ImageGeom::New(); }))
      .def_static("New", &ImageGeom::New)
      .def(py::init([](QString var_0) { return ImageGeom::CreateGeometry(var_0); }))
      .def_static("CreateGeometry", &ImageGeom::CreateGeometry)
      /* Class instance method setDimensions */
      .def("setDimensions", py::overload_cast<size_t, size_t, size_t>(&ImageGeom::setDimensions), py::arg("x"), py::arg("y"), py::arg("z"))
      /* Class instance method getDimensions */
      .def("getDimensions", &ImageGeom::getDimensions)
      /* Class instance method setResolution */
      .def("setResolution", py::overload_cast<float, float, float>(&ImageGeom::setResolution), py::arg("x"), py::arg("y"), py::arg("z"))
      /* Class instance method getResolution */
      .def("getResolution", py::overload_cast<>(&ImageGeom::getResolution, py::const_))
      /* Class instance method getXPoints */
      .def("getXPoints", &ImageGeom::getXPoints)
      /* Class instance method getYPoints */
      .def("getYPoints", &ImageGeom::getYPoints)
      /* Class instance method getZPoints */
      .def("getZPoints", &ImageGeom::getZPoints)
      /* Class instance method setOrigin */
      .def("setOrigin", py::overload_cast<float, float, float>(&ImageGeom::setOrigin), py::arg("x"), py::arg("y"), py::arg("z"))
      /* Class instance method getOrigin */
      .def("getOrigin", py::overload_cast<>(&ImageGeom::getOrigin, py::const_))
      /* Class instance method getBoundingBox */
      .def("getBoundingBox", py::overload_cast<>(&ImageGeom::getBoundingBox));

  /* Enumeration code for ImageGeom::ErrorType ******************/
  py::enum_<ImageGeom::ErrorType>(instance, "ErrorType")
      .value("XOutOfBoundsLow", ImageGeom::ErrorType::XOutOfBoundsLow)
      .value("XOutOfBoundsHigh", ImageGeom::ErrorType::XOutOfBoundsHigh)
      .value("YOutOfBoundsLow", ImageGeom::ErrorType::YOutOfBoundsLow)
      .value("YOutOfBoundsHigh", ImageGeom::ErrorType::YOutOfBoundsHigh)
      .value("ZOutOfBoundsLow", ImageGeom::ErrorType::ZOutOfBoundsLow)
      .value("ZOutOfBoundsHigh", ImageGeom::ErrorType::ZOutOfBoundsHigh)
      .value("IndexOutOfBounds", ImageGeom::ErrorType::IndexOutOfBounds)
      .value("NoError", ImageGeom::ErrorType::NoError)
      .export_values();

  /* Return the instance */
  return instance;
}

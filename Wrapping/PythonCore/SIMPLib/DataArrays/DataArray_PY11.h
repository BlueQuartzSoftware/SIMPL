#pragma once
#error
/**
* @brief 
*/

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include "SIMPLib/DataArrays/DataArray.hpp"

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

#define PYB11_DEFINE_DATAARRAY_INIT(T, NAME)\
PySharedPtrClass<DataArray<T>> pybind11_init_simplib_##NAME(py::module &m, PySharedPtrClass<IDataArray>& parent)\
{\
  using DataArrayType = DataArray<T>;\
  PySharedPtrClass<DataArrayType> instance(m, #NAME, parent, py::buffer_protocol());\
  instance\
  .def(py::init([](size_t numElements, QString name, bool allocate) {\
    return DataArrayType::CreateArray(numElements, name, allocate);\
    }))\
  .def(py::init([](T* ptr, size_t numElements, std::vector<size_t> cDims, QString name, bool ownsData) {\
    return DataArrayType::WrapPointer(ptr, numElements, QVector<size_t>::fromStdVector(cDims), name, ownsData);\
  }))\
  .def(py::init([](py::array_t<T, py::array::c_style> b, std::vector<size_t> cDims, QString name, bool ownsData) {\
      ssize_t numElements = 1;\
      ssize_t nDims = b.ndim();\
     for(ssize_t e = 0; e < nDims; e++) { numElements *= b.shape(e);}\
      return DataArrayType::WrapPointer(reinterpret_cast<T*>(b.mutable_data(0)), \
                                        static_cast<size_t>(numElements), \
                                        QVector<size_t>::fromStdVector(cDims), \
                                        name,\
                                        ownsData);\
  }))\
  /* Class instance method setValue */\
  .def("setValue", &DataArrayType::setValue, \
        py::arg("index"), \
        py::arg("value")\
      )\
  .def("getValue", &DataArrayType::getValue, \
        py::arg("index")\
      )\
  .def_property("Name", &DataArrayType::getName, &DataArrayType::setName);\
  ;\
  return instance;\
}

PYB11_DEFINE_DATAARRAY_INIT(int8_t, Int8ArrayType);
PYB11_DEFINE_DATAARRAY_INIT(uint8_t, UInt8ArrayType);

PYB11_DEFINE_DATAARRAY_INIT(int16_t, Int16ArrayType);
PYB11_DEFINE_DATAARRAY_INIT(uint16_t, UInt16ArrayType);

PYB11_DEFINE_DATAARRAY_INIT(int32_t, Int32ArrayType);
PYB11_DEFINE_DATAARRAY_INIT(uint32_t, UInt32ArrayType);

PYB11_DEFINE_DATAARRAY_INIT(int64_t, Int64ArrayType);
PYB11_DEFINE_DATAARRAY_INIT(uint64_t, UInt64ArrayType);

PYB11_DEFINE_DATAARRAY_INIT(float, FloatArrayType);
PYB11_DEFINE_DATAARRAY_INIT(double, DoubleArrayType);


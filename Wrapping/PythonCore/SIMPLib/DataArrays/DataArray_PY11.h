#pragma once

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
PySharedPtrClass<DataArray<T>> pybind11_init_SIMPLib_##NAME(py::module &m, PySharedPtrClass<IDataArray>& parent)\
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

#if 0
PySharedPtrClass<DataArray<int64_t>> pybind11_init_SIMPLib_Int64ArrayType(py::module &m, PySharedPtrClass<IDataArray>& parent)
{
  using DataArrayType = DataArray<int64_t>;
  PySharedPtrClass<DataArrayType> instance(m, "Int64ArrayType", parent, py::buffer_protocol());
  instance
  .def(py::init([](size_t numElements, QString name, bool allocate) {
    return DataArrayType::CreateArray(numElements, name, allocate);
    }))
  .def(py::init([](int64_t* ptr, size_t numElements, std::vector<size_t> cDims, QString name, bool ownsData) {
    return DataArrayType::WrapPointer(ptr, numElements, QVector<size_t>::fromStdVector(cDims), name, ownsData);
  }))
  .def(py::init([](py::array_t<int64_t, py::array::c_style> b, std::vector<size_t> cDims, QString name, bool ownsData) {
      ssize_t numElements = 1;
      ssize_t nDims = b.ndim();
     for(ssize_t e = 0; e < nDims; e++) { numElements *= b.shape(e);}
      return DataArrayType::WrapPointer(reinterpret_cast<int64_t*>(b.mutable_data(0)), 
                                        static_cast<size_t>(numElements), 
                                        QVector<size_t>::fromStdVector(cDims), 
                                        name,
                                        ownsData);
  }))
  /* Class instance method setValue */
  .def("setValue", &DataArrayType::setValue, 
        py::arg("index"), 
        py::arg("value")
      )
  .def("getValue", &DataArrayType::getValue, 
        py::arg("index")
      )
  .def_property("Name", &DataArrayType::getName, &DataArrayType::setName);
  ;
  return instance;
}
#endif

PYB11_DEFINE_DATAARRAY_INIT(uint64_t, UInt64ArrayType);

PYB11_DEFINE_DATAARRAY_INIT(float, FloatArrayType);
PYB11_DEFINE_DATAARRAY_INIT(double, DoubleArrayType);

#if 0
PySharedPtrClass<Int32ArrayType> pybind11_init_SIMPLib_Int32ArrayType(py::module &m)
{
  PySharedPtrClass<Int32ArrayType> instance(m, "Int32ArrayType", py::buffer_protocol());
  instance
  .def(py::init([](size_t numElements, QString name, bool allocate) {
    return Int32ArrayType::CreateArray(numElements, name, allocate);
    }))
  .def(py::init([](int32_t* ptr, size_t numElements, std::vector<size_t> cDims, QString name, bool ownsData) {
    return Int32ArrayType::WrapPointer(ptr, numElements, QVector<size_t>::fromStdVector(cDims), name, ownsData);
  }))
  .def(py::init([](py::buffer b, std::vector<size_t> cDims, QString name, bool ownsData) {
    py::buffer_info info = b.request();

    if(info.format != py::format_descriptor<int32_t>::format())
    {
      std::stringstream ss;
      ss << "Incompatible buffer format. Needed '" << py::format_descriptor<int32_t>::format()
      << "' but got '" << info.format << "'";
      throw std::runtime_error(ss.str());
    }
      ssize_t numElements = 1;
      for(auto e: info.shape)
        numElements *= e;
        
      return Int32ArrayType::WrapPointer(reinterpret_cast<int32_t*>(info.ptr),
                                        static_cast<size_t>(numElements),
                                        QVector<size_t>::fromStdVector(cDims),
                                        name,
                                        ownsData);

  }))
  /* Class instance method setValue */
  .def("setValue", &Int32ArrayType::setValue, 
        py::arg("index"), 
        py::arg("value")
      )
  .def("getValue", &Int32ArrayType::getValue, 
        py::arg("index")
      )
  ;

  return instance;
}



/**
* @brief
* @param m The Python module
* @return A std::shared_ptr<T> wrapped instance of the wrapped class properly 
* initialized.
*/
PySharedPtrClass<Int8ArrayType> pybind11_init_SIMPLib_Int8ArrayType(py::module &m)
{
  PySharedPtrClass<Int8ArrayType> instance(m, "Int8ArrayType", py::buffer_protocol());
  instance
  .def(py::init([](size_t numElements, QString name, bool allocate) {
      return Int8ArrayType::CreateArray(numElements, name, allocate);
    }))
  .def(py::init([](int8_t* ptr, size_t numElements, std::vector<size_t> cDims, QString name, bool ownsData) {
    return Int8ArrayType::WrapPointer(ptr, numElements, QVector<size_t>::fromStdVector(cDims), name, ownsData);
  }))
  /* Class instance method setValue */
  .def("setValue", &Int8ArrayType::setValue, 
        py::arg("index"), 
        py::arg("value")
      )
  .def("getValue", &Int8ArrayType::getValue, 
        py::arg("index")
      )
  ;

  return instance;
}
#endif

/*
typedef DataArray<bool> BoolArrayType;

typedef DataArray<unsigned char> UCharArrayType;

typedef DataArray<int8_t>  Int8ArrayType;
typedef DataArray<uint8_t>  UInt8ArrayType;

typedef DataArray<int16_t>  Int16ArrayType;
typedef DataArray<uint16_t>  UInt16ArrayType;

typedef DataArray<int32_t>  Int32ArrayType;
typedef DataArray<uint32_t>  UInt32ArrayType;

typedef DataArray<int64_t>  Int64ArrayType;
typedef DataArray<uint64_t>  UInt64ArrayType;

typedef DataArray<float>  FloatArrayType;
typedef DataArray<double>  DoubleArrayType;

typedef DataArray<size_t>  SizeTArrayType;






*/

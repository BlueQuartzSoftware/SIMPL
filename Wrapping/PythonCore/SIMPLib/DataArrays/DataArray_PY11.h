#pragma once

/**
* @brief 
*/

#include <pybind11/pybind11.h>

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
  .def(py::init([](py::buffer b, std::vector<size_t> cDims, QString name, bool ownsData) {\
    py::buffer_info info = b.request();\
    if(info.format != py::format_descriptor<T>::format())\
    {\
      std::stringstream ss;\
      ss << "Incompatible buffer format. Needed '" << py::format_descriptor<T>::format()\
      << "' but got '" << info.format << "'";\
      throw std::runtime_error(ss.str());\
    }\
      ssize_t numElements = 1;\
      for(auto e: info.shape) {numElements *= e;}\
      return DataArrayType::WrapPointer(reinterpret_cast<T*>(info.ptr), static_cast<size_t>(numElements), QVector<size_t>::fromStdVector(cDims), name,ownsData);\
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
PYB11_DEFINE_DATAARRAY_INIT(int16_t, Int16ArrayType);
PYB11_DEFINE_DATAARRAY_INIT(int32_t, Int32ArrayType);
PYB11_DEFINE_DATAARRAY_INIT(int64_t, Int64ArrayType);

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

  /*
  clsExampleOne.def("computeSomethingElse",
                  (double (ExampleOne::*)(int, double) const) & ExampleOne::computeSomethingElse,
                  "myFirstParam"_a, "mySecondParam"_a);
  clsExampleOne.def("computeSomethingElse",
                  (double (ExampleOne::*)(int, std::string) const) &ExampleOne::computeSomethingElse,
                  "myFirstParam"_a, "anotherParam"_a="foo");
                  */
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



    // std::cout << "int8: " << py::format_descriptor<int8_t>::format() << std::endl;
    // std::cout << "uint8: " << py::format_descriptor<uint8_t>::format() << std::endl;
    // std::cout << "int16: " << py::format_descriptor<int16_t>::format() << std::endl;
    // std::cout << "uint16: " << py::format_descriptor<uint16_t>::format() << std::endl;
    // std::cout << "int32: " << py::format_descriptor<int32_t>::format() << std::endl;
    // std::cout << "uint32: " << py::format_descriptor<uint32_t>::format() << std::endl;
    // std::cout << "int64: " << py::format_descriptor<int64_t>::format() << std::endl;
    // std::cout << "uint64: " << py::format_descriptor<uint64_t>::format() << std::endl;
    // std::cout << "float: " << py::format_descriptor<float>::format() << std::endl;
    // std::cout << "double: " << py::format_descriptor<double>::format() << std::endl;

    // std::cout << "int: " << py::format_descriptor<int>::format() << std::endl;
    // std::cout << "unsigned int: " << py::format_descriptor<unsigned int>::format() << std::endl;
    // std::cout << "long: " << py::format_descriptor<long>::format() << std::endl;
    // std::cout << "unsigned long: " << py::format_descriptor<unsigned long>::format() << std::endl;
    // std::cout << "long long int: " << py::format_descriptor<long long int>::format() << std::endl;
    // std::cout << "unsigned long long int: " << py::format_descriptor<unsigned long long int>::format() << std::endl;



*/

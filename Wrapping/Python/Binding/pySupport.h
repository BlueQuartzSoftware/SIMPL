#pragma once

#include <pybind11/pybind11.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"

template <class T>
void registerDataArray(pybind11::module& mod, const char* name)
{
  namespace py = pybind11;
  using namespace py::literals;
  using DataArrayType = DataArray<T>;
  py::class_<DataArrayType, IDataArray, std::shared_ptr<DataArrayType>>(mod, name, py::buffer_protocol())
      .def(py::init([](size_t numElements, const QString& name, T initValue) { return std::make_shared<DataArrayType>(numElements, name, initValue); }))
      .def(py::init([](size_t numElements, const std::vector<size_t>& cDims, const QString& name, T initValue) { return std::make_shared<DataArrayType>(numElements, name, cDims, initValue); }))
      .def(py::init([](py::array_t<T, py::array::c_style> data, const QString& name, bool ownsData) {
        py::buffer_info buf = data.request();
        const std::vector<ssize_t>& shape = buf.shape;
        ssize_t ndim = buf.ndim;
        size_t numTuples = 0;
        std::vector<size_t> cDims;
        if(ndim == 0)
        {
          numTuples = 1;
          cDims.push_back(1);
        }
        else if(ndim == 1)
        {
          numTuples = static_cast<size_t>(shape.front());
          cDims.push_back(1);
        }
        else
        {
          numTuples = static_cast<size_t>(shape.front());
          cDims.resize(ndim - 1);
          for(ssize_t e = 0; e < ndim - 1; e++)
          {
            cDims[e] = static_cast<size_t>(shape[e + 1]);
          }
        }
        return DataArrayType::WrapPointer(reinterpret_cast<T*>(data.mutable_data(0)), numTuples, cDims, name, ownsData);
      }))
      .def_property("name", &DataArrayType::getName, &DataArrayType::setName)
      .def("__getitem__", [](const DataArrayType& dataArray, size_t i) { return dataArray.at(i); })
      .def("__setitem__", [](DataArrayType& dataArray, size_t i, T value) { dataArray.at(i) = value; })
      .def("__len__", &DataArrayType::size)
      .def_property_readonly("size", &DataArrayType::size)
      .def_property_readonly("cdims",
                             [](const DataArrayType& dataArray) {
                               std::vector<size_t> cDims = dataArray.getComponentDimensions();
                               py::tuple dims(cDims.size());
                               for(size_t i = 0; i < cDims.size(); i++)
                               {
                                 dims[i] = cDims[i];
                               }
                               return dims;
                             })
      .def_property_readonly("tuples", &DataArrayType::getNumberOfTuples)
      .def(
          "__iter__", [](const DataArrayType& dataArray) { return py::make_iterator(dataArray.begin(), dataArray.end()); }, py::keep_alive<0, 1>())
      .def_buffer([](const DataArrayType& dataArray) -> py::buffer_info {
        int nComp = dataArray.getNumberOfComponents();
        size_t numTuples = dataArray.getNumberOfTuples();
        ssize_t ndim = 0;
        std::vector<ssize_t> shape;
        std::vector<ssize_t> strides;
        if(nComp == 1)
        {
          ndim = 1;
          shape.push_back(numTuples);
          strides.push_back(sizeof(T));
        }
        else
        {
          std::vector<size_t> cDims = dataArray.getComponentDimensions();
          ndim = static_cast<ssize_t>(cDims.size()) + 1;
          shape.resize(ndim);
          shape[0] = numTuples;
          for(ssize_t e = 0; e < ndim - 1; e++)
          {
            shape[e + 1] = static_cast<ssize_t>(cDims[e]);
          }
          ssize_t cur = 1;
          std::vector<ssize_t> rshape(shape);
          std::reverse(rshape.begin(), rshape.end());
          for(auto val : rshape)
          {
            strides.push_back(cur * val * sizeof(T));
            cur *= val;
          }
          strides.pop_back();
          strides.insert(strides.begin(), sizeof(T));
          std::reverse(strides.begin(), strides.end());
        }
        return py::buffer_info(dataArray.getPointer(0), sizeof(T), py::format_descriptor<T>::format(), ndim, shape, strides);
      })
      .def(
          "npview", [](DataArrayType& dataArray) { return py::array_t<T, py::array::c_style>(dataArray.size(), dataArray.data(), py::cast(dataArray)); }, py::return_value_policy::reference_internal)
      .def("__repr__", [](const DataArrayType& a) {
        std::stringstream ss;
        ss << "<'" << a.getFullNameOfClass().toStdString() << "  NAME=" << a.getName().toStdString() << ": TUPLES: " << a.getNumberOfTuples() << "  COMPONENTS: [";
        std::vector<size_t> comps = a.getComponentDimensions();
        bool first = true;
        for(const auto& c : comps)
        {
          if(!first)
          {
            ss << ",";
          }
          ss << c;
          first = false;
        }
        ss << "]'>";
        return ss.str();
      });
}

template <class T, unsigned int Dim_>
struct IVecType
{
  using Type = SIMPLArray<T, Dim_>;
};

template <class T>
struct IVecType<T, 2>
{
  using Type = IVec2<T>;
};

template <class T>
struct IVecType<T, 3>
{
  using Type = IVec3<T>;
};

template <class T>
struct IVecType<T, 4>
{
  using Type = IVec4<T>;
};

template <class T>
struct IVecType<T, 6>
{
  using Type = IVec6<T>;
};

template <class T, unsigned int Dim_>
void registerSIMPLArray(pybind11::module& mod, const char* name)
{
  namespace py = pybind11;
  using namespace py::literals;
  using array_t = typename IVecType<T, Dim_>::Type;
  py::class_<array_t>(mod, name)
      .def(py::init<>([](py::list values) {
        array_t vec;
        for(size_t i = 0; i < vec.size(); i++)
        {
          vec[i] = py::cast<T>(values[i]);
        }
        return vec;
      }))
      .def("__repr__",
           [](const array_t& input) {
             py::list output;
             for(auto value : input)
             {
               output.append(value);
             }
             return py::str(output);
           })
      .def("__str__",
           [](const array_t& input) {
             py::list output;
             for(auto value : input)
             {
               output.append(value);
             }
             return py::str(output);
           })
      .def("__getitem__", [](const array_t& values, size_t index) { return values.at(index); });
}

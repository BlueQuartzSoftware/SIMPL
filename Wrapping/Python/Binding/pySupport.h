#pragma once

#include <stdexcept>

#include <pybind11/pybind11.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#ifdef SIMPL_EMBED_PYTHON
#include "SIMPLib/Python/FilterPyObject.h"
#endif

template <class T>
void registerDataArray(pybind11::module& mod, const char* name)
{
  namespace py = pybind11;
  using namespace py::literals;
  using DataArrayType = DataArray<T>;
  py::class_<DataArrayType, IDataArray, std::shared_ptr<DataArrayType>>(mod, name, py::buffer_protocol())
      .def(py::init([](size_t numElements, const QString& name, T initValue) { return std::make_shared<DataArrayType>(numElements, name, initValue); }), "num_elements"_a, "name"_a, "init_value"_a)
      .def(py::init([](size_t numElements, const std::vector<size_t>& cDims, const QString& name, T initValue) { return std::make_shared<DataArrayType>(numElements, name, cDims, initValue); }),
           "num_elements"_a, "c_dims"_a, "name"_a, "init_value"_a)
      .def(py::init([](py::array_t<T, py::array::c_style> data, const QString& name) {
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
             return DataArrayType::WrapPointer(reinterpret_cast<T*>(data.mutable_data(0)), numTuples, cDims, name, false);
           }),
           py::keep_alive<1, 2>(), "data"_a, "name"_a)
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
      .def_property_readonly_static("dtype", []([[maybe_unused]] py::object self) { return py::dtype::of<T>(); })
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
        ss << "<" << a.getFullNameOfClass().toStdString() << " NAME=\"" << a.getName().toStdString() << "\" TUPLES=" << a.getNumberOfTuples() << " COMPONENTS=(";
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
        ss << ")>";
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

void registerDataContainerArray(pybind11::class_<DataContainerArray, std::shared_ptr<DataContainerArray>>& instance)
{
  namespace py = pybind11;
  using namespace py::literals;

  instance.def("__repr__", [](const DataContainerArray& dca) { return QString("<DataContainerArray DCs=%1>").arg(QString::number(dca.getNumDataContainers())); });

  instance.def("__getitem__", [](const DataContainerArray& dca, const QString& name) {
    if(!dca.doesDataContainerExist(name))
    {
      throw pybind11::key_error(name.toStdString());
    }
    return dca.getDataContainer(name);
  });

  instance.def("__contains__", py::overload_cast<const QString&>(&DataContainerArray::doesDataContainerExist, py::const_));

  instance.def("__len__", &DataContainerArray::size);
  instance.def_property_readonly("size", &DataContainerArray::size);

  instance.def(
      "__iter__", [](DataContainerArray& dca) { return py::make_iterator(dca.begin(), dca.end()); }, py::keep_alive<0, 1>());

#ifdef SIMPL_EMBED_PYTHON
  instance.def(
      "createNonPrereqArrayFromPath",
      [](DataContainerArray& dca, PythonSupport::FilterDelegate& filter, SIMPL::NumericTypes::Type type, const DataArrayPath& path, py::object initValue, const std::vector<size_t>& compDims,
         const QString& property, RenameDataPath::DataID_t id) {
        switch(type)
        {
        case SIMPL::NumericTypes::Type::Int8:
          return py::cast(dca.createNonPrereqArrayFromPath<Int8ArrayType>(filter.filter, path, initValue.cast<int8_t>(), compDims, property, id));
        case SIMPL::NumericTypes::Type::UInt8:
          return py::cast(dca.createNonPrereqArrayFromPath<Int8ArrayType>(filter.filter, path, initValue.cast<uint8_t>(), compDims, property, id));
        case SIMPL::NumericTypes::Type::Int16:
          return py::cast(dca.createNonPrereqArrayFromPath<Int16ArrayType>(filter.filter, path, initValue.cast<int16_t>(), compDims, property, id));
        case SIMPL::NumericTypes::Type::UInt16:
          return py::cast(dca.createNonPrereqArrayFromPath<UInt16ArrayType>(filter.filter, path, initValue.cast<uint16_t>(), compDims, property, id));
        case SIMPL::NumericTypes::Type::Int32:
          return py::cast(dca.createNonPrereqArrayFromPath<Int32ArrayType>(filter.filter, path, initValue.cast<int32_t>(), compDims, property, id));
        case SIMPL::NumericTypes::Type::UInt32:
          return py::cast(dca.createNonPrereqArrayFromPath<UInt32ArrayType>(filter.filter, path, initValue.cast<uint32_t>(), compDims, property, id));
        case SIMPL::NumericTypes::Type::Int64:
          return py::cast(dca.createNonPrereqArrayFromPath<Int64ArrayType>(filter.filter, path, initValue.cast<int64_t>(), compDims, property, id));
        case SIMPL::NumericTypes::Type::UInt64:
          return py::cast(dca.createNonPrereqArrayFromPath<UInt64ArrayType>(filter.filter, path, initValue.cast<uint64_t>(), compDims, property, id));
        case SIMPL::NumericTypes::Type::SizeT:
          return py::cast(dca.createNonPrereqArrayFromPath<SizeTArrayType>(filter.filter, path, initValue.cast<size_t>(), compDims, property, id));
        case SIMPL::NumericTypes::Type::Float:
          return py::cast(dca.createNonPrereqArrayFromPath<FloatArrayType>(filter.filter, path, initValue.cast<float>(), compDims, property, id));
        case SIMPL::NumericTypes::Type::Double:
          return py::cast(dca.createNonPrereqArrayFromPath<DoubleArrayType>(filter.filter, path, initValue.cast<double>(), compDims, property, id));
        case SIMPL::NumericTypes::Type::Bool:
          return py::cast(dca.createNonPrereqArrayFromPath<BoolArrayType>(filter.filter, path, initValue.cast<bool>(), compDims, property, id));
        default:
          throw std::invalid_argument("Invalid numeric type");
        }
      },
      "filter"_a, "type"_a, "path"_a, "initValue"_a, "compDims"_a, "property"_a = std::string(""), "id"_a = RenameDataPath::k_Invalid_ID);

  instance.def(
      "createNonPrereqDataContainer",
      [](DataContainerArray& dca, PythonSupport::FilterDelegate& filter, const QString& dataContainerName, RenameDataPath::DataID_t id) {
        return dca.createNonPrereqDataContainer(filter.filter, dataContainerName, id);
      },
      "filter"_a, "dataContainerName"_a, "id"_a = RenameDataPath::k_Invalid_ID);

  instance.def(
      "createNonPrereqDataContainer",
      [](DataContainerArray& dca, PythonSupport::FilterDelegate& filter, const DataArrayPath& dap, RenameDataPath::DataID_t id) { return dca.createNonPrereqDataContainer(filter.filter, dap, id); },
      "filter"_a, "dap"_a, "id"_a = RenameDataPath::k_Invalid_ID);
#endif
}

void registerDataContainer(pybind11::class_<DataContainer, std::shared_ptr<DataContainer>>& instance)
{
  namespace py = pybind11;
  using namespace py::literals;

  instance.def("__repr__", [](const DataContainer& dc) { return QString("<DataContainer NAME=\"%1\" AMs=%2>").arg(dc.getName(), QString::number(dc.getNumAttributeMatrices())); });

  instance.def("__getitem__", [](const DataContainer& dc, const QString& name) {
    if(!dc.doesAttributeMatrixExist(name))
    {
      throw pybind11::key_error(name.toStdString());
    }
    return dc.getAttributeMatrix(name);
  });

  instance.def("__contains__", py::overload_cast<const QString&>(&DataContainer::doesAttributeMatrixExist, py::const_));

  instance.def("__len__", &DataContainer::size);
  instance.def_property_readonly("size", &DataContainer::size);

  instance.def(
      "__iter__", [](DataContainer& dc) { return py::make_iterator(dc.begin(), dc.end()); }, py::keep_alive<0, 1>());

#ifdef SIMPL_EMBED_PYTHON
  instance.def(
      "createNonPrereqAttributeMatrix",
      [](DataContainer& dc, PythonSupport::FilterDelegate& filter, const DataArrayPath& path, const std::vector<size_t>& tDims, AttributeMatrix::Type amType, RenameDataPath::DataID_t id) {
        return dc.createNonPrereqAttributeMatrix(filter.filter, path, tDims, amType, id);
      },
      "filter"_a, "path"_a, "tDims"_a, "amType"_a, "id"_a = RenameDataPath::k_Invalid_ID);

  instance.def(
      "createNonPrereqAttributeMatrix",
      [](DataContainer& dc, PythonSupport::FilterDelegate& filter, const QString& attributeMatrixName, const std::vector<size_t>& tDims, AttributeMatrix::Type amType, RenameDataPath::DataID_t id) {
        return dc.createNonPrereqAttributeMatrix(filter.filter, attributeMatrixName, tDims, amType, id);
      },
      "filter"_a, "attributeMatrixName"_a, "tDims"_a, "amType"_a, "id"_a = RenameDataPath::k_Invalid_ID);
#endif
}

void registerAttributeMatrix(pybind11::class_<AttributeMatrix, std::shared_ptr<AttributeMatrix>>& instance)
{
  namespace py = pybind11;
  using namespace py::literals;

  instance.def("__repr__", [](const AttributeMatrix& am) {
    QString dimString = "(";
    auto dims = am.getTupleDimensions();
    for(size_t i = 0; i < dims.size() - 1; i++)
    {
      dimString.append(QString("%1, ").arg(QString::number(dims[i])));
    }
    dimString.append(QString("%1)").arg(QString::number(dims.back())));
    return QString("<AttributeMatrix NAME=\"%1\" DIMS=%2 TYPE=%3 DAs=%4>").arg(am.getName(), dimString, AttributeMatrix::TypeToString(am.getType()), QString::number(am.getNumAttributeArrays()));
  });

  instance.def("__contains__", py::overload_cast<const QString&>(&AttributeMatrix::doesAttributeArrayExist, py::const_));

  instance.def("__len__", &AttributeMatrix::size);
  instance.def_property_readonly("size", &AttributeMatrix::size);

  instance.def(
      "__iter__", [](AttributeMatrix& am) { return py::make_iterator(am.begin(), am.end()); }, py::keep_alive<0, 1>());

  instance.def("__getitem__", [](const AttributeMatrix& am, const QString& name) {
    if(!am.doesAttributeArrayExist(name))
    {
      throw pybind11::key_error(name.toStdString());
    }
    return am.getAttributeArray(name);
  });
}

void registerDataArrayPath(py::class_<DataArrayPath>& instance)
{
  namespace py = pybind11;
  using namespace py::literals;

  instance.def("__repr__", [](const DataArrayPath& path) { return QString("DataArrayPath(\"%1\")").arg(path.serialize()); });
}

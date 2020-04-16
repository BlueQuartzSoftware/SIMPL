#pragma once

#include <QtCore/QString>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/CreateDataArray.h"
#include "SIMPLib/CoreFilters/util/ASCIIWizardData.hpp"
#include "SIMPLib/DataContainers/DataContainerProxy.h"
#include "SIMPLib/FilterParameters/RangeFilterParameter.h"
#include "SIMPLib/Common/ShapeType.h"
#include "SIMPLib/Common/PhaseType.h"

using DataContainersMap = QMap<QString, DataContainerProxy>;
using AttributeMatricesMap = QMap<QString, AttributeMatrixProxy>;
using VectorOfFloatArray = QVector<FloatArrayType::Pointer>;

namespace pybind11
{
namespace detail
{
/* Create a TypeCaster for auto python string <--> QString conversion */
template <>
struct type_caster<QString>
{
public:
  /**
   * This macro establishes the name 'QString' in
   * function signatures and declares a local variable
   * 'value' of type QString
   */
  PYBIND11_TYPE_CASTER(QString, _("QString"));

  /**
   *  @brief Conversion part 1 (Python->C++): convert a PyObject into a QString
   * instance or return false upon failure. The second argument
   * indicates whether implicit conversions should be applied.
   * @param src
   * @return boolean
   */
  bool load(handle src, bool b)
  {
    if(!src)
    {
      return false;
    }
    object temp;
    handle load_src = src;
    if(PyUnicode_Check(load_src.ptr()))
    {
      temp = reinterpret_steal<object>(PyUnicode_AsUTF8String(load_src.ptr()));
      if(!temp) /* A UnicodeEncodeError occured */
      {
        PyErr_Clear();
        return false;
      }
      load_src = temp;
    }
    char* buffer = nullptr;
    ssize_t length = 0;
    int err = PYBIND11_BYTES_AS_STRING_AND_SIZE(load_src.ptr(), &buffer, &length);
    if(err == -1) /* A TypeError occured */
    {
      PyErr_Clear();
      return false;
    }
    value = QString::fromUtf8(buffer, static_cast<int>(length));
    return true;
  }

  /**
   * @brief Conversion part 2 (C++ -> Python): convert an QString instance into
   * a Python object. The second and third arguments are used to
   * indicate the return value policy and parent object (for
   * ``return_value_policy::reference_internal``) and are generally
   * ignored by implicit casters.
   *
   * @param src
   * @return
   */
  static handle cast(const QString& src, return_value_policy /* policy */, handle /* parent */)
  {
#if PY_VERSION_HEX >= 0x03030000 // Python 3.3
    assert(sizeof(QChar) == 2);
    return PyUnicode_FromKindAndData(PyUnicode_2BYTE_KIND, src.constData(), src.length());
#else
    QByteArray a = src.toUtf8();
    return PyUnicode_FromStringAndSize(a.data(), (ssize_t)a.length());
#endif
  }
};
/* Create a TypeCaster for auto python tuple <--> QPair conversion */
template <>
struct type_caster<FPRangePair>
{
public:
  /**
   * This macro establishes the name 'FPRangePair' in
   * function signatures and declares a local variable
   * 'value' of type FPRangePair
   */
  PYBIND11_TYPE_CASTER(FPRangePair, _("FPRangePair"));

  /**
   *  @brief Conversion part 1 (Python->C++): convert a tuple into a FPRangePair
   * instance or return false upon failure. The second argument
   * indicates whether implicit conversions should be applied.
   * @param src
   * @return boolean
   */
  bool load(handle src, bool b)
  {
    if(!src)
    {
      return false;
    }
    if(py::isinstance<py::tuple>(src))
    {
      py::tuple valuePair = py::cast<py::tuple>(src);
      value = FPRangePair();
      value.first = py::cast<double>(valuePair[0]);
      value.second = py::cast<double>(valuePair[1]);
      return true;
    }
    return false;
  }

  /**
   * @brief Conversion part 2 (C++ -> Python): convert  FPRangePair instance into
   * a Python object. The second and third arguments are used to
   * indicate the return value policy and parent object (for
   * ``return_value_policy::reference_internal``) and are generally
   * ignored by implicit casters.
   *
   * @param src
   * @return
   */
  static handle cast(const FPRangePair& src, return_value_policy /* policy */, handle /* parent */)
  {
    return py::tuple((src.first, src.second));
  }
};

/* Create a TypeCaster for auto python dict <--> ASCIIWizardData conversion */
template <>
struct type_caster<ASCIIWizardData>
{
public:
  /**
   * This macro establishes the name 'ASCIIWizardData' in
   * function signatures and declares a local variable
   * 'value' of type ASCIIWizardData
   */
  PYBIND11_TYPE_CASTER(ASCIIWizardData, _("ASCIIWizardData"));

  /**
   *  @brief Conversion part 1 (Python->C++): convert a dict into a ASCIIWizardData
   * instance or return false upon failure. The second argument
   * indicates whether implicit conversions should be applied.
   * @param src
   * @return boolean
   */
  bool load(handle src, bool b)
  {
    if(!src)
    {
      return false;
    }
    if(py::isinstance<py::dict>(src))
    {
      py::dict awdDict = py::cast<py::dict>(src);
      value = ASCIIWizardData();
      value.inputFilePath = py::cast<QString>(awdDict["inputFilePath"]);
      value.beginIndex = py::cast<int>(awdDict["beginIndex"]);
      value.numberOfLines = py::cast<int>(awdDict["numberOfLines"]);
      QList<char> delimiters;
      for(auto object : awdDict["delimiters"])
      {
        delimiters.push_back(py::cast<char>(object));
      }
      value.delimiters = delimiters;
      value.consecutiveDelimiters = py::cast<bool>(awdDict["consecutiveDelimiters"]);
      value.automaticAM = py::cast<bool>(awdDict["automaticAM"]);
      value.selectedPath = py::cast<DataArrayPath>(awdDict["selectedPath"]);
      QStringList dataHeaders;
      for(auto header : awdDict["headers"])
      {
        dataHeaders.push_back(py::cast<QString>(header));
      }
      value.dataHeaders = dataHeaders;
      value.attrMatType = py::cast<int>(awdDict["attrMatType"]);
      std::vector<size_t> tupleDims;
      for(auto tupleDim : awdDict["tupleDimensions"])
      {
        tupleDims.push_back(py::cast<size_t>(tupleDim));
      }
      value.tupleDims = tupleDims;
      QStringList dataTypes;
      for(auto dataType : awdDict["dataTypes"])
      {
        dataTypes.push_back(py::cast<QString>(dataType));
      }
      value.dataTypes = dataTypes;
      return true;
    }
    return false;
  }

  /**
   * @brief Conversion part 2 (C++ -> Python): convert  ASCIIWizardData instance into
   * a Python object. The second and third arguments are used to
   * indicate the return value policy and parent object (for
   * ``return_value_policy::reference_internal``) and are generally
   * ignored by implicit casters.
   *
   * @param src
   * @return
   */
  static handle cast(const ASCIIWizardData& src, return_value_policy /* policy */, handle /* parent */)
  {
    py::dict awdDict = py::dict();
    awdDict["inputFilePath"] = src.inputFilePath;
    awdDict["beginIndex"] = src.beginIndex;
    awdDict["numberOfLines"] = src.numberOfLines;
    awdDict["delimiters"] = src.delimiters;
    awdDict["consecutiveDelimiters"] = src.consecutiveDelimiters;
    awdDict["automaticAM"] = src.automaticAM;
    awdDict["selectedPath"] = src.selectedPath;
    awdDict["headers"] = src.dataHeaders;
    awdDict["attrMatType"] = src.attrMatType;
    awdDict["tupleDimensions"] = src.tupleDims;
    awdDict["dataTypes"] = src.dataTypes;
    return awdDict;
  }
};

/* Create a TypeCaster for auto python list <--> QVector<DataArrayPath> conversion */
template <>
struct type_caster<QVector<DataArrayPath>>
{
public:
  /**
   * This macro establishes the name 'QVector<DataArrayPath>' in
   * function signatures and declares a local variable
   * 'value' of type QVector<DataArrayPath>
   */
  PYBIND11_TYPE_CASTER(QVector<DataArrayPath>, _("QVector<DataArrayPath>"));

  /**
   *  @brief Conversion part 1 (Python->C++): convert a lsit into a QVector<DataArrayPath>
   * instance or return false upon failure. The second argument
   * indicates whether implicit conversions should be applied.
   * @param src
   * @return boolean
   */
  bool load(handle src, bool b)
  {
    if(!src)
    {
      return false;
    }
    if(py::isinstance<py::list>(src))
    {
      value = QVector<DataArrayPath>();
      for(auto dataArrayPath : src)
      {
        value.push_back(py::cast<DataArrayPath>(dataArrayPath));
      }
      return true;
    }
    return false;
  }

  /**
   * @brief Conversion part 2 (C++ -> Python): convert QVector<DataArrayPath> instance into
   * a Python object. The second and third arguments are used to
   * indicate the return value policy and parent object (for
   * ``return_value_policy::reference_internal``) and are generally
   * ignored by implicit casters.
   *
   * @param src
   * @return
   */
  static handle cast(const QVector<DataArrayPath>& src, return_value_policy /* policy */, handle /* parent */)
  {
    py::list dataArrayPaths = py::list();
    for(auto dataArrayPath : src)
    {
      dataArrayPaths.append(dataArrayPath);
    }
    return dataArrayPaths;
  }
};

/* Create a TypeCaster for auto python dict <--> QMap<QString, DataContainerProxy> conversion */
template <>
struct type_caster<DataContainersMap>
{
public:
  /**
   * This macro establishes the name 'QMap<QString, V>' in
   * function signatures and declares a local variable
   * 'value' of type QMap<QString, V>
   */
  PYBIND11_TYPE_CASTER(DataContainersMap, _("DataContainersMap"));

  /**
   *  @brief Conversion part 1 (Python->C++): convert a lsit into a QMap<QString, V>
   * instance or return false upon failure. The second argument
   * indicates whether implicit conversions should be applied.
   * @param src
   * @return boolean
   */
  bool load(handle src, bool b)
  {
    if(!src)
    {
      return false;
    }
    if(py::isinstance<py::dict>(src))
    {
      std::map<QString, DataContainerProxy> mapDict = py::cast<std::map<QString, DataContainerProxy>>(src);
      value = QMap<QString, DataContainerProxy>();
      for(auto it = mapDict.begin(); it != mapDict.end(); ++it)
      {
        value.insert(it->first, it->second);
      }
      return true;
    }
    return false;
  }

  /**
   * @brief Conversion part 2 (C++ -> Python): convert QMap<QString, V> instance into
   * a Python object. The second and third arguments are used to
   * indicate the return value policy and parent object (for
   * ``return_value_policy::reference_internal``) and are generally
   * ignored by implicit casters.
   *
   * @param src
   * @return
   */
  static handle cast(const QMap<QString, DataContainerProxy>& src, return_value_policy /* policy */, handle /* parent */)
  {
    std::map<QString, DataContainerProxy> qmapStd = src.toStdMap();
    py::dict qmapDict = py::dict();
    for(auto it = qmapStd.begin(); it != qmapStd.end(); ++it)
    {
      QString key(it->first);
      qmapDict[py::str(key.toStdString())] = py::cast(it->second);
    }
    return qmapDict;
  }
};

/* Create a TypeCaster for auto python dict <--> QMap<QString, DataContainerProxy> conversion */
template <>
struct type_caster<AttributeMatricesMap>
{
public:
  /**
   * This macro establishes the name 'QMap<QString, V>' in
   * function signatures and declares a local variable
   * 'value' of type QMap<QString, V>
   */
  PYBIND11_TYPE_CASTER(AttributeMatricesMap, _("AttributeMatricesMap"));

  /**
   *  @brief Conversion part 1 (Python->C++): convert a lsit into a QMap<QString, V>
   * instance or return false upon failure. The second argument
   * indicates whether implicit conversions should be applied.
   * @param src
   * @return boolean
   */
  bool load(handle src, bool b)
  {
    if(!src)
    {
      return false;
    }
    if(py::isinstance<py::dict>(src))
    {
      std::map<QString, AttributeMatrixProxy> mapDict = py::cast<std::map<QString, AttributeMatrixProxy>>(src);
      value = QMap<QString, AttributeMatrixProxy>();
      for(auto it = mapDict.begin(); it != mapDict.end(); ++it)
      {
        value.insert(it->first, it->second);
      }
      return true;
    }
    return false;
  }

  /**
   * @brief Conversion part 2 (C++ -> Python): convert QMap<QString, V> instance into
   * a Python object. The second and third arguments are used to
   * indicate the return value policy and parent object (for
   * ``return_value_policy::reference_internal``) and are generally
   * ignored by implicit casters.
   *
   * @param src
   * @return
   */
  static handle cast(const QMap<QString, AttributeMatrixProxy>& src, return_value_policy /* policy */, handle /* parent */)
  {
    py::dict qmapDict = py::dict();
    std::map<QString, AttributeMatrixProxy> qmapStd = src.toStdMap();
    for(auto it = qmapStd.begin(); it != qmapStd.end(); ++it)
    {
      QString key(it->first);
      qmapDict[py::str(key.toStdString())] = py::cast(it->second);
    }
    return qmapDict;
  }
};

/* Create a TypeCaster for auto python list <--> QVector<ShapeType::Type> conversion */
template <>
struct type_caster<QVector<ShapeType::Type>>
{
public:
  /**
   * This macro establishes the name 'QVector<ShapeType::Type>' in
   * function signatures and declares a local variable
   * 'value' of type QVector<ShapeType::Type>
   */
  PYBIND11_TYPE_CASTER(QVector<ShapeType::Type>, _("QVector<ShapeType::Type>"));

  /**
   *  @brief Conversion part 1 (Python->C++): convert a lsit into a QVector<ShapeType::Type>
   * instance or return false upon failure. The second argument
   * indicates whether implicit conversions should be applied.
   * @param src
   * @return boolean
   */
  bool load(handle src, bool b)
  {
    if(!src)
    {
      return false;
    }
    if(py::isinstance<py::list>(src))
    {
      value = QVector<ShapeType::Type>();
      for(auto shapeType : src)
      {
        value.push_back(py::cast<ShapeType::Type>(shapeType));
      }
      return true;
    }
    return false;
  }

  /**
   * @brief Conversion part 2 (C++ -> Python): convert QVector<ShapeType::Type> instance into
   * a Python object. The second and third arguments are used to
   * indicate the return value policy and parent object (for
   * ``return_value_policy::reference_internal``) and are generally
   * ignored by implicit casters.
   *
   * @param src
   * @return
   */
  static handle cast(const QVector<ShapeType::Type>& src, return_value_policy /* policy */, handle /* parent */)
  {
    py::list shapeTypes = py::list();
    for(auto shapeType : src)
    {
      shapeTypes.append(shapeType);
    }
    return shapeTypes;
  }
};

/* Create a TypeCaster for auto python list <--> QVector<PhaseType::Type> conversion */
template <>
struct type_caster<QVector<PhaseType::Type>>
{
public:
  /**
   * This macro establishes the name 'QVector<PhaseType::Type>' in
   * function signatures and declares a local variable
   * 'value' of type QVector<PhaseType::Type>
   */
  PYBIND11_TYPE_CASTER(QVector<PhaseType::Type>, _("QVector<PhaseType::Type>"));

  /**
   *  @brief Conversion part 1 (Python->C++): convert a lsit into a QVector<PhaseType::Type>
   * instance or return false upon failure. The second argument
   * indicates whether implicit conversions should be applied.
   * @param src
   * @return boolean
   */
  bool load(handle src, bool)
  {
    if(!src)
    {
      return false;
    }
    if(py::isinstance<py::list>(src))
    {
      value = QVector<PhaseType::Type>();
      for(auto phaseType : src)
      {
        value.push_back(py::cast<PhaseType::Type>(phaseType));
      }
      return true;
    }
    else
    {
      return false;
    }
  }

  /**
   * @brief Conversion part 2 (C++ -> Python): convert QVector<PhaseType::Type> instance into
   * a Python object. The second and third arguments are used to
   * indicate the return value policy and parent object (for
   * ``return_value_policy::reference_internal``) and are generally
   * ignored by implicit casters.
   *
   * @param src
   * @return
   */
  static handle cast(const QVector<PhaseType::Type>& src, return_value_policy /* policy */, handle /* parent */)
  {
    py::list phaseTypes = py::list();
    for(PhaseType::Type phaseType : src)
    {
      phaseTypes.append(phaseType);
    }
    return phaseTypes;
  }
};

/* Create a TypeCaster for auto python list <--> VectorOfFloatArray conversion */
template <>
struct type_caster<VectorOfFloatArray>
{
public:
  /**
   * This macro establishes the name 'VectorOfFloatArray' in
   * function signatures and declares a local variable
   * 'value' of type VectorOfFloatArray
   */
  PYBIND11_TYPE_CASTER(VectorOfFloatArray, _("VectorOfFloatArray"));

  /**
   *  @brief Conversion part 1 (Python->C++): convert a list into a VectorOfFloatArray
   * instance or return false upon failure. The second argument
   * indicates whether implicit conversions should be applied.
   * @param src
   * @return boolean
   */
  bool load(handle src, bool b)
  {
    if(!src)
    {
      return false;
    }
    if(py::isinstance<py::list>(src))
    {
      /* Extract PyObject from handle */
      PyObject* source = src.ptr();
      /* Try converting into a Python integer value */
      VectorOfFloatArray floatArrays;
      for(auto floatArray : src)
      {
        if(py::isinstance<py::list>(floatArray) || py::isinstance<py::tuple>(floatArray))
        {
          FloatArrayType::Pointer floatArrayPtr;
          std::vector<float> floatArrayVector;
          size_t index = 0;
          QString name;
          for(auto value : floatArray)
          {
            if(index == 0)
            {
              name = py::cast<QString>(value);
            }
            else
            {
              floatArrayVector.push_back(py::cast<float>(value));
            }
            index++;
          }
          floatArrayPtr = FloatArrayType::FromStdVector(floatArrayVector, name);
          floatArrays.push_back(floatArrayPtr);
        }
        else if(py::isinstance<FloatArrayType>(floatArray))
        {
          PyObject* floatArrayPy = floatArray.ptr();
          floatArrays.push_back(py::cast<FloatArrayType::Pointer>(floatArray));
          Py_XDECREF(floatArrayPy);
        }
        else
        {
          py::print("object must be list, tuple, or FloatArray");
        }
      }

      value = floatArrays;
      Py_XDECREF(source);
      return true;
    }
    return false;
  }

  /**
   * @brief Conversion part 2 (C++ -> Python): convert VectorOfFloatArray instance into
   * a Python object. The second and third arguments are used to
   * indicate the return value policy and parent object (for
   * ``return_value_policy::reference_internal``) and are generally
   * ignored by implicit casters.
   *
   * @param src
   * @return
   */
  static handle cast(const VectorOfFloatArray& src, return_value_policy /* policy */, handle /* parent */)
  {
    py::list floatArrays = py::list();
    for(FloatArrayType::Pointer floatArray : src)
    {
      py::list floatArrayList;
      floatArrayList.append(floatArray->getName());
      for(float value : *floatArray)
      {
        floatArrayList.append(value);
      }

      floatArrays.append(floatArray);
    }
    return floatArrays;
  }
};

} // namespace detail
} // namespace pybind11

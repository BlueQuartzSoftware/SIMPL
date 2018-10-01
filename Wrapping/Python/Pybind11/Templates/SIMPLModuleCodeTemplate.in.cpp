
/* =============================================================================
 * BEGIN Template file SIMPLModuleCodeTemplate.in.cpp
 * ========================================================================== */

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>

PYBIND11_MAKE_OPAQUE(std::vector<int8_t>);
PYBIND11_MAKE_OPAQUE(std::vector<uint8_t>);
PYBIND11_MAKE_OPAQUE(std::vector<int16_t>);
PYBIND11_MAKE_OPAQUE(std::vector<uint16_t>);
PYBIND11_MAKE_OPAQUE(std::vector<int32_t>);
PYBIND11_MAKE_OPAQUE(std::vector<uint32_t>);
PYBIND11_MAKE_OPAQUE(std::vector<int64_t>);
PYBIND11_MAKE_OPAQUE(std::vector<uint64_t>);

PYBIND11_MAKE_OPAQUE(std::vector<float>);
PYBIND11_MAKE_OPAQUE(std::vector<double>);

/*
 * Linux does not like the below line because unsigned long int and size_t are
 * the same thing. Apple (clang) and Windows (MSVC) do not seem to have a problem
 * with the line.
 */
#if defined(__APPLE__)
PYBIND11_MAKE_OPAQUE(std::vector<size_t>);
#endif

#include <utility>

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include "SIMPLib/Common/ShapeType.h"
#include "SIMPLib/FilterParameters/RangeFilterParameter.h"
#include "SIMPLib/CoreFilters/util/ASCIIWizardData.hpp"
#include "SIMPLib/CoreFilters/CreateDataArray.h"
#include "SIMPLib/DataContainers/DataContainerProxy.h"
#include "OrientationLib/OrientationMath/OrientationConverter.hpp"

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef QMap<QString, DataContainerProxy> DataContainersMap;
typedef QMap<QString, AttributeMatrixProxy> AttributeMatricesMap;
typedef QVector<FloatArrayType::Pointer> VectorOfFloatArray;

//PYBIND11_MAKE_OPAQUE(DataContainersMap)
//PYBIND11_MAKE_OPAQUE(AttributeMatricesMap)

namespace pybind11
{
namespace detail
{
/* Create a TypeCaster for auto python string <--> QString conversion */
template <> struct type_caster<QString>
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
  bool load(handle src, bool)
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
		template <> struct type_caster<FPRangePair>
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
			bool load(handle src, bool)
			{
				if (!src)
				{
					return false;
				}
				if (py::isinstance<py::tuple>(src))
				{
					py::tuple valuePair = py::cast<py::tuple>(src);
					value = FPRangePair();
					value.first = py::cast<double>(valuePair[0]);
					value.second = py::cast<double>(valuePair[1]);
					return true;
				}
				else
				{
					return false;
				}
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
		template <> struct type_caster<ASCIIWizardData>
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
			bool load(handle src, bool)
			{
				if (!src)
				{
					return false;
				}
				if (py::isinstance<py::dict>(src))
				{
					py::dict awdDict = py::cast<py::dict>(src);
					value = ASCIIWizardData();
					value.inputFilePath = py::cast<QString>(awdDict["inputFilePath"]);
					value.beginIndex = py::cast<int>(awdDict["beginIndex"]);
					value.numberOfLines = py::cast<int>(awdDict["numberOfLines"]);
					QList<char> delimiters;
					for (auto object : awdDict["delimiters"])
					{
						delimiters.push_back(py::cast<char>(object));
					}
					value.delimiters = delimiters;
					value.consecutiveDelimiters = py::cast<bool>(awdDict["consecutiveDelimiters"]);
					value.automaticAM = py::cast<bool>(awdDict["automaticAM"]);
					value.selectedPath = py::cast<DataArrayPath>(awdDict["selectedPath"]);
					QStringList dataHeaders;
					for (auto header : awdDict["headers"])
					{
						dataHeaders.push_back(py::cast<QString>(header));
					}
					value.dataHeaders = dataHeaders;
					value.attrMatType = py::cast<int>(awdDict["attrMatType"]);
					QVector<size_t> tupleDims;
					for (auto tupleDim : awdDict["tupleDimensions"])
					{
						tupleDims.push_back(py::cast<size_t>(tupleDim));
					}
					value.tupleDims = tupleDims;
					QStringList dataTypes;
					for (auto dataType : awdDict["dataTypes"])
					{
						dataTypes.push_back(py::cast<QString>(dataType));
					}
					value.dataTypes = dataTypes;
					return true;
				}
				else
				{
					return false;
				}
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
				awdDict["conescutiveDelimiters"] = src.consecutiveDelimiters;
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
		template <> struct type_caster<QVector<DataArrayPath>>
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
			bool load(handle src, bool)
			{
				if (!src)
				{
					return false;
				}
				if (py::isinstance<py::list>(src))
				{
					value = QVector<DataArrayPath>();
					for (auto dataArrayPath : src)
					{
						value.push_back(py::cast<DataArrayPath>(dataArrayPath));
					}
					return true;
				}
				else
				{
					return false;
				}
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
				for (auto dataArrayPath : src) {
					dataArrayPaths.append(dataArrayPath);
				}
				return dataArrayPaths;
			}
		};

     /* Create a TypeCaster for auto python dict <--> QMap<QString, DataContainerProxy> conversion */
		template <> struct type_caster<DataContainersMap>
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
			bool load(handle src, bool)
			{
				if (!src)
				{
					return false;
				}
				if (py::isinstance<py::dict>(src))
				{
					std::map<QString, DataContainerProxy> mapDict = py::cast<std::map<QString, DataContainerProxy>>(src);
					value = QMap<QString, DataContainerProxy>();
					for (auto it = mapDict.begin(); it != mapDict.end(); ++it) {
						value.insert(it->first, it->second);
					}
					return true;
				}
				else
				{
					return false;
				}
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
				for (auto it = qmapStd.begin(); it != qmapStd.end(); ++it) {
					QString key(it->first);
					qmapDict[py::str(key.toStdString())] = py::cast(it->second);
				}
				return qmapDict;
			}
		};

		/* Create a TypeCaster for auto python dict <--> QMap<QString, DataContainerProxy> conversion */
		template <> struct type_caster<AttributeMatricesMap>
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
			bool load(handle src, bool)
			{
				if (!src)
				{
					return false;
				}
				if (py::isinstance<py::dict>(src))
				{
					std::map<QString, AttributeMatrixProxy> mapDict = py::cast<std::map<QString, AttributeMatrixProxy>>(src);
					value = QMap<QString, AttributeMatrixProxy>();
					for (auto it = mapDict.begin(); it != mapDict.end(); ++it) {
						value.insert(it->first, it->second);
					}
					return true;
				}
				else
				{
					return false;
				}
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
				for (auto it = qmapStd.begin(); it != qmapStd.end(); ++it) {
					QString key(it->first);
					qmapDict[py::str(key.toStdString())] = py::cast(it->second);
				}
				return qmapDict;
			}
		};

		/* Create a TypeCaster for auto python list <--> QVector<ShapeType::Type> conversion */
		template <> struct type_caster<QVector<ShapeType::Type>>
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
			bool load(handle src, bool)
			{
				if (!src)
				{
					return false;
				}
				if (py::isinstance<py::list>(src))
				{
					value = QVector<ShapeType::Type>();
					for (auto shapeType : src)
					{
						value.push_back(py::cast<ShapeType::Type>(shapeType));
					}
					return true;
				}
				else
				{
					return false;
				}
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
				for (auto shapeType : src) {
					shapeTypes.append(shapeType);
				}
				return shapeTypes;
			}
		};

		/* Create a TypeCaster for auto python list <--> VectorOfFloatArray conversion */
		template <> struct type_caster<VectorOfFloatArray>
		{
		public:
			/**
			 * This macro establishes the name 'VectorOfFloatArray' in
			 * function signatures and declares a local variable
			 * 'value' of type VectorOfFloatArray
			 */
			PYBIND11_TYPE_CASTER(VectorOfFloatArray, _("VectorOfFloatArray"));

			/**
			 *  @brief Conversion part 1 (Python->C++): convert a lsit into a VectorOfFloatArray
			 * instance or return false upon failure. The second argument
			 * indicates whether implicit conversions should be applied.
			 * @param src
			 * @return boolean
			 */
			bool load(handle src, bool)
			{
				py::print("Python to C++");
				if (!src)
				{
					return false;
				}
				if (py::isinstance<py::list>(src))
				{
					/* Extract PyObject from handle */
					PyObject *source = src.ptr();
					/* Try converting into a Python integer value */
					VectorOfFloatArray floatArrays;
					for (auto floatArray : src)
					{
						py::print(floatArray);
						if (py::isinstance<py::list>(floatArray) || py::isinstance<py::tuple>(floatArray))
						{
							FloatArrayType::Pointer floatArrayPtr;
							std::vector<float> floatArrayVector;
							size_t index = 0;
							for (auto value : floatArray)
							{
								if (index == 0)
								{
									QString name = py::cast<QString>(value);
									floatArrayPtr->setName(name);
								}
								else 
								{
									floatArrayVector.push_back(py::cast<float>(value));
								}
							}
							floatArrayPtr = FloatArrayType::FromStdVector(floatArrayVector, "FloatArrays");
							floatArrays.push_back(floatArrayPtr);
						}
						else if (py::isinstance<FloatArrayType>(floatArray))
						{
							PyObject *floatArrayPy = floatArray.ptr();
							floatArrays.push_back(py::cast<FloatArrayType::Pointer>(floatArray));
							Py_XDECREF(floatArrayPy);
						}
						else
						{
							py::print(floatArray, " is an invalid float array");
						}
					}

					value = floatArrays;
					Py_XDECREF(source);
					return true;
				}
				else
				{
					return false;
				}
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
				std::cout << "C++ to Python" << std::endl;
				py::list floatArrays = py::list();
				for (FloatArrayType::Pointer floatArray : src) 
				{			
					std::cout << "Float array name: " << floatArray->getName().toStdString() << std::endl;
					py::list floatArrayList = py::list();
					floatArrayList.append(floatArray->getName());
					for (float value : floatArray->getArray())
					{
						std::cout << "Value: " << value << std::endl;
						floatArrayList.append(value);
					}
					
					floatArrays.append(floatArray);
				}
				return floatArrays;
			}
		};

	} // namespace detail
} // namespace pybind11

#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T> using PySharedPtrClass = py::class_<T, std::shared_ptr<T>>;
#endif


/******************************************************************************
 * This is a list of all the SIMPL headers that are needed/wrapped by Pybind11
 ******************************************************************************/
@HEADER_PATH@

/******************************************************************************
 *
 ******************************************************************************/
#include "SIMPLib/DataArrays/DataArray.hpp"

/**
 * @brief Initializes a template specialization of DataArray<T>
 * @param T The Type
 * @param NAME The name of the Variable
 */
#define PYB11_DEFINE_DATAARRAY_INIT(T, NAME)                                                                                                                                                           \
  PySharedPtrClass<DataArray<T>> declare##NAME(py::module& m, PySharedPtrClass<IDataArray>& parent)                                                                                                    \
  {                                                                                                                                                                                                    \
    using DataArrayType = DataArray<T>;                                                                                                                                                                \
    PySharedPtrClass<DataArrayType> instance(m, #NAME, parent, py::buffer_protocol());                                                                                                                 \
    instance.def(py::init([](size_t numElements, QString name, bool allocate) { return DataArrayType::CreateArray(numElements, name, allocate); }))                                                    \
        .def(py::init([](T* ptr, size_t numElements, std::vector<size_t> cDims, QString name, bool ownsData) {                                                                                         \
          return DataArrayType::WrapPointer(ptr, numElements, QVector<size_t>::fromStdVector(cDims), name, ownsData);                                                                                  \
        }))                                                                                                                                                                                            \
        .def(py::init([](py::array_t<T, py::array::c_style> b, std::vector<size_t> cDims, QString name, bool ownsData) {                                                                               \
          ssize_t numElements = 1;                                                                                                                                                                     \
          ssize_t nDims = b.ndim();                                                                                                                                                                    \
          for(ssize_t e = 0; e < nDims; e++)                                                                                                                                                           \
          {                                                                                                                                                                                            \
            numElements *= b.shape(e);                                                                                                                                                                 \
          }                                                                                                                                                                                            \
          return DataArrayType::WrapPointer(reinterpret_cast<T*>(b.mutable_data(0)), static_cast<size_t>(numElements), QVector<size_t>::fromStdVector(cDims), name, ownsData);                         \
        })) /* Class instance method setValue */                                                                                                                                                       \
        .def("setValue", &DataArrayType::setValue, py::arg("index"), py::arg("value"))                                                                                                                 \
        .def("getValue", &DataArrayType::getValue, py::arg("index"))                                                                                                                                   \
	    .def_property("Data", &DataArrayType::getArray, &DataArrayType::setArray, py::return_value_policy::reference)                                                                                          \
        .def_property("Name", &DataArrayType::getName, &DataArrayType::setName)                                                                                                                        \
        .def("Cleanup", []() { return DataArrayType::NullPointer(); });                                                                                                                                \
    ;                                                                                                                                                                                                  \
    return instance;                                                                                                                                                                                   \
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



//------------------------------------------------------------------------------
// This header file is auto-generated and contains include directives for each
// cxx file that contains a specific plugins init code.
//------------------------------------------------------------------------------
#include "DREAM3D_SubmoduleHeaders.h"


/**
 * @brief PYBIND11_MODULE This section declares our python module, its name and
 * what classes are available within the module.
 *
 */
PYBIND11_MODULE(dream3d_py, m)
{
  m.doc() = "Python wrapping for dream3d";

  /* Define a python submodule for SIMPL */
  py::module mod = m.def_submodule("simpl_py", "  Python wrapping for  SIMPL");

  //
  //
  //
  py::class_<FloatVec3_t>(mod, "FloatVec3")
      .def(py::init<const float &, const float &, const float &>())
  ;
  py::class_<FloatVec2_t>(mod, "FloatVec2")
	  .def(py::init<const float &, const float &>())
  ;
  py::class_<IntVec3_t>(mod, "IntVec3")
      .def(py::init<const int &, const int &, const int &>())
  ;

  py::class_<AxisAngleInput_t>(mod, "AxisAngleInput")
	  .def(py::init< const float &, const float &, const float &, const float &>())
  ;

  // Handle QSet of QString
  py::class_<QSet<QString>>(mod, "StringSet")
	  .def(py::init<>([](py::set stringSet) {
	  QSet<QString> newQStringSet = QSet<QString>();
	  for (auto newString : stringSet)
	  {
		  newQStringSet.insert(py::cast<QString>(newString));
	  }
	  return newQStringSet;
      }))
  ;

  // Handle QDateTime
  py::class_<QDateTime>(mod, "DateTime")
	  .def(py::init<>([](int year, int month, int day, int seconds) {
	  QDateTime dateTime(QDate(year, month, day));
	  dateTime.setTime_t(seconds);
	  return dateTime;
      }))
  ;

  /* STL Binding code */
  py::bind_vector<std::vector<int8_t>>(mod, "VectorInt8");
  py::bind_vector<std::vector<uint8_t>>(mod, "VectorUInt8");

  py::bind_vector<std::vector<int16_t>>(mod, "VectorInt16");
  py::bind_vector<std::vector<uint16_t>>(mod, "VectorUInt16");

  py::bind_vector<std::vector<int32_t>>(mod, "VectorInt32");
  py::bind_vector<std::vector<uint32_t>>(mod, "VectorUInt32");

  py::bind_vector<std::vector<int64_t>>(mod, "VectorInt64");
  py::bind_vector<std::vector<uint64_t>>(mod, "VectorUInt64");

  py::bind_vector<std::vector<float>>(mod, "VectorFloat");
  py::bind_vector<std::vector<double>>(mod, "VectorDouble");

  if(py::detail::get_type_info(typeid(std::vector<size_t>)))
  {
    mod.attr("VectorSizeT") = mod.attr("VectorUInt64");
  } 
  else 
  {
    py::bind_vector<std::vector<size_t>>(mod, "VectorSizeT");
  }

  /* Enumeration code for Comparison Operators */
  py::enum_<SIMPL::Comparison::Enumeration>(mod, "ComparisonOperators")
	  .value("LessThan", SIMPL::Comparison::Enumeration::Operator_LessThan)
	  .value("GreaterThan", SIMPL::Comparison::Enumeration::Operator_GreaterThan)
	  .value("Equal", SIMPL::Comparison::Enumeration::Operator_Equal)
	  .value("NotEqual", SIMPL::Comparison::Enumeration::Operator_NotEqual)
	  .value("Unknown", SIMPL::Comparison::Enumeration::Operator_Unknown)
	  .export_values();

  /* Enumeration code for PhaseType */
  py::enum_<PhaseType::Type>(mod, "PhaseType")
	  .value("Primary", PhaseType::Type::Primary)
	  .value("Precipitate", PhaseType::Type::Precipitate)
	  .value("Transformation", PhaseType::Type::Transformation)
	  .value("Matrix", PhaseType::Type::Matrix)
	  .value("Boundary", PhaseType::Type::Boundary)
	  .value("Unknown", PhaseType::Type::Unknown)
	  .value("Any", PhaseType::Type::Any)
	  .export_values();

  /* Enumeration code for ShapeType */
  py::enum_<ShapeType::Type>(mod, "ShapeType")
	  .value("Ellipsoid", ShapeType::Type::Ellipsoid)
	  .value("SuperEllipsoid", ShapeType::Type::SuperEllipsoid)
	  .value("CubeOctahedron", ShapeType::Type::CubeOctahedron)
	  .value("CylinderA", ShapeType::Type::CylinderA)
	  .value("CylinderB", ShapeType::Type::CylinderB)
	  .value("CylinderC", ShapeType::Type::CylinderC)
	  .value("ShapeTypeEnd", ShapeType::Type::ShapeTypeEnd)
	  .value("Unknown", ShapeType::Type::Unknown)
	  .value("Any", ShapeType::Type::Any)
	  .export_values();

  /* Enumeration code for AttributeMatrix::Type ******************/
  py::enum_<SIMPL::ScalarTypes::Type>(mod, "ScalarTypes")
    .value("Int8", SIMPL::ScalarTypes::Type::Int8)
    .value("UInt8", SIMPL::ScalarTypes::Type::UInt8)
    .value("Int16", SIMPL::ScalarTypes::Type::Int16)
    .value("UInt16", SIMPL::ScalarTypes::Type::UInt16)
    .value("Int32", SIMPL::ScalarTypes::Type::Int32)
    .value("UInt32", SIMPL::ScalarTypes::Type::UInt32)
    .value("Int64", SIMPL::ScalarTypes::Type::Int64)
    .value("UInt64", SIMPL::ScalarTypes::Type::UInt64)
    .value("Float", SIMPL::ScalarTypes::Type::Float)
    .value("Double", SIMPL::ScalarTypes::Type::Double)
    .value("Bool", SIMPL::ScalarTypes::Type::Bool)

    .export_values();

  /* Enumeration code for OrientationType */
  py::enum_<OrientationConverter<float>::OrientationType>(mod, "OrientationType")
	  .value("Euler", OrientationConverter<float>::Euler)
	  .value("OrientationMatrix", OrientationConverter<float>::OrientationMatrix)
	  .value("Quaternion", OrientationConverter<float>::Quaternion)
	  .value("AxisAngle", OrientationConverter<float>::AxisAngle)
	  .value("Rodrigues", OrientationConverter<float>::Rodrigues)
	  .value("Homochoric", OrientationConverter<float>::Homochoric)
	  .value("Cubochoric", OrientationConverter<float>::Cubochoric)
	  .value("UnknownOrientationType", OrientationConverter<float>::UnknownOrientationType)
	  .export_values();

  /* Enumeration code for Initialization Choices */
  py::enum_<CreateDataArray::InitializationChoices>(mod, "InitializationChoice")
	  .value("Manual", CreateDataArray::InitializationChoices::Manual)
	  .value("RandomWithRange", CreateDataArray::InitializationChoices::RandomWithRange)
	  .export_values();


  /* Init codes for classes in the Module */
  @MODULE_INIT_CODE@

  /* Init codes for the DataArray<T> classes */
  PySharedPtrClass<Int8ArrayType> @LIB_NAME@_Int8ArrayType = declareInt8ArrayType(mod, @LIB_NAME@_IDataArray);
  PySharedPtrClass<UInt8ArrayType> @LIB_NAME@_UInt8ArrayType = declareUInt8ArrayType(mod, @LIB_NAME@_IDataArray);

  PySharedPtrClass<Int16ArrayType> @LIB_NAME@_Int16ArrayType = declareInt16ArrayType(mod, @LIB_NAME@_IDataArray);
  PySharedPtrClass<UInt16ArrayType> @LIB_NAME@_UInt16ArrayType = declareUInt16ArrayType(mod, @LIB_NAME@_IDataArray);

  PySharedPtrClass<Int32ArrayType> @LIB_NAME@_Int32ArrayType = declareInt32ArrayType(mod, @LIB_NAME@_IDataArray);
  PySharedPtrClass<UInt32ArrayType> @LIB_NAME@_UInt32ArrayType = declareUInt32ArrayType(mod, @LIB_NAME@_IDataArray);

  PySharedPtrClass<Int64ArrayType> @LIB_NAME@_Int64ArrayType = declareInt64ArrayType(mod, @LIB_NAME@_IDataArray);
  PySharedPtrClass<UInt64ArrayType> @LIB_NAME@_UInt64ArrayType = declareUInt64ArrayType(mod, @LIB_NAME@_IDataArray);

  PySharedPtrClass<FloatArrayType> @LIB_NAME@_FloatArrayType = declareFloatArrayType(mod, @LIB_NAME@_IDataArray);
  PySharedPtrClass<DoubleArrayType> @LIB_NAME@_DoubleArrayType = declareDoubleArrayType(mod, @LIB_NAME@_IDataArray);

  py::enum_<SIMPL::InfoStringFormat>(mod, "InfoStringFormat").value("HtmlFormat", SIMPL::InfoStringFormat::HtmlFormat).value("UnknownFormat", SIMPL::InfoStringFormat::UnknownFormat).export_values();

  
  //--------------------------------------------------------------------------
  // This header file is auto-generated and contains some C++ code to create
  // a submole for each dream3d plugin
  //--------------------------------------------------------------------------
  #include "DREAM3D_SubmoduleInit.hpp"
 
}



/* =============================================================================
 * END Template file SIMPLModuleCodeTemplate.in.cpp
 * ========================================================================== */

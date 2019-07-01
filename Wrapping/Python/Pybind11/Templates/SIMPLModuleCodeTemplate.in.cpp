// clang-format off
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

#include <QtCore/QDateTime>
#include <QtCore/QString>

#include "SIMPLib/Common/PhaseType.h"
#include "SIMPLib/Common/ShapeType.h"
#include "SIMPLib/CoreFilters/ArrayCalculator.h"
#include "SIMPLib/CoreFilters/ImportHDF5Dataset.h"
#include "SIMPLib/FilterParameters/AxisAngleInput.h"
#include "SIMPLib/FilterParameters/FileListInfo.h"
#include "SIMPLib/FilterParameters/ThirdOrderPolynomial.h"

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

//PYBIND11_MAKE_OPAQUE(DataContainersMap)
//PYBIND11_MAKE_OPAQUE(AttributeMatricesMap)

#include "Templates/Pybind11CustomTypeCasts.in.h"

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
#include "SIMPLib/Common/SIMPLArray.hpp"

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
    instance.def(py::init([](size_t numElements, QString name, T initValue) { return DataArrayType::CreateArray(numElements, name, initValue); }))                                                    \
        .def(py::init([](T* ptr, size_t numElements, std::vector<size_t> cDims, QString name, bool ownsData) {                                                                                         \
          return DataArrayType::WrapPointer(ptr, numElements, cDims, name, ownsData);                                                                                  \
        }))                                                                                                                                                                                            \
        .def(py::init([](py::array_t<T, py::array::c_style> b, std::vector<size_t> cDims, QString name, bool ownsData) {                                                                               \
          ssize_t numElements = 1;                                                                                                                                                                     \
          ssize_t nDims = b.ndim();                                                                                                                                                                    \
          for(ssize_t e = 0; e < nDims; e++)                                                                                                                                                           \
          {                                                                                                                                                                                            \
            numElements *= b.shape(e);                                                                                                                                                                 \
          }                                                                                                                                                                                            \
          numElements /= cDims[0];                                                                                                                                                                     \
          return DataArrayType::WrapPointer(reinterpret_cast<T*>(b.mutable_data(0)), static_cast<size_t>(numElements), cDims, name, ownsData);                         \
        })) /* Class instance method setValue */                                                                                                                                                       \
        .def("setValue", &DataArrayType::setValue, py::arg("index"), py::arg("value"))                                                                                                                 \
        .def("getValue", &DataArrayType::getValue, py::arg("index"))                                                                                                                                   \
	      .def_property("Data", &DataArrayType::getArray, &DataArrayType::setArray, py::return_value_policy::reference)                                                                                  \
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


#define PYB11_DEFINE_SIMPLARRAY_2_INIT(T, NAME)\
  py::class_<NAME>(mod, #NAME)\
	  .def(py::init<>([](py::list values) {\
      NAME vec = NAME(py::cast<T>(values[0]), py::cast<T>(values[1]));\
      return vec;\
    }))\
	  .def("__repr__", [](const NAME &inValues) {\
		  py::list outValues;\
		  for (T value : inValues) { outValues.append(value); }\
		  return outValues;\
	  })\
	  .def("__str__", [](const NAME &inValues) {\
		  py::list outValues;\
		  for (T value : inValues) { outValues.append(value); }\
      return py::str(outValues);\
	  })\
	  .def("__getitem__", [](const NAME &values, int index) {\
		  return values[index];\
	  })\
  ;

#define PYB11_DEFINE_SIMPLARRAY_3_INIT(T, NAME)\
  py::class_<NAME>(mod, #NAME)\
	  .def(py::init<>([](py::list values) {\
      NAME vec = NAME(py::cast<T>(values[0]), py::cast<T>(values[1]), py::cast<T>(values[2]));\
      return vec;\
    }))\
	  .def("__repr__", [](const NAME &inValues) {\
		  py::list outValues;\
		  for (T value : inValues) { outValues.append(value); }\
		  return outValues;\
	  })\
	  .def("__str__", [](const NAME &inValues) {\
		  py::list outValues;\
		  for (T value : inValues) { outValues.append(value); }\
      return py::str(outValues);\
	  })\
	  .def("__getitem__", [](const NAME &values, int index) {\
		  return values[index];\
	  })\
  ;

#define PYB11_DEFINE_SIMPLARRAY_4_INIT(T, NAME)\
  py::class_<NAME>(mod, #NAME)\
	  .def(py::init<>([](py::list values) {\
      NAME vec = NAME(py::cast<T>(values[0]), py::cast<T>(values[1]), py::cast<T>(values[2]), py::cast<T>(values[3]));\
      return vec;\
    }))\
	  .def("__repr__", [](const NAME &inValues) {\
		  py::list outValues;\
		  for (T value : inValues) { outValues.append(value); }\
		  return outValues;\
	  })\
	  .def("__str__", [](const NAME &inValues) {\
		  py::list outValues;\
		  for (T value : inValues) { outValues.append(value); }\
      return py::str(outValues);\
	  })\
	  .def("__getitem__", [](const NAME &values, int index) {\
		  return values[index];\
	  })\
  ;

#define PYB11_DEFINE_SIMPLARRAY_6_INIT(T, NAME)\
  py::class_<NAME>(mod, #NAME)\
	  .def(py::init<>([](py::list values) {\
      NAME vec = NAME(py::cast<T>(values[0]), py::cast<T>(values[1]), py::cast<T>(values[2]), py::cast<T>(values[3]), py::cast<T>(values[4]), py::cast<T>(values[5]));\
      return vec;\
    }))\
	  .def("__repr__", [](const NAME &inValues) {\
		  py::list outValues;\
		  for (T value : inValues) { outValues.append(value); }\
		  return outValues;\
	  })\
	  .def("__str__", [](const NAME &inValues) {\
		  py::list outValues;\
		  for (T value : inValues) { outValues.append(value); }\
      return py::str(outValues);\
	  })\
	  .def("__getitem__", [](const NAME &values, int index) {\
		  return values[index];\
	  })\
  ;

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
PYBIND11_MODULE(dream3d, m)
{
  m.doc() = "Python wrapping for dream3d";

  /* Define a python submodule for SIMPL */
  py::module mod = m.def_submodule("simpl", "  Python wrapping for SIMPL");

  // SIMPLArray declarations/definitions
  PYB11_DEFINE_SIMPLARRAY_2_INIT(float, FloatVec2Type)
  PYB11_DEFINE_SIMPLARRAY_2_INIT(int32_t, IntVec2Type)
  PYB11_DEFINE_SIMPLARRAY_2_INIT(size_t, SizeVec2Type)

  PYB11_DEFINE_SIMPLARRAY_3_INIT(float, FloatVec3Type)
  PYB11_DEFINE_SIMPLARRAY_3_INIT(int, IntVec3Type)
  PYB11_DEFINE_SIMPLARRAY_3_INIT(size_t, SizeVec3Type)

  PYB11_DEFINE_SIMPLARRAY_4_INIT(float, FloatVec4Type)
  PYB11_DEFINE_SIMPLARRAY_4_INIT(int, IntVec4Type)
  PYB11_DEFINE_SIMPLARRAY_4_INIT(size_t, SizeVec4Type)

  PYB11_DEFINE_SIMPLARRAY_6_INIT(float, FloatVec6Type)
  PYB11_DEFINE_SIMPLARRAY_6_INIT(int32_t, IntVec6Type)

  py::class_<AxisAngleInput_t>(mod, "AxisAngleInput")
	  .def(py::init< const float &, const float &, const float &, const float &>())
  ;

  py::class_<FileListInfo_t>(mod, "FileListInfo")
	  .def(py::init <> ([] ( const int & paddingDigits, const unsigned int & ordering, const int & startIndex,
		  const int & endIndex, const int & incrementIndex, const py::str & inputPath, const py::str & filePrefix,
		  const py::str & fileSuffix, const py::str &fileExtension)
      {
				FileListInfo_t fileListInfo;
				fileListInfo.PaddingDigits = paddingDigits;
				fileListInfo.Ordering = ordering;
				fileListInfo.StartIndex = startIndex;
				fileListInfo.EndIndex = endIndex;
				fileListInfo.IncrementIndex = incrementIndex;
				QString InputPath = QString::fromStdString(py::cast<std::string>(inputPath));
				QString FilePrefix = QString::fromStdString(py::cast<std::string>(filePrefix));
				QString FileSuffix = QString::fromStdString(py::cast<std::string>(fileSuffix));
				QString FileExtension = QString::fromStdString(py::cast<std::string>(fileExtension));
				fileListInfo.InputPath = InputPath;
				fileListInfo.FilePrefix = FilePrefix;
				fileListInfo.FileSuffix = FileSuffix;
				fileListInfo.FileExtension = FileExtension;
				return fileListInfo;
      }))
  ;

#if 0
  // Handle QVector of size_t
  py::class_<std::vector<size_t>>(mod, "Dims")
	  .def(py::init<>([](py::list dimensions) {
	  std::vector<size_t> dims = std::vector<size_t>();
	  for (auto dimension : dimensions)
	  {
		  dims.push_back(py::cast<size_t>(dimension));
	  }
	  return dims;
      }))
	  .def("__repr__", [](const std::vector<size_t> &dims) {
		  py::list dimensions;
		  for (size_t dim : dims)
		  {
			  dimensions.append(dim);
		  }
		  return dimensions;
	  })
	  .def("__str__", [](const std::vector<size_t> &dims) {
		  py::list dimensions;
		  for (size_t dim : dims)
		  {
			  dimensions.append(dim);
		  }
      return py::str(dimensions);
	  })
	  .def("__getitem__", [](const std::vector<size_t> &dims, int key) {
		  return dims[key];
	  })
  ;
#endif

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

  // Handle QJsonArray
  py::class_<QJsonArray>(mod, "JsonArray")
	  .def(py::init<>([](py::list values) {
	  QJsonArray qJsonArray;
	  for (auto value : values) {
		  QJsonValue qJsonValue(py::cast<double>(value));
		  qJsonArray.push_back(qJsonValue);
	  }
	  return qJsonArray;
      }))
  ;

  py::class_<QList<ImportHDF5Dataset::DatasetImportInfo>>(mod, "DatasetImportInfoList")
     .def(py::init<>([](py::list values) {
	  QList<ImportHDF5Dataset::DatasetImportInfo> datasetImportInfoList;
	  for (auto value : values) {
		  py::list valueAsList = py::cast<py::list>(value);
		  ImportHDF5Dataset::DatasetImportInfo datasetImportInfo;
		  datasetImportInfo.dataSetPath = py::cast<QString>(valueAsList[0]);
		  datasetImportInfo.componentDimensions = py::cast<QString>(valueAsList[1]);
		  datasetImportInfoList.push_back(datasetImportInfo);
	  }
	  return datasetImportInfoList;
	  }));

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

  /* Enumeration code for Delimiter types */
  py::enum_<SIMPL::DelimiterTypes::Type>(mod, "DelimiterTypes")
	  .value("Comma", SIMPL::DelimiterTypes::Type::Comma)
	  .value("Semicolon", SIMPL::DelimiterTypes::Type::Semicolon)
	  .value("Colon", SIMPL::DelimiterTypes::Type::Colon)
	  .value("Tab", SIMPL::DelimiterTypes::Type::Tab)
	  .value("Space", SIMPL::DelimiterTypes::Type::Space)
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

  /* Enumeration code for Crystal Structures */
  py::enum_<EnsembleInfo::CrystalStructure>(mod, "CrystalStructure")
      .value("Hexagonal_High", EnsembleInfo::CrystalStructure::Hexagonal_High)
      .value("Cubic_High", EnsembleInfo::CrystalStructure::Cubic_High)
      .value("Hexagonal_Low", EnsembleInfo::CrystalStructure::Hexagonal_Low)
      .value("Cubic_Low", EnsembleInfo::CrystalStructure::Cubic_Low)
      .value("Triclinic", EnsembleInfo::CrystalStructure::Triclinic)
      .value("Monoclinic", EnsembleInfo::CrystalStructure::Monoclinic)
      .value("OrthoRhombic", EnsembleInfo::CrystalStructure::OrthoRhombic)
      .value("Tetragonal_Low", EnsembleInfo::CrystalStructure::Tetragonal_Low)
      .value("Tetragonal_High", EnsembleInfo::CrystalStructure::Tetragonal_High)
      .value("Trigonal_Low", EnsembleInfo::CrystalStructure::Trigonal_Low)
      .value("Trigonal_High", EnsembleInfo::CrystalStructure::Trigonal_High)
      .value("UnknownCrystalStructure", EnsembleInfo::CrystalStructure::UnknownCrystalStructure)
      .export_values();

  /* Enumeration code for Data types ******************/
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

  /* Enumeration code for Initialization Choices */
  py::enum_<CreateDataArray::InitializationChoices>(mod, "InitializationType")
      .value("Manual", CreateDataArray::InitializationChoices::Manual)
      .value("RandomWithRange", CreateDataArray::InitializationChoices::RandomWithRange)
      .export_values();

  /* Enumeration code for AngleUnits */
  py::enum_<ArrayCalculator::AngleUnits>(mod, "AngleUnits")
	  .value("Radians", ArrayCalculator::AngleUnits::Radians)
	  .value("Degrees", ArrayCalculator::AngleUnits::Degrees)
	  .export_values();

  py::enum_<SIMPL::NumericTypes::Type>(mod, "NumericTypes")
	  .value("Int8", SIMPL::NumericTypes::Type::Int8)
	  .value("UInt8", SIMPL::NumericTypes::Type::UInt8)
	  .value("Int16", SIMPL::NumericTypes::Type::Int16)
	  .value("UInt16", SIMPL::NumericTypes::Type::UInt16)
	  .value("Int32", SIMPL::NumericTypes::Type::Int32)
	  .value("UInt32", SIMPL::NumericTypes::Type::UInt32)
	  .value("Int64", SIMPL::NumericTypes::Type::Int64)
	  .value("UInt64", SIMPL::NumericTypes::Type::UInt64)
	  .value("Float", SIMPL::NumericTypes::Type::Float)
	  .value("Double", SIMPL::NumericTypes::Type::Double)
	  .value("Bool", SIMPL::NumericTypes::Type::Bool)
	  .value("SizeT", SIMPL::NumericTypes::Type::SizeT)
	  .value("UnknownNumType", SIMPL::NumericTypes::Type::UnknownNumType)
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
  // a submodule for each dream3d plugin
  //--------------------------------------------------------------------------
  #include "DREAM3D_SubmoduleInit.hpp"
 
}



/* =============================================================================
 * END Template file SIMPLModuleCodeTemplate.in.cpp
 * ========================================================================== */
// clang-format on

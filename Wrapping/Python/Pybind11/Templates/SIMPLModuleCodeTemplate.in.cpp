
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

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

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
  py::class_<IntVec3_t>(mod, "IntVec3")
      .def(py::init<const int &, const int &, const int &>())
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

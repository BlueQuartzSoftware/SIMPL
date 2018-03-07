#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>


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

PYBIND11_MAKE_OPAQUE(std::vector<size_t>);

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
}
}


#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T> using PySharedPtrClass = py::class_<T, std::shared_ptr<T>>;
#endif

@HEADER_PATH@

#include "SIMPLib/DataArrays/DataArray_PY11.h"

/**
 * @brief PYBIND11_MODULE This section declares our python module, its name and
 * what classes are available within the module.
 *
 */
PYBIND11_MODULE(@LIB_NAME@Py, m)
{
  py::module mod = m.def_submodule("@LIB_NAME@Py", "Python wrapping for @LIB_NAME@");
  
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

  py::bind_vector<std::vector<size_t>>(mod, "VectorSizeT");


  /* Init codes for classes in the Module */
  @MODULE_INIT_CODE@


  /* Init codes for the DataArray<T> classes */
  PySharedPtrClass<Int8ArrayType> SIMPLib_Int8ArrayType = pybind11_init_SIMPLib_Int8ArrayType(mod, SIMPLib_IDataArray);
  PySharedPtrClass<UInt8ArrayType> SIMPLib_UInt8ArrayType = pybind11_init_SIMPLib_UInt8ArrayType(mod, SIMPLib_IDataArray);

  PySharedPtrClass<Int16ArrayType> SIMPLib_Int16ArrayType = pybind11_init_SIMPLib_Int16ArrayType(mod, SIMPLib_IDataArray);
  PySharedPtrClass<UInt16ArrayType> SIMPLib_UInt16ArrayType = pybind11_init_SIMPLib_UInt16ArrayType(mod, SIMPLib_IDataArray);

  PySharedPtrClass<Int32ArrayType> SIMPLib_Int32ArrayType = pybind11_init_SIMPLib_Int32ArrayType(mod, SIMPLib_IDataArray);
  PySharedPtrClass<UInt32ArrayType> SIMPLib_UInt32ArrayType = pybind11_init_SIMPLib_UInt32ArrayType(mod, SIMPLib_IDataArray);

  PySharedPtrClass<Int64ArrayType> SIMPLib_Int64ArrayType = pybind11_init_SIMPLib_Int64ArrayType(mod, SIMPLib_IDataArray);
  PySharedPtrClass<UInt64ArrayType> SIMPLib_UInt64ArrayType = pybind11_init_SIMPLib_UInt64ArrayType(mod, SIMPLib_IDataArray);

  PySharedPtrClass<FloatArrayType> SIMPLib_FloatArrayType = pybind11_init_SIMPLib_FloatArrayType(mod, SIMPLib_IDataArray);
  PySharedPtrClass<DoubleArrayType> SIMPLib_DoubleArrayType = pybind11_init_SIMPLib_DoubleArrayType(mod, SIMPLib_IDataArray);


}

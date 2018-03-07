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

/* These are all the pybind11 headers for each for each of the exported classes */
#include "SIMPLib/Common/Observable_PY11.h"
#include "SIMPLib/Filtering/AbstractFilter_PY11.h"
#include "SIMPLib/CoreFilters/ArrayCalculator_PY11.h"
#include "SIMPLib/CoreFilters/Breakpoint_PY11.h"
#include "SIMPLib/CoreFilters/CombineAttributeArrays_PY11.h"
#include "SIMPLib/CoreFilters/CombineAttributeMatrices_PY11.h"
#include "SIMPLib/CoreFilters/ConditionalSetValue_PY11.h"
#include "SIMPLib/CoreFilters/ConvertData_PY11.h"
#include "SIMPLib/CoreFilters/CopyFeatureArrayToElementArray_PY11.h"
#include "SIMPLib/CoreFilters/CreateAttributeMatrix_PY11.h"
#include "SIMPLib/CoreFilters/CreateDataArray_PY11.h"
#include "SIMPLib/CoreFilters/CreateDataContainer_PY11.h"
#include "SIMPLib/CoreFilters/CreateFeatureArrayFromElementArray_PY11.h"
#include "SIMPLib/CoreFilters/CreateImageGeometry_PY11.h"
#include "SIMPLib/CoreFilters/CreateStringArray_PY11.h"
#include "SIMPLib/CoreFilters/DataContainerReader_PY11.h"
#include "SIMPLib/CoreFilters/DataContainerWriter_PY11.h"
#include "SIMPLib/CoreFilters/EmptyFilter_PY11.h"
#include "SIMPLib/CoreFilters/ExecuteProcess_PY11.h"
#include "SIMPLib/CoreFilters/ExtractComponentAsArray_PY11.h"
#include "SIMPLib/CoreFilters/FeatureCountDecision_PY11.h"
#include "SIMPLib/CoreFilters/FeatureDataCSVWriter_PY11.h"
#include "SIMPLib/CoreFilters/FileWriter_PY11.h"
#include "SIMPLib/CoreFilters/FindDerivatives_PY11.h"
#include "SIMPLib/CoreFilters/GenerateColorTable_PY11.h"
#include "SIMPLib/CoreFilters/ImportAsciDataArray_PY11.h"
#include "SIMPLib/CoreFilters/ImportHDF5Dataset_PY11.h"
#include "SIMPLib/CoreFilters/LinkFeatureMapToElementArray_PY11.h"
#include "SIMPLib/CoreFilters/MaskCountDecision_PY11.h"
#include "SIMPLib/CoreFilters/MoveData_PY11.h"
#include "SIMPLib/CoreFilters/MoveMultiData_PY11.h"
#include "SIMPLib/CoreFilters/MultiThresholdObjects_PY11.h"
#include "SIMPLib/CoreFilters/MultiThresholdObjects2_PY11.h"
#include "SIMPLib/CoreFilters/PipelineAnnotation_PY11.h"
#include "SIMPLib/CoreFilters/PostSlackMessage_PY11.h"
#include "SIMPLib/CoreFilters/RawBinaryReader_PY11.h"
#include "SIMPLib/CoreFilters/ReadASCIIData_PY11.h"
#include "SIMPLib/CoreFilters/RemoveArrays_PY11.h"
#include "SIMPLib/CoreFilters/RemoveComponentFromArray_PY11.h"
#include "SIMPLib/CoreFilters/RenameAttributeArray_PY11.h"
#include "SIMPLib/CoreFilters/RenameAttributeMatrix_PY11.h"
#include "SIMPLib/CoreFilters/RenameDataContainer_PY11.h"
#include "SIMPLib/CoreFilters/ReplaceValueInArray_PY11.h"
#include "SIMPLib/CoreFilters/RequiredZThickness_PY11.h"
#include "SIMPLib/CoreFilters/ScaleVolume_PY11.h"
#include "SIMPLib/CoreFilters/SetOriginResolutionImageGeom_PY11.h"
#include "SIMPLib/CoreFilters/WriteASCIIData_PY11.h"
#include "SIMPLib/CoreFilters/WriteTriangleGeometry_PY11.h"
#include "SIMPLib/TestFilters/ArraySelectionExample_PY11.h"
#include "SIMPLib/TestFilters/ChangeGlobalValue_PY11.h"
#include "SIMPLib/TestFilters/DynamicTableExample_PY11.h"
#include "SIMPLib/TestFilters/ErrorWarningFilter_PY11.h"
#include "SIMPLib/TestFilters/GenericExample_PY11.h"
#include "SIMPLib/TestFilters/MakeDataContainer_PY11.h"
#include "SIMPLib/TestFilters/ThresholdExample_PY11.h"
#include "SIMPLib/DataArrays/IDataArray_PY11.h"
#include "SIMPLib/DataContainers/AttributeMatrix_PY11.h"
#include "SIMPLib/DataContainers/DataArrayPath_PY11.h"
#include "SIMPLib/DataContainers/DataContainer_PY11.h"
#include "SIMPLib/DataContainers/DataContainerArray_PY11.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy_PY11.h"
#include "SIMPLib/Filtering/FilterPipeline_PY11.h"
#include "SIMPLib/Utilities/SIMPLH5DataReader_PY11.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements_PY11.h"


/******************************************************************************
*
******************************************************************************/

#include "SIMPLib/DataArrays/DataArray.hpp"

#define PYB11_DEFINE_DATAARRAY_INIT(T, NAME)\
PySharedPtrClass<DataArray<T>> pybind11_init_simpl_##NAME(py::module &m, PySharedPtrClass<IDataArray>& parent)\
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

/******************************************************************************
*
******************************************************************************/


/**
 * @brief PYBIND11_MODULE This section declares our python module, its name and
 * what classes are available within the module.
 *
 */
PYBIND11_MODULE(simpl, m)
{
  py::module mod = m.def_submodule("simpl", "  Python wrapping for simpl");
  
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
  /* These are all the pybind11 instantiations for each of the exported classes */

  PySharedPtrClass<Observable> simpl_Observable = pybind11_init_simpl_Observable(mod);

  PySharedPtrClass<AbstractFilter> simpl_AbstractFilter = pybind11_init_simpl_AbstractFilter(mod);

  PySharedPtrClass<ArrayCalculator> simpl_ArrayCalculator = pybind11_init_simpl_ArrayCalculator(mod, simpl_AbstractFilter);
  PySharedPtrClass<Breakpoint> simpl_Breakpoint = pybind11_init_simpl_Breakpoint(mod, simpl_AbstractFilter);
  PySharedPtrClass<CombineAttributeArrays> simpl_CombineAttributeArrays = pybind11_init_simpl_CombineAttributeArrays(mod, simpl_AbstractFilter);
  PySharedPtrClass<CombineAttributeMatrices> simpl_CombineAttributeMatrices = pybind11_init_simpl_CombineAttributeMatrices(mod, simpl_AbstractFilter);
  PySharedPtrClass<ConditionalSetValue> simpl_ConditionalSetValue = pybind11_init_simpl_ConditionalSetValue(mod, simpl_AbstractFilter);
  PySharedPtrClass<ConvertData> simpl_ConvertData = pybind11_init_simpl_ConvertData(mod, simpl_AbstractFilter);
  PySharedPtrClass<CopyFeatureArrayToElementArray> simpl_CopyFeatureArrayToElementArray = pybind11_init_simpl_CopyFeatureArrayToElementArray(mod, simpl_AbstractFilter);
  PySharedPtrClass<CreateAttributeMatrix> simpl_CreateAttributeMatrix = pybind11_init_simpl_CreateAttributeMatrix(mod, simpl_AbstractFilter);
  PySharedPtrClass<CreateDataArray> simpl_CreateDataArray = pybind11_init_simpl_CreateDataArray(mod, simpl_AbstractFilter);
  PySharedPtrClass<CreateDataContainer> simpl_CreateDataContainer = pybind11_init_simpl_CreateDataContainer(mod, simpl_AbstractFilter);
  PySharedPtrClass<CreateFeatureArrayFromElementArray> simpl_CreateFeatureArrayFromElementArray = pybind11_init_simpl_CreateFeatureArrayFromElementArray(mod, simpl_AbstractFilter);
  PySharedPtrClass<CreateImageGeometry> simpl_CreateImageGeometry = pybind11_init_simpl_CreateImageGeometry(mod, simpl_AbstractFilter);
  PySharedPtrClass<CreateStringArray> simpl_CreateStringArray = pybind11_init_simpl_CreateStringArray(mod, simpl_AbstractFilter);
  PySharedPtrClass<DataContainerReader> simpl_DataContainerReader = pybind11_init_simpl_DataContainerReader(mod, simpl_AbstractFilter);
  PySharedPtrClass<DataContainerWriter> simpl_DataContainerWriter = pybind11_init_simpl_DataContainerWriter(mod, simpl_AbstractFilter);
  PySharedPtrClass<EmptyFilter> simpl_EmptyFilter = pybind11_init_simpl_EmptyFilter(mod, simpl_AbstractFilter);
  PySharedPtrClass<ExecuteProcess> simpl_ExecuteProcess = pybind11_init_simpl_ExecuteProcess(mod, simpl_AbstractFilter);
  PySharedPtrClass<ExtractComponentAsArray> simpl_ExtractComponentAsArray = pybind11_init_simpl_ExtractComponentAsArray(mod, simpl_AbstractFilter);
  PySharedPtrClass<FeatureCountDecision> simpl_FeatureCountDecision = pybind11_init_simpl_FeatureCountDecision(mod, simpl_AbstractFilter);
  PySharedPtrClass<FeatureDataCSVWriter> simpl_FeatureDataCSVWriter = pybind11_init_simpl_FeatureDataCSVWriter(mod, simpl_AbstractFilter);
  PySharedPtrClass<FileWriter> simpl_FileWriter = pybind11_init_simpl_FileWriter(mod, simpl_AbstractFilter);
  PySharedPtrClass<FindDerivatives> simpl_FindDerivatives = pybind11_init_simpl_FindDerivatives(mod, simpl_AbstractFilter);
  PySharedPtrClass<GenerateColorTable> simpl_GenerateColorTable = pybind11_init_simpl_GenerateColorTable(mod, simpl_AbstractFilter);
  PySharedPtrClass<ImportAsciDataArray> simpl_ImportAsciDataArray = pybind11_init_simpl_ImportAsciDataArray(mod, simpl_AbstractFilter);
  PySharedPtrClass<ImportHDF5Dataset> simpl_ImportHDF5Dataset = pybind11_init_simpl_ImportHDF5Dataset(mod, simpl_AbstractFilter);
  PySharedPtrClass<LinkFeatureMapToElementArray> simpl_LinkFeatureMapToElementArray = pybind11_init_simpl_LinkFeatureMapToElementArray(mod, simpl_AbstractFilter);
  PySharedPtrClass<MaskCountDecision> simpl_MaskCountDecision = pybind11_init_simpl_MaskCountDecision(mod, simpl_AbstractFilter);
  PySharedPtrClass<MoveData> simpl_MoveData = pybind11_init_simpl_MoveData(mod, simpl_AbstractFilter);
  PySharedPtrClass<MoveMultiData> simpl_MoveMultiData = pybind11_init_simpl_MoveMultiData(mod, simpl_AbstractFilter);
  PySharedPtrClass<MultiThresholdObjects> simpl_MultiThresholdObjects = pybind11_init_simpl_MultiThresholdObjects(mod, simpl_AbstractFilter);
  PySharedPtrClass<MultiThresholdObjects2> simpl_MultiThresholdObjects2 = pybind11_init_simpl_MultiThresholdObjects2(mod, simpl_AbstractFilter);
  PySharedPtrClass<PipelineAnnotation> simpl_PipelineAnnotation = pybind11_init_simpl_PipelineAnnotation(mod, simpl_AbstractFilter);
  PySharedPtrClass<PostSlackMessage> simpl_PostSlackMessage = pybind11_init_simpl_PostSlackMessage(mod, simpl_AbstractFilter);
  PySharedPtrClass<RawBinaryReader> simpl_RawBinaryReader = pybind11_init_simpl_RawBinaryReader(mod, simpl_AbstractFilter);
  PySharedPtrClass<ReadASCIIData> simpl_ReadASCIIData = pybind11_init_simpl_ReadASCIIData(mod, simpl_AbstractFilter);
  PySharedPtrClass<RemoveArrays> simpl_RemoveArrays = pybind11_init_simpl_RemoveArrays(mod, simpl_AbstractFilter);
  PySharedPtrClass<RemoveComponentFromArray> simpl_RemoveComponentFromArray = pybind11_init_simpl_RemoveComponentFromArray(mod, simpl_AbstractFilter);
  PySharedPtrClass<RenameAttributeArray> simpl_RenameAttributeArray = pybind11_init_simpl_RenameAttributeArray(mod, simpl_AbstractFilter);
  PySharedPtrClass<RenameAttributeMatrix> simpl_RenameAttributeMatrix = pybind11_init_simpl_RenameAttributeMatrix(mod, simpl_AbstractFilter);
  PySharedPtrClass<RenameDataContainer> simpl_RenameDataContainer = pybind11_init_simpl_RenameDataContainer(mod, simpl_AbstractFilter);
  PySharedPtrClass<ReplaceValueInArray> simpl_ReplaceValueInArray = pybind11_init_simpl_ReplaceValueInArray(mod, simpl_AbstractFilter);
  PySharedPtrClass<RequiredZThickness> simpl_RequiredZThickness = pybind11_init_simpl_RequiredZThickness(mod, simpl_AbstractFilter);
  PySharedPtrClass<ScaleVolume> simpl_ScaleVolume = pybind11_init_simpl_ScaleVolume(mod, simpl_AbstractFilter);
  PySharedPtrClass<SetOriginResolutionImageGeom> simpl_SetOriginResolutionImageGeom = pybind11_init_simpl_SetOriginResolutionImageGeom(mod, simpl_AbstractFilter);
  PySharedPtrClass<WriteASCIIData> simpl_WriteASCIIData = pybind11_init_simpl_WriteASCIIData(mod, simpl_AbstractFilter);
  PySharedPtrClass<WriteTriangleGeometry> simpl_WriteTriangleGeometry = pybind11_init_simpl_WriteTriangleGeometry(mod, simpl_AbstractFilter);
  PySharedPtrClass<ArraySelectionExample> simpl_ArraySelectionExample = pybind11_init_simpl_ArraySelectionExample(mod, simpl_AbstractFilter);
  PySharedPtrClass<ChangeGlobalValue> simpl_ChangeGlobalValue = pybind11_init_simpl_ChangeGlobalValue(mod, simpl_AbstractFilter);
  PySharedPtrClass<DynamicTableExample> simpl_DynamicTableExample = pybind11_init_simpl_DynamicTableExample(mod, simpl_AbstractFilter);
  PySharedPtrClass<ErrorWarningFilter> simpl_ErrorWarningFilter = pybind11_init_simpl_ErrorWarningFilter(mod, simpl_AbstractFilter);
  PySharedPtrClass<GenericExample> simpl_GenericExample = pybind11_init_simpl_GenericExample(mod, simpl_AbstractFilter);
  PySharedPtrClass<MakeDataContainer> simpl_MakeDataContainer = pybind11_init_simpl_MakeDataContainer(mod, simpl_AbstractFilter);
  PySharedPtrClass<ThresholdExample> simpl_ThresholdExample = pybind11_init_simpl_ThresholdExample(mod, simpl_AbstractFilter);

  PySharedPtrClass<IDataArray> simpl_IDataArray = pybind11_init_simpl_IDataArray(mod);

  PySharedPtrClass<AttributeMatrix> simpl_AttributeMatrix = pybind11_init_simpl_AttributeMatrix(mod);

  PySharedPtrClass<DataArrayPath> simpl_DataArrayPath = pybind11_init_simpl_DataArrayPath(mod);

  PySharedPtrClass<DataContainer> simpl_DataContainer = pybind11_init_simpl_DataContainer(mod);

  PySharedPtrClass<DataContainerArray> simpl_DataContainerArray = pybind11_init_simpl_DataContainerArray(mod);

  PySharedPtrClass<DataContainerArrayProxy> simpl_DataContainerArrayProxy = pybind11_init_simpl_DataContainerArrayProxy(mod);

  PySharedPtrClass<FilterPipeline> simpl_FilterPipeline = pybind11_init_simpl_FilterPipeline(mod);

  PySharedPtrClass<SIMPLH5DataReader> simpl_SIMPLH5DataReader = pybind11_init_simpl_SIMPLH5DataReader(mod);

  PySharedPtrClass<SIMPLH5DataReaderRequirements> simpl_SIMPLH5DataReaderRequirements = pybind11_init_simpl_SIMPLH5DataReaderRequirements(mod);




  /* Init codes for the DataArray<T> classes */
  PySharedPtrClass<Int8ArrayType> simpl_Int8ArrayType = pybind11_init_simpl_Int8ArrayType(mod, simpl_IDataArray);
  PySharedPtrClass<UInt8ArrayType> simpl_UInt8ArrayType = pybind11_init_simpl_UInt8ArrayType(mod, simpl_IDataArray);

  PySharedPtrClass<Int16ArrayType> simpl_Int16ArrayType = pybind11_init_simpl_Int16ArrayType(mod, simpl_IDataArray);
  PySharedPtrClass<UInt16ArrayType> simpl_UInt16ArrayType = pybind11_init_simpl_UInt16ArrayType(mod, simpl_IDataArray);

  PySharedPtrClass<Int32ArrayType> simpl_Int32ArrayType = pybind11_init_simpl_Int32ArrayType(mod, simpl_IDataArray);
  PySharedPtrClass<UInt32ArrayType> simpl_UInt32ArrayType = pybind11_init_simpl_UInt32ArrayType(mod, simpl_IDataArray);

  PySharedPtrClass<Int64ArrayType> simpl_Int64ArrayType = pybind11_init_simpl_Int64ArrayType(mod, simpl_IDataArray);
  PySharedPtrClass<UInt64ArrayType> simpl_UInt64ArrayType = pybind11_init_simpl_UInt64ArrayType(mod, simpl_IDataArray);

  PySharedPtrClass<FloatArrayType> simpl_FloatArrayType = pybind11_init_simpl_FloatArrayType(mod, simpl_IDataArray);
  PySharedPtrClass<DoubleArrayType> simpl_DoubleArrayType = pybind11_init_simpl_DoubleArrayType(mod, simpl_IDataArray);


}

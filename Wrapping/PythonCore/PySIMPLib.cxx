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


#include "SIMPLib/DataArrays/DataArray_PY11.h"

/**
 * @brief PYBIND11_MODULE This section declares our python module, its name and
 * what classes are available within the module.
 *
 */
PYBIND11_MODULE(SIMPLibPy, m)
{
  py::module mod = m.def_submodule("SIMPLibPy", "Python wrapping for SIMPLib");
  
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

  PySharedPtrClass<Observable> SIMPLib_Observable = pybind11_init_SIMPLib_Observable(mod);

  PySharedPtrClass<AbstractFilter> SIMPLib_AbstractFilter = pybind11_init_SIMPLib_AbstractFilter(mod);

  PySharedPtrClass<ArrayCalculator> SIMPLib_ArrayCalculator = pybind11_init_SIMPLib_ArrayCalculator(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<Breakpoint> SIMPLib_Breakpoint = pybind11_init_SIMPLib_Breakpoint(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<CombineAttributeArrays> SIMPLib_CombineAttributeArrays = pybind11_init_SIMPLib_CombineAttributeArrays(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<CombineAttributeMatrices> SIMPLib_CombineAttributeMatrices = pybind11_init_SIMPLib_CombineAttributeMatrices(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ConditionalSetValue> SIMPLib_ConditionalSetValue = pybind11_init_SIMPLib_ConditionalSetValue(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ConvertData> SIMPLib_ConvertData = pybind11_init_SIMPLib_ConvertData(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<CopyFeatureArrayToElementArray> SIMPLib_CopyFeatureArrayToElementArray = pybind11_init_SIMPLib_CopyFeatureArrayToElementArray(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<CreateAttributeMatrix> SIMPLib_CreateAttributeMatrix = pybind11_init_SIMPLib_CreateAttributeMatrix(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<CreateDataArray> SIMPLib_CreateDataArray = pybind11_init_SIMPLib_CreateDataArray(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<CreateDataContainer> SIMPLib_CreateDataContainer = pybind11_init_SIMPLib_CreateDataContainer(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<CreateFeatureArrayFromElementArray> SIMPLib_CreateFeatureArrayFromElementArray = pybind11_init_SIMPLib_CreateFeatureArrayFromElementArray(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<CreateImageGeometry> SIMPLib_CreateImageGeometry = pybind11_init_SIMPLib_CreateImageGeometry(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<CreateStringArray> SIMPLib_CreateStringArray = pybind11_init_SIMPLib_CreateStringArray(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<DataContainerReader> SIMPLib_DataContainerReader = pybind11_init_SIMPLib_DataContainerReader(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<DataContainerWriter> SIMPLib_DataContainerWriter = pybind11_init_SIMPLib_DataContainerWriter(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<EmptyFilter> SIMPLib_EmptyFilter = pybind11_init_SIMPLib_EmptyFilter(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ExecuteProcess> SIMPLib_ExecuteProcess = pybind11_init_SIMPLib_ExecuteProcess(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ExtractComponentAsArray> SIMPLib_ExtractComponentAsArray = pybind11_init_SIMPLib_ExtractComponentAsArray(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<FeatureCountDecision> SIMPLib_FeatureCountDecision = pybind11_init_SIMPLib_FeatureCountDecision(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<FeatureDataCSVWriter> SIMPLib_FeatureDataCSVWriter = pybind11_init_SIMPLib_FeatureDataCSVWriter(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<FileWriter> SIMPLib_FileWriter = pybind11_init_SIMPLib_FileWriter(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<FindDerivatives> SIMPLib_FindDerivatives = pybind11_init_SIMPLib_FindDerivatives(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<GenerateColorTable> SIMPLib_GenerateColorTable = pybind11_init_SIMPLib_GenerateColorTable(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ImportAsciDataArray> SIMPLib_ImportAsciDataArray = pybind11_init_SIMPLib_ImportAsciDataArray(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ImportHDF5Dataset> SIMPLib_ImportHDF5Dataset = pybind11_init_SIMPLib_ImportHDF5Dataset(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<LinkFeatureMapToElementArray> SIMPLib_LinkFeatureMapToElementArray = pybind11_init_SIMPLib_LinkFeatureMapToElementArray(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<MaskCountDecision> SIMPLib_MaskCountDecision = pybind11_init_SIMPLib_MaskCountDecision(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<MoveData> SIMPLib_MoveData = pybind11_init_SIMPLib_MoveData(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<MoveMultiData> SIMPLib_MoveMultiData = pybind11_init_SIMPLib_MoveMultiData(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<MultiThresholdObjects> SIMPLib_MultiThresholdObjects = pybind11_init_SIMPLib_MultiThresholdObjects(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<MultiThresholdObjects2> SIMPLib_MultiThresholdObjects2 = pybind11_init_SIMPLib_MultiThresholdObjects2(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<PipelineAnnotation> SIMPLib_PipelineAnnotation = pybind11_init_SIMPLib_PipelineAnnotation(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<PostSlackMessage> SIMPLib_PostSlackMessage = pybind11_init_SIMPLib_PostSlackMessage(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<RawBinaryReader> SIMPLib_RawBinaryReader = pybind11_init_SIMPLib_RawBinaryReader(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ReadASCIIData> SIMPLib_ReadASCIIData = pybind11_init_SIMPLib_ReadASCIIData(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<RemoveArrays> SIMPLib_RemoveArrays = pybind11_init_SIMPLib_RemoveArrays(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<RemoveComponentFromArray> SIMPLib_RemoveComponentFromArray = pybind11_init_SIMPLib_RemoveComponentFromArray(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<RenameAttributeArray> SIMPLib_RenameAttributeArray = pybind11_init_SIMPLib_RenameAttributeArray(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<RenameAttributeMatrix> SIMPLib_RenameAttributeMatrix = pybind11_init_SIMPLib_RenameAttributeMatrix(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<RenameDataContainer> SIMPLib_RenameDataContainer = pybind11_init_SIMPLib_RenameDataContainer(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ReplaceValueInArray> SIMPLib_ReplaceValueInArray = pybind11_init_SIMPLib_ReplaceValueInArray(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<RequiredZThickness> SIMPLib_RequiredZThickness = pybind11_init_SIMPLib_RequiredZThickness(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ScaleVolume> SIMPLib_ScaleVolume = pybind11_init_SIMPLib_ScaleVolume(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<SetOriginResolutionImageGeom> SIMPLib_SetOriginResolutionImageGeom = pybind11_init_SIMPLib_SetOriginResolutionImageGeom(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<WriteASCIIData> SIMPLib_WriteASCIIData = pybind11_init_SIMPLib_WriteASCIIData(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<WriteTriangleGeometry> SIMPLib_WriteTriangleGeometry = pybind11_init_SIMPLib_WriteTriangleGeometry(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ArraySelectionExample> SIMPLib_ArraySelectionExample = pybind11_init_SIMPLib_ArraySelectionExample(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ChangeGlobalValue> SIMPLib_ChangeGlobalValue = pybind11_init_SIMPLib_ChangeGlobalValue(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<DynamicTableExample> SIMPLib_DynamicTableExample = pybind11_init_SIMPLib_DynamicTableExample(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ErrorWarningFilter> SIMPLib_ErrorWarningFilter = pybind11_init_SIMPLib_ErrorWarningFilter(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<GenericExample> SIMPLib_GenericExample = pybind11_init_SIMPLib_GenericExample(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<MakeDataContainer> SIMPLib_MakeDataContainer = pybind11_init_SIMPLib_MakeDataContainer(mod, SIMPLib_AbstractFilter);
  PySharedPtrClass<ThresholdExample> SIMPLib_ThresholdExample = pybind11_init_SIMPLib_ThresholdExample(mod, SIMPLib_AbstractFilter);

  PySharedPtrClass<IDataArray> SIMPLib_IDataArray = pybind11_init_SIMPLib_IDataArray(mod);

  PySharedPtrClass<AttributeMatrix> SIMPLib_AttributeMatrix = pybind11_init_SIMPLib_AttributeMatrix(mod);

  PySharedPtrClass<DataArrayPath> SIMPLib_DataArrayPath = pybind11_init_SIMPLib_DataArrayPath(mod);

  PySharedPtrClass<DataContainer> SIMPLib_DataContainer = pybind11_init_SIMPLib_DataContainer(mod);

  PySharedPtrClass<DataContainerArray> SIMPLib_DataContainerArray = pybind11_init_SIMPLib_DataContainerArray(mod);

  PySharedPtrClass<DataContainerArrayProxy> SIMPLib_DataContainerArrayProxy = pybind11_init_SIMPLib_DataContainerArrayProxy(mod);

  PySharedPtrClass<FilterPipeline> SIMPLib_FilterPipeline = pybind11_init_SIMPLib_FilterPipeline(mod);

  PySharedPtrClass<SIMPLH5DataReader> SIMPLib_SIMPLH5DataReader = pybind11_init_SIMPLib_SIMPLH5DataReader(mod);

  PySharedPtrClass<SIMPLH5DataReaderRequirements> SIMPLib_SIMPLH5DataReaderRequirements = pybind11_init_SIMPLib_SIMPLH5DataReaderRequirements(mod);




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

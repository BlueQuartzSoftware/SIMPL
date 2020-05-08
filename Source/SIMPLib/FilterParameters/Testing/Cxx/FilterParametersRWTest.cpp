/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QPair>

#include "SIMPLib/Common/PhaseType.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/AxisAngleFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/CalculatorFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/ComparisonSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerArrayProxyFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerReaderFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/FourthOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/InputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/PhaseTypeSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/RangeFilterParameter.h"
#include "SIMPLib/FilterParameters/SecondOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/ShapeTypeSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/ThirdOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/RangeFilterParameter.h"
#include "SIMPLib/FilterParameters/UnknownFilterParameter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class FilterParametersRWTest
{
public:
  FilterParametersRWTest()
  {
    m_Bool1 = true;
    m_Int1 = 24;
    m_Double1 = 4.7;
    m_String1 = "This is the FilterParametersRWTest";
    m_ArrayPath1 = DataArrayPath("DataContainer", "AttributeMatrix", "DataArray");

    m_Path1 = DataArrayPath("DataContainer Name", "", "");

    m_AxisAngle1.angle = 2.2f;
    m_AxisAngle1.h = 3.4f;
    m_AxisAngle1.k = 6.4f;
    m_AxisAngle1.l = 8.2f;

    m_ComparisonInputs1.addInput("DataContainer", "AttributeMatrix", "DataArrayName", 2, 4.6);

    {
      DataArrayProxy daProxy = DataArrayProxy("TestPath", "TestName");

      AttributeMatrixProxy amProxy = AttributeMatrixProxy("AttributeMatrixProxyTestName");
      amProxy.insertDataArray("TestName", daProxy);
      amProxy.setAMType(AttributeMatrix::Type::Face);
      amProxy.setFlag(3);
      amProxy.setName("AttributeMatrixProxyTestName");

      DataContainerProxy dcProxy = DataContainerProxy("DataContainerProxyTestName");
      dcProxy.insertAttributeMatrix("AttributeMatrixProxyTestName", amProxy);
      dcProxy.setDCType(1);
      dcProxy.setFlag(4);
      dcProxy.setName("DataContainerProxyTestName");

      m_DcaProxy1.insertDataContainer("DataContainerProxyTestName", dcProxy);
    }

    {
      QStringList rHeaders, cHeaders;
      rHeaders.push_back("VerticalHeader1");
      rHeaders.push_back("VerticalHeader2");
      cHeaders.push_back("HorizontalHeader1");
      cHeaders.push_back("HorizontalHeader2");
      std::vector<std::vector<double>> data(2, std::vector<double>(2, 3));

      m_DynamicData1 = DynamicTableData(data, rHeaders, cHeaders);
    }

    {
      m_FileListInfo1.EndIndex = 6;
      m_FileListInfo1.FileExtension = "txt";
      m_FileListInfo1.FilePrefix = "Prefix";
      m_FileListInfo1.FileSuffix = "Suffix";
      m_FileListInfo1.InputPath = "Path";
      m_FileListInfo1.Ordering = 42;
      m_FileListInfo1.PaddingDigits = 2;
      m_FileListInfo1.StartIndex = 3;
    }

    {
      m_FloatVec3_1[0] = 3.0f;
      m_FloatVec3_1[1] = 6.7f;
      m_FloatVec3_1[2] = 32.42f;
    }

    {
      m_IntVec3_1[0] = 3;
      m_IntVec3_1[1] = 6;
      m_IntVec3_1[2] = 32;
    }

    {
      m_Float2ndOrderPoly_1.c00 = 3.0f;
      m_Float2ndOrderPoly_1.c01 = 4.0f;
      m_Float2ndOrderPoly_1.c02 = 5.0f;
      m_Float2ndOrderPoly_1.c10 = 6.0f;
      m_Float2ndOrderPoly_1.c11 = 7.0f;
      m_Float2ndOrderPoly_1.c20 = 8.0f;
    }

    {
      m_Float3rdOrderPoly_1.c00 = 3.0f;
      m_Float3rdOrderPoly_1.c01 = 4.0f;
      m_Float3rdOrderPoly_1.c02 = 5.0f;
      m_Float3rdOrderPoly_1.c10 = 6.0f;
      m_Float3rdOrderPoly_1.c11 = 7.0f;
      m_Float3rdOrderPoly_1.c20 = 8.0f;
      m_Float3rdOrderPoly_1.c03 = 3.2f;
      m_Float3rdOrderPoly_1.c12 = 1.5f;
      m_Float3rdOrderPoly_1.c21 = 6.43f;
      m_Float3rdOrderPoly_1.c30 = 5.6f;
    }

    {
      m_Float4thOrderPoly_1.c00 = 3.0f;
      m_Float4thOrderPoly_1.c01 = 4.0f;
      m_Float4thOrderPoly_1.c02 = 5.0f;
      m_Float4thOrderPoly_1.c10 = 6.0f;
      m_Float4thOrderPoly_1.c11 = 7.0f;
      m_Float4thOrderPoly_1.c20 = 8.0f;
      m_Float4thOrderPoly_1.c03 = 3.2f;
      m_Float4thOrderPoly_1.c12 = 1.5f;
      m_Float4thOrderPoly_1.c21 = 6.43f;
      m_Float4thOrderPoly_1.c30 = 5.6f;
      m_Float4thOrderPoly_1.c04 = 6.6f;
      m_Float4thOrderPoly_1.c13 = 3.3f;
      m_Float4thOrderPoly_1.c22 = 8.9f;
      m_Float4thOrderPoly_1.c31 = 8.7f;
      m_Float4thOrderPoly_1.c40 = 7.2f;
    }

    DataArrayPath dap1("DataContainer1", "AttributeMatrix1", "DataArray1");
    DataArrayPath dap2("DataContainer2", "AttributeMatrix2", "DataArray2");
    m_DataArrayPaths1.push_back(dap1);
    m_DataArrayPaths1.push_back(dap2);

    {
      m_ShapeTypeVector_1.push_back(ShapeType::Type::CylinderA);
      m_ShapeTypeVector_1.push_back(ShapeType::Type::CylinderB);
    }

    {
      m_PhaseTypeVector_1.push_back(PhaseType::Type::Primary);
      m_PhaseTypeVector_1.push_back(PhaseType::Type::Precipitate);
      m_PhaseTypeVector_1.push_back(PhaseType::Type::Transformation);
      m_PhaseTypeVector_1.push_back(PhaseType::Type::Matrix);
      m_PhaseTypeVector_1.push_back(PhaseType::Type::Boundary);
      m_PhaseTypeVector_1.push_back(PhaseType::Type::Unknown);
      m_PhaseTypeVector_1.push_back(PhaseType::Type::Any);
    }

    {
      m_Pair1.first = 3.3;
      m_Pair1.second = 5.23;
    }
  }
  virtual ~FilterParametersRWTest() = default;

  // -----------------------------------------------------------------------------
  void setBool1(bool value)
  {
    m_Bool1 = value;
  }

  // -----------------------------------------------------------------------------
  bool getBool1() const
  {
    return m_Bool1;
  }

  Q_PROPERTY(bool Bool1 READ getBool1 WRITE setBool1)
  // -----------------------------------------------------------------------------
  void setBool2(bool value)
  {
    m_Bool2 = value;
  }

  // -----------------------------------------------------------------------------
  bool getBool2() const
  {
    return m_Bool2;
  }

  Q_PROPERTY(bool Bool2 READ getBool2 WRITE setBool2)

  // -----------------------------------------------------------------------------
  void setInt1(int value)
  {
    m_Int1 = value;
  }

  // -----------------------------------------------------------------------------
  int getInt1() const
  {
    return m_Int1;
  }

  Q_PROPERTY(int Int1 READ getInt1 WRITE setInt1)
  // -----------------------------------------------------------------------------
  void setInt2(int value)
  {
    m_Int2 = value;
  }

  // -----------------------------------------------------------------------------
  int getInt2() const
  {
    return m_Int2;
  }

  Q_PROPERTY(int Int2 READ getInt2 WRITE setInt2)

  // -----------------------------------------------------------------------------
  void setDouble1(double value)
  {
    m_Double1 = value;
  }

  // -----------------------------------------------------------------------------
  double getDouble1() const
  {
    return m_Double1;
  }

  Q_PROPERTY(double Double1 READ getDouble1 WRITE setDouble1)
  // -----------------------------------------------------------------------------
  void setDouble2(double value)
  {
    m_Double2 = value;
  }

  // -----------------------------------------------------------------------------
  double getDouble2() const
  {
    return m_Double2;
  }

  Q_PROPERTY(double Double2 READ getDouble2 WRITE setDouble2)

  // -----------------------------------------------------------------------------
  void setString1(const QString& value)
  {
    m_String1 = value;
  }

  // -----------------------------------------------------------------------------
  QString getString1() const
  {
    return m_String1;
  }

  Q_PROPERTY(QString String1 READ getString1 WRITE setString1)
  // -----------------------------------------------------------------------------
  void setString2(const QString& value)
  {
    m_String2 = value;
  }

  // -----------------------------------------------------------------------------
  QString getString2() const
  {
    return m_String2;
  }

  Q_PROPERTY(QString String2 READ getString2 WRITE setString2)

  // -----------------------------------------------------------------------------
  void setPath1(const DataArrayPath& value)
  {
    m_Path1 = value;
  }

  // -----------------------------------------------------------------------------
  DataArrayPath getPath1() const
  {
    return m_Path1;
  }

  Q_PROPERTY(DataArrayPath Path1 READ getPath1 WRITE setPath1)
  // -----------------------------------------------------------------------------
  void setPath2(const DataArrayPath& value)
  {
    m_Path2 = value;
  }

  // -----------------------------------------------------------------------------
  DataArrayPath getPath2() const
  {
    return m_Path2;
  }

  Q_PROPERTY(DataArrayPath Path2 READ getPath2 WRITE setPath2)

  // -----------------------------------------------------------------------------
  void setArrayPath1(const DataArrayPath& value)
  {
    m_ArrayPath1 = value;
  }

  // -----------------------------------------------------------------------------
  DataArrayPath getArrayPath1() const
  {
    return m_ArrayPath1;
  }

  Q_PROPERTY(DataArrayPath ArrayPath1 READ getArrayPath1 WRITE setArrayPath1)
  // -----------------------------------------------------------------------------
  void setArrayPath2(const DataArrayPath& value)
  {
    m_ArrayPath2 = value;
  }

  // -----------------------------------------------------------------------------
  DataArrayPath getArrayPath2() const
  {
    return m_ArrayPath2;
  }

  Q_PROPERTY(DataArrayPath ArrayPath2 READ getArrayPath2 WRITE setArrayPath2)

  // -----------------------------------------------------------------------------
  void setAxisAngle1(const AxisAngleInput_t& value)
  {
    m_AxisAngle1 = value;
  }

  // -----------------------------------------------------------------------------
  AxisAngleInput_t getAxisAngle1() const
  {
    return m_AxisAngle1;
  }

  Q_PROPERTY(AxisAngleInput_t AxisAngle1 READ getAxisAngle1 WRITE setAxisAngle1)
  // -----------------------------------------------------------------------------
  void setAxisAngle2(const AxisAngleInput_t& value)
  {
    m_AxisAngle2 = value;
  }

  // -----------------------------------------------------------------------------
  AxisAngleInput_t getAxisAngle2() const
  {
    return m_AxisAngle2;
  }

  Q_PROPERTY(AxisAngleInput_t AxisAngle2 READ getAxisAngle2 WRITE setAxisAngle2)

  // -----------------------------------------------------------------------------
  void setComparisonInputs1(const ComparisonInputs& value)
  {
    m_ComparisonInputs1 = value;
  }

  // -----------------------------------------------------------------------------
  ComparisonInputs getComparisonInputs1() const
  {
    return m_ComparisonInputs1;
  }

  Q_PROPERTY(ComparisonInputs ComparisonInputs1 READ getComparisonInputs1 WRITE setComparisonInputs1)
  // -----------------------------------------------------------------------------
  void setComparisonInputs2(const ComparisonInputs& value)
  {
    m_ComparisonInputs2 = value;
  }

  // -----------------------------------------------------------------------------
  ComparisonInputs getComparisonInputs2() const
  {
    return m_ComparisonInputs2;
  }

  Q_PROPERTY(ComparisonInputs ComparisonInputs2 READ getComparisonInputs2 WRITE setComparisonInputs2)

  // -----------------------------------------------------------------------------
  void setDcaProxy1(const DataContainerArrayProxy& value)
  {
    m_DcaProxy1 = value;
  }

  // -----------------------------------------------------------------------------
  DataContainerArrayProxy getDcaProxy1() const
  {
    return m_DcaProxy1;
  }

  Q_PROPERTY(DataContainerArrayProxy DcaProxy1 READ getDcaProxy1 WRITE setDcaProxy1)
  // -----------------------------------------------------------------------------
  void setDcaProxy2(const DataContainerArrayProxy& value)
  {
    m_DcaProxy2 = value;
  }

  // -----------------------------------------------------------------------------
  DataContainerArrayProxy getDcaProxy2() const
  {
    return m_DcaProxy2;
  }

  Q_PROPERTY(DataContainerArrayProxy DcaProxy2 READ getDcaProxy2 WRITE setDcaProxy2)

  // -----------------------------------------------------------------------------
  void setDynamicData1(const DynamicTableData& value)
  {
    m_DynamicData1 = value;
  }

  // -----------------------------------------------------------------------------
  DynamicTableData getDynamicData1() const
  {
    return m_DynamicData1;
  }

  Q_PROPERTY(DynamicTableData DynamicData1 READ getDynamicData1 WRITE setDynamicData1)
  // -----------------------------------------------------------------------------
  void setDynamicData2(const DynamicTableData& value)
  {
    m_DynamicData2 = value;
  }

  // -----------------------------------------------------------------------------
  DynamicTableData getDynamicData2() const
  {
    return m_DynamicData2;
  }

  Q_PROPERTY(DynamicTableData DynamicData2 READ getDynamicData2 WRITE setDynamicData2)

  // -----------------------------------------------------------------------------
  void setFileListInfo1(const StackFileListInfo& value)
  {
    m_FileListInfo1 = value;
  }

  // -----------------------------------------------------------------------------
  StackFileListInfo getFileListInfo1() const
  {
    return m_FileListInfo1;
  }

  Q_PROPERTY(StackFileListInfo FileListInfo1 READ getFileListInfo1 WRITE setFileListInfo1)
  // -----------------------------------------------------------------------------
  void setFileListInfo2(const StackFileListInfo& value)
  {
    m_FileListInfo2 = value;
  }

  // -----------------------------------------------------------------------------
  StackFileListInfo getFileListInfo2() const
  {
    return m_FileListInfo2;
  }

  Q_PROPERTY(StackFileListInfo FileListInfo2 READ getFileListInfo2 WRITE setFileListInfo2)

  // -----------------------------------------------------------------------------
  void setIntVec3_1(const IntVec3Type& value)
  {
    m_IntVec3_1 = value;
  }

  // -----------------------------------------------------------------------------
  IntVec3Type getIntVec3_1() const
  {
    return m_IntVec3_1;
  }

  Q_PROPERTY(IntVec3Type IntVec3_1 READ getIntVec3_1 WRITE setIntVec3_1)
  // -----------------------------------------------------------------------------
  void setIntVec3_2(const IntVec3Type& value)
  {
    m_IntVec3_2 = value;
  }

  // -----------------------------------------------------------------------------
  IntVec3Type getIntVec3_2() const
  {
    return m_IntVec3_2;
  }

  Q_PROPERTY(IntVec3Type IntVec3_2 READ getIntVec3_2 WRITE setIntVec3_2)

  // -----------------------------------------------------------------------------
  void setFloatVec3_1(const FloatVec3Type& value)
  {
    m_FloatVec3_1 = value;
  }

  // -----------------------------------------------------------------------------
  FloatVec3Type getFloatVec3_1() const
  {
    return m_FloatVec3_1;
  }

  Q_PROPERTY(FloatVec3Type FloatVec3_1 READ getFloatVec3_1 WRITE setFloatVec3_1)
  // -----------------------------------------------------------------------------
  void setFloatVec3_2(const FloatVec3Type& value)
  {
    m_FloatVec3_2 = value;
  }

  // -----------------------------------------------------------------------------
  FloatVec3Type getFloatVec3_2() const
  {
    return m_FloatVec3_2;
  }

  Q_PROPERTY(FloatVec3Type FloatVec3_2 READ getFloatVec3_2 WRITE setFloatVec3_2)

  // -----------------------------------------------------------------------------
  void setFloat2ndOrderPoly_1(const Float2ndOrderPoly_t& value)
  {
    m_Float2ndOrderPoly_1 = value;
  }

  // -----------------------------------------------------------------------------
  Float2ndOrderPoly_t getFloat2ndOrderPoly_1() const
  {
    return m_Float2ndOrderPoly_1;
  }

  Q_PROPERTY(Float2ndOrderPoly_t Float2ndOrderPoly_1 READ getFloat2ndOrderPoly_1 WRITE setFloat2ndOrderPoly_1)
  // -----------------------------------------------------------------------------
  void setFloat2ndOrderPoly_2(const Float2ndOrderPoly_t& value)
  {
    m_Float2ndOrderPoly_2 = value;
  }

  // -----------------------------------------------------------------------------
  Float2ndOrderPoly_t getFloat2ndOrderPoly_2() const
  {
    return m_Float2ndOrderPoly_2;
  }

  Q_PROPERTY(Float2ndOrderPoly_t Float2ndOrderPoly_2 READ getFloat2ndOrderPoly_2 WRITE setFloat2ndOrderPoly_2)

  // -----------------------------------------------------------------------------
  void setFloat3rdOrderPoly_1(const Float3rdOrderPoly_t& value)
  {
    m_Float3rdOrderPoly_1 = value;
  }

  // -----------------------------------------------------------------------------
  Float3rdOrderPoly_t getFloat3rdOrderPoly_1() const
  {
    return m_Float3rdOrderPoly_1;
  }

  Q_PROPERTY(Float3rdOrderPoly_t Float3rdOrderPoly_1 READ getFloat3rdOrderPoly_1 WRITE setFloat3rdOrderPoly_1)
  // -----------------------------------------------------------------------------
  void setFloat3rdOrderPoly_2(const Float3rdOrderPoly_t& value)
  {
    m_Float3rdOrderPoly_2 = value;
  }

  // -----------------------------------------------------------------------------
  Float3rdOrderPoly_t getFloat3rdOrderPoly_2() const
  {
    return m_Float3rdOrderPoly_2;
  }

  Q_PROPERTY(Float3rdOrderPoly_t Float3rdOrderPoly_2 READ getFloat3rdOrderPoly_2 WRITE setFloat3rdOrderPoly_2)

  // -----------------------------------------------------------------------------
  void setFloat4thOrderPoly_1(const Float4thOrderPoly_t& value)
  {
    m_Float4thOrderPoly_1 = value;
  }

  // -----------------------------------------------------------------------------
  Float4thOrderPoly_t getFloat4thOrderPoly_1() const
  {
    return m_Float4thOrderPoly_1;
  }

  Q_PROPERTY(Float4thOrderPoly_t Float4thOrderPoly_1 READ getFloat4thOrderPoly_1 WRITE setFloat4thOrderPoly_1)
  // -----------------------------------------------------------------------------
  void setFloat4thOrderPoly_2(const Float4thOrderPoly_t& value)
  {
    m_Float4thOrderPoly_2 = value;
  }

  // -----------------------------------------------------------------------------
  Float4thOrderPoly_t getFloat4thOrderPoly_2() const
  {
    return m_Float4thOrderPoly_2;
  }

  Q_PROPERTY(Float4thOrderPoly_t Float4thOrderPoly_2 READ getFloat4thOrderPoly_2 WRITE setFloat4thOrderPoly_2)

  // -----------------------------------------------------------------------------
  void setDataArrayPaths1(const QVector<DataArrayPath>& value)
  {
    m_DataArrayPaths1 = value;
  }

  // -----------------------------------------------------------------------------
  QVector<DataArrayPath> getDataArrayPaths1() const
  {
    return m_DataArrayPaths1;
  }

  Q_PROPERTY(QVector<DataArrayPath> DataArrayPaths1 READ getDataArrayPaths1 WRITE setDataArrayPaths1)
  // -----------------------------------------------------------------------------
  void setDataArrayPaths2(const QVector<DataArrayPath>& value)
  {
    m_DataArrayPaths2 = value;
  }

  // -----------------------------------------------------------------------------
  QVector<DataArrayPath> getDataArrayPaths2() const
  {
    return m_DataArrayPaths2;
  }

  Q_PROPERTY(QVector<DataArrayPath> DataArrayPaths2 READ getDataArrayPaths2 WRITE setDataArrayPaths2)

  // -----------------------------------------------------------------------------
  void setShapeTypeVector_1(const ShapeType::Types& value)
  {
    m_ShapeTypeVector_1 = value;
  }

  // -----------------------------------------------------------------------------
  ShapeType::Types getShapeTypeVector_1() const
  {
    return m_ShapeTypeVector_1;
  }

  Q_PROPERTY(ShapeType::Types ShapeTypeVector_1 READ getShapeTypeVector_1 WRITE setShapeTypeVector_1)
  // -----------------------------------------------------------------------------
  void setShapeTypeVector_2(const ShapeType::Types& value)
  {
    m_ShapeTypeVector_2 = value;
  }

  // -----------------------------------------------------------------------------
  ShapeType::Types getShapeTypeVector_2() const
  {
    return m_ShapeTypeVector_2;
  }

  Q_PROPERTY(ShapeType::Types ShapeTypeVector_2 READ getShapeTypeVector_2 WRITE setShapeTypeVector_2)

  // -----------------------------------------------------------------------------
  void setPhaseTypeVector_1(const PhaseType::Types& value)
  {
    m_PhaseTypeVector_1 = value;
  }

  // -----------------------------------------------------------------------------
  PhaseType::Types getPhaseTypeVector_1() const
  {
    return m_PhaseTypeVector_1;
  }

  Q_PROPERTY(PhaseType::Types::Types PhaseTypeVector_1 READ getPhaseTypeVector_1 WRITE setPhaseTypeVector_1)
  // -----------------------------------------------------------------------------
  void setPhaseTypeVector_2(const PhaseType::Types& value)
  {
    m_PhaseTypeVector_2 = value;
  }

  // -----------------------------------------------------------------------------
  PhaseType::Types getPhaseTypeVector_2() const
  {
    return m_PhaseTypeVector_2;
  }

  Q_PROPERTY(PhaseType::Types PhaseTypeVector_2 READ getPhaseTypeVector_2 WRITE setPhaseTypeVector_2)

  //  SIMPL_INSTANCE_PROPERTY(UInt32Vector_t, UInt32Vector_1)
  //  Q_PROPERTY(UInt32Vector_t UInt32Vector_1 READ getUInt32Vector_1 WRITE setUInt32Vector_1)
  //  SIMPL_INSTANCE_PROPERTY(UInt32Vector_t, UInt32Vector_2)
  //  Q_PROPERTY(UInt32Vector_t UInt32Vector_2 READ getUInt32Vector_2 WRITE setUInt32Vector_2)

  // -----------------------------------------------------------------------------
  void setPair1(const FPRangePair& value)
  {
    m_Pair1 = value;
  }

  // -----------------------------------------------------------------------------
  FPRangePair getPair1() const
  {
    return m_Pair1;
  }

  Q_PROPERTY(FPRangePair Pair1 READ getPair1 WRITE setPair1)
  // -----------------------------------------------------------------------------
  void setPair2(const FPRangePair& value)
  {
    m_Pair2 = value;
  }

  // -----------------------------------------------------------------------------
  FPRangePair getPair2() const
  {
    return m_Pair2;
  }

  Q_PROPERTY(FPRangePair Pair2 READ getPair2 WRITE setPair2)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::FilterParametersRWTest::OutputFile);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterParametersRW()
  {
    {
      AttributeMatrixCreationFilterParameter::RequirementType req;
      AttributeMatrixCreationFilterParameter::Pointer fp =
          AttributeMatrixCreationFilterParameter::New("Test", "ArrayPath1", getArrayPath1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, ArrayPath2),
                                                      SIMPL_BIND_GETTER(FilterParametersRWTest, this, ArrayPath1), req);

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataContainerName(), m_ArrayPath2.getDataContainerName())
      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getAttributeMatrixName(), m_ArrayPath2.getAttributeMatrixName())
      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataArrayName(), m_ArrayPath2.getDataArrayName())

      m_ArrayPath2 = DataArrayPath("", "", "");
    }

    {
      AttributeMatrixSelectionFilterParameter::RequirementType req;
      AttributeMatrixSelectionFilterParameter::Pointer fp =
          AttributeMatrixSelectionFilterParameter::New("Test", "ArrayPath1", getArrayPath1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, ArrayPath2),
                                                       SIMPL_BIND_GETTER(FilterParametersRWTest, this, ArrayPath1), req);

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataContainerName(), m_ArrayPath2.getDataContainerName())
      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getAttributeMatrixName(), m_ArrayPath2.getAttributeMatrixName())
      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataArrayName(), m_ArrayPath2.getDataArrayName())

      m_ArrayPath2 = DataArrayPath("", "", "");
    }

    {
      AxisAngleFilterParameter::Pointer fp = AxisAngleFilterParameter::New("Test", "AxisAngle1", getAxisAngle1(), FilterParameter::Parameter,
                                                                           SIMPL_BIND_SETTER(FilterParametersRWTest, this, AxisAngle2), SIMPL_BIND_GETTER(FilterParametersRWTest, this, AxisAngle1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_AxisAngle1.angle, m_AxisAngle2.angle)
      DREAM3D_REQUIRE_EQUAL(m_AxisAngle1.h, m_AxisAngle2.h)
      DREAM3D_REQUIRE_EQUAL(m_AxisAngle1.k, m_AxisAngle2.k)
      DREAM3D_REQUIRE_EQUAL(m_AxisAngle1.l, m_AxisAngle2.l)

      m_AxisAngle2 = AxisAngleInput_t();
    }

    {
      BooleanFilterParameter::Pointer fp = BooleanFilterParameter::New("Test", "Bool1", getBool1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, Bool2),
                                                                       SIMPL_BIND_GETTER(FilterParametersRWTest, this, Bool1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Bool1, m_Bool2)

      m_Bool2 = false;
    }

    {
      CalculatorFilterParameter::Pointer fp = CalculatorFilterParameter::New("Test", "String1", getString1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, String2),
                                                                             SIMPL_BIND_GETTER(FilterParametersRWTest, this, String1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_String1, m_String2)

      m_String2.clear();
    }

    {
      ChoiceFilterParameter::Pointer fp = ChoiceFilterParameter::New("Test", "Int1", getInt1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, Int2),
                                                                     SIMPL_BIND_GETTER(FilterParametersRWTest, this, Int1), QVector<QString>(), true);

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Int1, m_Int2)

      m_Int2 = 0;
    }

    {
      ComparisonSelectionFilterParameter::Pointer fp =
          ComparisonSelectionFilterParameter::New("Test", "ComparisonInputs1", ComparisonInputs(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, ComparisonInputs2),
                                                  SIMPL_BIND_GETTER(FilterParametersRWTest, this, ComparisonInputs1), QVector<QString>(), true);

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_ComparisonInputs1.size(), m_ComparisonInputs2.size())
      for(int i = 0; i < m_ComparisonInputs1.size(); i++)
      {
        ComparisonInput_t input1 = m_ComparisonInputs1[i];
        ComparisonInput_t input2 = m_ComparisonInputs2[i];
        DREAM3D_REQUIRE_EQUAL(input1.dataContainerName, input2.dataContainerName)
        DREAM3D_REQUIRE_EQUAL(input1.attributeMatrixName, input2.attributeMatrixName)
        DREAM3D_REQUIRE_EQUAL(input1.attributeArrayName, input2.attributeArrayName)
        DREAM3D_REQUIRE_EQUAL(input1.compOperator, input2.compOperator)
        DREAM3D_REQUIRE_EQUAL(input1.compValue, input2.compValue)
      }

      m_ComparisonInputs2 = ComparisonInputs();
    }

    {
      DataArrayCreationFilterParameter::RequirementType req;
      DataArrayCreationFilterParameter::Pointer fp =
          DataArrayCreationFilterParameter::New("Test", "ArrayPath1", getArrayPath1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, ArrayPath2),
                                                SIMPL_BIND_GETTER(FilterParametersRWTest, this, ArrayPath1), req);

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataContainerName(), m_ArrayPath2.getDataContainerName())
      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getAttributeMatrixName(), m_ArrayPath2.getAttributeMatrixName())
      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataArrayName(), m_ArrayPath2.getDataArrayName())

      m_ArrayPath2 = DataArrayPath("", "", "");
    }

    {
      DataArraySelectionFilterParameter::RequirementType req;
      DataArraySelectionFilterParameter::Pointer fp =
          DataArraySelectionFilterParameter::New("Test", "ArrayPath1", getArrayPath1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, ArrayPath2),
                                                 SIMPL_BIND_GETTER(FilterParametersRWTest, this, ArrayPath1), req);

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataContainerName(), m_ArrayPath2.getDataContainerName())
      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getAttributeMatrixName(), m_ArrayPath2.getAttributeMatrixName())
      DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataArrayName(), m_ArrayPath2.getDataArrayName())

      m_ArrayPath2 = DataArrayPath("", "", "");
    }

    {
      DataContainerArrayProxyFilterParameter::Pointer fp =
          DataContainerArrayProxyFilterParameter::New("Test", "String1", DataContainerArrayProxy(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, DcaProxy2),
                                                      SIMPL_BIND_GETTER(FilterParametersRWTest, this, DcaProxy1), DataContainerArrayProxy(), Qt::Checked);

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      if(m_DcaProxy1 != m_DcaProxy2)
      {
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }

      m_DcaProxy2 = DataContainerArrayProxy();
    }

    {
      DataContainerCreationFilterParameter::Pointer fp = DataContainerCreationFilterParameter::New(
          "Test", "Path1", getPath1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, Path2), SIMPL_BIND_GETTER(FilterParametersRWTest, this, Path1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Path1, m_Path2)

      m_String2.clear();
    }

    {
      DataContainerSelectionFilterParameter::RequirementType req;
      DataContainerSelectionFilterParameter::Pointer fp = DataContainerSelectionFilterParameter::New(
          "Test", "Path1", getPath1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, Path2), SIMPL_BIND_GETTER(FilterParametersRWTest, this, Path1), req);

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Path1, m_Path2)

      m_String2.clear();
    }

    {
      DoubleFilterParameter::Pointer fp = DoubleFilterParameter::New("Test", "Double1", getDouble1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, Double2),
                                                                     SIMPL_BIND_GETTER(FilterParametersRWTest, this, Double1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Double1, m_Double2)

      m_Double2 = 0.0;
    }

    {
      DynamicChoiceFilterParameter::Pointer fp = DynamicChoiceFilterParameter::New(
          "Test", "String1", getString1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, String2), SIMPL_BIND_GETTER(FilterParametersRWTest, this, String1), "");

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_String1, m_String2)

      m_String2.clear();
    }

    {
      DynamicTableFilterParameter::Pointer fp =
          DynamicTableFilterParameter::New("Test", "DynamicData1", DynamicTableData(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, DynamicData2),
                                           SIMPL_BIND_GETTER(FilterParametersRWTest, this, DynamicData1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      if(m_DynamicData1 != m_DynamicData2)
      {
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }

      m_DynamicData2 = DynamicTableData();
    }

    {
      FileListInfoFilterParameter::Pointer fp =
          FileListInfoFilterParameter::New("Test", "FileListInfo1", StackFileListInfo(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, FileListInfo2),
                                           SIMPL_BIND_GETTER(FilterParametersRWTest, this, FileListInfo1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_FileListInfo1.EndIndex, m_FileListInfo2.EndIndex)
      DREAM3D_REQUIRE_EQUAL(m_FileListInfo1.FileExtension, m_FileListInfo2.FileExtension)
      DREAM3D_REQUIRE_EQUAL(m_FileListInfo1.FilePrefix, m_FileListInfo2.FilePrefix)
      DREAM3D_REQUIRE_EQUAL(m_FileListInfo1.FileSuffix, m_FileListInfo2.FileSuffix)
      DREAM3D_REQUIRE_EQUAL(m_FileListInfo1.InputPath, m_FileListInfo2.InputPath)
      DREAM3D_REQUIRE_EQUAL(m_FileListInfo1.Ordering, m_FileListInfo2.Ordering)
      DREAM3D_REQUIRE_EQUAL(m_FileListInfo1.PaddingDigits, m_FileListInfo2.PaddingDigits)
      DREAM3D_REQUIRE_EQUAL(m_FileListInfo1.StartIndex, m_FileListInfo2.StartIndex)

      m_FileListInfo2 = StackFileListInfo();
    }

    {
      FloatVec3FilterParameter::Pointer fp = FloatVec3FilterParameter::New("Test", "FloatVec3_1", FloatVec3Type(), FilterParameter::Parameter,
                                                                           SIMPL_BIND_SETTER(FilterParametersRWTest, this, FloatVec3_2), SIMPL_BIND_GETTER(FilterParametersRWTest, this, FloatVec3_1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_FloatVec3_1[0], m_FloatVec3_2[0])
      DREAM3D_REQUIRE_EQUAL(m_FloatVec3_1[1], m_FloatVec3_2[1])
      DREAM3D_REQUIRE_EQUAL(m_FloatVec3_1[2], m_FloatVec3_2[2])

      m_FloatVec3_2 = FloatVec3Type();
    }

    {
      FourthOrderPolynomialFilterParameter::Pointer fp =
          FourthOrderPolynomialFilterParameter::New("Test", "Float4thOrderPoly_1", Float4thOrderPoly_t(), FilterParameter::Parameter,
                                                    SIMPL_BIND_SETTER(FilterParametersRWTest, this, Float4thOrderPoly_2), SIMPL_BIND_GETTER(FilterParametersRWTest, this, Float4thOrderPoly_1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c00, m_Float4thOrderPoly_2.c00)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c01, m_Float4thOrderPoly_2.c01)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c02, m_Float4thOrderPoly_2.c02)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c03, m_Float4thOrderPoly_2.c03)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c04, m_Float4thOrderPoly_2.c04)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c10, m_Float4thOrderPoly_2.c10)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c11, m_Float4thOrderPoly_2.c11)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c12, m_Float4thOrderPoly_2.c12)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c13, m_Float4thOrderPoly_2.c13)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c20, m_Float4thOrderPoly_2.c20)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c21, m_Float4thOrderPoly_2.c21)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c22, m_Float4thOrderPoly_2.c22)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c30, m_Float4thOrderPoly_2.c30)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c31, m_Float4thOrderPoly_2.c31)
      DREAM3D_REQUIRE_EQUAL(m_Float4thOrderPoly_1.c40, m_Float4thOrderPoly_2.c40)

      m_Float4thOrderPoly_2 = Float4thOrderPoly_t();
    }

    {
      InputFileFilterParameter::Pointer fp = InputFileFilterParameter::New("Test", "String1", getString1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, String2),
                                                                           SIMPL_BIND_GETTER(FilterParametersRWTest, this, String1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_String1, m_String2)

      m_String2.clear();
    }

    {
      InputPathFilterParameter::Pointer fp = InputPathFilterParameter::New("Test", "String1", getString1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, String2),
                                                                           SIMPL_BIND_GETTER(FilterParametersRWTest, this, String1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_String1, m_String2)

      m_String2.clear();
    }

    {
      IntFilterParameter::Pointer fp =
          IntFilterParameter::New("Test", "Int1", getInt1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, Int2), SIMPL_BIND_GETTER(FilterParametersRWTest, this, Int1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Int1, m_Int2)

      m_Int2 = 0;
    }

    {
      IntVec3FilterParameter::Pointer fp = IntVec3FilterParameter::New("Test", "IntVec3_1", IntVec3Type(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, IntVec3_2),
                                                                       SIMPL_BIND_GETTER(FilterParametersRWTest, this, IntVec3_1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_IntVec3_1[0], m_IntVec3_2[0])
      DREAM3D_REQUIRE_EQUAL(m_IntVec3_1[1], m_IntVec3_2[1])
      DREAM3D_REQUIRE_EQUAL(m_IntVec3_1[2], m_IntVec3_2[2])

      m_IntVec3_2 = IntVec3Type();
    }

    {
      LinkedBooleanFilterParameter::Pointer fp = LinkedBooleanFilterParameter::New("Test", "Bool1", getBool1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, Bool2),
                                                                                   SIMPL_BIND_GETTER(FilterParametersRWTest, this, Bool1), QStringList());

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Bool1, m_Bool2)

      m_Bool2 = false;
    }

    {
      LinkedChoicesFilterParameter::Pointer fp = LinkedChoicesFilterParameter::New("Test", "Int1", getInt1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, Int2),
                                                                                   SIMPL_BIND_GETTER(FilterParametersRWTest, this, Int1), QVector<QString>(), QStringList());

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Int1, m_Int2)

      m_Int2 = 0;
    }

    {
      MultiDataArraySelectionFilterParameter::RequirementType req;
      MultiDataArraySelectionFilterParameter::Pointer fp =
          MultiDataArraySelectionFilterParameter::New("Test", "DataArrayPaths1", getDataArrayPaths1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, DataArrayPaths2),
                                                      SIMPL_BIND_GETTER(FilterParametersRWTest, this, DataArrayPaths1), req);

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_DataArrayPaths1.size(), m_DataArrayPaths2.size())

      for(int i = 0; i < m_DataArrayPaths1.size(); i++)
      {
        DREAM3D_REQUIRE_EQUAL(m_DataArrayPaths1[i].getDataContainerName(), m_DataArrayPaths2[i].getDataContainerName())
        DREAM3D_REQUIRE_EQUAL(m_DataArrayPaths1[i].getAttributeMatrixName(), m_DataArrayPaths2[i].getAttributeMatrixName())
        DREAM3D_REQUIRE_EQUAL(m_DataArrayPaths1[i].getDataArrayName(), m_DataArrayPaths2[i].getDataArrayName())
      }

      m_DataArrayPaths2 = QVector<DataArrayPath>();
    }

    {
      OutputFileFilterParameter::Pointer fp = OutputFileFilterParameter::New("Test", "String1", getString1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, String2),
                                                                             SIMPL_BIND_GETTER(FilterParametersRWTest, this, String1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_String1, m_String2)

      m_String2.clear();
    }

    {
      OutputPathFilterParameter::Pointer fp = OutputPathFilterParameter::New("Test", "String1", getString1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, String2),
                                                                             SIMPL_BIND_GETTER(FilterParametersRWTest, this, String1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_String1, m_String2)

      m_String2.clear();
    }

    {
      PhaseTypeSelectionFilterParameter::Pointer fp =
          PhaseTypeSelectionFilterParameter::New("Test", "PhaseTypeVector_2", DataArrayPath(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, PhaseTypeVector_2),
                                                 SIMPL_BIND_GETTER(FilterParametersRWTest, this, PhaseTypeVector_1), "", "", "", QStringList());

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_PhaseTypeVector_1.size(), m_PhaseTypeVector_2.size())

      for(int i = 0; i < m_PhaseTypeVector_1.size(); i++)
      {
        DREAM3D_REQUIRE_EQUAL(static_cast<PhaseType::EnumType>(m_PhaseTypeVector_1[i]), static_cast<PhaseType::EnumType>(m_PhaseTypeVector_2[i]))
      }

      //      m_PhaseTypeVector_2 = PhaseTypeVector_t(); // Does this line do anything?
    }

#if 0
This Filter Parameter is NOT tesed since nothing is read or written for the filter parameter.
    {
      PreflightUpdatedValueFilterParameter::Pointer fp =
          PreflightUpdatedValueFilterParameter::New("Test", "String2", getString1(), FilterParameter::Parameter, SIMPL_BIND_GETTER(FilterParametersRWTest, this, String1));

      QJsonObject obj;
      fp->writeJson(obj);

      QJsonValue jsonValue = obj[fp->getPropertyName()];
      QString result = "";
      if(!jsonValue.isUndefined())
      {
        result = jsonValue.toString("");
      }

      DREAM3D_REQUIRE_EQUAL(m_String1, result)
    }
#endif
    {
      RangeFilterParameter::Pointer fp = RangeFilterParameter::New("Test", "Pair1", getPair1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, Pair2),
                                                                   SIMPL_BIND_GETTER(FilterParametersRWTest, this, Pair1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Pair1.first, m_Pair2.first)
      DREAM3D_REQUIRE_EQUAL(m_Pair1.second, m_Pair2.second)

      m_Pair2 = FPRangePair();
    }

    {
      SecondOrderPolynomialFilterParameter::Pointer fp =
          SecondOrderPolynomialFilterParameter::New("Test", "Float2ndOrderPoly_1", Float2ndOrderPoly_t(), FilterParameter::Parameter,
                                                    SIMPL_BIND_SETTER(FilterParametersRWTest, this, Float2ndOrderPoly_2), SIMPL_BIND_GETTER(FilterParametersRWTest, this, Float2ndOrderPoly_1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Float2ndOrderPoly_1.c00, m_Float2ndOrderPoly_2.c00)
      DREAM3D_REQUIRE_EQUAL(m_Float2ndOrderPoly_1.c01, m_Float2ndOrderPoly_2.c01)
      DREAM3D_REQUIRE_EQUAL(m_Float2ndOrderPoly_1.c02, m_Float2ndOrderPoly_2.c02)
      DREAM3D_REQUIRE_EQUAL(m_Float2ndOrderPoly_1.c10, m_Float2ndOrderPoly_2.c10)
      DREAM3D_REQUIRE_EQUAL(m_Float2ndOrderPoly_1.c11, m_Float2ndOrderPoly_2.c11)
      DREAM3D_REQUIRE_EQUAL(m_Float2ndOrderPoly_1.c20, m_Float2ndOrderPoly_2.c20)

      m_Float2ndOrderPoly_2 = Float2ndOrderPoly_t();
    }
#if 0
    {
      ShapeType::Types shapeData;
      ShapeTypeSelectionFilterParameter::Pointer fp =
          ShapeTypeSelectionFilterParameter::New("Test", "String1", shapeData, FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, ShapeType::Types),
                                                 SIMPL_BIND_GETTER(FilterParametersRWTest, this, ShapeType::Types), "String1", "");

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_ShapeTypeVector_1.size(), m_ShapeTypeVector_2.size())

      for(int i = 0; i < m_ShapeTypeVector_1.size(); i++)
      {
        DREAM3D_REQUIRE_EQUAL(static_cast<ShapeType::EnumType>(m_ShapeTypeVector_1[i]), static_cast<ShapeType::EnumType>(m_ShapeTypeVector_2[i]))
      }

      m_ShapeTypeVector_2 = ShapeType::Types();
    }
#endif
    {
      StringFilterParameter::Pointer fp = StringFilterParameter::New("Test", "String1", getString1(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FilterParametersRWTest, this, String2),
                                                                     SIMPL_BIND_GETTER(FilterParametersRWTest, this, String1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_String1, m_String2)

      m_String2.clear();
    }

    {
      ThirdOrderPolynomialFilterParameter::Pointer fp =
          ThirdOrderPolynomialFilterParameter::New("Test", "Float3rdOrderPoly_1", Float3rdOrderPoly_t(), FilterParameter::Parameter,
                                                   SIMPL_BIND_SETTER(FilterParametersRWTest, this, Float3rdOrderPoly_2), SIMPL_BIND_GETTER(FilterParametersRWTest, this, Float3rdOrderPoly_1));

      QJsonObject obj;
      fp->writeJson(obj);
      fp->readJson(obj);

      DREAM3D_REQUIRE_EQUAL(m_Float3rdOrderPoly_1.c00, m_Float3rdOrderPoly_2.c00)
      DREAM3D_REQUIRE_EQUAL(m_Float3rdOrderPoly_1.c01, m_Float3rdOrderPoly_2.c01)
      DREAM3D_REQUIRE_EQUAL(m_Float3rdOrderPoly_1.c02, m_Float3rdOrderPoly_2.c02)
      DREAM3D_REQUIRE_EQUAL(m_Float3rdOrderPoly_1.c03, m_Float3rdOrderPoly_2.c03)
      DREAM3D_REQUIRE_EQUAL(m_Float3rdOrderPoly_1.c10, m_Float3rdOrderPoly_2.c10)
      DREAM3D_REQUIRE_EQUAL(m_Float3rdOrderPoly_1.c11, m_Float3rdOrderPoly_2.c11)
      DREAM3D_REQUIRE_EQUAL(m_Float3rdOrderPoly_1.c12, m_Float3rdOrderPoly_2.c12)
      DREAM3D_REQUIRE_EQUAL(m_Float3rdOrderPoly_1.c20, m_Float3rdOrderPoly_2.c20)
      DREAM3D_REQUIRE_EQUAL(m_Float3rdOrderPoly_1.c21, m_Float3rdOrderPoly_2.c21)
      DREAM3D_REQUIRE_EQUAL(m_Float3rdOrderPoly_1.c30, m_Float3rdOrderPoly_2.c30)

      m_Float3rdOrderPoly_2 = Float3rdOrderPoly_t();
    }

    return EXIT_SUCCESS;
  }

#if 0
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestRWPhaseType(PhaseType::Type type)
  {
    QByteArray data;
    QDataStream is(&data, QIODevice::ReadOnly);
    QDataStream os(&data, QIODevice::WriteOnly);

    is << type;

    PhaseType::Type read;
    os >> read;

    Q_ASSERT(type == read);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestPhaseTypeDataStream()
  {
    PhaseType::Type type;

    type = PhaseType::Type::Any;
    TestRWPhaseType(type);

    type = PhaseType::Type::Boundary;
    TestRWPhaseType(type);

    type = PhaseType::Type::Matrix;
    TestRWPhaseType(type);

    type = PhaseType::Type::Precipitate;
    TestRWPhaseType(type);

    type = PhaseType::Type::Primary;
    TestRWPhaseType(type);

    type = PhaseType::Type::Transformation;
    TestRWPhaseType(type);

    type = PhaseType::Type::Unknown;
    TestRWPhaseType(type);

    return EXIT_SUCCESS;
  }
#endif

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### FilterParametersRWTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterParametersRW())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  bool m_Bool1 = {};
  bool m_Bool2 = {};
  int m_Int1 = {};
  int m_Int2 = {};
  double m_Double1 = {};
  double m_Double2 = {};
  QString m_String1 = {};
  QString m_String2 = {};
  DataArrayPath m_Path1 = {};
  DataArrayPath m_Path2 = {};
  DataArrayPath m_ArrayPath1 = {};
  DataArrayPath m_ArrayPath2 = {};
  AxisAngleInput_t m_AxisAngle1 = {};
  AxisAngleInput_t m_AxisAngle2 = {};
  ComparisonInputs m_ComparisonInputs1 = {};
  ComparisonInputs m_ComparisonInputs2 = {};
  DataContainerArrayProxy m_DcaProxy1 = {};
  DataContainerArrayProxy m_DcaProxy2 = {};
  DynamicTableData m_DynamicData1 = {};
  DynamicTableData m_DynamicData2 = {};
  StackFileListInfo m_FileListInfo1 = {};
  StackFileListInfo m_FileListInfo2 = {};
  IntVec3Type m_IntVec3_1 = {};
  IntVec3Type m_IntVec3_2 = {};
  FloatVec3Type m_FloatVec3_1 = {};
  FloatVec3Type m_FloatVec3_2 = {};
  Float2ndOrderPoly_t m_Float2ndOrderPoly_1 = {};
  Float2ndOrderPoly_t m_Float2ndOrderPoly_2 = {};
  Float3rdOrderPoly_t m_Float3rdOrderPoly_1 = {};
  Float3rdOrderPoly_t m_Float3rdOrderPoly_2 = {};
  Float4thOrderPoly_t m_Float4thOrderPoly_1 = {};
  Float4thOrderPoly_t m_Float4thOrderPoly_2 = {};
  QVector<DataArrayPath> m_DataArrayPaths1 = {};
  QVector<DataArrayPath> m_DataArrayPaths2 = {};
  ShapeType::Types m_ShapeTypeVector_1 = {};
  ShapeType::Types m_ShapeTypeVector_2 = {};
  PhaseType::Types m_PhaseTypeVector_1 = {};
  PhaseType::Types m_PhaseTypeVector_2 = {};
  FPRangePair m_Pair1 = {};
  FPRangePair m_Pair2 = {};

  FilterParametersRWTest(const FilterParametersRWTest&); // Copy Constructor Not Implemented
  void operator=(const FilterParametersRWTest&);         // Move assignment Not Implemented
};

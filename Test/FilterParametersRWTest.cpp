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

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QSet>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/AxisAngleFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/CalculatorFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/ComparisonSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataBundleSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerArrayProxyFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerReaderFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/FourthOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/InputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
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
#include "SIMPLib/FilterParameters/UnknownFilterParameter.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"

#include "SIMPLTestFileLocations.h"

namespace StringTest
{
  QString Key = "Test String";

  QString Value = "I am testing the string";
}

namespace StringVectorTest
{
  QString Key = "Test Vector";

  QString Value1 = "VecString1";
  QString Value2 = "VecString2";
  QString Value3 = "VecString3";
  QString Value4 = "VecString4";
}

namespace IntVec3Test
{
  QString Key = "IntVec3_t test";

  int X = -1;
  int Y = 2;
  int Z = 3;
}

namespace FloatVec3Test
{
  QString Key = "FloatVec3_t test";

  float X = 0.0f;
  float Y = 2.65f;
  float Z = -3.975f;
}

namespace FloatVec4Test
{
  QString Key = "FloatVec4_t test";

  float A = 0.0f;
  float B = 2.65f;
  float C = 3.975f;
  float D = -4.6876f;
}

namespace FloatVec21Test
{
  QString Key = "FloatVec21_t test";

  float Val1 = 0.0f;
  float Val2 = 2.65f;
  float Val3 = 3.975f;
  float Val4 = 4.6876f;
  float Val5 = 1.2f;
  float Val6 = 2.65f;
  float Val7 = 3.975f;
  float Val8 = 4.6876f;
  float Val9 = 1.2f;
  float Val10 = 2.65f;
  float Val11 = 3.975f;
  float Val12 = 4.6876f;
  float Val13 = 1.2f;
  float Val14 = 2.65f;
  float Val15 = 3.975f;
  float Val16 = 4.6876f;
  float Val17 = 1.2f;
  float Val18 = 2.65f;
  float Val19 = 3.975f;
  float Val20 = 4.6876f;
  float Val21 = 1.2f;
}

namespace Float2ndOrderPolyTest
{
  QString Key = "Float2ndOrderPoly_t test";

  float Val1 = 0.0f;
  float Val2 = 2.65f;
  float Val3 = 3.975f;
  float Val4 = 4.6876f;
  float Val5 = 1.2f;
  float Val6 = 2.65f;
}

namespace Float3rdOrderPolyTest
{
  QString Key = "Float3rdOrderPoly_t test";

  float Val1 = 0.0f;
  float Val2 = 2.65f;
  float Val3 = 3.975f;
  float Val4 = 4.6876f;
  float Val5 = 1.2f;
  float Val6 = 1.2f;
  float Val7 = 2.65f;
  float Val8 = 3.975f;
  float Val9 = 4.6876f;
  float Val10 = 1.2f;
}

namespace Float4thOrderPolyTest
{
  QString Key = "Float4thOrderPoly_t test";

  float Val1 = 0.0f;
  float Val2 = 2.65f;
  float Val3 = 3.975f;
  float Val4 = 4.6876f;
  float Val5 = 1.2f;
  float Val6 = 1.2f;
  float Val7 = 2.65f;
  float Val8 = 3.975f;
  float Val9 = 4.6876f;
  float Val10 = 1.2f;
  float Val11 = 1.2f;
  float Val12 = 2.65f;
  float Val13 = 3.975f;
  float Val14 = 4.6876f;
  float Val15 = 1.2f;
}

namespace FileListInfoTest
{
  QString Key = "FileListInfo_t test";

  qint32 PaddingDigits = -1;
  quint32 Ordering = 2;
  qint32 StartIndex = 66;
  qint32 EndIndex = 2345654;
  QString InputPath = "TestString1";
  QString FilePrefix = "TestString2";
  QString FileSuffix = "TestString3";
  QString FileExtension = "TestString4";
}

namespace ComparisonInputTest
{
  QString Key = "ComparisonInput_t test";

  QString DataContainerName = "DataContainer1";
  QString AttributeMatrixName = "AttributeMatrix1";
  QString AttributeArrayName = "AttributeArray1";
  int CompOperator = 3;
  double CompValue = 4.6876;
}

namespace MultipleComparisonInputTest
{
  QString Key = "ComparisonInputs test";

  QString DataContainerName1 = "DataContainer1";
  QString AttributeMatrixName1 = "AttributeMatrix1";
  QString AttributeArrayName1 = "AttributeArray1";
  int CompOperator1 = 3;
  double CompValue1 = 4.6876;

  QString DataContainerName2 = "DataContainer2";
  QString AttributeMatrixName2 = "AttributeMatrix2";
  QString AttributeArrayName2 = "AttributeArray2";
  int CompOperator2 = 4;
  double CompValue2 = 2.935;
}

namespace AxisAngleInputTest
{
  QString Key = "AxisAngleInput_t test";

  float Angle = 0.0f;
  float H = 1.23f;
  float K = 4.32f;
  float L = 3.54f;
}

namespace MultipleAxisAngleInputTest
{
  QString Key = "QVector<AxisAngleInput_t> test";

  float Angle1 = 0.0f;
  float H1 = 1.23f;
  float K1 = 4.32f;
  float L1 = 3.54f;

  float Angle2 = 6.32f;
  float H2 = 9.5677f;
  float K2 = 4.8124f;
  float L2 = 6.315f;
}

namespace StringSetTest
{
  QString Key = "QSet<QString> test";

  QString String1 = "StringSetTest-String1";
  QString String2 = "StringSetTest-String2";
  QString String3 = "StringSetTest-String3";
  QString String4 = "StringSetTest-String4";
}

namespace DataContainerArrayProxyTest
{
  QString Key = "DataContainerArrayProxy test";

  QString DcProxyName = "DataContainer1";
  QString AmProxyName1 = "AttributeMatrix1";
  QString AmProxyName2 = "AttributeMatrix2";
  QString DaProxyName1 = "AttributeArray1";
  QString DaProxyName2 = "AttributeArray2";
  QString DaProxyName3 = "AttributeArray3";
}

namespace DataArrayPathTest
{
  QString Key = "DataArrayPath test";

  QString DCName = "DataContainer";
  QString AMName = "AttributeMatrix";
  QString DAName = "AttributeArray";
}

namespace MultipleDataArrayPathTest
{
  QString Key = "QVector<DataArrayPath> test";

  QString DCName1 = "DataContainer1";
  QString AMName1 = "AttributeMatrix1";
  QString DAName1 = "AttributeArray1";

  QString DCName2 = "DataContainer2";
  QString AMName2 = "AttributeMatrix2";
  QString DAName2 = "AttributeArray2";
}

namespace DynamicTableDataTest
{
  QString Key = "DynamicTableData test";

  int NumRows = 6;
  int NumColumns = 8;
  QString RowHeader1 = "Row 1";
  QString RowHeader2 = "Row 2";
  QString RowHeader3 = "Row 3";
  QString RowHeader4 = "Row 4";
  QString RowHeader5 = "Row 5";
  QString RowHeader6 = "Row 6";
  QString ColumnHeader1 = "Column 1";
  QString ColumnHeader2 = "Column 2";
  QString ColumnHeader3 = "Column 3";
  QString ColumnHeader4 = "Column 4";
  QString ColumnHeader5 = "Column 5";
  QString ColumnHeader6 = "Column 6";
  QString ColumnHeader7 = "Column 7";
  QString ColumnHeader8 = "Column 8";
}


class FilterParametersRWTest
{
  public:
    FilterParametersRWTest()
    {
      m_Bool1 = true;
      m_Int1 = 24;
      m_String1 = "This is the FilterParametersRWTest";
      m_ArrayPath1 = DataArrayPath("DataContainer", "AttributeMatrix", "DataArray");

      m_AxisAngle1.angle = 2.2f;
      m_AxisAngle1.h = 3.4f;
      m_AxisAngle1.k = 6.4f;
      m_AxisAngle1.l = 8.2f;

      m_ComparisonInputs1.addInput("DataContainer", "AttributeMatrix", "DataArrayName", 2, 4.6);
    }
    virtual ~FilterParametersRWTest() {}

    SIMPL_INSTANCE_PROPERTY(bool, Bool1)
    Q_PROPERTY(bool Bool1 READ getBool1 WRITE setBool1)

    SIMPL_INSTANCE_PROPERTY(bool, Bool2)
    Q_PROPERTY(bool Bool2 READ getBool2 WRITE setBool2)

    SIMPL_INSTANCE_PROPERTY(int, Int1)
    Q_PROPERTY(int Int1 READ getInt1 WRITE setInt1)

    SIMPL_INSTANCE_PROPERTY(int, Int2)
    Q_PROPERTY(int Int2 READ getInt2 WRITE setInt2)

    SIMPL_INSTANCE_PROPERTY(QString, String1)
    Q_PROPERTY(QString String1 READ getString1 WRITE setString1)

    SIMPL_INSTANCE_PROPERTY(QString, String2)
    Q_PROPERTY(QString String2 READ getString2 WRITE setString2)

    SIMPL_INSTANCE_PROPERTY(DataArrayPath, ArrayPath1)
    Q_PROPERTY(DataArrayPath ArrayPath1 READ getArrayPath1 WRITE setArrayPath1)

    SIMPL_INSTANCE_PROPERTY(DataArrayPath, ArrayPath2)
    Q_PROPERTY(DataArrayPath ArrayPath2 READ getArrayPath2 WRITE setArrayPath2)

    SIMPL_INSTANCE_PROPERTY(AxisAngleInput_t, AxisAngle1)
    Q_PROPERTY(AxisAngleInput_t AxisAngle1 READ getAxisAngle1 WRITE setAxisAngle1)

    SIMPL_INSTANCE_PROPERTY(AxisAngleInput_t, AxisAngle2)
    Q_PROPERTY(AxisAngleInput_t AxisAngle2 READ getAxisAngle2 WRITE setAxisAngle2)

    SIMPL_INSTANCE_PROPERTY(ComparisonInputs, ComparisonInputs1)
    Q_PROPERTY(ComparisonInputs ComparisonInputs1 READ getComparisonInputs1 WRITE setComparisonInputs1)

    SIMPL_INSTANCE_PROPERTY(ComparisonInputs, ComparisonInputs2)
    Q_PROPERTY(ComparisonInputs ComparisonInputs2 READ getComparisonInputs2 WRITE setComparisonInputs2)


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
        AttributeMatrixCreationFilterParameter::Pointer fp = AttributeMatrixCreationFilterParameter::New("Test", "ArrayPath1",
                                                                                                         getArrayPath1(), FilterParameter::Parameter,
                                                                                                         req, SIMPL_BIND_SETTER(FilterParametersRWTest, this, ArrayPath2),
                                                                                                         SIMPL_BIND_GETTER(FilterParametersRWTest, this, ArrayPath1));

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
        AttributeMatrixSelectionFilterParameter::Pointer fp = AttributeMatrixSelectionFilterParameter::New("Test", "ArrayPath1",
                                                                                                         getArrayPath1(), FilterParameter::Parameter,
                                                                                                         req, SIMPL_BIND_SETTER(FilterParametersRWTest, this, ArrayPath2),
                                                                                                         SIMPL_BIND_GETTER(FilterParametersRWTest, this, ArrayPath1));

        QJsonObject obj;
        fp->writeJson(obj);
        fp->readJson(obj);

        DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataContainerName(), m_ArrayPath2.getDataContainerName())
        DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getAttributeMatrixName(), m_ArrayPath2.getAttributeMatrixName())
        DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataArrayName(), m_ArrayPath2.getDataArrayName())

        m_ArrayPath2 = DataArrayPath("", "", "");
      }

      {
        AxisAngleFilterParameter::Pointer fp = AxisAngleFilterParameter::New("Test", "AxisAngle1",
                                                                             getAxisAngle1(), FilterParameter::Parameter,
                                                                             SIMPL_BIND_SETTER(FilterParametersRWTest, this, AxisAngle2),
                                                                             SIMPL_BIND_GETTER(FilterParametersRWTest, this, AxisAngle1));

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
        BooleanFilterParameter::Pointer fp = BooleanFilterParameter::New("Test", "Bool1",
                                                                             getBool1(), FilterParameter::Parameter,
                                                                             SIMPL_BIND_SETTER(FilterParametersRWTest, this, Bool2),
                                                                             SIMPL_BIND_GETTER(FilterParametersRWTest, this, Bool1));

        QJsonObject obj;
        fp->writeJson(obj);
        fp->readJson(obj);

        DREAM3D_REQUIRE_EQUAL(m_Bool1, m_Bool2)

        m_Bool2 = false;
      }

      {
        CalculatorFilterParameter::Pointer fp = CalculatorFilterParameter::New("Test", "String1",
                                                                             getString1(), FilterParameter::Parameter,
                                                                             SIMPL_BIND_SETTER(FilterParametersRWTest, this, String2),
                                                                             SIMPL_BIND_GETTER(FilterParametersRWTest, this, String1));

        QJsonObject obj;
        fp->writeJson(obj);
        fp->readJson(obj);

        DREAM3D_REQUIRE_EQUAL(m_String1, m_String2)

        m_String2.clear();
      }

      {
        ChoiceFilterParameter::Pointer fp = ChoiceFilterParameter::New("Test", "Int1",
                                                                             getInt1(), QVector<QString>(), true, FilterParameter::Parameter,
                                                                             SIMPL_BIND_SETTER(FilterParametersRWTest, this, Int2),
                                                                             SIMPL_BIND_GETTER(FilterParametersRWTest, this, Int1));

        QJsonObject obj;
        fp->writeJson(obj);
        fp->readJson(obj);

        DREAM3D_REQUIRE_EQUAL(m_Int1, m_Int2)

        m_Int2 = 0;
      }

      {
        ComparisonSelectionFilterParameter::Pointer fp = ComparisonSelectionFilterParameter::New("Test", "ComparisonInputs1",
                                                                             "", QVector<QString>(), true, FilterParameter::Parameter,
                                                                             SIMPL_BIND_SETTER(FilterParametersRWTest, this, ComparisonInputs2),
                                                                             SIMPL_BIND_GETTER(FilterParametersRWTest, this, ComparisonInputs1));

        QJsonObject obj;
        fp->writeJson(obj);
        fp->readJson(obj);

        DREAM3D_REQUIRE_EQUAL(m_ComparisonInputs1.size(), m_ComparisonInputs2.size())
        for (int i=0; i<m_ComparisonInputs1.size(); i++)
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
        DataArrayCreationFilterParameter::Pointer fp = DataArrayCreationFilterParameter::New("Test", "ArrayPath1",
                                                                                                         getArrayPath1(), FilterParameter::Parameter,
                                                                                                         req, SIMPL_BIND_SETTER(FilterParametersRWTest, this, ArrayPath2),
                                                                                                         SIMPL_BIND_GETTER(FilterParametersRWTest, this, ArrayPath1));

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
        DataArraySelectionFilterParameter::Pointer fp = DataArraySelectionFilterParameter::New("Test", "ArrayPath1",
                                                                                                         getArrayPath1(), FilterParameter::Parameter,
                                                                                                         req, SIMPL_BIND_SETTER(FilterParametersRWTest, this, ArrayPath2),
                                                                                                         SIMPL_BIND_GETTER(FilterParametersRWTest, this, ArrayPath1));

        QJsonObject obj;
        fp->writeJson(obj);
        fp->readJson(obj);

        DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataContainerName(), m_ArrayPath2.getDataContainerName())
        DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getAttributeMatrixName(), m_ArrayPath2.getAttributeMatrixName())
        DREAM3D_REQUIRE_EQUAL(m_ArrayPath1.getDataArrayName(), m_ArrayPath2.getDataArrayName())

        m_ArrayPath2 = DataArrayPath("", "", "");
      }

      {
        DataBundleSelectionFilterParameter::Pointer fp = DataBundleSelectionFilterParameter::New("Test", "String1",
                                                                             getString1(), FilterParameter::Parameter,
                                                                             SIMPL_BIND_SETTER(FilterParametersRWTest, this, String2),
                                                                             SIMPL_BIND_GETTER(FilterParametersRWTest, this, String1));

        QJsonObject obj;
        fp->writeJson(obj);
        fp->readJson(obj);

        DREAM3D_REQUIRE_EQUAL(m_String1, m_String2)

        m_String2.clear();
      }

      {
        DataContainerArrayProxyFilterParameter::Pointer fp = DataContainerArrayProxyFilterParameter::New("Test", "String1",
                                                                             getString1(), FilterParameter::Parameter,
                                                                             SIMPL_BIND_SETTER(FilterParametersRWTest, this, String2),
                                                                             SIMPL_BIND_GETTER(FilterParametersRWTest, this, String1));

        QJsonObject obj;
        fp->writeJson(obj);
        fp->readJson(obj);

        DREAM3D_REQUIRE_EQUAL(m_String1, m_String2)

        m_String2.clear();
      }

      JsonFilterParametersWriter::Pointer writer = JsonFilterParametersWriter::New();
      writer->setFileName(UnitTest::FilterParametersRWTest::OutputFile);
      writer->setPipelineName("TestPipelineName");

      int err = 0;

      err = writer->openFilterGroup(NULL, 0);
      DREAM3D_REQUIRE_EQUAL(err, 0)

      // QString write test
      {
        err = writer->writeValue(StringTest::Key, StringTest::Value);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // QVector<QString> write test
      {
        QVector<QString> vector;
        vector.push_back(StringVectorTest::Value1);
        vector.push_back(StringVectorTest::Value2);
        vector.push_back(StringVectorTest::Value3);
        vector.push_back(StringVectorTest::Value4);
        err = writer->writeValue(StringVectorTest::Key, vector);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // IntVec3_t write test
      {
        IntVec3_t vec3;
        vec3.x = IntVec3Test::X;
        vec3.y = IntVec3Test::Y;
        vec3.z = IntVec3Test::Z;
        err = writer->writeValue(IntVec3Test::Key, vec3);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // FloatVec3_t write test
      {
        FloatVec3_t vec3;
        vec3.x = FloatVec3Test::X;
        vec3.y = FloatVec3Test::Y;
        vec3.z = FloatVec3Test::Z;
        err = writer->writeValue(FloatVec3Test::Key, vec3);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // Float2ndOrderPoly_t write test
      {
        Float2ndOrderPoly_t poly;
        poly.c20 = Float2ndOrderPolyTest::Val1;
        poly.c02 = Float2ndOrderPolyTest::Val2;
        poly.c11 = Float2ndOrderPolyTest::Val3;
        poly.c10 = Float2ndOrderPolyTest::Val4;
        poly.c01 = Float2ndOrderPolyTest::Val5;
        poly.c00 = Float2ndOrderPolyTest::Val6;

        err = writer->writeValue(Float2ndOrderPolyTest::Key, poly);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // Float3rdOrderPoly_t write test
      {
        Float3rdOrderPoly_t poly;
        poly.c30 = Float3rdOrderPolyTest::Val1;
        poly.c03 = Float3rdOrderPolyTest::Val2;
        poly.c21 = Float3rdOrderPolyTest::Val3;
        poly.c12 = Float3rdOrderPolyTest::Val4;
        poly.c20 = Float3rdOrderPolyTest::Val5;
        poly.c02 = Float3rdOrderPolyTest::Val6;
        poly.c11 = Float3rdOrderPolyTest::Val7;
        poly.c10 = Float3rdOrderPolyTest::Val8;
        poly.c01 = Float3rdOrderPolyTest::Val9;
        poly.c00 = Float3rdOrderPolyTest::Val10;

        err = writer->writeValue(Float3rdOrderPolyTest::Key, poly);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // Float4thOrderPoly_t write test
      {
        Float4thOrderPoly_t poly;
        poly.c02 = Float4thOrderPolyTest::Val1;
        poly.c11 = Float4thOrderPolyTest::Val2;
        poly.c10 = Float4thOrderPolyTest::Val3;
        poly.c01 = Float4thOrderPolyTest::Val4;
        poly.c00 = Float4thOrderPolyTest::Val5;
        poly.c40 = Float4thOrderPolyTest::Val6;
        poly.c04 = Float4thOrderPolyTest::Val7;
        poly.c31 = Float4thOrderPolyTest::Val8;
        poly.c13 = Float4thOrderPolyTest::Val9;
        poly.c22 = Float4thOrderPolyTest::Val10;
        poly.c30 = Float4thOrderPolyTest::Val11;
        poly.c03 = Float4thOrderPolyTest::Val12;
        poly.c21 = Float4thOrderPolyTest::Val13;
        poly.c12 = Float4thOrderPolyTest::Val14;
        poly.c20 = Float4thOrderPolyTest::Val15;

        err = writer->writeValue(Float4thOrderPolyTest::Key, poly);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // FileListInfo_t write test
      {
        FileListInfo_t info;
        info.PaddingDigits = FileListInfoTest::PaddingDigits;
        info.Ordering = FileListInfoTest::Ordering;
        info.StartIndex = FileListInfoTest::StartIndex;
        info.EndIndex = FileListInfoTest::EndIndex;
        info.InputPath = FileListInfoTest::InputPath;
        info.FilePrefix = FileListInfoTest::FilePrefix;
        info.FileSuffix = FileListInfoTest::FileSuffix;
        info.FileExtension = FileListInfoTest::FileExtension;

        err = writer->writeValue(FileListInfoTest::Key, info);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // ComparisonInput_t write test
      {
        ComparisonInput_t input;
        input.dataContainerName = ComparisonInputTest::DataContainerName;
        input.attributeMatrixName = ComparisonInputTest::AttributeMatrixName;
        input.attributeArrayName = ComparisonInputTest::AttributeArrayName;
        input.compOperator = ComparisonInputTest::CompOperator;
        input.compValue = ComparisonInputTest::CompValue;

        err = writer->writeValue(ComparisonInputTest::Key, input);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // ComparisonInputs write test
      {
        ComparisonInputs inputs;

        ComparisonInput_t input1;
        input1.dataContainerName = MultipleComparisonInputTest::DataContainerName1;
        input1.attributeMatrixName = MultipleComparisonInputTest::AttributeMatrixName1;
        input1.attributeArrayName = MultipleComparisonInputTest::AttributeArrayName1;
        input1.compOperator = MultipleComparisonInputTest::CompOperator1;
        input1.compValue = MultipleComparisonInputTest::CompValue1;
        inputs.addInput(input1);

        ComparisonInput_t input2;
        input2.dataContainerName = MultipleComparisonInputTest::DataContainerName2;
        input2.attributeMatrixName = MultipleComparisonInputTest::AttributeMatrixName2;
        input2.attributeArrayName = MultipleComparisonInputTest::AttributeArrayName2;
        input2.compOperator = MultipleComparisonInputTest::CompOperator2;
        input2.compValue = MultipleComparisonInputTest::CompValue2;
        inputs.addInput(input2);

        err = writer->writeValue(MultipleComparisonInputTest::Key, inputs);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // QVector<AxisAngleInput_t> write test
      {
        QVector<AxisAngleInput_t> inputs;

        AxisAngleInput_t input1;
        input1.angle = MultipleAxisAngleInputTest::Angle1;
        input1.h = MultipleAxisAngleInputTest::H1;
        input1.k = MultipleAxisAngleInputTest::K1;
        input1.l = MultipleAxisAngleInputTest::L1;
        inputs.push_back(input1);

        AxisAngleInput_t input2;
        input2.angle = MultipleAxisAngleInputTest::Angle2;
        input2.h = MultipleAxisAngleInputTest::H2;
        input2.k = MultipleAxisAngleInputTest::K2;
        input2.l = MultipleAxisAngleInputTest::L2;
        inputs.push_back(input2);

        err = writer->writeValue(MultipleAxisAngleInputTest::Key, inputs);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // AxisAngleInput_t write test
      {
        AxisAngleInput_t input;
        input.angle = AxisAngleInputTest::Angle;
        input.h = AxisAngleInputTest::H;
        input.k = AxisAngleInputTest::K;
        input.l = AxisAngleInputTest::L;

        err = writer->writeValue(AxisAngleInputTest::Key, input);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // QSet<QString> write test
      {
        QSet<QString> set;
        set << StringSetTest::String1 << StringSetTest::String2 << StringSetTest::String3 << StringSetTest::String4;

        err = writer->writeArraySelections(StringSetTest::Key, set);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // DataContainerArrayProxy write test
      {
        DataContainerArrayProxy proxy;
        DataContainerProxy dcProxy(DataContainerArrayProxyTest::DcProxyName);
        AttributeMatrixProxy amProxy1(DataContainerArrayProxyTest::AmProxyName1);
        AttributeMatrixProxy amProxy2(DataContainerArrayProxyTest::AmProxyName2);
        DataArrayProxy daProxy1(DataContainerArrayProxyTest::DaProxyName1, DataContainerArrayProxyTest::DaProxyName1);
        DataArrayProxy daProxy2(DataContainerArrayProxyTest::DaProxyName2, DataContainerArrayProxyTest::DaProxyName2);
        DataArrayProxy daProxy3(DataContainerArrayProxyTest::DaProxyName3, DataContainerArrayProxyTest::DaProxyName3);
        amProxy1.dataArrays.insert(daProxy1.name, daProxy1);
        amProxy1.dataArrays.insert(daProxy2.name, daProxy2);
        amProxy2.dataArrays.insert(daProxy3.name, daProxy3);
        dcProxy.attributeMatricies.insert(amProxy1.name, amProxy1);
        dcProxy.attributeMatricies.insert(amProxy2.name, amProxy2);
        proxy.dataContainers.insert(dcProxy.name, dcProxy);

        err = writer->writeValue(DataContainerArrayProxyTest::Key, proxy);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // DataArrayPath write test
      {
        DataArrayPath path(DataArrayPathTest::DCName, DataArrayPathTest::AMName, DataArrayPathTest::DAName);

        err = writer->writeValue(DataArrayPathTest::Key, path);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // QVector<DataArrayPath> write test
      {
        QVector<DataArrayPath> paths;
        paths.push_back(DataArrayPath(MultipleDataArrayPathTest::DCName1, MultipleDataArrayPathTest::AMName1, MultipleDataArrayPathTest::DAName1));
        paths.push_back(DataArrayPath(MultipleDataArrayPathTest::DCName2, MultipleDataArrayPathTest::AMName2, MultipleDataArrayPathTest::DAName2));

        err = writer->writeValue(MultipleDataArrayPathTest::Key, paths);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }

      // DynamicTableData write test
      {
        QStringList rHeaders, cHeaders;
        rHeaders << DynamicTableDataTest::RowHeader1 << DynamicTableDataTest::RowHeader2 << DynamicTableDataTest::RowHeader3 << DynamicTableDataTest::RowHeader4;
        rHeaders << DynamicTableDataTest::RowHeader5 << DynamicTableDataTest::RowHeader6;
        cHeaders << DynamicTableDataTest::ColumnHeader1 << DynamicTableDataTest::ColumnHeader2 << DynamicTableDataTest::ColumnHeader3 << DynamicTableDataTest::ColumnHeader4;
        cHeaders << DynamicTableDataTest::ColumnHeader5 << DynamicTableDataTest::ColumnHeader6 << DynamicTableDataTest::ColumnHeader7 << DynamicTableDataTest::ColumnHeader8;
        DynamicTableData data(DynamicTableDataTest::NumRows, DynamicTableDataTest::NumColumns, rHeaders, cHeaders);

        err = writer->writeValue(DynamicTableDataTest::Key, data);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }


      err = writer->closeFilterGroup();
      DREAM3D_REQUIRE_EQUAL(err, 0)

          return EXIT_SUCCESS;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template<typename T>
    void RWIntegerTest()
    {
      int err = 0;


      typename DataArray<T>::Pointer data = DataArray<T>::CreateArray(1, "Junk");
      QString primType = data->getTypeAsString();

      QString jsonFile = UnitTest::FilterParametersRWTest::OutputDir + primType + ".json";


      JsonFilterParametersWriter::Pointer writer = JsonFilterParametersWriter::New();
      writer->setFileName(jsonFile);
      writer->setPipelineName("TestPipelineName");

      err = writer->openFilterGroup(NULL, 0);
      DREAM3D_REQUIRE_EQUAL(err, 0)

          err = writer->writeValue(primType + QString("_min"), std::numeric_limits<T>::min() );
      DREAM3D_REQUIRE_EQUAL(err, 0)

          err = writer->writeValue(primType + QString("_max"), std::numeric_limits<T>::max() );
      DREAM3D_REQUIRE_EQUAL(err, 0)

          err = writer->writeValue(primType + QString("_zero"), static_cast<T>(0) );
      DREAM3D_REQUIRE_EQUAL(err, 0)

          // QVector<T> write test
      {
        QVector<T> vector;
        vector.push_back(std::numeric_limits<T>::min());
        vector.push_back(std::numeric_limits<T>::max());
        vector.push_back(static_cast<T>(0));
        vector.push_back(std::numeric_limits<T>::min() + 1);
        vector.push_back(std::numeric_limits<T>::max() - 1);
        err = writer->writeValue(primType + QString("_vector"), vector);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }


      err = writer->closeFilterGroup();
      DREAM3D_REQUIRE_EQUAL(err, 0)

          writer = JsonFilterParametersWriter::NullPointer();


      JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
      err = reader->openFile(jsonFile);
      DREAM3D_REQUIRE_EQUAL(err, 0)

          err = reader->openFilterGroup(NULL, 0);
      DREAM3D_REQUIRE_EQUAL(err, 0)

          T def = std::numeric_limits<T>::max() - 10;

      T val = reader->readValue(primType + QString("_min"), def);
      DREAM3D_REQUIRE_EQUAL(val, std::numeric_limits<T>::min() )

          val = reader->readValue(primType + QString("_max"), def);
      DREAM3D_REQUIRE_EQUAL(val, std::numeric_limits<T>::max() )

          val = reader->readValue(primType + QString("_zero"), def);
      DREAM3D_REQUIRE_EQUAL(val, 0 )


          // QVector<T> read test
      {
        QVector<T> vector = reader->readArray(primType + QString("_vector"), QVector<T>());
        DREAM3D_REQUIRE_EQUAL(vector.size(), 5)
            DREAM3D_REQUIRE_EQUAL(vector[0], std::numeric_limits<T>::min())
            DREAM3D_REQUIRE_EQUAL(vector[1], std::numeric_limits<T>::max())
            DREAM3D_REQUIRE_EQUAL(vector[2], 0)
            DREAM3D_REQUIRE_EQUAL(vector[3], std::numeric_limits<T>::min() + 1)
            DREAM3D_REQUIRE_EQUAL(vector[4], std::numeric_limits<T>::max() - 1)
      }

      err = reader->closeFilterGroup();
      DREAM3D_REQUIRE_EQUAL(err, 0)

          reader = JsonFilterParametersReader::NullPointer();

#if REMOVE_TEST_FILES
      QFile::remove(jsonFile);
#endif
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template<typename T>
    void RWFloatTest()
    {
      int err = 0;


      typename DataArray<T>::Pointer data = DataArray<T>::CreateArray(1, "Junk");
      QString primType = data->getTypeAsString();

      QString jsonFile = UnitTest::FilterParametersRWTest::OutputDir + primType + ".json";


      JsonFilterParametersWriter::Pointer writer = JsonFilterParametersWriter::New();
      writer->setFileName(jsonFile);
      writer->setPipelineName("TestPipelineName");

      err = writer->openFilterGroup(NULL, 0);
      DREAM3D_REQUIRE_EQUAL(err, 0)

          err = writer->writeValue(primType + QString("_min"), std::numeric_limits<T>::min() );
      DREAM3D_REQUIRE_EQUAL(err, 0)

          err = writer->writeValue(primType + QString("_max"), std::numeric_limits<T>::max() );
      DREAM3D_REQUIRE_EQUAL(err, 0)

          err = writer->writeValue(primType + QString("_zero"), static_cast<T>(0) );
      DREAM3D_REQUIRE_EQUAL(err, 0)

          // QVector<T> write test
      {
        QVector<T> vector;
        vector.push_back(std::numeric_limits<T>::min());
        vector.push_back(std::numeric_limits<T>::max());
        vector.push_back(static_cast<T>(0.0));
        vector.push_back(std::numeric_limits<T>::min() + 1);
        vector.push_back(std::numeric_limits<T>::max() - 1);
        err = writer->writeValue(primType + QString("_vector"), vector);
        DREAM3D_REQUIRE_EQUAL(err, 0)
      }


      err = writer->closeFilterGroup();
      DREAM3D_REQUIRE_EQUAL(err, 0)

          writer = JsonFilterParametersWriter::NullPointer();


      JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
      err = reader->openFile(jsonFile);
      DREAM3D_REQUIRE_EQUAL(err, 0)

          err = reader->openFilterGroup(NULL, 0);
      DREAM3D_REQUIRE_EQUAL(err, 0)

          T def = std::numeric_limits<T>::max() - static_cast<T>(10.0);

      T val = reader->readValue(primType + QString("_min"), def);
      DREAM3D_REQUIRE_EQUAL(val, std::numeric_limits<T>::min() )

          val = reader->readValue(primType + QString("_max"), def);
      DREAM3D_REQUIRE_EQUAL(val, std::numeric_limits<T>::max() )

          val = reader->readValue(primType + QString("_zero"), def);
      DREAM3D_REQUIRE_EQUAL(val, 0 )


          // QVector<T> read test
      {
        QVector<T> vector = reader->readArray(primType + QString("_vector"), QVector<T>());
        DREAM3D_REQUIRE_EQUAL(vector.size(), 5)
            DREAM3D_REQUIRE_EQUAL(vector[0], std::numeric_limits<T>::min())
            DREAM3D_REQUIRE_EQUAL(vector[1], std::numeric_limits<T>::max())
            DREAM3D_REQUIRE_EQUAL(vector[2], static_cast<T>(0.0))
            DREAM3D_REQUIRE_EQUAL(vector[3], std::numeric_limits<T>::min() + 1)
            DREAM3D_REQUIRE_EQUAL(vector[4], std::numeric_limits<T>::max() - 1)
      }

      err = reader->closeFilterGroup();
      DREAM3D_REQUIRE_EQUAL(err, 0)

          reader = JsonFilterParametersReader::NullPointer();

#if REMOVE_TEST_FILES
      QFile::remove(jsonFile);
#endif
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void operator()()
    {
      std::cout << "#### FilterParametersRWTest Starting ####" << std::endl;

      int err = EXIT_SUCCESS;

      DREAM3D_REGISTER_TEST(RWIntegerTest<uint8_t>())
      DREAM3D_REGISTER_TEST(RWIntegerTest<int8_t>())
      DREAM3D_REGISTER_TEST(RWIntegerTest<uint16_t>())
      DREAM3D_REGISTER_TEST(RWIntegerTest<int16_t>())
      DREAM3D_REGISTER_TEST(RWIntegerTest<uint32_t>())
      DREAM3D_REGISTER_TEST(RWIntegerTest<int32_t>())
      DREAM3D_REGISTER_TEST(RWIntegerTest<uint64_t>())
      DREAM3D_REGISTER_TEST(RWIntegerTest<int64_t>())

      DREAM3D_REGISTER_TEST(RWFloatTest<float>())
      DREAM3D_REGISTER_TEST(RWFloatTest<double>())

      DREAM3D_REGISTER_TEST(TestFilterParametersRW())

      DREAM3D_REGISTER_TEST(RemoveTestFiles())
    }

  private:
    FilterParametersRWTest(const FilterParametersRWTest&); // Copy Constructor Not Implemented
    void operator=(const FilterParametersRWTest&); // Operator '=' Not Implemented
};

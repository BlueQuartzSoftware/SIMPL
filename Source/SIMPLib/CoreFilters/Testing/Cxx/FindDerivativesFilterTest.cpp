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
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QtDebug>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

#define FDTEST_CREATE_DATA_CONTAINER(name, geom, dca)                                                                                                                                                  \
  DataContainer::Pointer _##geom##Container = DataContainer::New();                                                                                                                                    \
  _##geom##Container->setName(#name);                                                                                                                                                                  \
  _##geom##Container->setGeometry(geom);                                                                                                                                                               \
  dca->addDataContainer(_##geom##Container);

#define FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, dc, path, derivsName, data)                                                                                                                         \
  var.setValue(path);                                                                                                                                                                                  \
  propWasSet = filter->setProperty("SelectedArrayPath", var);                                                                                                                                          \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property SelectedArrayPath for" << #dc;                                                                                                                                 \
  }                                                                                                                                                                                                    \
  var.setValue(derivsName);                                                                                                                                                                            \
  propWasSet = filter->setProperty("DerivativesArrayPath", var);                                                                                                                                       \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property DerivativesArrayPath for" << #dc;                                                                                                                              \
  }                                                                                                                                                                                                    \
  filter->execute();                                                                                                                                                                                   \
  err = filter->getErrorCondition();                                                                                                                                                                   \
  DREAM3D_REQUIRE_EQUAL(err, 0);                                                                                                                                                                       \
  data = dc->getAttributeMatrix(derivsName.getAttributeMatrixName())->getAttributeArray(derivsName.getDataArrayName());                                                                                \
  if(nullptr == std::dynamic_pointer_cast<DoubleArrayType>(data))                                                                                                                                      \
  {                                                                                                                                                                                                    \
    err = 1;                                                                                                                                                                                           \
  }                                                                                                                                                                                                    \
  DREAM3D_REQUIRE_EQUAL(err, 0);                                                                                                                                                                       \
  checkDims = data->getComponentDimensions();                                                                                                                                                          \
  if((3 * cDims[0]) != checkDims[0])                                                                                                                                                                   \
  {                                                                                                                                                                                                    \
    err = 1;                                                                                                                                                                                           \
  }                                                                                                                                                                                                    \
  DREAM3D_REQUIRE_EQUAL(err, 0);

#define FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, dc, path, derivsName)                                                                                                                               \
  var.setValue(path);                                                                                                                                                                                  \
  propWasSet = filter->setProperty("SelectedArrayPath", var);                                                                                                                                          \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property SelectedArrayPath for" << #dc;                                                                                                                                 \
  }                                                                                                                                                                                                    \
  var.setValue(derivsName);                                                                                                                                                                            \
  propWasSet = filter->setProperty("DerivativesArrayPath", var);                                                                                                                                       \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property DerivativesArrayPath for" << #dc;                                                                                                                              \
  }                                                                                                                                                                                                    \
  filter->execute();                                                                                                                                                                                   \
  err = filter->getErrorCondition();                                                                                                                                                                   \
  DREAM3D_REQUIRE_NE(err, 0);

class FindDerivativesFilterTest
{
public:
  FindDerivativesFilterTest()
  {
  }
  virtual ~FindDerivativesFilterTest()
  {
  }
  SIMPL_TYPE_MACRO(FindDerivativesFilterTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void initializeDataArrays(DataContainer::Pointer m, QVector<size_t> cDims)
  {
    QMap<QString, AttributeMatrix::Pointer> attrMats = m->getAttributeMatrices();
    QMap<QString, AttributeMatrix::Pointer>::Iterator it;
    for(it = attrMats.begin(); it != attrMats.end(); ++it)
    {
      DoubleArrayType::Pointer dblArray = DoubleArrayType::CreateArray((*it)->getTupleDimensions(), cDims, "TEST_D");
      dblArray->initializeWithValue(5);
      (*it)->addAttributeArray("TEST_D", dblArray);
      FloatArrayType::Pointer fltArray = FloatArrayType::CreateArray((*it)->getTupleDimensions(), cDims, "TEST_F");
      fltArray->initializeWithValue(5);
      (*it)->addAttributeArray("TEST_F", fltArray);
      Int8ArrayType::Pointer int8Array = Int8ArrayType::CreateArray((*it)->getTupleDimensions(), cDims, "TEST_I8");
      int8Array->initializeWithValue(5);
      (*it)->addAttributeArray("TEST_I8", int8Array);
      UInt8ArrayType::Pointer uint8Array = UInt8ArrayType::CreateArray((*it)->getTupleDimensions(), cDims, "TEST_UI8");
      uint8Array->initializeWithValue(5);
      (*it)->addAttributeArray("TEST_UI8", uint8Array);
      Int16ArrayType::Pointer int16Array = Int16ArrayType::CreateArray((*it)->getTupleDimensions(), cDims, "TEST_I16");
      int16Array->initializeWithValue(5);
      (*it)->addAttributeArray("TEST_I16", int16Array);
      UInt16ArrayType::Pointer uint16Array = UInt16ArrayType::CreateArray((*it)->getTupleDimensions(), cDims, "TEST_UI16");
      uint16Array->initializeWithValue(5);
      (*it)->addAttributeArray("TEST_UI16", uint16Array);
      Int32ArrayType::Pointer int32Array = Int32ArrayType::CreateArray((*it)->getTupleDimensions(), cDims, "TEST_I32");
      int32Array->initializeWithValue(5);
      (*it)->addAttributeArray("TEST_I32", int32Array);
      UInt32ArrayType::Pointer uint32Array = UInt32ArrayType::CreateArray((*it)->getTupleDimensions(), cDims, "TEST_UI32");
      uint32Array->initializeWithValue(5);
      (*it)->addAttributeArray("TEST_UI32", uint32Array);
      Int64ArrayType::Pointer int64Array = Int64ArrayType::CreateArray((*it)->getTupleDimensions(), cDims, "TEST_I64");
      int64Array->initializeWithValue(5);
      (*it)->addAttributeArray("TEST_I64", int64Array);
      UInt64ArrayType::Pointer uint64Array = UInt64ArrayType::CreateArray((*it)->getTupleDimensions(), cDims, "TEST_UI64");
      uint64Array->initializeWithValue(5);
      (*it)->addAttributeArray("TEST_UI64", uint64Array);
      BoolArrayType::Pointer boolArray = BoolArrayType::CreateArray((*it)->getTupleDimensions(), cDims, "TEST_BOOL");
      boolArray->initializeWithZeros();
      (*it)->addAttributeArray("TEST_BOOL", boolArray);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void initializeAttributeMatrices(DataContainer::Pointer m, QVector<size_t> tDimsVert, QVector<size_t> tDimsCell)
  {
    AttributeMatrix::Pointer attrMat;
    QString ss = QObject::tr("AttrMatType%1").arg(0);
    attrMat = AttributeMatrix::New(tDimsVert, ss, AttributeMatrix::Type::Vertex);
    m->addAttributeMatrix(ss, attrMat);
    ss = QObject::tr("AttrMatType%1").arg(1);
    attrMat = AttributeMatrix::New(tDimsVert, ss, AttributeMatrix::Type::Edge);
    m->addAttributeMatrix(ss, attrMat);
    ss = QObject::tr("AttrMatType%1").arg(2);
    attrMat = AttributeMatrix::New(tDimsVert, ss, AttributeMatrix::Type::Face);
    m->addAttributeMatrix(ss, attrMat);
    ss = QObject::tr("AttrMatType%1").arg(3);
    attrMat = AttributeMatrix::New(tDimsCell, ss, AttributeMatrix::Type::Cell);
    m->addAttributeMatrix(ss, attrMat);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer initializeDataContainerArray()
  {

    DataContainerArray::Pointer dca = DataContainerArray::New();

    SharedVertexList::Pointer vertices1 = VertexGeom::CreateSharedVertexList(1);
    SharedVertexList::Pointer vertices2 = VertexGeom::CreateSharedVertexList(2);
    SharedVertexList::Pointer vertices3 = VertexGeom::CreateSharedVertexList(3);
    SharedVertexList::Pointer vertices4 = VertexGeom::CreateSharedVertexList(4);
    SharedVertexList::Pointer vertices5 = VertexGeom::CreateSharedVertexList(4);
    IGeometry::Pointer nullGeom = IGeometry::NullPointer();
    ImageGeom::Pointer image = ImageGeom::CreateGeometry("ImageGeom");
    RectGridGeom::Pointer rectGrid = RectGridGeom::CreateGeometry("RectGridGeom");
    VertexGeom::Pointer vertex = VertexGeom::CreateGeometry(vertices1, "VertexGeom");
    EdgeGeom::Pointer edges = EdgeGeom::CreateGeometry(1, vertices2, "EdgeGeom");
    TriangleGeom::Pointer tris = TriangleGeom::CreateGeometry(1, vertices3, "TriangleGeom");
    QuadGeom::Pointer quads = QuadGeom::CreateGeometry(1, vertices4, "QuadGeom");
    TetrahedralGeom::Pointer tets = TetrahedralGeom::CreateGeometry(1, vertices5, "TetrahedralGeom");

    image->setDimensions(10, 10, 10);
    image->setOrigin(0.0f, 0.0f, 0.0f);
    image->setResolution(1.0f, 1.0f, 1.0f);
    rectGrid->setDimensions(10, 10, 10);
    FloatArrayType::Pointer xBounds = FloatArrayType::CreateArray(11, SIMPL::Geometry::xBoundsList);
    FloatArrayType::Pointer yBounds = FloatArrayType::CreateArray(11, SIMPL::Geometry::yBoundsList);
    FloatArrayType::Pointer zBounds = FloatArrayType::CreateArray(11, SIMPL::Geometry::zBoundsList);
    float* xBoundsPtr = xBounds->getPointer(0);
    float* yBoundsPtr = yBounds->getPointer(0);
    float* zBoundsPtr = zBounds->getPointer(0);
    for(size_t i = 0; i < xBounds->getSize(); i++)
    {
      xBoundsPtr[i] = static_cast<float>(i);
      yBoundsPtr[i] = static_cast<float>(i);
      zBoundsPtr[i] = static_cast<float>(i) + (static_cast<float>(i) / 4.0f);
    }
    rectGrid->setXBounds(xBounds);
    rectGrid->setYBounds(yBounds);
    rectGrid->setZBounds(zBounds);

    float* verts1 = vertices1->getPointer(0);
    float* verts2 = vertices2->getPointer(0);
    float* verts3 = vertices3->getPointer(0);
    float* verts4 = vertices4->getPointer(0);
    float* verts5 = vertices5->getPointer(0);
    int64_t* edge = edges->getEdgePointer(0);
    int64_t* tri = tris->getTriPointer(0);
    int64_t* quad = quads->getQuadPointer(0);
    int64_t* tet = tets->getTetPointer(0);

    for(size_t i = 0; i < 3; i++) // first vertex at (0,0,0)
    {
      verts1[i] = 0.0f;
      verts2[i] = 0.0f;
      verts3[i] = 0.0f;
      verts4[i] = 0.0f;
      verts5[i] = 0.0f;
    }

    for(size_t i = 3; i < 5; i++) // second vertex at (1,1,0)
    {
      verts2[i] = 1.0f;
      verts3[i] = 1.0f;
      verts4[i] = 1.0f;
    }

    for(size_t i = 6; i < 9; i++) // third vertex at (1,1,1)
    {
      verts3[i] = 1.0f;
      verts4[i] = 1.0f;
    }

    verts4[11] = 1.0f; // fourth vertex at (0,0,1)

    // Finish making vertex list for single tetrahedron
    verts5[3] = 1.0f;  // (1,0,0)
    verts5[7] = 1.0f;  // (0,1,0)
    verts5[11] = 1.0f; // (0,0,1)

    edge[0] = 0;
    edge[1] = 1;
    tri[0] = 0;
    tri[1] = 1;
    tri[2] = 2;
    quad[0] = 0;
    quad[1] = 1;
    quad[2] = 2;
    quad[3] = 3;
    tet[0] = 0;
    tet[1] = 1;
    tet[2] = 2;
    tet[3] = 3;

    FDTEST_CREATE_DATA_CONTAINER(NullDC, nullGeom, dca);
    FDTEST_CREATE_DATA_CONTAINER(ImageDC, image, dca);
    FDTEST_CREATE_DATA_CONTAINER(RectGridDC, rectGrid, dca);
    FDTEST_CREATE_DATA_CONTAINER(VertexDC, vertex, dca);
    FDTEST_CREATE_DATA_CONTAINER(EdgeDC, edges, dca);
    FDTEST_CREATE_DATA_CONTAINER(TriDC, tris, dca);
    FDTEST_CREATE_DATA_CONTAINER(QuadDC, quads, dca);
    FDTEST_CREATE_DATA_CONTAINER(TetDC, tets, dca);

    QVector<size_t> tDims(1, 100);
    QVector<size_t> tDims2(1, 1);
    initializeAttributeMatrices(_nullGeomContainer, tDims, tDims);
    tDims[0] = 1000;
    initializeAttributeMatrices(_imageContainer, tDims, tDims);
    initializeAttributeMatrices(_rectGridContainer, tDims, tDims);
    tDims[0] = 1;
    initializeAttributeMatrices(_vertexContainer, tDims, tDims);
    tDims[0] = 2;
    initializeAttributeMatrices(_edgesContainer, tDims, tDims2);
    tDims[0] = 3;
    initializeAttributeMatrices(_trisContainer, tDims, tDims2);
    tDims[0] = 4;
    initializeAttributeMatrices(_quadsContainer, tDims, tDims2);
    initializeAttributeMatrices(_tetsContainer, tDims, tDims2);

    QVector<size_t> cDims(1, 3);

    initializeDataArrays(_nullGeomContainer, cDims);
    initializeDataArrays(_imageContainer, cDims);
    initializeDataArrays(_rectGridContainer, cDims);
    initializeDataArrays(_vertexContainer, cDims);
    initializeDataArrays(_edgesContainer, cDims);
    initializeDataArrays(_trisContainer, cDims);
    initializeDataArrays(_quadsContainer, cDims);
    initializeDataArrays(_tetsContainer, cDims);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void validateDerivativeValues(DataContainer::Pointer dc, DataArrayPath derivsPath, QString dataArrayName)
  {
    AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(derivsPath.getAttributeMatrixName());
    IDataArray::Pointer iArray = attrMat->getAttributeArray(dataArrayName);
    DoubleArrayType::Pointer array = std::dynamic_pointer_cast<DoubleArrayType>(iArray);
    DREAM3D_REQUIRE(array.get() != nullptr);

    size_t size = array->getSize();
    double* arrayPtr = array->getPointer(0);

    // Arrays were all initialized to same value, so the Jacobian should always be singular
    for(size_t i = 0; i < size; i++)
    {
      DREAM3D_REQUIRE_EQUAL(arrayPtr[i], 0.0);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void validateFindDerivatives(AbstractFilter::Pointer filter, DataContainerArray::Pointer dca)
  {
    DataContainer::Pointer null = dca->getDataContainer("NullDC");
    DataContainer::Pointer image = dca->getDataContainer("ImageDC");
    DataContainer::Pointer rectGrid = dca->getDataContainer("RectGridDC");
    DataContainer::Pointer vertex = dca->getDataContainer("VertexDC");
    DataContainer::Pointer edge = dca->getDataContainer("EdgeDC");
    DataContainer::Pointer tri = dca->getDataContainer("TriDC");
    DataContainer::Pointer quad = dca->getDataContainer("QuadDC");
    DataContainer::Pointer tet = dca->getDataContainer("TetDC");

    QVariant var;
    bool propWasSet;
    int err = 0;
    QVector<size_t> checkDims;
    QVector<size_t> cDims(1, 3);
    QString derivativeName = "Derivatives";
    DataArrayPath derivs;
    IDataArray::Pointer data;

    // All possible input array paths that require testing
    DataArrayPath vertPathD_N("NullDC", "AttrMatType0", "TEST_D");
    DataArrayPath vertPathD_I("ImageDC", "AttrMatType0", "TEST_D");
    DataArrayPath cellPathD_I("ImageDC", "AttrMatType3", "TEST_D");
    DataArrayPath vertPathD_R("RectGridDC", "AttrMatType0", "TEST_D");
    DataArrayPath cellPathD_R("RectGridDC", "AttrMatType3", "TEST_D");
    DataArrayPath vertPathD_V("VertexDC", "AttrMatType0", "TEST_D");
    DataArrayPath cellPathD_V("VertexDC", "AttrMatType3", "TEST_D");
    DataArrayPath vertPathD_E("EdgeDC", "AttrMatType0", "TEST_D");
    DataArrayPath edgePathD_E("EdgeDC", "AttrMatType1", "TEST_D");
    DataArrayPath vertPathD_T("TriDC", "AttrMatType0", "TEST_D");
    DataArrayPath facePathD_T("TriDC", "AttrMatType2", "TEST_D");
    DataArrayPath vertPathD_Q("QuadDC", "AttrMatType0", "TEST_D");
    DataArrayPath facePathD_Q("QuadDC", "AttrMatType2", "TEST_D");
    DataArrayPath vertPathD_Tet("TetDC", "AttrMatType0", "TEST_D");
    DataArrayPath facePathD_Tet("TetDC", "AttrMatType2", "TEST_D");
    DataArrayPath fltPath_I("ImageDC", "AttrMatType3", "TEST_F");
    DataArrayPath i8Path_I("ImageDC", "AttrMatType3", "TEST_I8");
    DataArrayPath ui8Path_I("ImageDC", "AttrMatType3", "TEST_UI8");
    DataArrayPath i16Path_I("ImageDC", "AttrMatType3", "TEST_I16");
    DataArrayPath ui16Path_I("ImageDC", "AttrMatType3", "TEST_UI16");
    DataArrayPath i32Path_I("ImageDC", "AttrMatType3", "TEST_I32");
    DataArrayPath ui32Path_I("ImageDC", "AttrMatType3", "TEST_UI32");
    DataArrayPath i64Path_I("ImageDC", "AttrMatType3", "TEST_I64");
    DataArrayPath ui64Path_I("ImageDC", "AttrMatType3", "TEST_UI64");
    DataArrayPath boolPath_I("ImageDC", "AttrMatType3", "TEST_BOOL");

    // All possible output array paths that require testing
    DataArrayPath vertOutPath_N("NullDC", "AttrMatType0", derivativeName);
    DataArrayPath vertOutPath_I("ImageDC", "AttrMatType0", derivativeName);
    DataArrayPath cellOutPath_I("ImageDC", "AttrMatType3", derivativeName);
    DataArrayPath vertOutPath_R("RectGridDC", "AttrMatType0", derivativeName);
    DataArrayPath cellOutPath_R("RectGridDC", "AttrMatType3", derivativeName);
    DataArrayPath vertOutPath_V("VertexDC", "AttrMatType0", derivativeName);
    DataArrayPath cellOutPath_V("VertexDC", "AttrMatType3", derivativeName);
    DataArrayPath vertOutPath_E("EdgeDC", "AttrMatType0", derivativeName);
    DataArrayPath edgeOutPath_E("EdgeDC", "AttrMatType1", derivativeName);
    DataArrayPath vertOutPath_T("TriDC", "AttrMatType0", derivativeName);
    DataArrayPath faceOutPath_T("TriDC", "AttrMatType2", derivativeName);
    DataArrayPath vertOutPath_Q("QuadDC", "AttrMatType0", derivativeName);
    DataArrayPath faceOutPath_Q("QuadDC", "AttrMatType2", derivativeName);
    DataArrayPath vertOutPath_Tet("TetDC", "AttrMatType0", derivativeName);
    DataArrayPath cellOutPath_Tet("TetDC", "AttrMatType3", derivativeName);

    // Fail if geometry is nullptr
    FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, null, vertPathD_N, vertOutPath_N);
    // Fail if incorrect AttributeMatrices set for ImageGeom/RectGridGeom
    FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, image, vertPathD_I, cellOutPath_I);
    FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, image, cellPathD_I, vertOutPath_I);
    FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, rectGrid, vertPathD_R, cellOutPath_R);
    FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, rectGrid, cellPathD_R, vertOutPath_R);
    // Fail if incorrect AttributeMatrices for VertexGeom
    FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, vertex, cellPathD_V, vertOutPath_V);
    FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, vertex, vertPathD_V, cellOutPath_V);
    // Fail if incorrect AttributeMatrices set for all other geometries
    FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, edge, vertPathD_E, vertOutPath_E);
    FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, tri, vertPathD_T, vertOutPath_T);
    FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, quad, vertPathD_Q, vertOutPath_Q);
    FDTEST_SET_PROPERTIES_AND_CHECK_NE(filter, tet, vertPathD_Tet, vertOutPath_Tet);
    // Succeed for each data type
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, image, fltPath_I, cellOutPath_I, data);
    validateDerivativeValues(image, cellOutPath_I, derivativeName);
    dca->getDataContainer(cellOutPath_I.getDataContainerName())->getAttributeMatrix(cellOutPath_I.getAttributeMatrixName())->removeAttributeArray(cellOutPath_I.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, image, i8Path_I, cellOutPath_I, data);
    validateDerivativeValues(image, cellOutPath_I, derivativeName);
    dca->getDataContainer(cellOutPath_I.getDataContainerName())->getAttributeMatrix(cellOutPath_I.getAttributeMatrixName())->removeAttributeArray(cellOutPath_I.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, image, ui8Path_I, cellOutPath_I, data);
    validateDerivativeValues(image, cellOutPath_I, derivativeName);
    dca->getDataContainer(cellOutPath_I.getDataContainerName())->getAttributeMatrix(cellOutPath_I.getAttributeMatrixName())->removeAttributeArray(cellOutPath_I.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, image, i16Path_I, cellOutPath_I, data);
    validateDerivativeValues(image, cellOutPath_I, derivativeName);
    dca->getDataContainer(cellOutPath_I.getDataContainerName())->getAttributeMatrix(cellOutPath_I.getAttributeMatrixName())->removeAttributeArray(cellOutPath_I.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, image, ui16Path_I, cellOutPath_I, data);
    validateDerivativeValues(image, cellOutPath_I, derivativeName);
    dca->getDataContainer(cellOutPath_I.getDataContainerName())->getAttributeMatrix(cellOutPath_I.getAttributeMatrixName())->removeAttributeArray(cellOutPath_I.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, image, i32Path_I, cellOutPath_I, data);
    validateDerivativeValues(image, cellOutPath_I, derivativeName);
    dca->getDataContainer(cellOutPath_I.getDataContainerName())->getAttributeMatrix(cellOutPath_I.getAttributeMatrixName())->removeAttributeArray(cellOutPath_I.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, image, ui32Path_I, cellOutPath_I, data);
    validateDerivativeValues(image, cellOutPath_I, derivativeName);
    dca->getDataContainer(cellOutPath_I.getDataContainerName())->getAttributeMatrix(cellOutPath_I.getAttributeMatrixName())->removeAttributeArray(cellOutPath_I.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, image, i64Path_I, cellOutPath_I, data);
    validateDerivativeValues(image, cellOutPath_I, derivativeName);
    dca->getDataContainer(cellOutPath_I.getDataContainerName())->getAttributeMatrix(cellOutPath_I.getAttributeMatrixName())->removeAttributeArray(cellOutPath_I.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, image, ui64Path_I, cellOutPath_I, data);
    validateDerivativeValues(image, cellOutPath_I, derivativeName);
    dca->getDataContainer(cellOutPath_I.getDataContainerName())->getAttributeMatrix(cellOutPath_I.getAttributeMatrixName())->removeAttributeArray(cellOutPath_I.getDataArrayName());
    // Succeed for each geometry
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, image, cellPathD_I, cellOutPath_I, data);
    validateDerivativeValues(image, cellOutPath_I, derivativeName);
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, rectGrid, cellPathD_R, cellOutPath_R, data);
    validateDerivativeValues(rectGrid, cellOutPath_R, derivativeName);
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, vertex, vertPathD_V, vertOutPath_V, data);
    validateDerivativeValues(vertex, vertOutPath_V, derivativeName);
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, edge, vertPathD_E, edgeOutPath_E, data);
    validateDerivativeValues(edge, edgeOutPath_E, derivativeName);
    dca->getDataContainer(edgeOutPath_E.getDataContainerName())->getAttributeMatrix(edgeOutPath_E.getAttributeMatrixName())->removeAttributeArray(edgeOutPath_E.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, tri, vertPathD_T, faceOutPath_T, data);
    validateDerivativeValues(tri, faceOutPath_T, derivativeName);
    dca->getDataContainer(faceOutPath_T.getDataContainerName())->getAttributeMatrix(faceOutPath_T.getAttributeMatrixName())->removeAttributeArray(faceOutPath_T.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, quad, vertPathD_Q, faceOutPath_Q, data);
    validateDerivativeValues(quad, faceOutPath_Q, derivativeName);
    dca->getDataContainer(faceOutPath_Q.getDataContainerName())->getAttributeMatrix(faceOutPath_Q.getAttributeMatrixName())->removeAttributeArray(faceOutPath_Q.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, tet, vertPathD_Tet, cellOutPath_Tet, data);
    validateDerivativeValues(tet, cellOutPath_Tet, derivativeName);
    dca->getDataContainer(cellOutPath_Tet.getDataContainerName())->getAttributeMatrix(cellOutPath_Tet.getAttributeMatrixName())->removeAttributeArray(cellOutPath_Tet.getDataArrayName());
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, edge, edgePathD_E, edgeOutPath_E, data);
    validateDerivativeValues(edge, edgeOutPath_E, derivativeName);
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, tri, facePathD_T, faceOutPath_T, data);
    validateDerivativeValues(tri, faceOutPath_T, derivativeName);
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, quad, facePathD_Q, faceOutPath_Q, data);
    validateDerivativeValues(quad, faceOutPath_Q, derivativeName);
    FDTEST_SET_PROPERTIES_AND_CHECK_EQ(filter, tet, facePathD_Tet, cellOutPath_Tet, data);
    validateDerivativeValues(tet, cellOutPath_Tet, derivativeName);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::FindDerivativesFilterTest::TestFile1);
    QFile::remove(UnitTest::FindDerivativesFilterTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FindDerivatives Filter from the FilterManager
    QString filtName = "FindDerivatives";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FindDerivativesTest Requires the use of the " << filtName.toStdString() << " filter which is found in the Core Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFindDerivatives()
  {

    DataContainerArray::Pointer dca = initializeDataContainerArray();

    QString filtName = "FindDerivatives";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr);

    AbstractFilter::Pointer findDerivsFilter = factory->create();
    DREAM3D_REQUIRE(findDerivsFilter.get() != nullptr);

    findDerivsFilter->setDataContainerArray(dca);

    validateFindDerivatives(findDerivsFilter, dca);

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //  Use test framework
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### FindDerivativesFilterTest Starting ####" << std::endl;
    int err = EXIT_SUCCESS;

    // Use this to register a specific function that will run a test
    DREAM3D_REGISTER_TEST(TestFilterAvailability());
    DREAM3D_REGISTER_TEST(TestFindDerivatives());
    DREAM3D_REGISTER_TEST(RemoveTestFiles());
  }

private:
  FindDerivativesFilterTest(const FindDerivativesFilterTest&); // Copy Constructor Not Implemented
  void operator=(const FindDerivativesFilterTest&);            // Operator '=' Not Implemented
};

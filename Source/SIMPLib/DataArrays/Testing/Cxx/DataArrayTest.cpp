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

#include <stdlib.h>

#include <iostream>
#include <vector>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QVector>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

#define NUM_ELEMENTS 10
#define NUM_COMPONENTS 2
#define NUM_TUPLES 5

#define NUM_ELEMENTS_2 20
#define NUM_COMPONENTS_2 2
#define NUM_TUPLES_2 10

#define NUM_ELEMENTS_3 16
#define NUM_COMPONENTS_3 2
#define NUM_TUPLES_3 8

#define NUM_ELEMENTS_4 16
#define NUM_COMPONENTS_4 4
#define NUM_TUPLES_4 4

#define RANK 3
#define DIM0 2
#define DIM1 3
#define DIM2 4

class DataArrayTest
{
public:
  DataArrayTest() = default;
  virtual ~DataArrayTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::DataArrayTest::TestFile);
    QDir tempDir(UnitTest::DataArrayTest::TestDir);
    tempDir.removeRecursively();
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void TestcopyTuplesForType()
  {
    int err = 0;
    QVector<size_t> dims(1, NUM_COMPONENTS_2);
    typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_TUPLES_2, dims, "TestcopyTuples");
    DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);

    for(size_t i = 0; i < NUM_TUPLES_2; ++i)
    {
      array->setComponent(i, 0, static_cast<T>(i));
      array->setComponent(i, 1, static_cast<T>(i));
    }

    err = array->copyTuple(0, 1);
    DREAM3D_REQUIRE_EQUAL(0, err);
    err = array->copyTuple(3, 2);
    DREAM3D_REQUIRE_EQUAL(0, err);
    err = array->copyTuple(4, 5);
    DREAM3D_REQUIRE_EQUAL(0, err);
    err = array->copyTuple(8, 9);
    DREAM3D_REQUIRE_EQUAL(0, err);
    err = array->copyTuple(18, 19);
    DREAM3D_REQUIRE_EQUAL(-1, err);

    DREAM3D_REQUIRE_EQUAL(array->getComponent(1, 0), 0);
    DREAM3D_REQUIRE_EQUAL(array->getComponent(1, 1), 0);
    DREAM3D_REQUIRE_EQUAL(array->getComponent(2, 0), 3);
    DREAM3D_REQUIRE_EQUAL(array->getComponent(2, 1), 3);
    DREAM3D_REQUIRE_EQUAL(array->getComponent(5, 0), 4);
    DREAM3D_REQUIRE_EQUAL(array->getComponent(5, 1), 4);
    DREAM3D_REQUIRE_EQUAL(array->getComponent(9, 0), 8);
    DREAM3D_REQUIRE_EQUAL(array->getComponent(9, 1), 8);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestcopyTuples()
  {
    TestcopyTuplesForType<int8_t>();
    TestcopyTuplesForType<uint8_t>();
    TestcopyTuplesForType<int16_t>();
    TestcopyTuplesForType<uint16_t>();
    TestcopyTuplesForType<int32_t>();
    TestcopyTuplesForType<uint32_t>();
    TestcopyTuplesForType<int64_t>();
    TestcopyTuplesForType<uint64_t>();
    TestcopyTuplesForType<float>();
    TestcopyTuplesForType<double>();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void TestEraseElementsForType()
  {
    // Test dropping of front elements only
    {
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS, "Test1");
      DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
      for(size_t i = 0; i < NUM_ELEMENTS; ++i)
      {
        array->setComponent(i, 0, static_cast<T>(i));
      }

      QVector<size_t> eraseElements;
      eraseElements.push_back(0);
      eraseElements.push_back(1);

      array->eraseTuples(eraseElements);

      DREAM3D_REQUIRE_EQUAL(array->getValue(0), 2);
      DREAM3D_REQUIRE_EQUAL(array->getValue(1), 3);
      DREAM3D_REQUIRE_EQUAL(array->getValue(2), 4);
      DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
    }

    // Test Dropping of internal elements
    {
      QVector<size_t> dims(1, NUM_COMPONENTS_2);
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS_2, dims, "Test2");
      DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
      for(size_t i = 0; i < NUM_TUPLES_2; ++i)
      {
        array->setComponent(i, 0, static_cast<T>(i));
        array->setComponent(i, 1, static_cast<T>(i));
      }

      QVector<size_t> eraseElements;
      eraseElements.push_back(3);
      eraseElements.push_back(6);
      eraseElements.push_back(8);

      array->eraseTuples(eraseElements);

      DREAM3D_REQUIRE_EQUAL(array->getComponent(3, 0), 4);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(3, 1), 4);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(5, 0), 7);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(5, 1), 7);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(6, 0), 9);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(6, 1), 9);
    }

    // Test Dropping of internal elements
    {
      QVector<size_t> dims(1, NUM_COMPONENTS_2);
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS_2, dims, "Test3");
      DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
      for(size_t i = 0; i < NUM_TUPLES_2; ++i)
      {
        array->setComponent(i, 0, static_cast<T>(i));
        array->setComponent(i, 1, static_cast<T>(i));
      }

      QVector<size_t> eraseElements;
      eraseElements.push_back(3);
      eraseElements.push_back(6);
      eraseElements.push_back(9);
      array->eraseTuples(eraseElements);

      DREAM3D_REQUIRE_EQUAL(array->getComponent(3, 0), 4);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(3, 1), 4);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(5, 0), 7);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(5, 1), 7);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(6, 0), 8);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(6, 1), 8);
    }

    // Test Dropping of internal continuous elements
    {
      QVector<size_t> dims(1, NUM_COMPONENTS_2);
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS_2, dims, "Test4");
      DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
      for(size_t i = 0; i < NUM_TUPLES_2; ++i)
      {
        array->setComponent(i, 0, static_cast<T>(i));
        array->setComponent(i, 1, static_cast<T>(i));
      }

      QVector<size_t> eraseElements;
      eraseElements.push_back(3);
      eraseElements.push_back(4);
      eraseElements.push_back(5);
      array->eraseTuples(eraseElements);

      DREAM3D_REQUIRE_EQUAL(array->getComponent(3, 0), 6);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(3, 1), 6);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(4, 0), 7);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(4, 1), 7);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(5, 0), 8);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(5, 1), 8);
    }

    // Test Dropping of Front and Back Elements
    {
      QVector<size_t> dims(1, NUM_COMPONENTS_2);
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS_2, dims, "Test5");
      DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
      for(size_t i = 0; i < NUM_TUPLES_2; ++i)
      {
        array->setComponent(i, 0, static_cast<T>(i));
        array->setComponent(i, 1, static_cast<T>(i));
      }

      QVector<size_t> eraseElements;
      eraseElements.push_back(0);
      eraseElements.push_back(9);

      array->eraseTuples(eraseElements);

      DREAM3D_REQUIRE_EQUAL(array->getComponent(0, 0), 1);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(0, 1), 1);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(7, 0), 8);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(7, 1), 8);
    }

    // Test Dropping of Back Elements
    {
      QVector<size_t> dims(1, NUM_COMPONENTS_2);
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS_2, dims, "Test6");
      DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
      for(size_t i = 0; i < NUM_TUPLES_2; ++i)
      {
        array->setComponent(i, 0, static_cast<T>(i));
        array->setComponent(i, 1, static_cast<T>(i));
      }

      QVector<size_t> eraseElements;
      eraseElements.push_back(7);
      eraseElements.push_back(8);
      eraseElements.push_back(9);
      array->eraseTuples(eraseElements);

      DREAM3D_REQUIRE_EQUAL(array->getComponent(4, 0), 4);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(4, 1), 4);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(5, 0), 5);
      DREAM3D_REQUIRE_EQUAL(array->getComponent(5, 1), 5);
    }

    // Test Dropping of indices larger than the number of tuples
    {
      QVector<size_t> dims(1, NUM_COMPONENTS_2);
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_TUPLES_2, dims, "Test6");
      DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
      for(size_t i = 0; i < NUM_TUPLES_2; ++i)
      {
        array->setComponent(i, 0, static_cast<T>(i));
        array->setComponent(i, 1, static_cast<T>(i));
      }

      QVector<size_t> eraseElements;
      eraseElements.push_back(10);
      int err = array->eraseTuples(eraseElements);
      DREAM3D_REQUIRE_EQUAL(err, -100)

      eraseElements.clear();
      err = array->eraseTuples(eraseElements);
      DREAM3D_REQUIRE_EQUAL(err, 0)

      eraseElements.resize(20);
      err = array->eraseTuples(eraseElements);
      DREAM3D_REQUIRE_EQUAL(err, 0)
      size_t nTuples = array->getNumberOfTuples();
      DREAM3D_REQUIRE_EQUAL(nTuples, 0)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestEraseElements()
  {
    TestEraseElementsForType<int8_t>();
    TestEraseElementsForType<uint8_t>();
    TestEraseElementsForType<int16_t>();
    TestEraseElementsForType<uint16_t>();
    TestEraseElementsForType<int32_t>();
    TestEraseElementsForType<uint32_t>();
    TestEraseElementsForType<int64_t>();
    TestEraseElementsForType<uint64_t>();
    TestEraseElementsForType<float>();
    TestEraseElementsForType<double>();
  }

  template <typename T> QString TypeToString(T v)
  {

    if(typeid(v) == typeid(long))
    {
      return QString("long");
    }
    if(typeid(v) == typeid(long int))
    {
      return QString("long int");
    }
    if(typeid(v) == typeid(unsigned long int))
    {
      return QString("unsigned long int");
    }
    if(typeid(v) == typeid(unsigned long))
    {
      return QString("unsigned long");
    }

    if(typeid(v) == typeid(long long int))
    {
      return QString("long long int");
    }
    if(typeid(v) == typeid(long long))
    {
      return QString("long long");
    }
    if(typeid(v) == typeid(unsigned long long int))
    {
      return QString("unsigned long long int");
    }
    if(typeid(v) == typeid(unsigned long long))
    {
      return QString("unsigned long long");
    }

    return QString("nullptr");
  }

#define TEST_TYPE_STRING(type)                                                                                                                                                                         \
  {                                                                                                                                                                                                    \
    DataArray<type>::Pointer p_##type = DataArray<type>::CreateArray(1, "Test", false);                                                                                                                \
    QString s_##type = p_##type->getTypeAsString();                                                                                                                                                    \
    /* qDebug() << #type << s_##type; */                                                                                                                                                               \
    type value = (type)(1);                                                                                                                                                                            \
    value = value;                                                                                                                                                                                     \
    /* qDebug() << TypeToString<type>(value); */                                                                                                                                                       \
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestTypeStrings()
  {
    TEST_TYPE_STRING(qint8)
    TEST_TYPE_STRING(quint8)
    TEST_TYPE_STRING(qint16)
    TEST_TYPE_STRING(quint16)
    TEST_TYPE_STRING(qint32)
    TEST_TYPE_STRING(quint32)
    TEST_TYPE_STRING(qint64)
    TEST_TYPE_STRING(quint64)

    TEST_TYPE_STRING(int8_t)
    TEST_TYPE_STRING(uint8_t)
    TEST_TYPE_STRING(int16_t)
    TEST_TYPE_STRING(uint16_t)
    TEST_TYPE_STRING(int32_t)
    TEST_TYPE_STRING(uint32_t)
    TEST_TYPE_STRING(int64_t)
    TEST_TYPE_STRING(uint64_t)
    TEST_TYPE_STRING(float)
    TEST_TYPE_STRING(double)
    //   long long int myint = 50;
    //   std::cout << "myint has type: " << typeid(myint).name() << '\n';

    //   qDebug() << typeid(int64_t).name() << " " << typeid(qint64).name() << " " << typeid(signed long long int).name() << " " << typeid(long long int).name() << " " << typeid(long long).name();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestDataArray()
  {
    int32_t* ptr = nullptr;
    {
      Int32ArrayType::Pointer d = Int32ArrayType::CreateArray(0, "Test7");
      DREAM3D_REQUIRE_EQUAL(0, d->getSize());
      DREAM3D_REQUIRE_EQUAL(0, d->getNumberOfTuples());
      ptr = d->getPointer(0);
      DREAM3D_REQUIRE_EQUAL(ptr, 0);
      DREAM3D_REQUIRE_EQUAL(d->isAllocated(), false);
    }

    {
      QVector<size_t> dims(1, NUM_COMPONENTS);
      Int32ArrayType::Pointer int32Array = Int32ArrayType::CreateArray(NUM_ELEMENTS, dims, "Test8");
      ptr = int32Array->getPointer(0);
      DREAM3D_REQUIRE_EQUAL(int32Array->isAllocated(), true);
      DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS, int32Array->getNumberOfTuples());
      DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS * NUM_COMPONENTS, int32Array->getSize());

      for(int i = 0; i < NUM_TUPLES; ++i)
      {
        for(int c = 0; c < NUM_COMPONENTS; ++c)
        {
          int32Array->setComponent(i, c, i + c);
        }
      }

      // Resize Larger
      int32Array->resize(NUM_TUPLES_2);
      DREAM3D_REQUIRE_EQUAL(NUM_TUPLES_2, int32Array->getNumberOfTuples());
      DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS_2, int32Array->getSize());
      DREAM3D_REQUIRE_EQUAL(int32Array->isAllocated(), true);

      // This should have saved our data so lets look at the data and compare it
      for(int i = 0; i < NUM_TUPLES; ++i)
      {
        for(int c = 0; c < NUM_COMPONENTS; ++c)
        {
          DREAM3D_REQUIRE_EQUAL((int32Array->getComponent(i, c)), (i + c))
        }
      }

      // Resize Smaller - Which should have still saved some of our data
      int32Array->resize(NUM_TUPLES_3);
      DREAM3D_REQUIRE_EQUAL(NUM_TUPLES_3, int32Array->getNumberOfTuples());
      DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS_3, int32Array->getSize());
      DREAM3D_REQUIRE_EQUAL(int32Array->isAllocated(), true);

      // This should have saved our data so lets look at the data and compare it
      for(int i = 0; i < NUM_TUPLES; ++i)
      {
        for(int c = 0; c < NUM_COMPONENTS; ++c)
        {
          DREAM3D_REQUIRE_EQUAL((int32Array->getComponent(i, c)), (i + c))
        }
      }

      // Change number of components
      //    dims[0] = NUM_COMPONENTS_4;
      //    int32Array->setDims(dims);
      //    DREAM3D_REQUIRE_EQUAL(NUM_TUPLES_4, int32Array->getNumberOfTuples());
      //    DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS_4, int32Array->getSize());

      int temp = 9999;
      int32Array->initializeTuple(0, &temp);
      for(int c = 0; c < NUM_COMPONENTS; ++c)
      {
        DREAM3D_REQUIRE_EQUAL((int32Array->getComponent(0, c)), (9999))
      }

      ptr = int32Array->getPointer(0);
    }
  }

  // -----------------------------------------------------------------------------
  //  Test the NeighborList Class
  // -----------------------------------------------------------------------------
  template <typename T> void TestNeighborListForType()
  {
    typename NeighborList<T>::Pointer n = NeighborList<T>::New();
    n->setName("Test");

    for(int i = 0; i < 4; ++i)
    {
      for(T j = 0; j < (T)(i + 4); ++j)
      {
        n->addEntry(i, static_cast<T>(j * i + 3));
      }
    }

    typename NeighborList<T>::SharedVectorType v;
    for(int i = 0; i < 4; ++i)
    {
      v = n->getList(i);
      DREAM3D_REQUIRE_NE(v.get(), 0);
    }

    // Remove the front 2 elements and test
    QVector<size_t> eraseElements;
    eraseElements.push_back(0);
    eraseElements.push_back(1);

    n->eraseTuples(eraseElements);
    for(int i = 0; i < 2; ++i)
    {
      v = n->getList(i);
      DREAM3D_REQUIRE_NE(v.get(), 0);
      DREAM3D_REQUIRE_EQUAL(v->size(), static_cast<size_t>(i + 2 + 4));
      for(T j = 0; j < (T)(i + 4 + 2); ++j)
      {
        DREAM3D_REQUIRE_EQUAL(v->at(j), j * (i + 2) + 3);
      }
    }

    // Reset and erase the back 2 "Tuples"
    n->clearAllLists();
    for(int i = 0; i < 4; ++i)
    {
      for(T j = 0; j < (T)(i + 4); ++j)
      {
        n->addEntry(i, j * i + 3);
      }
    }
    eraseElements.clear();
    eraseElements.push_back(2);
    eraseElements.push_back(3);
    n->eraseTuples(eraseElements);
    for(int i = 0; i < 2; ++i)
    {
      v = n->getList(i);
      DREAM3D_REQUIRE_NE(v.get(), 0);
      DREAM3D_REQUIRE_EQUAL(v->size(), static_cast<size_t>(i + 4));
      for(T j = 0; j < (T)(i + 4); ++j)
      {
        DREAM3D_REQUIRE_EQUAL(v->at(j), j * i + 3);
      }
    }

    // Reset and erase the back 2 "Tuples"
    n->clearAllLists();
    for(int i = 0; i < 4; ++i)
    {
      for(T j = 0; j < (T)(i + 4); ++j)
      {
        n->addEntry(i, j * i + 3);
      }
    }
    eraseElements.clear();
    eraseElements.push_back(1);
    eraseElements.push_back(2);
    n->eraseTuples(eraseElements);
    int i = 0;
    v = n->getList(i);
    DREAM3D_REQUIRE_NE(v.get(), 0);
    DREAM3D_REQUIRE_EQUAL(v->size(), static_cast<size_t>(i + 4));
    for(T j = 0; j < (T)(i + 4); ++j)
    {
      DREAM3D_REQUIRE_EQUAL(v->at(j), j * i + 3);
    }
    i = 1;
    v = n->getList(i);
    DREAM3D_REQUIRE_NE(v.get(), 0);
    i = 3;
    DREAM3D_REQUIRE_EQUAL(v->size(), static_cast<size_t>(i + 4));
    for(T j = 0; j < (T)(i + 4); ++j)
    {
      DREAM3D_REQUIRE_EQUAL(v->at(j), j * i + 3);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void TestNeighborListDeepCopyForType()
  {

    QVector<size_t> tDims(10);
    AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, "AttributeMatrix", AttributeMatrix::Type::Cell);

    typename NeighborList<T>::Pointer neiList = NeighborList<T>::CreateArray(10, "NeighborList");

    for(int i = 0; i < 10; ++i)
    {
      for(T j = 0; j < (T)(i + 4); ++j)
      {
        neiList->addEntry(i, static_cast<T>(i + 3));
      }
    }

    typename NeighborList<T>::Pointer copy = std::dynamic_pointer_cast<NeighborList<T>>(neiList->deepCopy());
    for(int i = 0; i < 10; ++i)
    {

      unsigned char value = 255;
      typename NeighborList<T>::SharedVectorType nEntry = neiList->getList(i);
      typename NeighborList<T>::SharedVectorType cEntry = copy->getList(i);
      DREAM3D_REQUIRED(nEntry.get(), !=, cEntry.get());
      (*nEntry)[0] = static_cast<T>(value);
      DREAM3D_REQUIRED((*cEntry)[0], !=, 10000000);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestNeighborList()
  {
    TestNeighborListForType<int8_t>();
    TestNeighborListForType<uint8_t>();
    TestNeighborListForType<int16_t>();
    TestNeighborListForType<uint16_t>();
    TestNeighborListForType<int32_t>();
    TestNeighborListForType<uint32_t>();
    TestNeighborListForType<int64_t>();
    TestNeighborListForType<uint64_t>();
    TestNeighborListForType<float>();
    TestNeighborListForType<double>();

    TestNeighborListDeepCopyForType<int8_t>();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> int ValidateArrayForType(typename DataArray<T>::Pointer array, size_t numTuples, int numComp)
  {
    int err = 0;
    DREAM3D_REQUIRED(true, ==, array->isAllocated());
    size_t nt = array->getNumberOfTuples();
    DREAM3D_REQUIRED(nt, ==, numTuples);
    int nc = array->getNumberOfComponents();
    DREAM3D_REQUIRED(nc, ==, numComp);

    size_t typeSize = array->getTypeSize();
    DREAM3D_REQUIRE_EQUAL(sizeof(T), typeSize);

    size_t numElements = array->getSize();
    DREAM3D_REQUIRED(numElements, ==, (nt * nc));
    // initialize the array with zeros to get a baseline
    array->initializeWithZeros();
    // Get a pointer to the data and loop through the array making sure all values are Zero
    T* ptr = array->getPointer(0);
    for(size_t i = 0; i < numElements; i++)
    {
      DREAM3D_REQUIRE_EQUAL(0, ptr[i]);
    }
    // Splat another value across the array starting at an offset into the array
    // and test those values made it into the array correctly
    array->initializeWithValue(static_cast<T>(1), numComp);
    for(size_t i = numComp; i < numElements; i++)
    {
      DREAM3D_REQUIRE_EQUAL(static_cast<T>(1), ptr[i]);
    }
    // Initialize the entire array with a value (offset = 0);
    array->initializeWithValue(static_cast<T>(2), 0);
    for(size_t i = 0; i < numElements; i++)
    {
      DREAM3D_REQUIRE_EQUAL(static_cast<T>(2), ptr[i]);
    }

    // Initialize the entire array with a value (offset = 0), this time using the default value for the offset
    array->initializeWithValue(static_cast<T>(3));
    ptr = array->getPointer(0);
    for(size_t i = 0; i < numElements; i++)
    {
      DREAM3D_REQUIRE_EQUAL(static_cast<T>(3), ptr[i]);
      array->setValue(i, static_cast<T>(4));
      T val = array->getValue(i);
      DREAM3D_REQUIRE_EQUAL(val, static_cast<T>(4))
    }

    // Test setting of a Tuple with a value, which means all components of that tuple will have the same value
    size_t index = 0;
    array->initializeWithZeros();
    for(size_t t = 0; t < numTuples; t++)
    {
      T temp = 6.0;
      array->initializeTuple(t, &temp);
      for(int j = 0; j < numComp; j++)
      {
        T val = array->getComponent(t, j);
        DREAM3D_REQUIRE_EQUAL(val, (static_cast<T>(6)))
      }
    }

    // Test setting individual components to a specific value
    index = 0;
    array->initializeWithZeros();
    for(size_t t = 0; t < numTuples; t++)
    {
      for(int j = 0; j < numComp; j++)
      {
        index = t * numComp + j;
        array->setComponent(t, j, static_cast<T>(t + j));
        T val = array->getComponent(t, j);
        DREAM3D_REQUIRE_EQUAL(val, (static_cast<T>(t + j)))
        val = array->getValue(index);
        DREAM3D_REQUIRE_EQUAL(val, (static_cast<T>(t + j)))
      }
    }

    ///     virtual QVector<size_t> getComponentDimensions()
    // Test resizing the array based on a give number of tuples. The number of Components will stay the same at each tuple
    array->resize(numTuples * 2);
    array->initializeWithZeros(); // Init the grown array to all Zeros
    nt = array->getNumberOfTuples();
    DREAM3D_REQUIRED(nt, ==, (numTuples * 2));
    nc = array->getNumberOfComponents();
    DREAM3D_REQUIRED(nc, ==, numComp);

    // Test resizing the array to a smaller size
    array->resize(numTuples);
    array->initializeWithZeros(); // Init the grown array to all Zeros
    nt = array->getNumberOfTuples();
    DREAM3D_REQUIRED(nt, ==, (numTuples));
    nc = array->getNumberOfComponents();
    DREAM3D_REQUIRED(nc, ==, numComp);

    ////clear()

    // This resizes the array to Zero destroying all the data in the process.
    array->clear();
    DREAM3D_REQUIRED(false, ==, array->isAllocated());
    nt = array->getNumberOfTuples();
    DREAM3D_REQUIRED(nt, ==, 0);
    nc = array->getNumberOfComponents();
    DREAM3D_REQUIRED(nc, ==, numComp);
    nt = array->getSize();
    DREAM3D_REQUIRED(nt, ==, 0);
    ptr = array->getPointer(0);
    DREAM3D_REQUIRED_PTR(ptr, ==, nullptr);

    // Test resizing the array to a any larger size
    array->resize(numTuples);
    array->initializeWithZeros(); // Init the grown array to all Zeros
    nt = array->getNumberOfTuples();
    DREAM3D_REQUIRED(nt, ==, (numTuples));
    nc = array->getNumberOfComponents();
    DREAM3D_REQUIRED(nc, ==, numComp);
    ptr = array->getPointer(0);
    DREAM3D_REQUIRED_PTR(ptr, !=, nullptr);

    return err;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void TestArrayCreationForType()
  {
    int err = 0;
    size_t numTuples = NUM_TUPLES;
    int rank = RANK;
    size_t dims[RANK] = {DIM0, DIM1, DIM2};
    int numComp = 1;
    {
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(numTuples, "TEST");
      err = ValidateArrayForType<T>(array, numTuples, numComp);
      DREAM3D_REQUIRED(err, >=, 0)
    }

    {
      numComp = DIM0 * DIM1 * DIM2;
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(numTuples, rank, dims, "TEST");
      err = ValidateArrayForType<T>(array, numTuples, numComp);
      DREAM3D_REQUIRED(err, >=, 0)
    }

    {
      numComp = DIM0 * DIM1 * DIM2;
      std::vector<size_t> vDims(3, 0);
      vDims[0] = DIM0;
      vDims[1] = DIM1;
      vDims[2] = DIM2;
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(numTuples, vDims, "TEST");
      err = ValidateArrayForType<T>(array, numTuples, numComp);
      DREAM3D_REQUIRED(err, >=, 0)
    }

    {
      numComp = DIM0 * DIM1 * DIM2;
      QVector<size_t> vDims(3, 0);
      vDims[0] = DIM0;
      vDims[1] = DIM1;
      vDims[2] = DIM2;
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(numTuples, vDims, "TEST");
      err = ValidateArrayForType<T>(array, numTuples, numComp);
      DREAM3D_REQUIRED(err, >=, 0)
    }

    {
      QVector<size_t> tDims(2, 4);
      QVector<size_t> vDims(3, 0);
      vDims[0] = DIM0;
      vDims[1] = DIM1;
      vDims[2] = DIM2;
      typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(tDims, vDims, "TEST");
      err = ValidateArrayForType<T>(array, tDims[0] * tDims[1], numComp);
      DREAM3D_REQUIRED(err, >=, 0)
    }

#if 0
      {
        union {
            uint8_t ui8;
            int8_t i8;
            uint16_t ui16;
            int16_t i16;
            uint32_t ui32;
            int32_t i32;
            uint64_t ui64;
            int64_t i64;
            float f32;
            double d64;
        } mudflap;
        unsigned char* cptr = nullptr;

        QVector<size_t> tDims(1, 10);
        typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(DIM0, "TEST");
        array->initializeWithZeros();
        cptr = reinterpret_cast<uint8_t*>(array->getPointer(0));

        if (sizeof(T) == 1) {
          uint8_t* ptr = reinterpret_cast<uint8_t*>(cptr);
          mudflap.ui8 = *ptr;
          DREAM3D_REQUIRED(mudflap.ui8, ==, 0x00)
        }
        if (sizeof(T) == 2) {
          uint16_t* ptr = reinterpret_cast<uint16_t*>(cptr);
          mudflap.ui16 = *ptr;
          DREAM3D_REQUIRED(mudflap.ui16, ==, 0x0000)
        }
        if (sizeof(T) == 4) {
          uint32_t* ptr = reinterpret_cast<uint32_t*>(cptr);
          mudflap.ui32 = *ptr;
          DREAM3D_REQUIRED(mudflap.ui32, ==, 0x00000000)
        }
        if (sizeof(T) == 8) {
          uint64_t* ptr = reinterpret_cast<uint64_t*>(cptr);
          mudflap.ui64 = *ptr;
          DREAM3D_REQUIRED(mudflap.ui64, ==, 0x0000000000000000)
        }

        array->clear();


        if (sizeof(T) == 1) {
          uint8_t* ptr = reinterpret_cast<uint8_t*>(cptr);
          mudflap.ui8 = *ptr;
          DREAM3D_REQUIRED(mudflap.ui8, ==, 0xAB)
        }
        if (sizeof(T) == 2) {
          uint16_t* ptr = reinterpret_cast<uint16_t*>(cptr);
          mudflap.ui16 = *ptr;
          DREAM3D_REQUIRED(mudflap.ui16, ==, 0xABAB)
        }
        if (sizeof(T) == 4) {
          uint32_t* ptr = reinterpret_cast<uint32_t*>(cptr);
          mudflap.ui32 = *ptr;
          DREAM3D_REQUIRED(mudflap.ui32, ==, 0xABABABAB)
        }
        if (sizeof(T) == 8) {
          uint64_t* ptr = reinterpret_cast<uint64_t*>(cptr);
          mudflap.ui64 = *ptr;
          DREAM3D_REQUIRED(mudflap.ui64, ==, 0xABABABABABABABAB)
        }
      }
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestArrayCreation()
  {
    TestArrayCreationForType<int8_t>();
    TestArrayCreationForType<uint8_t>();
    TestArrayCreationForType<int16_t>();
    TestArrayCreationForType<uint16_t>();
    TestArrayCreationForType<int32_t>();
    TestArrayCreationForType<uint32_t>();
    TestArrayCreationForType<int64_t>();
    TestArrayCreationForType<uint64_t>();
    TestArrayCreationForType<float>();
    TestArrayCreationForType<double>();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void TestDeepCopyDataArrayForType()
  {
    size_t numTuples = 10;
    QVector<size_t> cDims(1, 5);
    QString name("Source Array");

    // First lets try it without allocating any memory
    typename DataArray<T>::Pointer src = DataArray<T>::CreateArray(numTuples, cDims, name, false);

    typename DataArray<T>::Pointer copy = std::dynamic_pointer_cast<DataArray<T>>(src->deepCopy());

    DREAM3D_REQUIRED(copy->getNumberOfTuples(), ==, src->getNumberOfTuples());
    DREAM3D_REQUIRED(copy->isAllocated(), ==, src->isAllocated());

    // Create the array again, this time allocating the data and putting in some known data
    src = DataArray<T>::CreateArray(numTuples, cDims, name, true);
    for(size_t i = 0; i < numTuples; i++)
    {
      for(size_t j = 0; j < cDims[0]; j++)
      {
        src->setComponent(i, j, static_cast<T>(i + j));
      }
    }
    copy = std::dynamic_pointer_cast<DataArray<T>>(src->deepCopy());
    for(size_t i = 0; i < numTuples; i++)
    {
      for(size_t j = 0; j < cDims[0]; j++)
      {
        src->setComponent(i, j, static_cast<T>(i + j));
        T cpy = copy->getComponent(i, j);
        T val = src->getComponent(i, j);
        DREAM3D_REQUIRE_EQUAL(cpy, val)
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestDeepCopyArray()
  {
    TestDeepCopyDataArrayForType<uint8_t>();
    TestDeepCopyDataArrayForType<int8_t>();
    TestDeepCopyDataArrayForType<uint16_t>();
    TestDeepCopyDataArrayForType<int16_t>();
    TestDeepCopyDataArrayForType<uint32_t>();
    TestDeepCopyDataArrayForType<int32_t>();
    TestDeepCopyDataArrayForType<uint64_t>();
    TestDeepCopyDataArrayForType<int64_t>();
    TestDeepCopyDataArrayForType<float>();
    TestDeepCopyDataArrayForType<double>();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void TestCopyDataForType()
  {
    size_t numTuples = 10;
    QVector<size_t> cDims(1, 5);
    QString name("Source Array");

    // First lets try it without allocating any memory
    typename DataArray<T>::Pointer src = DataArray<T>::CreateArray(numTuples, cDims, name, false);

    typename DataArray<T>::Pointer copy = DataArray<T>::CreateArray(numTuples, cDims, name, false);

    DREAM3D_REQUIRED(copy->getNumberOfTuples(), ==, src->getNumberOfTuples());
    DREAM3D_REQUIRED(copy->isAllocated(), ==, src->isAllocated());

    // Create the array again, this time allocating the data and putting in some known data
    src = DataArray<T>::CreateArray(numTuples, cDims, name, true);
    copy = DataArray<T>::CreateArray(numTuples, cDims, name, true);

    for(size_t i = 0; i < numTuples; i++)
    {
      for(size_t j = 0; j < cDims[0]; j++)
      {
        src->setComponent(i, j, static_cast<T>(i + j));
        copy->setComponent(i, j, static_cast<T>(i + j));
      }
    }
    // We should FAIL this test as we are going to be off the end of the array
    size_t copyOffset = numTuples;
    bool didCopy = src->copyFromArray(copyOffset, copy);
    DREAM3D_REQUIRE_EQUAL(didCopy, false);

    // Resize the DataArray to accomondate the true amount of data that we want (20 Tuples)
    src->resize(numTuples * 2);
    didCopy = src->copyFromArray(numTuples, copy);
    DREAM3D_REQUIRE_EQUAL(didCopy, true);

    copy = std::dynamic_pointer_cast<DataArray<T>>(src->deepCopy());
    for(size_t i = 0; i < numTuples; i++)
    {
      for(size_t j = 0; j < cDims[0]; j++)
      {
        src->setComponent(i, j, static_cast<T>(i + j));
        T cpy = src->getComponent(i + numTuples, j);
        T val = src->getComponent(i, j);
        DREAM3D_REQUIRE_EQUAL(cpy, val)
      }
    }

    // Create the array again to test copying in the middle of the array
    src = DataArray<T>::CreateArray(numTuples, cDims, name, true);
    typename DataArray<T>::Pointer dest = DataArray<T>::CreateArray(numTuples, cDims, name, true);

    for(size_t i = 0; i < numTuples; i++)
    {
      for(size_t j = 0; j < cDims[0]; j++)
      {
        src->setComponent(i, j, static_cast<T>(i + j));
        dest->setComponent(i, j, static_cast<T>(i + j));
      }
    }

    typename DataArray<T>::Pointer destCopy = std::dynamic_pointer_cast<DataArray<T>>(dest->deepCopy());

    didCopy = dest->copyFromArray(3, src, 5, 5);
    DREAM3D_REQUIRE_EQUAL(didCopy, true);

    // Check first 3 values
    for(size_t i = 0; i <= 2; i++)
    {
      for(size_t j = 0; j < cDims[0]; j++)
      {
        T destVal = dest->getComponent(i, j);
        T destCopyVal = destCopy->getComponent(i, j);
        DREAM3D_REQUIRE_EQUAL(destVal, destCopyVal)
      }
    }

    // Check next 5 values
    for(size_t i = 3; i <= 7; i++)
    {
      for(size_t j = 0; j < cDims[0]; j++)
      {
        T destVal = dest->getComponent(i, j);
        T srcVal = src->getComponent(i + 2, j);
        DREAM3D_REQUIRE_EQUAL(destVal, srcVal)
      }
    }

    // Check last 2 values
    for(size_t i = 8; i <= 9; i++)
    {
      for(size_t j = 0; j < cDims[0]; j++)
      {
        T destVal = dest->getComponent(i, j);
        T destCopyVal = destCopy->getComponent(i, j);
        DREAM3D_REQUIRE_EQUAL(destVal, destCopyVal)
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCopyData()
  {
    TestCopyDataForType<uint8_t>();
    TestCopyDataForType<int8_t>();
    TestCopyDataForType<uint16_t>();
    TestCopyDataForType<int16_t>();
    TestCopyDataForType<uint32_t>();
    TestCopyDataForType<int32_t>();
    TestCopyDataForType<uint64_t>();
    TestCopyDataForType<int64_t>();
    TestCopyDataForType<float>();
    TestCopyDataForType<double>();
  }

#define TEST_SIZE 1024

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void TestWrapPointerForType()
  {
    QVector<size_t> cDims = {1};
    T* ptr = new T[TEST_SIZE]; // Allocate on the heap
    {
      // Wrap the pointer, write some data to it, and then let the object go
      // out of scope. We should then be able to delete the memory without
      // any issues. Note that Windows did not complain about this. If we
      // claim ownership on the pointer but let the delete stand OS X will complain
      // to the point of not completing the test. Windows silently worked. Odd.
      typename DataArray<T>::Pointer dataPtr = DataArray<T>::WrapPointer(ptr, TEST_SIZE, cDims, "Wrapped Pointer", false);
      dataPtr->initializeWithZeros();
    }
    delete[] ptr;
    ptr = nullptr;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestWrapPointer()
  {
    TestWrapPointerForType<uint8_t>();
    TestWrapPointerForType<int8_t>();
    TestWrapPointerForType<uint16_t>();
    TestWrapPointerForType<int16_t>();
    TestWrapPointerForType<uint32_t>();
    TestWrapPointerForType<int32_t>();
    TestWrapPointerForType<uint64_t>();
    TestWrapPointerForType<int64_t>();
    TestWrapPointerForType<float>();
    TestWrapPointerForType<double>();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestPrintDataArray()
  {
    FloatArrayType::Pointer floats = FloatArrayType::CreateArray(1, "Test_Float", true);
    floats->setValue(0, SIMPLib::Constants::k_Pif);
    QString outStr;
    QTextStream out(&outStr);
    floats->printTuple(out, 0);

    QString matchString = "3.1415927";
    int32_t length = outStr.length();
    DREAM3D_REQUIRE_EQUAL(length, 9);
    int comp = matchString.compare(outStr);
    DREAM3D_REQUIRE_EQUAL(comp, 0);

    outStr.clear();
    DoubleArrayType::Pointer doubles = DoubleArrayType::CreateArray(1, "Test_Double", true);
    doubles->setValue(0, SIMPLib::Constants::k_Pi);
    doubles->printTuple(out, 0);
    matchString = "3.141592653589793";
    length = outStr.length();
    DREAM3D_REQUIRE_EQUAL(length, 17);
    comp = matchString.compare(outStr);
    DREAM3D_REQUIRE_EQUAL(comp, 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    QDir dir(UnitTest::DataArrayTest::TestDir);
    dir.mkpath(".");
    std::cout << "#### DataArrayTest Starting ####" << std::endl;
#if !REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif
    DREAM3D_REGISTER_TEST(TestCopyData())
    DREAM3D_REGISTER_TEST(TestTypeStrings())
    DREAM3D_REGISTER_TEST(TestArrayCreation())
    DREAM3D_REGISTER_TEST(TestDataArray())
    DREAM3D_REGISTER_TEST(TestEraseElements())
    DREAM3D_REGISTER_TEST(TestcopyTuples())
    DREAM3D_REGISTER_TEST(TestDeepCopyArray())
    DREAM3D_REGISTER_TEST(TestNeighborList())
    DREAM3D_REGISTER_TEST(TestWrapPointer())
    DREAM3D_REGISTER_TEST(TestPrintDataArray())

#if REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif
  }

private:
  DataArrayTest(const DataArrayTest&);  // Copy Constructor Not Implemented
  void operator=(const DataArrayTest&); // Move assignment Not Implemented
};

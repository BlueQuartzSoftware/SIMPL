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
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class SplitAttributeArrayTest
{

public:
	SplitAttributeArrayTest() {}
	virtual ~SplitAttributeArrayTest() {}

	// -----------------------------------------------------------------------------
	//
	// -----------------------------------------------------------------------------
	DataContainerArray::Pointer createDataContainerArray()
	{
		DataContainerArray::Pointer dca = DataContainerArray::New();
		DataContainer::Pointer dc = DataContainer::New("DataContainer");
		AttributeMatrix::Pointer am1 = AttributeMatrix::New(QVector<size_t>(1, 10), "AttributeMatrix", AttributeMatrix::Type::Any);

		UInt32ArrayType::Pointer mcArray1 = UInt32ArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array uint32_t");
		fillDataArray<uint32_t>(mcArray1);

//		BoolArrayType::Pointer mcArray2 = BoolArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array bool");
//		fillDataArray<bool>(mcArray2);

		UCharArrayType::Pointer mcArray3 = UCharArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array unsigned char");
		fillDataArray<unsigned char>(mcArray3);

		Int8ArrayType::Pointer mcArray4 = Int8ArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array int8_t");
		fillDataArray<int8_t>(mcArray4);

		UInt8ArrayType::Pointer mcArray5 = UInt8ArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array uint8_t");
		fillDataArray<uint8_t>(mcArray5);

		Int16ArrayType::Pointer mcArray6 = Int16ArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array int16_t");
		fillDataArray<int16_t>(mcArray6);

		UInt16ArrayType::Pointer mcArray7 = UInt16ArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array uint16_t");
		fillDataArray<uint16_t>(mcArray7);

		Int32ArrayType::Pointer mcArray8 = Int32ArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array int32_t");
		fillDataArray<int32_t>(mcArray8);

		Int64ArrayType::Pointer mcArray9 = Int64ArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array int64_t");
		fillDataArray<int64_t>(mcArray9);

		UInt64ArrayType::Pointer mcArray10 = UInt64ArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array uint64_t");
		fillDataArray<uint64_t>(mcArray10);

		FloatArrayType::Pointer mcArray11 = FloatArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array float");
		fillDataArray<float>(mcArray11);

		DoubleArrayType::Pointer mcArray12 = DoubleArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array double");
		fillDataArray<double>(mcArray12);

		SizeTArrayType::Pointer mcArray13 = SizeTArrayType::CreateArray(QVector<size_t>(1, 10), QVector<size_t>(1, 5), "MultiComponent Array size_t");
		fillDataArray<size_t>(mcArray13);
	
		am1->addAttributeArray("MultiComponent Array uint32_t", mcArray1);
		//am1->addAttributeArray("MultiComponent Array bool", mcArray2);
		am1->addAttributeArray("MultiComponent Array unsigned char", mcArray3);
		am1->addAttributeArray("MultiComponent Array int8_t", mcArray4);
		am1->addAttributeArray("MultiComponent Array uint8_t", mcArray5);
		am1->addAttributeArray("MultiComponent Array int16_t", mcArray6);
		am1->addAttributeArray("MultiComponent Array uint16_t", mcArray7);
		am1->addAttributeArray("MultiComponent Array int32_t", mcArray8);
		am1->addAttributeArray("MultiComponent Array int64_t", mcArray9);
		am1->addAttributeArray("MultiComponent Array uint64_t", mcArray10);
		am1->addAttributeArray("MultiComponent Array float", mcArray11);
		am1->addAttributeArray("MultiComponent Array double", mcArray12);
		am1->addAttributeArray("MultiComponent Array size_t", mcArray13);
		dc->addAttributeMatrix("AttributeMatrix", am1);
		dca->addDataContainer(dc);

		return dca;
	}

	template <typename T> void fillDataArray(IDataArray::Pointer inputArray)
	{
		typename DataArray<T>::Pointer daPtr = std::dynamic_pointer_cast<DataArray<T>>(inputArray);
		T num = 0;
		for (int i = 0; i < daPtr->getNumberOfTuples(); i++)
		{
			daPtr->setComponent(i, 0, num);
			num++;
			daPtr->setComponent(i, 1, num);
			num++;
			daPtr->setComponent(i, 2, num);
			num++;
			daPtr->setComponent(i, 3, num);
			num++;
			daPtr->setComponent(i, 4, num);
			num++;
		}
	}

	// -----------------------------------------------------------------------------
	//
	// -----------------------------------------------------------------------------
	void RemoveTestFiles()
	{
	}

	// -----------------------------------------------------------------------------
	//
	// -----------------------------------------------------------------------------
	int TestFilterAvailability()
	{
		// Now instantiate the SplitAttributeArrayTest Filter from the FilterManager
		QString filtName = "SplitAttributeArray";
		FilterManager* fm = FilterManager::Instance();
		IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
		if (nullptr == filterFactory.get())
		{
			std::stringstream ss;
			ss << "DREAM3D requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
			DREAM3D_TEST_THROW_EXCEPTION(ss.str())
		}
		return 0;
	}

	// -----------------------------------------------------------------------------
	//
	// -----------------------------------------------------------------------------
	int TestSplitAttributeArrayTest()
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		/* Please write SplitAttributeArrayTest test code here.
		*
		* Your IO test files are:
		* UnitTest::SplitAttributeArrayTest::TestFile1
		* UnitTest::SplitAttributeArrayTest::TestFile2
		*
		* SIMPLib provides some macros that will throw exceptions when a test fails
		* and thus report that during testing. These macros are located in the
		* SIMPLib/Common/UnitTestSupport.hpp file. Some examples are:
		*
		* SIMPLib_REQUIRE_EQUAL(foo, 0)
		* This means that if the variable foo is NOT equal to Zero then test will fail
		* and the current test will exit immediately. If there are more tests registered
		* with the SIMPLib_REGISTER_TEST() macro, the next test will execute. There are
		* lots of examples in the SIMPLib/Test folder to look at.
		*/
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


		DataContainerArray::Pointer dca = createDataContainerArray();
		FilterManager* fm = FilterManager::Instance();

		QString filtName = "SplitAttributeArray";
		IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
		if (nullptr != filterFactory.get())
		{
			AbstractFilter::Pointer filter = filterFactory->create();

			TestSplitByType<uint32_t>(filter, dca, "uint32_t");
			TestSplitByType<bool>(filter, dca, "bool");
			TestSplitByType<unsigned char>(filter, dca, "unsigned char");
			TestSplitByType<int8_t>(filter, dca, "int8_t");
			TestSplitByType<uint8_t>(filter, dca, "uint8_t");
			TestSplitByType<int16_t>(filter, dca, "int16_t");
			TestSplitByType<uint16_t>(filter, dca, "uint16_t");
			TestSplitByType<int32_t>(filter, dca, "int32_t");
			TestSplitByType<int64_t>(filter, dca, "int64_t");
			TestSplitByType<uint64_t>(filter, dca, "uint64_t");
			TestSplitByType<float>(filter, dca, "float");
			TestSplitByType<double>(filter, dca, "double");
			TestSplitByType<size_t>(filter, dca, "size_t");

		}


		return EXIT_SUCCESS;
	}

	template <typename T> 
	void TestSplitByType(AbstractFilter::Pointer filter, DataContainerArray::Pointer dca, QString dataType)
	{

		DataArrayPath arrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType);
		int err = 0;
		bool propWasSet = false;
		QVariant var;

		filter->setDataContainerArray(dca);

		var.setValue(arrayPath);
		propWasSet = filter->setProperty("InputArrayPath", var);
		DREAM3D_REQUIRE_EQUAL(propWasSet, true)

		filter->preflight();
		err = filter->getErrorCondition();
		DREAM3D_REQUIRED(err, >= , 0)

		dca = createDataContainerArray();
		filter->setDataContainerArray(dca);
		filter->execute();
		err = filter->getErrorCondition();
		DREAM3D_REQUIRED(err, >= , 0)
    using DataArrayPtrType = std::shared_ptr<DataArray<T>>;
		DataArrayPtrType mcArray_original =
			filter->getDataContainerArray()->getPrereqIDataArrayFromPath<DataArray<T>, AbstractFilter>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType));

		std::vector<DataArrayPtrType> mcArraysAfterSplit;
		DataArrayPtrType mcArray0 =
			filter->getDataContainerArray()->getPrereqIDataArrayFromPath<DataArray<T>, AbstractFilter>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType + "Component0"));
		mcArraysAfterSplit.push_back(mcArray0);
		DataArrayPtrType mcArray1 =
			filter->getDataContainerArray()->getPrereqIDataArrayFromPath<DataArray<T>, AbstractFilter>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType + "Component1"));
		mcArraysAfterSplit.push_back(mcArray1);
		DataArrayPtrType mcArray2 =
			filter->getDataContainerArray()->getPrereqIDataArrayFromPath<DataArray<T>, AbstractFilter>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType + "Component2"));
		mcArraysAfterSplit.push_back(mcArray2);
		DataArrayPtrType mcArray3 =
			filter->getDataContainerArray()->getPrereqIDataArrayFromPath<DataArray<T>, AbstractFilter>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType + "Component3"));
		mcArraysAfterSplit.push_back(mcArray3);
		DataArrayPtrType mcArray4 =
			filter->getDataContainerArray()->getPrereqIDataArrayFromPath<DataArray<T>, AbstractFilter>(filter.get(), DataArrayPath("DataContainer", "AttributeMatrix", "MultiComponent Array " + dataType + "Component4"));
		mcArraysAfterSplit.push_back(mcArray4);


		size_t numTuples = mcArray_original->getNumberOfTuples();
		int32_t numComps = mcArray_original->getNumberOfComponents();
		for (int i = 0; i < numTuples; i++)
		{
			for (int j = 0; j < numComps; j++)
			{
				int originalValue = mcArray_original->getComponent(i, j);
				int afterSplitValue = mcArraysAfterSplit.at(j)->getComponent(i, 0);
				DREAM3D_REQUIRE_EQUAL(originalValue, afterSplitValue)
			}
		}
	}

	// -----------------------------------------------------------------------------
	//
	// -----------------------------------------------------------------------------
	void operator()()
	{
		int err = EXIT_SUCCESS;

		DREAM3D_REGISTER_TEST(TestFilterAvailability());

		DREAM3D_REGISTER_TEST(TestSplitAttributeArrayTest())

		DREAM3D_REGISTER_TEST(RemoveTestFiles())
	}

private:
	SplitAttributeArrayTest(const SplitAttributeArrayTest&); // Copy Constructor Not Implemented
	void operator=(const SplitAttributeArrayTest&); // Operator '=' Not Implemented


};

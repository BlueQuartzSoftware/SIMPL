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
#include <string>

#include <QtCore/QDebug>

#include "SIMPLib/Utilities/StringOperations.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

/**
 * @brief The StringOperationsTest class
 */
class StringOperationsTest
{
public:
  StringOperationsTest() = default;
  virtual ~StringOperationsTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestGenerateIndexString()
  {

    QString testString = StringOperations::GenerateIndexString(10, 0);
    DREAM3D_REQUIRE((testString.compare("10") == 0))

    testString = StringOperations::GenerateIndexString(1, 9);
    DREAM3D_REQUIRE((testString.compare("1") == 0))

    testString = StringOperations::GenerateIndexString(9, 10);
    DREAM3D_REQUIRE((testString.compare("09") == 0))

    testString = StringOperations::GenerateIndexString(10, 10);
    DREAM3D_REQUIRE((testString.compare("10") == 0))

    testString = StringOperations::GenerateIndexString(99, 99);
    DREAM3D_REQUIRE((testString.compare("99") == 0))

    testString = StringOperations::GenerateIndexString(99, 100);
    DREAM3D_REQUIRE((testString.compare("099") == 0))

    testString = StringOperations::GenerateIndexString(100, 100);
    DREAM3D_REQUIRE((testString.compare("100") == 0))

    testString = StringOperations::GenerateIndexString(101, 101);
    DREAM3D_REQUIRE((testString.compare("101") == 0))
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestGeneratePaddedString()
  {

    QString testString = StringOperations::GeneratePaddedString(10, 0, '0');
    DREAM3D_REQUIRE((testString.compare("10") == 0))

    testString = StringOperations::GeneratePaddedString(1, 4, '0');
    DREAM3D_REQUIRE((testString.compare("0001") == 0))

    testString = StringOperations::GeneratePaddedString(10, 4, '0');
    DREAM3D_REQUIRE((testString.compare("0010") == 0))

    testString = StringOperations::GeneratePaddedString(100, 4, '0');
    DREAM3D_REQUIRE((testString.compare("0100") == 0))

    testString = StringOperations::GeneratePaddedString(1000, 4, '0');
    DREAM3D_REQUIRE((testString.compare("1000") == 0))

    testString = StringOperations::GeneratePaddedString(10000, 4, '0');
    DREAM3D_REQUIRE((testString.compare("10000") == 0))
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### StringOperationsTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

#if !REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif
    DREAM3D_REGISTER_TEST(TestGenerateIndexString())

    DREAM3D_REGISTER_TEST(TestGeneratePaddedString())

#if REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif
  }

private:
  StringOperationsTest(const StringOperationsTest&); // Copy Constructor Not Implemented
  void operator=(const StringOperationsTest&);       // Move assignment Not Implemented
};

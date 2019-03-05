/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <iostream>

#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class AbstractVecTest
{
public:
  AbstractVecTest() = default;
  virtual ~AbstractVecTest() = default;

  AbstractVecTest(const AbstractVecTest&) = delete;            // Copy Constructor Not Implemented
  AbstractVecTest(AbstractVecTest&&) = delete;                 // Move Constructor Not Implemented
  AbstractVecTest& operator=(const AbstractVecTest&) = delete; // Copy Assignment Not Implemented
  AbstractVecTest& operator=(AbstractVecTest&&) = delete;      // Move Assignment Not Implemented
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES

#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestVec2()
  {
    FloatVec2Type f3(1.0f, 2.0f);

    std::array<int32_t, 2> i32Array = {1, 2};
    IntVec2Type i3(i32Array);

    std::tuple<int32_t, int32_t> tpl = std::make_tuple(1, 2);
    i3 = tpl;

    std::vector<int32_t> iv3 = {4, 5};
    i3 = iv3.data();

    int32_t x = i3.x();
    DREAM3D_REQUIRED(x, ==, 4)
    int32_t y = i3.y();
    DREAM3D_REQUIRED(y, ==, 5)

    tpl = i3.toTuple();
    DREAM3D_REQUIRED(std::get<0>(tpl), ==, 4)
    DREAM3D_REQUIRED(std::get<1>(tpl), ==, 5)

    DREAM3D_REQUIRED(i3.size(), ==, 2)
    DREAM3D_REQUIRED(i3[0], ==, 4)
    DREAM3D_REQUIRED(i3[1], ==, 5)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestVec3()
  {
    FloatVec3Type f3(1.0f, 2.0f, 3.0f);

    std::array<int32_t, 3> i32Array = {1, 2, 3};
    IntVec3Type i3(i32Array);

    std::tuple<int32_t, int32_t, int32_t> tpl = std::make_tuple(1, 2, 3);
    i3 = tpl;

    std::vector<int32_t> iv3 = {4, 5, 6};
    i3 = iv3.data();

    int32_t x = i3.x();
    DREAM3D_REQUIRED(x, ==, 4)
    int32_t y = i3.y();
    DREAM3D_REQUIRED(y, ==, 5)
    int32_t z = i3.z();
    DREAM3D_REQUIRED(z, ==, 6)

    tpl = i3.toTuple();
    DREAM3D_REQUIRED(std::get<0>(tpl), ==, 4)
    DREAM3D_REQUIRED(std::get<1>(tpl), ==, 5)
    DREAM3D_REQUIRED(std::get<2>(tpl), ==, 6)

    DREAM3D_REQUIRED(i3.size(), ==, 3)
    DREAM3D_REQUIRED(i3[0], ==, 4)
    DREAM3D_REQUIRED(i3[1], ==, 5)
    DREAM3D_REQUIRED(i3[2], ==, 6)
  }
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#### AbstractVecTest Starting ####" << std::endl;
#if !REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif
    DREAM3D_REGISTER_TEST(TestVec2())
    DREAM3D_REGISTER_TEST(TestVec3())

#if REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif
  }
};

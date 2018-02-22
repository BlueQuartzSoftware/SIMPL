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

#include <cmath>

#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/SIMPLib.h"

#ifdef SIMPL_BUILD_TEST_FILTERS
#include "SIMPLib/TestFilters/ArraySelectionExample.h"
#include "SIMPLib/TestFilters/GenericExample.h"
#include "SIMPLib/TestFilters/MakeDataContainer.h"
#include "SIMPLib/TestFilters/TestFilters.h"
#include "SIMPLib/TestFilters/ThresholdExample.h"
#endif

#include "SIMPLib/Utilities/FloatSummation.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class FloatSummationTest
{
public:
  FloatSummationTest()
  {
  }
  virtual ~FloatSummationTest()
  {
  }

  const size_t NUM_ITEMS = 10000;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  float CalculateStandardSummation(std::vector<float> values)
  {
    float sum = 0;
    for(std::vector<float>::size_type i = 0; i < values.size(); i++)
    {
      sum += values[i];
    }

    return sum;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool IsMoreAccurate(float value1, float value2, float target)
  {
    float diff1 = std::abs(target - value1);
    float diff2 = std::abs(target - value2);

    return diff1 < diff2;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  std::vector<float> GenerateValues()
  {
    std::vector<float> values(NUM_ITEMS);
    for(std::vector<float>::size_type i = 0; i < NUM_ITEMS; i++)
    {
      // Adding values in descending order increases the chance of error compared to adding in increasing order
      values.push_back(NUM_ITEMS - i);
    }

    return values;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int GetTargetValue()
  {
    return static_cast<int>(NUM_ITEMS * (NUM_ITEMS + 1) / 2);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestKahanAccuracy()
  {
    std::vector<float> values = GenerateValues();

    float stdSum = CalculateStandardSummation(values);
    float kahanSum = FloatSummation::Kahanf(values);

    int targetValue = GetTargetValue();

    if(stdSum != targetValue)
    {
      bool moreAccurate = IsMoreAccurate(kahanSum, stdSum, targetValue);
      if(false == moreAccurate)
      {
        DREAM3D_ASSERT(false)
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### FloatSummationTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestKahanAccuracy());
  }

private:
  FloatSummationTest(const FloatSummationTest&); // Copy Constructor Not Implemented
  void operator=(const FloatSummationTest&);     // Operator '=' Not Implemented
};

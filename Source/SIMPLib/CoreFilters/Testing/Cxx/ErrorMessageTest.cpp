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

#include <string>

#include "SIMPLib/Messages/FilterErrorMessage.h"
#include "SIMPLib/Messages/FilterProgressMessage.h"
#include "SIMPLib/Messages/FilterStatusMessage.h"
#include "SIMPLib/Messages/FilterWarningMessage.h"
#include "SIMPLib/Filtering/BadFilter.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class ErrorMessageTest
{
public:
  ErrorMessageTest() = default;
  virtual ~ErrorMessageTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestErrorMessage()
  {
    AbstractFilter::Pointer f = BadFilter::New();
    f->setErrorCondition(-10, "Description");

    FilterProgressMessage::Pointer progressMessage = FilterProgressMessage::New(f->getNameOfClass(), f->getHumanLabel(), -1, "Testing Warning Message...", -23);
    FilterErrorMessage::Pointer errorMessage = FilterErrorMessage::New(f->getNameOfClass(), f->getHumanLabel(), -1, "Testing Error Message...", -23);
    FilterStatusMessage::Pointer statusMessage = FilterStatusMessage::New(f->getNameOfClass(), f->getHumanLabel(), -1, "Testing Status Message...");
    FilterWarningMessage::Pointer warningMessage = FilterWarningMessage::New(f->getNameOfClass(), f->getHumanLabel(), -1, "Testing Status Message...", 23);
    emit f->messageGenerated(progressMessage);
    emit f->messageGenerated(errorMessage);
    emit f->messageGenerated(statusMessage);
    emit f->messageGenerated(warningMessage);

    if(true)
    {
      return;
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ErrorMessageTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;

#if !REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif
    DREAM3D_REGISTER_TEST(TestErrorMessage())

#if REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif
  }

private:
  ErrorMessageTest(const ErrorMessageTest&); // Copy Constructor Not Implemented
  void operator=(const ErrorMessageTest&);   // Move assignment Not Implemented
};

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
// int main(int argc, char** argv)
//{

//  PRINT_TEST_SUMMARY();
//  return err;
//}

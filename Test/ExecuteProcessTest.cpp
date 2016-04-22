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
#include <QtCore/QObject>

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

#include "SIMPLib/CoreFilters/ExecuteProcess.h"

#include "SIMPLTestFileLocations.h"

class ExecuteProcessTest
{

  public:
    ExecuteProcessTest() {}
    virtual ~ExecuteProcessTest() {}

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the ExecuteProcess Filter from the FilterManager
    QString filtName = "ExecuteProcess";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if (NULL == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The ExecuteProcessTest Requires the use of the " << filtName.toStdString() << " filter which is found in CoreFilters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestExecuteProcess()
  {
    ExecuteProcess::Pointer filter = ExecuteProcess::New();

    filter->setArguments("ls");
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0)

    filter->setArguments("ls -laF");
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0)

    filter->setArguments("cd ~/");
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0)

    filter->setArguments("lsz");
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -4005)

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST( TestFilterAvailability() );

    DREAM3D_REGISTER_TEST( TestExecuteProcess() )
  }

  private:

    ExecuteProcessTest(const ExecuteProcessTest&); // Copy Constructor Not Implemented
    void operator=(const ExecuteProcessTest&); // Operator '=' Not Implemented
};


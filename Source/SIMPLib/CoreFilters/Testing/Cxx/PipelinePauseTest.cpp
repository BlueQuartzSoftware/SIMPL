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
#include <QtCore/QMetaObject>
#include <QtCore/QThread>
#include <QtCore/QTimer>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"

#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

#include "SIMPLib/CoreFilters/Breakpoint.h"
#include "SIMPLib/TestFilters/ChangeGlobalValue.h"

int GlobalVariable = 0;

class PipelinePauseTest : public QObject
{
  Q_OBJECT

public:
  PipelinePauseTest()
  : m_WorkerThread(nullptr)
  {
  }

  virtual ~PipelinePauseTest()
  {
  }
  SIMPL_TYPE_MACRO(PipelinePauseTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  FilterPipeline::Pointer createPipeline()
  {
    FilterPipeline::Pointer pipeline = FilterPipeline::New();
    {
      ChangeGlobalValue::Pointer filter = ChangeGlobalValue::New();
      filter->setValue(3);
      filter->setGlobalValue(&GlobalVariable);
      pipeline->pushBack(filter);
    }
    {
      Breakpoint::Pointer filter = Breakpoint::New();
      connect(filter.get(), SIGNAL(pipelineHasPaused()), pipeline.get(), SIGNAL(pipelineHasPaused()));
      connect(pipeline.get(), SIGNAL(pipelineIsResuming()), filter.get(), SLOT(resumePipeline()));

      pipeline->pushBack(filter);
    }
    {
      ChangeGlobalValue::Pointer filter = ChangeGlobalValue::New();
      filter->setValue(7);
      filter->setGlobalValue(&GlobalVariable);
      pipeline->pushBack(filter);
    }

    return pipeline;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void PauseTest()
  {
    if(m_WorkerThread != nullptr)
    {
      m_WorkerThread->wait(); // Wait until the thread is complete
      if(m_WorkerThread->isFinished() == true)
      {
        delete m_WorkerThread;
        m_WorkerThread = nullptr;
      }
    }
    m_WorkerThread = new QThread(); // Create a new Thread Resource

    m_Pipeline = createPipeline();

    // When the thread starts its event loop, start the PipelineBuilder going
    connect(m_WorkerThread, SIGNAL(started()), m_Pipeline.get(), SLOT(run()));

    // When the pipeline pauses then tell this object that it has paused
    connect(m_Pipeline.get(), SIGNAL(pipelineHasPaused()), this, SLOT(PipelinePaused()));

    // When the pipeline pauses then tell this object that it has paused
    connect(this, SIGNAL(pipelineIsResuming()), m_Pipeline.get(), SIGNAL(pipelineIsResuming()), Qt::DirectConnection);

    // When the pipeline ends then tell the QThread to stop its event loop
    connect(m_Pipeline.get(), SIGNAL(pipelineFinished()), m_WorkerThread, SLOT(quit()));

    // When the QThread finishes, tell this object that it has finished.
    connect(m_WorkerThread, SIGNAL(finished()), this, SLOT(PipelineDidFinish()));

    m_Pipeline->moveToThread(m_WorkerThread);

    m_WorkerThread->start();
  }

public slots:
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void StartTest()
  {
    std::cout << "#### PipelinePauseTest Starting ####" << std::endl;
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(PauseTest())
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void PipelinePaused()
  {
    DREAM3D_REQUIRE_EQUAL(GlobalVariable, 3);
    std::cout << "GlobalVariable = 3" << std::endl;

    emit pipelineIsResuming();
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void PipelineDidFinish()
  {
    qApp->quit();
    DREAM3D_REQUIRE_EQUAL(GlobalVariable, 7);
    std::cout << "GlobalVariable = 7" << std::endl;
  }

signals:
  void pipelineIsResuming();
  void testFinished();

private:
  QThread* m_WorkerThread;
  FilterPipeline::Pointer m_Pipeline;

  PipelinePauseTest(const PipelinePauseTest&); // Copy Constructor Not Implemented
  void operator=(const PipelinePauseTest&);    // Operator '=' Not Implemented
};

Q_DECLARE_METATYPE(int*);

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("Your Company");
  QCoreApplication::setOrganizationDomain("Your Domain");
  QCoreApplication::setApplicationName("");
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();

  PipelinePauseTest* test = new PipelinePauseTest();
  QTimer::singleShot(500, test, SLOT(StartTest()));

  app.exec();

  PRINT_TEST_SUMMARY();

  return err;
}

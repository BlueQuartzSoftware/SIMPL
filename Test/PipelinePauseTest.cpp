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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include "SIMPLib/CoreFilters/Breakpoint.h"
#include "SIMPLib/TestFilters/ChangeGlobalValue.h"

#include "SIMPLTestFileLocations.h"

int GlobalVariable = 0;

class PipelinePauseTest : public QObject
{
  Q_OBJECT

public:
  PipelinePauseTest() :
    m_WorkerThread(NULL),
    m_IsPaused(false)
  {

  }

  virtual ~PipelinePauseTest() {}
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
      connect(filter.get(), SIGNAL(pipelineHasPaused()), pipeline.get(), SIGNAL(pipelineHasPaused()), Qt::DirectConnection);
      connect(pipeline.get(), SIGNAL(pipelineIsResuming()), filter.get(), SLOT(resumePipeline()), Qt::DirectConnection);

      pipeline->pushBack(filter);
    }
    {
      ChangeGlobalValue::Pointer filter = ChangeGlobalValue::New();
      filter->setValue(7);
      filter->setGlobalValue(&GlobalVariable);
      pipeline->pushBack(filter);
    }

    // When the thread starts its event loop, start the PipelineBuilder going
    connect(m_WorkerThread, SIGNAL(started()),
      pipeline.get(), SLOT(run()), Qt::DirectConnection);

    // When the pipeline pauses then tell this object that it has paused
    connect(pipeline.get(), SIGNAL(pipelineHasPaused()),
      this, SLOT(PipelinePaused()), Qt::DirectConnection);

    // When the pipeline pauses then tell this object that it has paused
    connect(this, SIGNAL(pipelineIsResuming()),
      pipeline.get(), SIGNAL(pipelineIsResuming()), Qt::DirectConnection);

    // When the pipeline ends then tell the QThread to stop its event loop
    connect(pipeline.get(), SIGNAL(pipelineFinished()),
      m_WorkerThread, SLOT(quit()), Qt::DirectConnection);

    return pipeline;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void PauseTest()
  {
    if (m_WorkerThread != NULL)
    {
      m_WorkerThread->wait(); // Wait until the thread is complete
      if (m_WorkerThread->isFinished() == true)
      {
        delete m_WorkerThread;
        m_WorkerThread = NULL;
      }
    }
    m_WorkerThread = new QThread(); // Create a new Thread Resource

    FilterPipeline::Pointer pipeline = createPipeline();

    // When the QThread finishes, tell this object that it has finished.
    connect(m_WorkerThread, SIGNAL(finished()),
      this, SLOT(PipelineDidFinish()), Qt::DirectConnection);

    pipeline->moveToThread(m_WorkerThread);

    m_WorkerThread->start();

    m_Mutex.lock();
    while (m_IsPaused == false)
    {
      m_WaitCondition.wait(&m_Mutex, 1000);
    }
    m_Mutex.unlock();

    DREAM3D_REQUIRE_EQUAL(GlobalVariable, 7);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### PipelinePauseTest Starting ####" << std::endl;
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(PauseTest())
  }

public slots:
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
    m_IsPaused = true;
    std::cout << "PipelineDidFinish()" << std::endl;

    m_WaitCondition.wakeAll();
  }

signals:
  void pipelineIsResuming();

private:
  QThread*                            m_WorkerThread;
  bool                                m_IsPaused;

  QWaitCondition                      m_WaitCondition;
  QMutex                              m_Mutex;

  PipelinePauseTest(const PipelinePauseTest&); // Copy Constructor Not Implemented
  void operator=(const PipelinePauseTest&); // Operator '=' Not Implemented
};

#include "moc_PipelinePauseTest.cpp"

Q_DECLARE_METATYPE(int*);


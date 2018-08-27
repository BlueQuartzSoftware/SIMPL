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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

#include "SIMPLib/CoreFilters/ExecuteProcess.h"

/**
* @brief
*/
class ExecuteProcessObserver : public QObject, public IObserver
{
  Q_OBJECT

public:
  ExecuteProcessObserver() = default;
  SIMPL_TYPE_MACRO_SUPER(ExecuteProcessObserver, IObserver)

  ~ExecuteProcessObserver() override = default;

  QString getStdOutput()
  {
    return m_StdOutput;
  }

  void clearStdOutput()
  {
    m_StdOutput.clear();
  }

public slots:
  void processPipelineMessage(const PipelineMessage& pm) override
  {
    if(pm.getType() == PipelineMessage::MessageType::StandardOutputMessage)
    {
      PipelineMessage msg = pm;
      QString str;
      QTextStream ss(&str);
      ss << msg.generateStandardOutputString();
      m_StdOutput.append(str);
    }
  }

private:
  QString m_StdOutput;

  ExecuteProcessObserver(const ExecuteProcessObserver&); // Copy Constructor Not Implemented
  void operator=(const ExecuteProcessObserver&);         // Move assignment Not Implemented
};

#include "ExecuteProcessTest.moc"

class ExecuteProcessTest
{

public:
  ExecuteProcessTest() = default;
  virtual ~ExecuteProcessTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the ExecuteProcess Filter from the FilterManager
    QString filtName = "ExecuteProcess";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
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
    {
      ExecuteProcess::Pointer filter = ExecuteProcess::New();
      ExecuteProcessObserver obs;

      QObject::connect(filter.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)), &obs, SLOT(processPipelineMessage(const PipelineMessage&)));

      filter->setArguments(QObject::tr("%1 -query QMAKE_VERSION").arg(UnitTest::ExecuteProcessTest::QMakeLocation));
      filter->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0)
      QString stdOutput = obs.getStdOutput();
#ifdef Q_OS_WIN
      stdOutput.remove('\r');
#endif
      stdOutput.remove('\n');
      QString versionString = UnitTest::ExecuteProcessTest::QMakeVersionString;
#ifdef Q_OS_WIN
      stdOutput.remove('\r');
#endif
      versionString.remove('\n');
      DREAM3D_REQUIRE_EQUAL(stdOutput, versionString)
    }

    //    {
    //      ExecuteProcess::Pointer filter = ExecuteProcess::New();
    //      ExecuteProcessObserver obs;

    //      QObject::connect(filter.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)), &obs, SLOT(processPipelineMessage(const PipelineMessage&)));

    //      filter->setArguments(QObject::tr("%1 -version").arg(UnitTest::ExecuteProcessTest::CMakeLocation));
    //      filter->execute();
    //      QString stdOutput = obs.getStdOutput();
    //      stdOutput.remove('\r');
    //      stdOutput.remove('\n');
    //      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0)
    //      QString versionString = UnitTest::ExecuteProcessTest::CMakeVersionString;
    //      versionString.remove('\r');
    //      versionString.remove('\n');
    //      DREAM3D_REQUIRE_EQUAL(stdOutput, versionString)
    //    }

    {
      ExecuteProcess::Pointer filter = ExecuteProcess::New();
      ExecuteProcessObserver obs;

      QObject::connect(filter.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)), &obs, SLOT(processPipelineMessage(const PipelineMessage&)));

      filter->setArguments("sdhsdrtfn");
      filter->execute();
      QString stdOutput = obs.getStdOutput();
      stdOutput.remove('\r');
      stdOutput.remove('\n');
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -4005)
    }

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestExecuteProcess())
  }

private:
  ExecuteProcessTest(const ExecuteProcessTest&); // Copy Constructor Not Implemented
  void operator=(const ExecuteProcessTest&);     // Move assignment Not Implemented
};

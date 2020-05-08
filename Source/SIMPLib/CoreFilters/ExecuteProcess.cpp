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
#include "ExecuteProcess.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExecuteProcess::ExecuteProcess() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExecuteProcess::~ExecuteProcess() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_STRING_FP("Command Line Arguments", Arguments, FilterParameter::Parameter, ExecuteProcess));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setArguments(reader->readString("Arguments", getArguments()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::initialize()
{
  m_Pause = false;
  m_ProcessPtr.reset();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  QStringList arguments = splitArgumentsString(m_Arguments);
  if(arguments.empty())
  {
    QString ss = QObject::tr("No command line arguments have been specified.");
    setErrorCondition(-4001, ss);
    return;
  }

  //  QString prog = arguments.at(0);

  //  QFileInfo fi(QDir::currentPath() + QDir::separator() + prog);
  //  if( !fi.exists())
  //  {
  //    QString ss = QObject::tr("The executable at %1 does not exist.").arg(prog);
  //    setErrorCondition(-4000, ss);
  //    return;
  //  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  QStringList arguments = splitArgumentsString(m_Arguments);
  QString command = arguments[0];

  arguments.removeAt(0);

  m_ProcessPtr = QSharedPointer<QProcess>(new QProcess(nullptr));
  qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
  qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
  connect(m_ProcessPtr.data(), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processHasFinished(int, QProcess::ExitStatus)), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(error(QProcess::ProcessError)), this, SLOT(processHasErroredOut(QProcess::ProcessError)), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(readyReadStandardError()), this, SLOT(sendErrorOutput()), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(readyReadStandardOutput()), this, SLOT(sendStandardOutput()), Qt::QueuedConnection);
  m_ProcessPtr->start(command, arguments);
  m_ProcessPtr->waitForStarted(2000);

  m_Mutex.lock();
  m_Pause = true;
  while(m_Pause)
  {
    m_WaitCondition.wait(&m_Mutex, 200);
    QCoreApplication::processEvents();

    if(getCancel())
    {
      m_ProcessPtr->kill(); // Kill the process;
      m_ProcessPtr->waitForFinished(10000);
    }
  }
  m_Mutex.unlock();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ExecuteProcess::splitArgumentsString(QString arguments)
{
  QStringList argumentList;
  for(int i = 0; i < m_Arguments.size(); i++)
  {
    if(m_Arguments[i] == '\"')
    {
      i++;
      int start = i;
      int index = m_Arguments.indexOf("\"", start);
      if(index == -1)
      {
        index = m_Arguments.size();
      }
      int end = index - 1;
      argumentList.push_back(m_Arguments.mid(start, end - start + 1));
      i = index;
    }
    else
    {
      int start = i;
      int index = m_Arguments.indexOf(" ", start + 1);
      if(index == -1)
      {
        index = m_Arguments.size();
      }
      int end = index - 1;
      argumentList.push_back(m_Arguments.mid(start, end - start + 1));
      i = index;
    }
  }

  return argumentList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::processHasFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  if(getCancel())
  {
  }
  else if(exitStatus == QProcess::CrashExit)
  {
    QString ss = QObject::tr("The process crashed during its exit.");
    setErrorCondition(-4003, ss);
  }
  else if(exitCode < 0)
  {
    QString ss = QObject::tr("The process finished with exit code %1.").arg(QString::number(exitCode));
    setErrorCondition(-4004, ss);
  }
  else if(getErrorCode() >= 0)
  {
  }

  m_Pause = false;
  m_WaitCondition.wakeAll();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::processHasErroredOut(QProcess::ProcessError error)
{
  if(getCancel())
  {
    QString ss = QObject::tr("The process was killed by the user.");
    setWarningCondition(-4004, ss);
  }
  else if(error == QProcess::FailedToStart)
  {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString pathEnv = env.value("PATH");

    QString ss = QObject::tr("The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program \
or the path containing the executble is not in the system's environment path. PATH=%1.\n Try using the absolute path to the executable.")
                     .arg(pathEnv);
    setErrorCondition(-4005, ss);
  }
  else if(error == QProcess::Crashed)
  {
    QString ss = QObject::tr("The process crashed some time after starting successfully.");
    setErrorCondition(-4006, ss);
  }
  else if(error == QProcess::Timedout)
  {
    QString ss = QObject::tr("The process timed out.");
    setErrorCondition(-4007, ss);
  }
  else if(error == QProcess::WriteError)
  {
    QString ss = QObject::tr("An error occurred when attempting to write to the process.");
    setErrorCondition(-4008, ss);
  }
  else if(error == QProcess::ReadError)
  {
    QString ss = QObject::tr("An error occurred when attempting to read from the process.");
    setErrorCondition(-4009, ss);
  }
  else
  {
    QString ss = QObject::tr("An unknown error occurred.");
    setErrorCondition(-4010, ss);
  }

  m_Pause = false;
  m_WaitCondition.wakeAll();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::sendErrorOutput()
{
  if(m_ProcessPtr.data() != nullptr)
  {
    QString error = m_ProcessPtr->readAllStandardError();
    if(error[error.size() - 1] == '\n')
    {
      error.chop(1);
    }
    notifyStatusMessage(error);
    m_WaitCondition.wakeAll();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::sendStandardOutput()
{
  if(m_ProcessPtr.data() != nullptr)
  {
    notifyStatusMessage(m_ProcessPtr->readAllStandardOutput());
    m_WaitCondition.wakeAll();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExecuteProcess::newFilterInstance(bool copyFilterParameters) const
{
  ExecuteProcess::Pointer filter = ExecuteProcess::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecuteProcess::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecuteProcess::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecuteProcess::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecuteProcess::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ExecuteProcess::getUuid() const
{
  return QUuid("{8a2308ec-86cd-5636-9a0a-6c7d383e9e7f}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecuteProcess::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecuteProcess::getHumanLabel() const
{
  return "Execute Process";
}

// -----------------------------------------------------------------------------
ExecuteProcess::Pointer ExecuteProcess::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ExecuteProcess> ExecuteProcess::New()
{
  struct make_shared_enabler : public ExecuteProcess
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ExecuteProcess::getNameOfClass() const
{
  return QString("ExecuteProcess");
}

// -----------------------------------------------------------------------------
QString ExecuteProcess::ClassName()
{
  return QString("ExecuteProcess");
}

// -----------------------------------------------------------------------------
void ExecuteProcess::setArguments(const QString& value)
{
  m_Arguments = value;
}

// -----------------------------------------------------------------------------
QString ExecuteProcess::getArguments() const
{
  return m_Arguments;
}

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

#include "Breakpoint.h"

#include <QtCore/QCoreApplication>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Breakpoint::Breakpoint() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Breakpoint::~Breakpoint() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Breakpoint::setupFilterParameters()
{
  FilterParameterVector parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Breakpoint::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Breakpoint::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Breakpoint::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Breakpoint::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Breakpoint::execute()
{
  pause();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Breakpoint::pause()
{
  // Pause the pipeline at this point until someone chooses to resume
  emit pipelineHasPaused();

  m_Mutex.lock();
  QString ss = "The pipeline is paused - Press \"Resume\" to continue execution.";
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  m_WaitCondition.wait(&m_Mutex);
  m_Mutex.unlock();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Breakpoint::resumePipeline()
{
  // Resume the pipeline
  m_WaitCondition.wakeAll();
  emit pipelineHasResumed();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer Breakpoint::newFilterInstance(bool copyFilterParameters) const
{
  Breakpoint::Pointer filter = Breakpoint::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Breakpoint::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Breakpoint::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Breakpoint::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Breakpoint::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid Breakpoint::getUuid()
{
  return QUuid("{a6d82abf-7043-51c0-88ed-b8d0153bf8ab}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Breakpoint::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Breakpoint::getHumanLabel() const
{
  return "Breakpoint";
}

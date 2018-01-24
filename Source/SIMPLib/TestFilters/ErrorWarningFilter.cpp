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
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ErrorWarningFilter.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErrorWarningFilter::ErrorWarningFilter()
: AbstractFilter()
, m_PreflightWarning(false)
, m_PreflightError(false)
, m_ExecuteWarning(false)
, m_ExecuteError(false)
, m_PropertyError(false)
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErrorWarningFilter::~ErrorWarningFilter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorWarningFilter::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorWarningFilter::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_BOOL_FP("Preflight Warning", PreflightWarning, FilterParameter::Parameter, ErrorWarningFilter));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Preflight Error", PreflightError, FilterParameter::Parameter, ErrorWarningFilter));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Execute Warning", ExecuteWarning, FilterParameter::Parameter, ErrorWarningFilter));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Execute Error", ExecuteError, FilterParameter::Parameter, ErrorWarningFilter));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Property Error", PropertyError, FilterParameter::Parameter, ErrorWarningFilter));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorWarningFilter::dataCheck()
{
  initialize();
  if(getPreflightWarning())
  {
    QString ss = QObject::tr("Intentional preflight warning generated");
    setWarningCondition(-666000);
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }
  if(getPreflightError())
  {
    QString ss = QObject::tr("Intentional preflight error generated");
    setErrorCondition(-666001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if(getPropertyError())
  {
    notifyMissingProperty(getFilterParameters()[4].get());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorWarningFilter::preflight()
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
void ErrorWarningFilter::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(getCancel() == true)
  {
    return;
  }

  if(getExecuteWarning())
  {
    QString ss = QObject::tr("Intentional execute warning generated");
    setWarningCondition(-666000);
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }
  if(getExecuteError())
  {
    QString ss = QObject::tr("Intentional execute error generated");
    setErrorCondition(-666001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ErrorWarningFilter::newFilterInstance(bool copyFilterParameters) const
{
  ErrorWarningFilter::Pointer filter = ErrorWarningFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErrorWarningFilter::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErrorWarningFilter::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErrorWarningFilter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErrorWarningFilter::getGroupName() const
{
  return SIMPL::FilterGroups::Generic;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ErrorWarningFilter::getUuid()
{
  return QUuid("{c367f998-3cc8-5a24-8616-70b659c5ce46}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErrorWarningFilter::getSubGroupName() const
{
  return "Test";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErrorWarningFilter::getHumanLabel() const
{
  return "Error Warning and Test Filter";
}

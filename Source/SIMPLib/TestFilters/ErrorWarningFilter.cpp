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

#include <memory>

#include "ErrorWarningFilter.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErrorWarningFilter::ErrorWarningFilter()
: m_PreflightWarning(false)
, m_PreflightError(false)
, m_ExecuteWarning(false)
, m_ExecuteError(false)
, m_PropertyError(false)
{
  initialize();
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
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorWarningFilter::setupFilterParameters()
{
  FilterParameterVectorType parameters;

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
    setWarningCondition(-666000, ss);
  }
  if(getPreflightError())
  {
    QString ss = QObject::tr("Intentional preflight error generated");
    setErrorCondition(-666001, ss);
  }
  if(getPropertyError())
  {
    notifyMissingProperty(getFilterParameters()[4].get());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorWarningFilter::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  if(getExecuteWarning())
  {
    QString ss = QObject::tr("Intentional execute warning generated");
    setWarningCondition(-666000, ss);
  }
  if(getExecuteError())
  {
    QString ss = QObject::tr("Intentional execute error generated");
    setErrorCondition(-666001, ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ErrorWarningFilter::newFilterInstance(bool copyFilterParameters) const
{
  ErrorWarningFilter::Pointer filter = ErrorWarningFilter::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErrorWarningFilter::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErrorWarningFilter::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErrorWarningFilter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErrorWarningFilter::getGroupName() const
{
  return SIMPL::FilterGroups::Generic;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ErrorWarningFilter::getUuid() const
{
  return QUuid("{c367f998-3cc8-5a24-8616-70b659c5ce46}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErrorWarningFilter::getSubGroupName() const
{
  return "Test";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErrorWarningFilter::getHumanLabel() const
{
  return "Error Warning and Test Filter";
}

// -----------------------------------------------------------------------------
ErrorWarningFilter::Pointer ErrorWarningFilter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ErrorWarningFilter> ErrorWarningFilter::New()
{
  struct make_shared_enabler : public ErrorWarningFilter
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ErrorWarningFilter::getNameOfClass() const
{
  return QString("ErrorWarningFilter");
}

// -----------------------------------------------------------------------------
QString ErrorWarningFilter::ClassName()
{
  return QString("ErrorWarningFilter");
}

// -----------------------------------------------------------------------------
void ErrorWarningFilter::setPreflightWarning(bool value)
{
  m_PreflightWarning = value;
}

// -----------------------------------------------------------------------------
bool ErrorWarningFilter::getPreflightWarning() const
{
  return m_PreflightWarning;
}

// -----------------------------------------------------------------------------
void ErrorWarningFilter::setPreflightError(bool value)
{
  m_PreflightError = value;
}

// -----------------------------------------------------------------------------
bool ErrorWarningFilter::getPreflightError() const
{
  return m_PreflightError;
}

// -----------------------------------------------------------------------------
void ErrorWarningFilter::setExecuteWarning(bool value)
{
  m_ExecuteWarning = value;
}

// -----------------------------------------------------------------------------
bool ErrorWarningFilter::getExecuteWarning() const
{
  return m_ExecuteWarning;
}

// -----------------------------------------------------------------------------
void ErrorWarningFilter::setExecuteError(bool value)
{
  m_ExecuteError = value;
}

// -----------------------------------------------------------------------------
bool ErrorWarningFilter::getExecuteError() const
{
  return m_ExecuteError;
}

// -----------------------------------------------------------------------------
void ErrorWarningFilter::setPropertyError(bool value)
{
  m_PropertyError = value;
}

// -----------------------------------------------------------------------------
bool ErrorWarningFilter::getPropertyError() const
{
  return m_PropertyError;
}

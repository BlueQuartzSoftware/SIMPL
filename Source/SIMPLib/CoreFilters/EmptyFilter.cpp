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
#include "EmptyFilter.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/UnknownFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EmptyFilter::EmptyFilter()
: m_HumanLabel("Unknown Filter")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EmptyFilter::~EmptyFilter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  QString str;
  QTextStream ss(&str);
  ss << "The original filter '<b><i>" << getOriginalFilterName() << "</i></b>' was not loaded. ";
  ss << "Either a plugin with the original filter was not loaded or the original ";
  ss << "filter has been renamed.";
  parameters.push_back(UnknownFilterParameter::New(str, "OriginalFilterName", getOriginalFilterName(), FilterParameter::Parameter));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOriginalFilterName(reader->readString("OriginalFilterName", getOriginalFilterName()));
  reader->closeFilterGroup();

  /* Specifically for this filter we need to reset the filter parameters so that
   * it picks up the Original Filter name. This is NOT normal operation for any
   * other filters. Your filter should not be doing this. If you are copy/pasting
   * from this filter. STOP. Pick another filter.
   */
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::initialize()
{
  m_HumanLabel = QString("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::dataCheck()
{
  QString ss = QObject::tr("This filter does nothing and was inserted as a place holder for filter '%1' that does not exist anymore.").arg(getOriginalFilterName());
  setErrorCondition(-9999, ss);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::execute()
{
  dataCheck();
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EmptyFilter::newFilterInstance(bool copyFilterParameters) const
{
  EmptyFilter::Pointer filter = EmptyFilter::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EmptyFilter::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EmptyFilter::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EmptyFilter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EmptyFilter::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid EmptyFilter::getUuid() const
{
  return QUuid("{0af0e81f-1f98-5734-a38a-74cfccd6e6f8}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EmptyFilter::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EmptyFilter::getHumanLabel() const
{
  if(m_HumanLabel.isEmpty())
  {
    return QString::fromLatin1("Unknown Filter");
  }

  return QString("Unknown Filter: %1").arg(m_OriginalFilterName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmptyFilter::setHumanLabel(const QString& humanLabel)
{
  m_HumanLabel = humanLabel;
}

// -----------------------------------------------------------------------------
EmptyFilter::Pointer EmptyFilter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<EmptyFilter> EmptyFilter::New()
{
  struct make_shared_enabler : public EmptyFilter
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString EmptyFilter::getNameOfClass() const
{
  return QString("EmptyFilter");
}

// -----------------------------------------------------------------------------
QString EmptyFilter::ClassName()
{
  return QString("EmptyFilter");
}

// -----------------------------------------------------------------------------
void EmptyFilter::setOriginalFilterName(const QString& value)
{
  m_OriginalFilterName = value;
}

// -----------------------------------------------------------------------------
QString EmptyFilter::getOriginalFilterName() const
{
  return m_OriginalFilterName;
}

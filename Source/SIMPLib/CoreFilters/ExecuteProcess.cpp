/*
 * Your License or Copyright can go here
 */

#include "ExecuteProcess.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

// Include the MOC generated file for this class
#include "moc_ExecuteProcess.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExecuteProcess::ExecuteProcess() :
  AbstractFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExecuteProcess::~ExecuteProcess()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::setupFilterParameters()
{
  FilterParameterVector parameters;

#if !defined (Q_OS_MAC)
  parameters.push_back(InputFileFilterParameter::New("Process Path", "ProcessPath", getProcessPath(), FilterParameter::Parameter, "*.exe"));
#else
  parameters.push_back(InputFileFilterParameter::New("Process File", "ProcessFile", getProcessFile(), FilterParameter::Parameter));
#endif

  parameters.push_back(StringFilterParameter::New("Command Line Arguments", "Arguments", getArguments(), FilterParameter::Parameter));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ExecuteProcess::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::dataCheck()
{
  setErrorCondition(0);

  QFileInfo fi(m_ProcessFile);
  if (fi.exists() == false)
  {
    QString ss = QObject::tr("The process file does not exist on the file system.");
    setErrorCondition(-4001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecuteProcess::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

//  if (getCancel() == true) { return; }

//  if (getErrorCondition() < 0)
//  {
//    QString ss = QObject::tr("Some error message");
//    setErrorCondition(-99999999);
//    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
//    return;
//  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExecuteProcess::newFilterInstance(bool copyFilterParameters)
{
  ExecuteProcess::Pointer filter = ExecuteProcess::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExecuteProcess::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExecuteProcess::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExecuteProcess::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExecuteProcess::getGroupName()
{ return SIMPL::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExecuteProcess::getSubGroupName()
{ return SIMPL::FilterSubGroups::MiscFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExecuteProcess::getHumanLabel()
{ return "Execute Process"; }


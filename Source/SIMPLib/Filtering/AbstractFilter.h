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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QUuid>

#include "SIMPLib/Common/Observable.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/SIMPLib.h"

class AbstractFilterParametersReader;
class ISIMPLibPlugin;

/**
 * @class AbstractFilter AbstractFilter.h DREAM3DLib/Common/AbstractFilter.h
 * @brief This class is the basic class to subclass when creating a new Filter for
 * SIMPLView. The subclass must implement at least the  execute method. If an
 * error occurs during the execution of the filter set the errorCondition to
 * a non zero value and optionally use the setErrorMessage() method to explain what the
 * error was. This class also inherits from Observable so that the filter can
 * provide updates to the user interface during execution.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Nov 28, 2011
 * @version 1.0
 */
class SIMPLib_EXPORT AbstractFilter : public Observable
{
  Q_OBJECT
  Q_PROPERTY(int PipelineIndex READ getPipelineIndex WRITE setPipelineIndex)
  Q_PROPERTY(QString GroupName READ getGroupName CONSTANT)
  Q_PROPERTY(QString SubGroupName READ getSubGroupName CONSTANT)
  Q_PROPERTY(QString HumanLabel READ getHumanLabel CONSTANT)
  Q_PROPERTY(QString FilterVersion READ getFilterVersion CONSTANT)
  Q_PROPERTY(QString CompiledLibraryName READ getCompiledLibraryName CONSTANT)
  Q_PROPERTY(int Cancel READ getCancel WRITE setCancel)
  Q_PROPERTY(bool Enabled READ getEnabled WRITE setEnabled)
  Q_PROPERTY(bool Removing READ getRemoving WRITE setRemoving)
  
  // This line MUST be first when exposing a class and properties to Python
  PYB11_CREATE_BINDINGS(AbstractFilter)
  PYB11_PROPERTY(QString NameOfClass READ getNameOfClass)  
  PYB11_PROPERTY(QString GroupName READ getGroupName)
  PYB11_PROPERTY(QString SubGroupName READ getSubGroupName)
  PYB11_PROPERTY(QString HumanLabel READ getHumanLabel)
  PYB11_PROPERTY(QString FilterVersion READ getFilterVersion)
  PYB11_PROPERTY(QString CompiledLibraryName READ getCompiledLibraryName)
  PYB11_PROPERTY(bool Cancel READ getCancel WRITE setCancel)
  PYB11_PROPERTY(bool Enabled READ getEnabled WRITE setEnabled)
  PYB11_PROPERTY(QString MessagePrefix READ getMessagePrefix WRITE setMessagePrefix)
  PYB11_PROPERTY(int ErrorCondition READ getErrorCondition WRITE setErrorCondition)
  PYB11_PROPERTY(int WarningCondition READ getWarningCondition WRITE setWarningCondition)
  PYB11_PROPERTY(bool InPreflight READ getInPreflight WRITE setInPreflight)
  PYB11_PROPERTY(int PipelineIndex READ getPipelineIndex WRITE setPipelineIndex)

  PYB11_METHOD(void generateHtmlSummary)
  PYB11_METHOD(void execute)
  PYB11_METHOD(void preflight)
  PYB11_METHOD(void setDataContainerArray)
  
public:
  SIMPL_SHARED_POINTERS(AbstractFilter)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(AbstractFilter, Observable)
  SIMPL_STATIC_NEW_MACRO(AbstractFilter)

  virtual ~AbstractFilter();

  /**
   * @brief CreateFilterFromClassName
   * @param className
   * @return
   */
  static AbstractFilter::Pointer CreateFilterFromClassName(const QString& className);

  /**
   * @brief getGroupName Returns the group name for the filter, which determines its
   * top level sorting in the GUI
   * @return Group name
   */
  virtual const QString getGroupName() const;

  /**
   * @brief getSubGroupName Returns the subgroup name for the filter, which determines its
   * second level sorting in the GUI
   * @return Subgroup name
   */
  virtual const QString getSubGroupName() const;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  virtual const QUuid getUuid();
  
  /**
   * @brief getHumanLabel Returns the human label for the filter, which determines its
   * primary labeling inthe GUI
   * @return Human lable
   */
  virtual const QString getHumanLabel() const;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with a specific plugin
   * @return Branding string
   */
  virtual const QString getBrandingString() const;

  /**
   * @brief getCompiledLibraryName Returns the library name for the filter, which is the
   * actual C++ library in which the filter resides; this is usually the filter's parent
   * plugin
   * @return Compiled library name
   */
  virtual const QString getCompiledLibraryName() const;

  /**
   * @brief generateHtmlSummary Generates a brief HTML summary of the filter
   * @return HTML summary
   */
  virtual const QString generateHtmlSummary() const;

  /**
   * @brief setupFilterParameters Instantiates the filter parameters that are allowed
   * to be modified by the user
   */
  virtual void setupFilterParameters();

  /**
  * @brief readFilterParameters Reads the filter parameters from a file
  * @param reader Reader that is used to read the parameters from a file
  */
  virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

  /**
  * @brief readFilterParametersFromJson Reads the filter parameters from a file
  * @param reader Reader that is used to read the parameters from a file
  */
  virtual void readFilterParameters(QJsonObject& obj);

  /**
   * @brief This method is called just before the writeFilterParameters() completes
  * @param obj The json object to add the filter parameters into
   * @param rootObject The parent object of **obj**
   */
  virtual void preWriteFilterParameters(QJsonObject& obj, QJsonObject& rootObject);

  /**
  * @brief writeFilterParametersToJson Writes the filter parameters to a file
  * @param obj The json object to add the filter parameters into
  */
  virtual void writeFilterParameters(QJsonObject& obj) const;

  /**
   * @brief This method is called just after the writeFilterParameters() completes
  * @param obj The json object to add the filter parameters into
   * @param rootObject The parent object of **obj**
   */
  virtual void postWriteFilterParameters(QJsonObject& obj, QJsonObject& rootObject);

  /**
  * @brief toJson Returns the filter and its parameters as a QJsonObject
  * @return
  */
  virtual QJsonObject toJson();

  /**
   * @brief execute Implements the main functionality of the filter
   */
  virtual void execute();

  /**
   * @brief preflight Communicates with the GUI to request user settings for the filter and
   * run any necessary sanity checks before execution
   */
  virtual void preflight();

  /**
   * @brief getPluginInstance Returns an instance of the filter's plugin
   * @return
   */
  ISIMPLibPlugin* getPluginInstance();

  /**
   * @brief newFilterInstance Instantiates a new instance of the filter
   * @param copyFilterParameters Whether to copy current filter parameters to the new instance
   * @return Shared pointer to the new filter instance
   */
  virtual Pointer newFilterInstance(bool copyFilterParameters) const;

  // ------------------------------
  // Standard methods for this class the are commonly used by subclasses.
  // ------------------------------
  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  virtual const QString getFilterVersion() const;

  SIMPL_INSTANCE_PROPERTY(DataContainerArray::Pointer, DataContainerArray)

  SIMPL_INSTANCE_PROPERTY(QVector<FilterParameter::Pointer>, FilterParameters)

  SIMPL_INSTANCE_PROPERTY(QString, MessagePrefix)

  SIMPL_INSTANCE_PROPERTY(int, ErrorCondition)

  SIMPL_INSTANCE_PROPERTY(int, WarningCondition)

  SIMPL_INSTANCE_PROPERTY(bool, InPreflight)

  SIMPL_INSTANCE_PROPERTY(bool, Enabled)

  SIMPL_INSTANCE_PROPERTY(bool, Removing)

  // ------------------------------
  // These functions allow interogating the position the filter is in the pipeline and the previous and next filters
  // ------------------------------

  /**
  * @brief This property tells which index the filter is in a pipeline
  */
  SIMPL_INSTANCE_PROPERTY(int, PipelineIndex)

  /**
  * @brief Returns the previous filter
  */
  SIMPL_INSTANCE_PROPERTY(AbstractFilter::WeakPointer, PreviousFilter)
  /**
  * @brief Returns the next filter in the pipeline
  */
  SIMPL_INSTANCE_PROPERTY(AbstractFilter::WeakPointer, NextFilter)

  /**
   * @brief doesPipelineContainFilterBeforeThis
   * @param name
   * @return
   */
  virtual bool doesPipelineContainFilterBeforeThis(const QString& name);

  /**
   * @brief doesPipelineContainFilterAfterThis
   * @param name
   * @return
   */
  virtual bool doesPipelineContainFilterAfterThis(const QString& name);

  // ------------------------------
  // These functions allow renaming DataArrayPaths to update proceeding filters in the pipeline
  // ------------------------------

  /**
   * @brief Returns a set of DataArrayPaths created by this filter.
   * This method requires preflight() or execute() to have already run.
   * @return
   */
  std::list<DataArrayPath> getCreatedPaths();

  /**
   * @brief Returns a list of DataArrayPaths that have been renamed along with their corresponding renamed value
   * @return
   */
  virtual DataArrayPath::RenameContainer getRenamedPaths();

  // ------------------------------
  // These methods are over ridden from the superclass in order to add the
  // pipeline index to the PipelineMessage Object.
  // ------------------------------

  /**
   * @brief notifyErrorMessage
   * @param humanLabel
   * @param ss
   * @param code
   */
  void notifyErrorMessage(const QString& humanLabel, const QString& ss, int code) override;

  /**
   * @brief notifyWarningMessage
   * @param humanLabel
   * @param ss
   * @param code
   */
  void notifyWarningMessage(const QString& humanLabel, const QString& ss, int code) override;

  /**
   * @brief notifyStatusMessage
   * @param humanLabel
   * @param ss
   */
  void notifyStatusMessage(const QString& humanLabel, const QString& ss) override;

  /**
   * @brief notifyStandardOutputMessage
   * @param humanLabel
   * @param pipelineIndex
   * @param ss
   */
  void notifyStandardOutputMessage(const QString& humanLabel, int pipelineIndex, const QString& ss) override;

  /**
   * @brief notifyStatusMessage
   * @param prefix
   * @param humanLabel
   * @param ss
   */
  void notifyStatusMessage(const QString& prefix, const QString& humanLabel, const QString& ss) override;

  /**
   * @brief notifyProgressMessage
   * @param prefix
   * @param humanLabel
   * @param str
   * @param progress
   */
  void notifyProgressMessage(const QString& prefix, const QString& humanLabel, const QString& str, int progress) override;

  /**
   * @brief notifyMissingProperty
   * @param filterParameter
  */
  void notifyMissingProperty(FilterParameter* filterParameter);

  //---------------
  // Other convenience methods
  // --------------
  /**
   * @brief printValues Optional method to print out values as the developer sees fit.
   * @param out
   */
  virtual void printValues(std::ostream& out) const;

  /**
   * @brief getCancel Returns if the filter has been cancelled.
   * @return
   */
  virtual bool getCancel();

  /**
   * @brief copyFilterParameterInstanceVariables
   * @param filter
   */
  virtual void copyFilterParameterInstanceVariables(AbstractFilter* filter) const;

signals:
  /**
   * @brief Signal is emitted when filter has completed the execute() method
   */
  void filterCompleted(AbstractFilter* filter);

  /**
   * @brief Signal is emitted when filter is executing
   */
  void filterInProgress(AbstractFilter* filter);

  /**
  * @brief Signal is emitted when a DataArrayPath property is updated
  * @param propertyName
  * @param renamePath
  */
  void dataArrayPathUpdated(QString propertyName, DataArrayPath::RenameType renamePath);

public slots:

  /**
    * @brief Cancel the operation
    */
  virtual void setCancel(bool value);

  /**
   * @brief Updates any DataArrayPath properties from the old path to a new path
   * For DataArrayPaths longer than the given path, only the specified values are modified
   * @param renamePath
   */
  virtual void renameDataArrayPath(DataArrayPath::RenameType renamePath);

  /**
  * @brief Updates any DataArrayPath properties from the old paths to their corresponding new paths.
  * For DataArrayPaths longer than the new path, only the values provided by the new path are modified
  * @param renamedPaths
  */
  virtual void renameDataArrayPaths(DataArrayPath::RenameContainer renamedPaths);

protected:
  AbstractFilter();

protected slots:
  /**
   * @brief This function will be called after the pipeline is completely done executing.  This can be reimplemented
   * in subclasses to provide cleanup code between pipeline executions. (resetting/clearing variables, updating settings, etc.)
   */
  virtual void cleanupFilter();

private:
  bool m_Cancel;
  QUuid m_Uuid;

  AbstractFilter(const AbstractFilter&) = delete; // Copy Constructor Not Implemented
  void operator=(const AbstractFilter&) = delete; // Move assignment Not Implemented
};


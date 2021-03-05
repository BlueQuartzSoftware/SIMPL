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

#include <map>
#include <memory>

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QUuid>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Observable.h"
#include "SIMPLib/DataContainers/RenameDataPath.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"

class AbstractFilterParametersReader;
class ISIMPLibPlugin;
class DataContainerArray;
using DataContainerArrayShPtrType = std::shared_ptr<DataContainerArray>;

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

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(AbstractFilter)
  PYB11_SHARED_POINTERS(AbstractFilter)
  PYB11_PROPERTY(QString NameOfClass READ getNameOfClass)
  PYB11_PROPERTY(QString GroupName READ getGroupName)
  PYB11_PROPERTY(QString SubGroupName READ getSubGroupName)
  PYB11_PROPERTY(QString HumanLabel READ getHumanLabel)
  PYB11_PROPERTY(QString FilterVersion READ getFilterVersion)
  PYB11_PROPERTY(QString CompiledLibraryName READ getCompiledLibraryName)
  PYB11_PROPERTY(bool Cancel READ getCancel WRITE setCancel)
  PYB11_PROPERTY(bool Enabled READ getEnabled WRITE setEnabled)
  PYB11_PROPERTY(int ErrorCode READ getErrorCode)
  PYB11_PROPERTY(int WarningCode READ getWarningCode)
  PYB11_PROPERTY(bool InPreflight READ getInPreflight WRITE setInPreflight)
  PYB11_PROPERTY(int PipelineIndex READ getPipelineIndex WRITE setPipelineIndex)
  PYB11_METHOD(void generateHtmlSummary)
  PYB11_METHOD(void execute)
  PYB11_METHOD(void preflight)
  PYB11_METHOD(void setDataContainerArray)
  PYB11_METHOD(void setErrorCondition ARGS code messageText)
  PYB11_METHOD(void setWarningCondition ARGS code messageText)
  PYB11_METHOD(void notifyStatusMessage ARGS messageText)
  PYB11_METHOD(void notifyProgressMessage ARGS progress messageText)
  PYB11_METHOD(void clearErrorCode)
  PYB11_METHOD(void clearWarningCode)
  PYB11_CUSTOM()
  PYB11_END_BINDINGS()
  // End Python bindings declarations

  // Friend declarations for RenameDataPath so that it can set and check the instance's created data by ID.
  friend void RenameDataPath::AlertFilterCreatedPath(AbstractFilter*, RenameDataPath::DataID_t, const DataArrayPath&);

public:
  using Self = AbstractFilter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for AbstractFilter
   */
  QString getNameOfClass() const override = 0;

  /**
   * @brief Returns the name of the class for AbstractFilter
   */
  static QString ClassName();

  ~AbstractFilter() override;

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
  virtual QString getGroupName() const;

  /**
   * @brief getSubGroupName Returns the subgroup name for the filter, which determines its
   * second level sorting in the GUI
   * @return Subgroup name
   */
  virtual QString getSubGroupName() const;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  virtual QUuid getUuid() const = 0;

  /**
   * @brief getHumanLabel Returns the human label for the filter, which determines its
   * primary labeling inthe GUI
   * @return Human lable
   */
  virtual QString getHumanLabel() const;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with a specific plugin
   * @return Branding string
   */
  virtual QString getBrandingString() const;

  /**
   * @brief getCompiledLibraryName Returns the library name for the filter, which is the
   * actual C++ library in which the filter resides; this is usually the filter's parent
   * plugin
   * @return Compiled library name
   */
  virtual QString getCompiledLibraryName() const;

  /**
   * @brief generateHtmlSummary Generates a brief HTML summary of the filter
   * @return HTML summary
   */
  virtual QString generateHtmlSummary() const;

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
  virtual void execute() = 0;

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
  virtual QString getFilterVersion() const;

  /**
   * @brief Setter property for DataContainerArray
   */
  void setDataContainerArray(const DataContainerArrayShPtrType& value);
  /**
   * @brief Getter property for DataContainerArray
   * @return Value of DataContainerArray
   */
  DataContainerArrayShPtrType getDataContainerArray() const;

  /**
   * @brief Setter property for FilterParameters
   */
  void setFilterParameters(const FilterParameterVectorType& value);
  /**
   * @brief Getter property for FilterParameters
   * @return Value of FilterParameters
   */
  FilterParameterVectorType getFilterParameters() const;

  /**
   * @brief Getter property for ErrorCode
   * @return Value of ErrorCode
   */
  int getErrorCode() const;

  /**
   * @brief Getter property for WarningCode
   * @return Value of WarningCode
   */
  int getWarningCode() const;

  /**
   * @brief Setter property for InPreflight
   */
  void setInPreflight(bool value);
  /**
   * @brief Getter property for InPreflight
   * @return Value of InPreflight
   */
  bool getInPreflight() const;

  /**
   * @brief Setter property for Enabled
   */
  void setEnabled(bool value);
  /**
   * @brief Getter property for Enabled
   * @return Value of Enabled
   */
  bool getEnabled() const;

  /**
   * @brief Setter property for Removing
   */
  void setRemoving(bool value);
  /**
   * @brief Getter property for Removing
   * @return Value of Removing
   */
  bool getRemoving() const;

  // ------------------------------
  // These functions allow interogating the position the filter is in the pipeline and the previous and next filters
  // ------------------------------

  /**
   * @brief This property tells which index the filter is in a pipeline
   */
  /**
   * @brief Setter property for PipelineIndex
   */
  void setPipelineIndex(int value);
  /**
   * @brief Getter property for PipelineIndex
   * @return Value of PipelineIndex
   */
  int getPipelineIndex() const;

  /**
   * @brief Returns the previous filter
   */
  /**
   * @brief Setter property for PreviousFilter
   */
  void setPreviousFilter(const AbstractFilter::WeakPointer& value);
  /**
   * @brief Getter property for PreviousFilter
   * @return Value of PreviousFilter
   */
  AbstractFilter::WeakPointer getPreviousFilter() const;

  /**
   * @brief Returns the next filter in the pipeline
   */
  /**
   * @brief Setter property for NextFilter
   */
  void setNextFilter(const AbstractFilter::WeakPointer& value);
  /**
   * @brief Getter property for NextFilter
   * @return Value of NextFilter
   */
  AbstractFilter::WeakPointer getNextFilter() const;

  /**
   * @brief clearErrorCondition
   */
  void clearErrorCode();

  /**
   * @brief clearWarningCondition
   */
  void clearWarningCode();

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
   * @brief Returns a list of DataArrayPaths created but not renamed by the filter.
   * @return
   */
  std::list<DataArrayPath> getCreatedPathsNotRenamed();

  /**
   * @brief Returns the list of deleted data paths.
   * @return
   */
  virtual std::list<DataArrayPath> getDeletedPaths();

  /**
   * @brief Returns a list of DataArrayPaths that have been renamed along with their corresponding renamed value
   * @return
   */
  virtual DataArrayPath::RenameContainer getRenamedPaths();

  /**
   * @brief setErrorCondition
   * @param code
   * @param messageText
   */
  void setErrorCondition(int code, const QString& messageText) override;

  /**
   * @brief setWarningCondition
   * @param code
   * @param messageText
   */
  void setWarningCondition(int code, const QString& messageText) override;

  /**
   * @brief notifyStatusMessage
   * @param messageText
   */
  void notifyStatusMessage(const QString& messageText) const override;

  /**
   * @brief notifyProgressMessage
   * @param progress
   * @param messageText
   */
  void notifyProgressMessage(int progress, const QString& messageText) const override;

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
  virtual bool getCancel() const;

  /**
   * @brief copyFilterParameterInstanceVariables
   * @param filter
   */
  virtual void copyFilterParameterInstanceVariables(AbstractFilter* filter) const;

  /**
   * @brief Clears the renamed paths for the filter instance.
   */
  void clearRenamedPaths();

Q_SIGNALS:
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
  void dataArrayPathUpdated(const QString& propertyName, const DataArrayPath::RenameType& renamePath);

  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

public Q_SLOTS:

  /**
   * @brief Cancel the operation
   */
  virtual void setCancel(bool value);

  /**
   * @brief Updates any DataArrayPath properties from the old path to a new path
   * For DataArrayPaths longer than the given path, only the specified values are modified
   * @param renamePath
   */
  virtual void renameDataArrayPath(const DataArrayPath::RenameType& renamePath);

  /**
   * @brief Updates any DataArrayPath properties from the old paths to their corresponding new paths.
   * For DataArrayPaths longer than the new path, only the values provided by the new path are modified
   * @param renamedPaths
   */
  inline void renameDataArrayPaths(const DataArrayPath::RenameContainer& renamedPaths)
  {
    for(const DataArrayPath::RenameType& rename : renamedPaths)
    {
      renameDataArrayPath(rename);
    }
  }

protected:
  AbstractFilter();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  virtual void dataCheck() = 0;

  /**
   * @brief Checks if the path matches the one saved with the specified ID.  Index 0 is used for
   * non-renamable DataArrayPaths, and any DataID 0 value will bypass the check and return false.
   * If the path does not match and the ID is already used, return true and update the path.  If
   * the ID is used and the paths match, return false.  If the ID has not been used, add the path
   * to the createdPaths map.
   */
  bool checkIfPathRenamed(RenameDataPath::DataID_t id, const DataArrayPath& path);

  /**
   * @brief Adds the specified change to the list of renamed DataArrayPaths.
   * @param oldPath
   * @param newPath
   */
  void addPathRename(const DataArrayPath& oldPath, const DataArrayPath& newPath);

protected Q_SLOTS:
  /**
   * @brief This function will be called after the pipeline is completely done executing.  This can be reimplemented
   * in subclasses to provide cleanup code between pipeline executions. (resetting/clearing variables, updating settings, etc.)
   */
  virtual void cleanupFilter();

private:
  DataContainerArrayShPtrType m_DataContainerArray = {};
  FilterParameterVectorType m_FilterParameters = {};
  bool m_InPreflight = {false};
  bool m_Enabled = {true};
  bool m_Removing = {false};
  int m_PipelineIndex = {0};
  AbstractFilter::WeakPointer m_PreviousFilter = {};
  AbstractFilter::WeakPointer m_NextFilter = {};

  bool m_Cancel = false;
  int m_ErrorCode = 0;
  int m_WarningCode = 0;

  std::map<RenameDataPath::DataID_t, DataArrayPath> m_CreatedPaths;
  DataArrayPath::RenameContainer m_RenamedPaths;

public:
  AbstractFilter(const AbstractFilter&) = delete;            // Copy Constructor Not Implemented
  AbstractFilter(AbstractFilter&&) = delete;                 // Move Constructor Not Implemented
  AbstractFilter& operator=(const AbstractFilter&) = delete; // Copy Assignment Not Implemented
  AbstractFilter& operator=(AbstractFilter&&) = delete;      // Move Assignment Not Implemented
};

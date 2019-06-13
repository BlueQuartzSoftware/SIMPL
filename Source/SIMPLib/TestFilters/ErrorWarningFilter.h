
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

#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

/**
 * @brief The ErrorWarningFilter class. See [Filter documentation](@ref ErrorWarningFilter) for details.
 */
class SIMPLib_EXPORT ErrorWarningFilter : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(ErrorWarningFilter SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(ErrorWarningFilter)
  PYB11_FILTER_NEW_MACRO(ErrorWarningFilter)
  PYB11_FILTER_PARAMETER(bool, PreflightWarning)
  PYB11_FILTER_PARAMETER(bool, PreflightError)
  PYB11_FILTER_PARAMETER(bool, ExecuteWarning)
  PYB11_FILTER_PARAMETER(bool, ExecuteError)
  PYB11_FILTER_PARAMETER(bool, PropertyError)
  PYB11_PROPERTY(bool PreflightWarning READ getPreflightWarning WRITE setPreflightWarning)
  PYB11_PROPERTY(bool PreflightError READ getPreflightError WRITE setPreflightError)
  PYB11_PROPERTY(bool ExecuteWarning READ getExecuteWarning WRITE setExecuteWarning)
  PYB11_PROPERTY(bool ExecuteError READ getExecuteError WRITE setExecuteError)
  PYB11_PROPERTY(bool PropertyError READ getPropertyError WRITE setPropertyError)
#endif

public:
  using Self = ErrorWarningFilter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer();

  static std::shared_ptr<ErrorWarningFilter> New();

  /**
   * @brief Returns the name of the class for ErrorWarningFilter
   */
  const QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ErrorWarningFilter
   */
  static QString ClassName();

  ~ErrorWarningFilter() override;

  /**
   * @brief Setter property for PreflightWarning
   */
  void setPreflightWarning(const bool& value);
  /**
   * @brief Getter property for PreflightWarning
   * @return Value of PreflightWarning
   */
  bool getPreflightWarning() const;

  Q_PROPERTY(bool PreflightWarning READ getPreflightWarning WRITE setPreflightWarning)

  /**
   * @brief Setter property for PreflightError
   */
  void setPreflightError(const bool& value);
  /**
   * @brief Getter property for PreflightError
   * @return Value of PreflightError
   */
  bool getPreflightError() const;

  Q_PROPERTY(bool PreflightError READ getPreflightError WRITE setPreflightError)

  /**
   * @brief Setter property for ExecuteWarning
   */
  void setExecuteWarning(const bool& value);
  /**
   * @brief Getter property for ExecuteWarning
   * @return Value of ExecuteWarning
   */
  bool getExecuteWarning() const;

  Q_PROPERTY(bool ExecuteWarning READ getExecuteWarning WRITE setExecuteWarning)

  /**
   * @brief Setter property for ExecuteError
   */
  void setExecuteError(const bool& value);
  /**
   * @brief Getter property for ExecuteError
   * @return Value of ExecuteError
   */
  bool getExecuteError() const;

  Q_PROPERTY(bool ExecuteError READ getExecuteError WRITE setExecuteError)

  /**
   * @brief Setter property for PropertyError
   */
  void setPropertyError(const bool& value);
  /**
   * @brief Getter property for PropertyError
   * @return Value of PropertyError
   */
  bool getPropertyError() const;

  Q_PROPERTY(bool PropertyError READ getPropertyError WRITE setPropertyError)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  void preflight() override;

signals:
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

protected:
  ErrorWarningFilter();

  /**
  * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
  */
  void dataCheck();

  /**
  * @brief Initializes all the private instance variables.
  */
  void initialize();

public:
  ErrorWarningFilter(const ErrorWarningFilter&) = delete; // Copy Constructor Not Implemented
  ErrorWarningFilter(ErrorWarningFilter&&) = delete;      // Move Constructor Not Implemented
  ErrorWarningFilter& operator=(const ErrorWarningFilter&) = delete; // Copy Assignment Not Implemented
  ErrorWarningFilter& operator=(ErrorWarningFilter&&) = delete;      // Move Assignment Not Implemented

private:
  bool m_PreflightWarning = {};
  bool m_PreflightError = {};
  bool m_ExecuteWarning = {};
  bool m_ExecuteError = {};
  bool m_PropertyError = {};
};


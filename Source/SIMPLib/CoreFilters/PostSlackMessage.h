/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#include <memory>

#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class QNetworkAccessManager;
class QNetworkReply;

/**
 * @brief The PostSlackMessage class. See [Filter documentation](@ref postslackmessage) for details.
 */
class SIMPLib_EXPORT PostSlackMessage : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(PostSlackMessage SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(PostSlackMessage)
  PYB11_FILTER_NEW_MACRO(PostSlackMessage)
  PYB11_FILTER_PARAMETER(QString, SlackUser)
  PYB11_FILTER_PARAMETER(QString, SlackUrl)
  PYB11_FILTER_PARAMETER(QString, SlackMessage)
  PYB11_FILTER_PARAMETER(bool, WarningsAsError)
  PYB11_PROPERTY(QString SlackUser READ getSlackUser WRITE setSlackUser)
  PYB11_PROPERTY(QString SlackUrl READ getSlackUrl WRITE setSlackUrl)
  PYB11_PROPERTY(QString SlackMessage READ getSlackMessage WRITE setSlackMessage)
  PYB11_PROPERTY(bool WarningsAsError READ getWarningsAsError WRITE setWarningsAsError)
#endif

public:
  using Self = PostSlackMessage;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  
  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for PostSlackMessage
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for PostSlackMessage
   */
  static QString ClassName();

  ~PostSlackMessage() override;

  /**
   * @brief Setter property for SlackUser
   */
  void setSlackUser(const QString& value);
  /**
   * @brief Getter property for SlackUser
   * @return Value of SlackUser
   */
  QString getSlackUser() const;

  Q_PROPERTY(QString SlackUser READ getSlackUser WRITE setSlackUser)

  /**
   * @brief Setter property for SlackUrl
   */
  void setSlackUrl(const QString& value);
  /**
   * @brief Getter property for SlackUrl
   * @return Value of SlackUrl
   */
  QString getSlackUrl() const;

  Q_PROPERTY(QString SlackUrl READ getSlackUrl WRITE setSlackUrl)

  /**
   * @brief Setter property for SlackMessage
   */
  void setSlackMessage(const QString& value);
  /**
   * @brief Getter property for SlackMessage
   * @return Value of SlackMessage
   */
  QString getSlackMessage() const;

  Q_PROPERTY(QString SlackMessage READ getSlackMessage WRITE setSlackMessage)

  /**
   * @brief Setter property for WarningsAsError
   */
  void setWarningsAsError(bool value);
  /**
   * @brief Getter property for WarningsAsError
   * @return Value of WarningsAsError
   */
  bool getWarningsAsError() const;

  Q_PROPERTY(bool WarningsAsError READ getWarningsAsError WRITE setWarningsAsError)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;


protected:
  PostSlackMessage();

  /**
  * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
  */
  void dataCheck() override;

  /**
  * @brief Initializes all the private instance variables.
  */
  void initialize();

private:
  QString m_SlackUser = {};
  QString m_SlackUrl = {};
  QString m_SlackMessage = {};
  bool m_WarningsAsError = {};

  QWaitCondition m_WaitCondition;
  QMutex m_Mutex;
  QNetworkAccessManager* m_NetworkManager;

  public:
    PostSlackMessage(const PostSlackMessage&) = delete; // Copy Constructor Not Implemented
    PostSlackMessage(PostSlackMessage&&) = delete;      // Move Constructor Not Implemented
    PostSlackMessage& operator=(const PostSlackMessage&) = delete; // Copy Assignment Not Implemented
    PostSlackMessage& operator=(PostSlackMessage&&) = delete;      // Move Assignment Not Implemented
};


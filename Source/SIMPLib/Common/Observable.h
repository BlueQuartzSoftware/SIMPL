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

#include <QtCore/QString>
#include <QtCore/QObject>

#include "SIMPLib/Messages/AbstractMessage.h"

/**
 * @class Observable Observable.h DREAM3D/Common/Observable.h
 * @brief
 *
 *
 * @date Sep 22, 2011
 * @version 1.0
 */
class SIMPLib_EXPORT Observable : public QObject
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(Observable)
  PYB11_METHOD(void setErrorCondition ARGS code messageText)
  PYB11_METHOD(void setErrorConditionWithPrefix ARGS code prefix messageText)
  PYB11_METHOD(void setWarningCondition ARGS code messageText)
  PYB11_METHOD(void setWarningConditionWithPrefix ARGS code prefix messageText)
  PYB11_METHOD(void notifyStatusMessage ARGS messageText)
  PYB11_METHOD(void notifyStatusMessageWithPrefix ARGS prefix messageText)
  PYB11_METHOD(void notifyProgressMessage ARGS progress messageText)
  PYB11_METHOD(void notifyProgressMessageWithPrefix ARGS progress prefix messageText)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  /**
   * @brief Returns the name of the class for Observable
   */
  virtual QString getNameOfClass() const;
  /**
   * @brief Returns the name of the class for Observable
   */
  static QString ClassName();

  Observable();

  Observable(const Observable& rhs);

  ~Observable() override;

  void operator=(const Observable&);

  // ------------------------------
  // These are convenience methods that construct a @see AbstractMessage object and then 'emit' that object
  // ------------------------------
  virtual void setErrorCondition(int code, const QString& messageText);

  void setErrorConditionWithPrefix(int code, const QString& prefix, const QString& messageText);

  virtual void setWarningCondition(int code, const QString& messageText);

  void setWarningConditionWithPrefix(int code, const QString& prefix, const QString& messageText);

  virtual void notifyStatusMessage(const QString& messageText) const;

  void notifyStatusMessageWithPrefix(const QString& prefix, const QString& messageText) const;

  virtual void notifyProgressMessage(int progress, const QString& messageText) const;

  void notifyProgressMessageWithPrefix(int progress, const QString& prefix, const QString& messageText) const;

signals:

  /**
   * @brief messageGenerated This is a Qt Signal that is used when the filter generates Errors, Warnings, Status and Progress Messages
   * @param msg
   */
  void messageGenerated(const AbstractMessage::Pointer& msg) const;

private:
};

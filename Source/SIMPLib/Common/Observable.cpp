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

#include "Observable.h"

#include "SIMPLib/Messages/GenericErrorMessage.h"
#include "SIMPLib/Messages/GenericProgressMessage.h"
#include "SIMPLib/Messages/GenericStatusMessage.h"
#include "SIMPLib/Messages/GenericWarningMessage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Observable::Observable()
: QObject(nullptr)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Observable::Observable(const Observable&)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Observable::~Observable() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::operator=(const Observable&)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::setErrorCondition(int code, const QString& messageText)
{
  GenericErrorMessage::Pointer pm = GenericErrorMessage::New(messageText, code);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::setErrorConditionWithPrefix(int code, const QString& prefix, const QString& messageText)
{
  QString msg = messageText;
  msg.prepend(tr("%1 - ").arg(prefix));

  setErrorCondition(code, msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::setWarningCondition(int code, const QString& messageText)
{
  GenericWarningMessage::Pointer pm = GenericWarningMessage::New(messageText, code);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::setWarningConditionWithPrefix(int code, const QString& prefix, const QString& messageText)
{
  QString msg = messageText;
  msg.prepend(tr("%1 - ").arg(prefix));

  setWarningCondition(code, msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::notifyStatusMessage(const QString& messageText) const
{
  GenericStatusMessage::Pointer pm = GenericStatusMessage::New(messageText);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::notifyStatusMessageWithPrefix(const QString& prefix, const QString& messageText) const
{
  QString msg = messageText;
  msg.prepend(tr("%1 - ").arg(prefix));

  notifyStatusMessage(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::notifyProgressMessage(int progress, const QString& messageText) const
{
  GenericProgressMessage::Pointer pm = GenericProgressMessage::New(messageText, progress);
  Q_EMIT messageGenerated(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::notifyProgressMessageWithPrefix(int progress, const QString& prefix, const QString& messageText) const
{
  QString msg = messageText;
  msg.prepend(tr("%1: ").arg(prefix));

  notifyProgressMessage(progress, msg);
}

// -----------------------------------------------------------------------------
QString Observable::getNameOfClass() const
{
  return QString("Observable");
}

// -----------------------------------------------------------------------------
QString Observable::ClassName()
{
  return QString("Observable");
}

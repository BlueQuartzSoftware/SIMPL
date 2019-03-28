/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "GenericWarningMessage.h"

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "SIMPLib/Messages/AbstractMessageHandler.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericWarningMessage::GenericWarningMessage()
: VisitableWarningMessage<GenericWarningMessage>()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericWarningMessage::GenericWarningMessage(const QString& prefix, const QString& msgText, int code)
: VisitableWarningMessage<GenericWarningMessage>(prefix, msgText, code)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericWarningMessage::~GenericWarningMessage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericWarningMessage::Pointer GenericWarningMessage::New(const QString& prefix, const QString& msgText, int code)
{
  GenericWarningMessage::Pointer shared_ptr (new GenericWarningMessage(prefix, msgText, code));
  return shared_ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenericWarningMessage::generateMessageString() const
{
  QString ss = QObject::tr("Error (%1): %2: %3").arg(getCode()).arg(getPrefix()).arg(getMessageText());
  return ss;
}

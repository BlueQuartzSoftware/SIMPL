/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PipelineWarningMessage.h"

#include <QtCore/QObject>

#include "AbstractMessageHandler.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineWarningMessage::PipelineWarningMessage()
: AbstractWarningMessage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineWarningMessage::PipelineWarningMessage(const QString& pipelineName, const QString& msgText, int code)
: AbstractWarningMessage(msgText, code)
, m_PipelineName(pipelineName)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineWarningMessage::~PipelineWarningMessage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineWarningMessage::Pointer PipelineWarningMessage::New(const QString& pipelineName, const QString& msgText, int code)
{
  PipelineWarningMessage::Pointer shared_ptr(new PipelineWarningMessage(pipelineName, msgText, code));
  return shared_ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineWarningMessage::generateMessageString() const
{
  QString ss = QObject::tr("Warning (%1): %2").arg(getCode()).arg(getMessageText());
  return ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineWarningMessage::visit(AbstractMessageHandler* msgHandler) const
{
  msgHandler->processMessage(this);
}

// -----------------------------------------------------------------------------
PipelineWarningMessage::Pointer PipelineWarningMessage::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
PipelineWarningMessage::Pointer PipelineWarningMessage::New()
{
  Pointer sharedPtr(new(PipelineWarningMessage));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString PipelineWarningMessage::getNameOfClass() const
{
  return QString("PipelineWarningMessage");
}

// -----------------------------------------------------------------------------
QString PipelineWarningMessage::ClassName()
{
  return QString("PipelineWarningMessage");
}

// -----------------------------------------------------------------------------
void PipelineWarningMessage::setPipelineName(const QString& value)
{
  m_PipelineName = value;
}

// -----------------------------------------------------------------------------
QString PipelineWarningMessage::getPipelineName() const
{
  return m_PipelineName;
}

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

#include "PipelineProgressMessage.h"

#include <QtCore/QObject>

#include "AbstractMessageHandler.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineProgressMessage::PipelineProgressMessage()
: AbstractProgressMessage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineProgressMessage::PipelineProgressMessage(const QString& pipelineName, const QString& msgText, int progress)
: AbstractProgressMessage(msgText, progress)
, m_PipelineName(pipelineName)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineProgressMessage::~PipelineProgressMessage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineProgressMessage::Pointer PipelineProgressMessage::New(const QString& pipelineName, const QString& msgText, int progress)
{
  PipelineProgressMessage::Pointer shared_ptr(new PipelineProgressMessage(pipelineName, msgText, progress));
  return shared_ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineProgressMessage::generateMessageString() const
{
  QString ss = QObject::tr("%1 %2%%").arg(getMessageText()).arg(getProgressValue());
  return ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineProgressMessage::visit(AbstractMessageHandler* msgHandler) const
{
  msgHandler->processMessage(this);
}

// -----------------------------------------------------------------------------
PipelineProgressMessage::Pointer PipelineProgressMessage::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
PipelineProgressMessage::Pointer PipelineProgressMessage::New()
{
  Pointer sharedPtr(new(PipelineProgressMessage));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString PipelineProgressMessage::getNameOfClass() const
{
  return QString("PipelineProgressMessage");
}

// -----------------------------------------------------------------------------
QString PipelineProgressMessage::ClassName()
{
  return QString("PipelineProgressMessage");
}

// -----------------------------------------------------------------------------
void PipelineProgressMessage::setPipelineName(const QString& value)
{
  m_PipelineName = value;
}

// -----------------------------------------------------------------------------
QString PipelineProgressMessage::getPipelineName() const
{
  return m_PipelineName;
}

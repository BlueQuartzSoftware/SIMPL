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

#include "FilterStatusMessage.h"

#include "AbstractMessageHandler.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterStatusMessage::FilterStatusMessage()
: AbstractStatusMessage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterStatusMessage::FilterStatusMessage(const QString& className, const QString& humanLabel, int pipelineIndex, const QString& msgText)
: AbstractStatusMessage(msgText)
, m_ClassName(className)
, m_HumanLabel(humanLabel)
, m_PipelineIndex(pipelineIndex)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterStatusMessage::~FilterStatusMessage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterStatusMessage::Pointer FilterStatusMessage::New(const QString& className, const QString& humanLabel, int pipelineIndex, const QString& msgText)
{
  FilterStatusMessage::Pointer shared_ptr(new FilterStatusMessage(className, humanLabel, pipelineIndex, msgText));
  return shared_ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterStatusMessage::generateMessageString() const
{
  return getMessageText();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterStatusMessage::visit(AbstractMessageHandler* msgHandler) const
{
  msgHandler->processMessage(this);
}

// -----------------------------------------------------------------------------
FilterStatusMessage::Pointer FilterStatusMessage::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
FilterStatusMessage::Pointer FilterStatusMessage::New()
{
  Pointer sharedPtr(new(FilterStatusMessage));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString FilterStatusMessage::getNameOfClass() const
{
  return QString("FilterStatusMessage");
}

// -----------------------------------------------------------------------------
QString FilterStatusMessage::ClassName()
{
  return QString("FilterStatusMessage");
}

// -----------------------------------------------------------------------------
void FilterStatusMessage::setClassName(const QString& value)
{
  m_ClassName = value;
}

// -----------------------------------------------------------------------------
QString FilterStatusMessage::getClassName() const
{
  return m_ClassName;
}

// -----------------------------------------------------------------------------
void FilterStatusMessage::setHumanLabel(const QString& value)
{
  m_HumanLabel = value;
}

// -----------------------------------------------------------------------------
QString FilterStatusMessage::getHumanLabel() const
{
  return m_HumanLabel;
}

// -----------------------------------------------------------------------------
void FilterStatusMessage::setPipelineIndex(int value)
{
  m_PipelineIndex = value;
}

// -----------------------------------------------------------------------------
int FilterStatusMessage::getPipelineIndex() const
{
  return m_PipelineIndex;
}

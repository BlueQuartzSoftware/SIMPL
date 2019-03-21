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
#include "FilterMessage.h"

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "SIMPLib/Common/IMessageHandler.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMessage::FilterMessage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMessage::FilterMessage(const QString& className, const char* msg, int code, MessageType msgType, int progress)
: AbstractMessage("", msg, code, msgType, progress)
, m_FilterClassName(className)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMessage::FilterMessage(const QString& className, const QString& msg, int code, MessageType msgType, int progress)
: AbstractMessage("", msg, code, msgType, progress)
, m_FilterClassName(className)
, m_FilterHumanLabel("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMessage::FilterMessage(const QString& className, const QString& humanLabel, const QString& msg, int code, MessageType msgType, int progress)
: AbstractMessage("", msg, code, msgType, progress)
, m_FilterClassName(className)
, m_FilterHumanLabel(humanLabel)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMessage::FilterMessage(const QString& humanLabel, int pipelineIndex, const QString& msg, MessageType msgType)
: AbstractMessage("", msg, 0, msgType)
, m_FilterHumanLabel(humanLabel)
, m_PipelineIndex(pipelineIndex)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMessage FilterMessage::CreateErrorMessage(const QString className, const QString humanLabel, const QString msg, int code)
{
  FilterMessage em(className, humanLabel, msg, code, MessageType::Error, -1);
  return em;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMessage FilterMessage::CreateStatusMessage(const QString className, const QString humanLabel, const QString msg)
{
  FilterMessage em(className, humanLabel, msg, 0, MessageType::StatusMessage, -1);
  return em;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMessage FilterMessage::CreateWarningMessage(const QString className, const QString humanLabel, const QString msg, int code)
{
  FilterMessage em(className, humanLabel, msg, code, MessageType::Warning, -1);
  return em;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMessage FilterMessage::CreateStandardOutputMessage(const QString humanLabel, int pipelineIndex, const QString msg)
{
  FilterMessage em(humanLabel, pipelineIndex, msg, MessageType::StandardOutputMessage);
  return em;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMessage::~FilterMessage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterMessage::operator==(const FilterMessage& rhs)
{
  return (m_FilterClassName == rhs.m_FilterClassName && getPrefix() == rhs.getPrefix() && m_FilterHumanLabel == rhs.m_FilterHumanLabel && getText() == rhs.getText() && getCode() == rhs.getCode() &&
          getType() == rhs.getType() && getProgressValue() == rhs.getProgressValue() && m_PipelineIndex == rhs.m_PipelineIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMessage::generateErrorString() const
{
  QString ss = QObject::tr("Error (%1): %2: %3").arg(getCode()).arg(getPrefix()).arg(getText());
  return ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMessage::generateWarningString() const
{
  QString ss = QObject::tr("Warning (%1): %2: %3").arg(getCode()).arg(getPrefix()).arg(getText());
  return ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMessage::generateStatusString() const
{
  if(getPrefix().isEmpty())
  {
    QString ss = QObject::tr("%2").arg(getText());
    return ss;
  }

  QString ss = QObject::tr("%1: %2").arg(getPrefix()).arg(getText());
  return ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMessage::generateProgressString() const
{
  if(getPrefix().isEmpty())
  {
    QString ss = QObject::tr("%1 %2%%").arg(getText()).arg(getProgressValue());
    return ss;
  }

  QString ss = QObject::tr("%1: %2 %3%%").arg(getPrefix()).arg(getText()).arg(getProgressValue());
  return ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMessage::generateStandardOutputString() const
{
  return getText();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMessage::visit(IMessageHandler* msgHandler)
{
  msgHandler->processMessage(*this);
}

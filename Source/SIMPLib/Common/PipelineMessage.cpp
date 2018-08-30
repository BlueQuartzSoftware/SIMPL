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
#include "PipelineMessage.h"

#include <QtCore/QMetaType>
#include <QtCore/QString>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineMessage::PipelineMessage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineMessage::PipelineMessage(const PipelineMessage& rhs)
{
  m_FilterClassName = rhs.m_FilterClassName;
  m_FilterHumanLabel = rhs.m_FilterHumanLabel;
  m_Prefix = rhs.m_Prefix;
  m_Text = rhs.m_Text;
  m_Code = rhs.m_Code;
  m_Type = rhs.m_Type;
  m_ProgressValue = rhs.m_ProgressValue;
  m_PipelineIndex = rhs.m_PipelineIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineMessage::PipelineMessage(const QString& className, const char* msg, int code, MessageType msgType, int progress)
: m_FilterClassName(className)
, m_Text(msg)
, m_Code(code)
, m_Type(msgType)
, m_ProgressValue(progress)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineMessage::PipelineMessage(const QString& className, const QString& msg, int code, MessageType msgType, int progress)
: m_FilterClassName(className)
, m_FilterHumanLabel("")
, m_Text(msg)
, m_Code(code)
, m_Type(msgType)
, m_ProgressValue(progress)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineMessage::PipelineMessage(const QString& className, const QString& humanLabel, const QString& msg, int code, MessageType msgType, int progress)
: m_FilterClassName(className)
, m_FilterHumanLabel(humanLabel)
, m_Text(msg)
, m_Code(code)
, m_Type(msgType)
, m_ProgressValue(progress)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineMessage::PipelineMessage(const QString& humanLabel, int pipelineIndex, const QString& msg, MessageType msgType)
: m_FilterHumanLabel(humanLabel)
, m_Text(msg)
, m_PipelineIndex(pipelineIndex)
, m_Type(msgType)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineMessage PipelineMessage::CreateErrorMessage(const QString className, const QString humanLabel, const QString msg, int code)
{
  PipelineMessage em(className, humanLabel, msg, code, MessageType::Error, -1);
  return em;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineMessage PipelineMessage::CreateStatusMessage(const QString className, const QString humanLabel, const QString msg)
{
  PipelineMessage em(className, humanLabel, msg, 0, MessageType::StatusMessage, -1);
  return em;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineMessage PipelineMessage::CreateWarningMessage(const QString className, const QString humanLabel, const QString msg, int code)
{
  PipelineMessage em(className, humanLabel, msg, code, MessageType::Warning, -1);
  return em;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineMessage PipelineMessage::CreateStandardOutputMessage(const QString humanLabel, int pipelineIndex, const QString msg)
{
  PipelineMessage em(humanLabel, pipelineIndex, msg, MessageType::StandardOutputMessage);
  return em;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineMessage::~PipelineMessage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineMessage::operator==(const PipelineMessage& rhs)
{
  return (m_FilterClassName == rhs.m_FilterClassName && m_Prefix == rhs.m_Prefix && m_FilterHumanLabel == rhs.m_FilterHumanLabel && m_Text == rhs.m_Text && m_Code == rhs.m_Code &&
          m_Type == rhs.m_Type && m_ProgressValue == rhs.m_ProgressValue && m_PipelineIndex == rhs.m_PipelineIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineMessage::operator=(const PipelineMessage& rhs)
{
  m_FilterClassName = rhs.m_FilterClassName;
  m_Prefix = rhs.m_Prefix;
  m_FilterHumanLabel = rhs.m_FilterHumanLabel;
  m_Text = rhs.m_Text;
  m_Code = rhs.m_Code;
  m_Type = rhs.m_Type;
  m_ProgressValue = rhs.m_ProgressValue;
  m_PipelineIndex = rhs.m_PipelineIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineMessage::generateErrorString() const
{
  QString ss = QObject::tr("Error (%1): %2: %3").arg(m_Code).arg(m_Prefix).arg(m_Text);
  return ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineMessage::generateWarningString() const
{
  QString ss = QObject::tr("Warning (%1): %2: %3").arg(m_Code).arg(m_Prefix).arg(m_Text);
  return ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineMessage::generateStatusString() const
{
  if(m_Prefix.isEmpty())
  {
    QString ss = QObject::tr("%2").arg(m_Text);
    return ss;
  }

  QString ss = QObject::tr("%1: %2").arg(m_Prefix).arg(m_Text);
  return ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineMessage::generateProgressString() const
{
  if(m_Prefix.isEmpty())
  {
    QString ss = QObject::tr("%1 %2%%").arg(m_Text).arg(m_ProgressValue);
    return ss;
  }

  QString ss = QObject::tr("%1: %2 %3%%").arg(m_Prefix).arg(m_Text).arg(m_ProgressValue);
  return ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineMessage::generateStandardOutputString() const
{
  return m_Text;
}

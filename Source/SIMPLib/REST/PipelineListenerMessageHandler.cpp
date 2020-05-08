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

#include "PipelineListenerMessageHandler.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Messages/GenericErrorMessage.h"
#include "SIMPLib/Messages/GenericWarningMessage.h"
#include "SIMPLib/Messages/GenericStatusMessage.h"
#include "SIMPLib/Messages/GenericProgressMessage.h"
#include "SIMPLib/Messages/PipelineErrorMessage.h"
#include "SIMPLib/Messages/PipelineWarningMessage.h"
#include "SIMPLib/Messages/PipelineStatusMessage.h"
#include "SIMPLib/Messages/PipelineProgressMessage.h"
#include "SIMPLib/Messages/FilterErrorMessage.h"
#include "SIMPLib/Messages/FilterWarningMessage.h"
#include "SIMPLib/Messages/FilterStatusMessage.h"
#include "SIMPLib/Messages/FilterProgressMessage.h"

#include "REST/PipelineListener.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineListenerMessageHandler::PipelineListenerMessageHandler(PipelineListener* listener)
: m_Listener(listener)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(const PipelineErrorMessage* msg) const
{
  QFile* log = m_Listener->m_ErrorLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_ErrorMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(const PipelineWarningMessage* msg) const
{
  QFile* log = m_Listener->m_WarningLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_WarningMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(const PipelineStatusMessage* msg) const
{
  QFile* log = m_Listener->m_StatusLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_StatusMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(const PipelineProgressMessage* msg) const
{
  m_Listener->m_ProgressMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(const FilterErrorMessage* msg) const
{
  QFile* log = m_Listener->m_ErrorLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_ErrorMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(const FilterWarningMessage* msg) const
{
  QFile* log = m_Listener->m_WarningLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_WarningMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(const FilterStatusMessage* msg) const
{
  QFile* log = m_Listener->m_StatusLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_StatusMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(const FilterProgressMessage* msg) const
{
  m_Listener->m_ProgressMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::streamToLog(const QString& msgString, QFile* log) const
{
  if(log && log->open(QIODevice::ReadWrite))
  {
    QTextStream stream(log);
    stream.readAll();
    stream << msgString << "\n";
    log->close();
  }
}

#include "PipelineListenerMessageHandler.h"

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
void PipelineListenerMessageHandler::processMessage(PipelineErrorMessage* msg) const
{
  QFile* log = m_Listener->m_ErrorLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_ErrorMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(PipelineWarningMessage* msg) const
{
  QFile* log = m_Listener->m_WarningLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_WarningMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(PipelineStatusMessage* msg) const
{
  QFile* log = m_Listener->m_StatusLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_StatusMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(PipelineProgressMessage* msg) const
{
  m_Listener->m_ProgressMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(FilterErrorMessage* msg) const
{
  QFile* log = m_Listener->m_ErrorLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_ErrorMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(FilterWarningMessage* msg) const
{
  QFile* log = m_Listener->m_WarningLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_WarningMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(FilterStatusMessage* msg) const
{
  QFile* log = m_Listener->m_StatusLog;
  QString msgString = msg->generateMessageString();
  streamToLog(msgString, log);

  m_Listener->m_StatusMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::processMessage(FilterProgressMessage* msg) const
{
  m_Listener->m_ProgressMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListenerMessageHandler::streamToLog(const QString &msgString, QFile* log) const
{
  if(log && log->open(QIODevice::ReadWrite))
  {
    QTextStream stream(log);
    stream.readAll();
    stream << msgString << endl;
    log->close();
  }
}

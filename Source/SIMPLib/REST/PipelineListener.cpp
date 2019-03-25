#include "PipelineListener.h"

#include "REST/PipelineListenerMessageHandler.h"

#include "SIMPLib/Messages/AbstractErrorMessage.h"
#include "SIMPLib/Messages/AbstractWarningMessage.h"
#include "SIMPLib/Messages/AbstractStatusMessage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineListener::PipelineListener(QObject* parent)
: QObject(parent)
, m_ErrorLog(nullptr)
, m_WarningLog(nullptr)
, m_StatusLog(nullptr)
, m_StandardOutputLog(nullptr)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineListener::~PipelineListener()
{
  closeFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<AbstractMessage*> PipelineListener::getMessages()
{
  return m_Messages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<AbstractErrorMessage*> PipelineListener::getErrorMessages()
{
  return m_ErrorMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<AbstractWarningMessage*> PipelineListener::getWarningMessages()
{
  return m_WarningMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<AbstractStatusMessage*> PipelineListener::getStatusMessages()
{
  return m_StatusMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<AbstractProgressMessage*> PipelineListener::getProgressMessages()
{
  return m_ProgressMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListener::processPipelineMessage(AbstractMessage::Pointer pm)
{
  PipelineListenerMessageHandler msgHandler(this);
  pm->visit(&msgHandler);

  m_SharedMessages.push_back(pm);
  m_Messages.push_back(pm.get());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineListener::getErrorLog()
{
  std::vector<AbstractErrorMessage*> messages = getErrorMessages();
  int count = messages.size();
  QString log;

  for(int i = 0; i < count; i++)
  {
    log += messages[i]->generateMessageString() + "\n";
  }

  return log;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineListener::getWarningLog()
{
  std::vector<AbstractWarningMessage*> messages = getWarningMessages();
  int count = messages.size();
  QString log;

  for(int i = 0; i < count; i++)
  {
    log += messages[i]->generateMessageString() + "\n";
  }

  return log;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineListener::getStatusLog()
{
  std::vector<AbstractStatusMessage*> messages = getStatusMessages();
  int count = messages.size();
  QString log;

  for(int i = 0; i < count; i++)
  {
    log += messages[i]->generateMessageString() + "\n";
  }

  return log;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListener::createErrorLogFile(QString path)
{
  if(m_ErrorLog)
  {
    m_ErrorLog->close();
    delete m_ErrorLog;
  }

  m_ErrorLog = new QFile(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListener::createWarningLogFile(QString path)
{
  if(m_WarningLog)
  {
    m_WarningLog->close();
    delete m_WarningLog;
  }

  m_WarningLog = new QFile(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListener::createStatusLogFile(QString path)
{
  if(m_StatusLog)
  {
    m_StatusLog->close();
    delete m_StatusLog;
  }

  m_StatusLog = new QFile(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListener::closeFiles()
{
  if(m_ErrorLog)
  {
    m_ErrorLog->close();
  }

  if(m_WarningLog)
  {
    m_WarningLog->close();
  }

  if(m_StatusLog)
  {
    m_StatusLog->close();
  }

  if(m_StandardOutputLog)
  {
    m_StandardOutputLog->close();
  }
}

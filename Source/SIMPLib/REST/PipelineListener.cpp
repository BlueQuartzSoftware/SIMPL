#include "PipelineListener.h"

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
std::vector<PipelineMessage> PipelineListener::getMessages()
{
  return m_Messages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<PipelineMessage> PipelineListener::getErrorMessages()
{
  std::vector<PipelineMessage> errorMessages;

  size_t numMessages = m_Messages.size();
  for(size_t i = 0; i < numMessages; i++)
  {
    if(m_Messages[i].getType() == PipelineMessage::MessageType::Error)
    {
      errorMessages.push_back(m_Messages[i]);
    }
  }

  return errorMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<PipelineMessage> PipelineListener::getWarningMessages()
{
  std::vector<PipelineMessage> warningMessages;

  size_t numMessages = m_Messages.size();
  for(size_t i = 0; i < numMessages; i++)
  {
    if(m_Messages[i].getType() == PipelineMessage::MessageType::Warning)
    {
      warningMessages.push_back(m_Messages[i]);
    }
  }

  return warningMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<PipelineMessage> PipelineListener::getStatusMessages()
{
  std::vector<PipelineMessage> statusMessages;

  size_t numMessages = m_Messages.size();
  for(size_t i = 0; i < numMessages; i++)
  {
    if(m_Messages[i].getType() == PipelineMessage::MessageType::StatusMessage)
    {
      statusMessages.push_back(m_Messages[i]);
    }
  }

  return statusMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<PipelineMessage> PipelineListener::getStandardOutputMessages()
{
  std::vector<PipelineMessage> stdOutMessages;

  size_t numMessages = m_Messages.size();
  for(size_t i = 0; i < numMessages; i++)
  {
    if(m_Messages[i].getType() == PipelineMessage::MessageType::StandardOutputMessage)
    {
      stdOutMessages.push_back(m_Messages[i]);
    }
  }

  return stdOutMessages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListener::processPipelineMessage(const PipelineMessage& pm)
{
  m_Messages.push_back(pm);

  switch(pm.getType())
  {
  case PipelineMessage::MessageType::Error:
    if(m_ErrorLog && m_ErrorLog->open(QIODevice::ReadWrite))
    {
      QTextStream stream(m_ErrorLog);
      stream.readAll();
      stream << pm.generateErrorString() << endl;
      m_ErrorLog->close();
    }
    break;
  case PipelineMessage::MessageType::Warning:
    if(m_WarningLog && m_WarningLog->open(QIODevice::ReadWrite))
    {
      QTextStream stream(m_WarningLog);
      stream.readAll();
      stream << pm.generateWarningString() << endl;
      m_WarningLog->close();
    }
    break;
  case PipelineMessage::MessageType::StatusMessage:
    if(m_StatusLog && m_StatusLog->open(QIODevice::ReadWrite))
    {
      QTextStream stream(m_StatusLog);
      stream.readAll();
      stream << pm.generateStatusString() << endl;
      m_StatusLog->close();
    }

    qDebug() << pm.getText();
    break;
  case PipelineMessage::MessageType::StandardOutputMessage:
    if(m_StandardOutputLog && m_StandardOutputLog->open(QIODevice::ReadWrite))
    {
      QTextStream stream(m_StandardOutputLog);
      stream.readAll();
      stream << pm.generateStandardOutputString() << endl;
      m_StandardOutputLog->close();
    }
    break;
  default:
    break;
  }

  if(pm.getType() == PipelineMessage::MessageType::StandardOutputMessage)
  {
    qDebug() << pm.getText();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineListener::getErrorLog()
{
  std::vector<PipelineMessage> messages = getErrorMessages();
  int count = messages.size();
  QString log;

  for(int i = 0; i < count; i++)
  {
    log += messages[i].generateErrorString() + "\n";
  }

  return log;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineListener::getWarningLog()
{
  std::vector<PipelineMessage> messages = getWarningMessages();
  int count = messages.size();
  QString log;

  for(int i = 0; i < count; i++)
  {
    log += messages[i].generateWarningString() + "\n";
  }

  return log;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineListener::getStatusLog()
{
  std::vector<PipelineMessage> messages = getStatusMessages();
  int count = messages.size();
  QString log;

  for(int i = 0; i < count; i++)
  {
    log += messages[i].generateStatusString() + "\n";
  }

  return log;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineListener::getStandardOutputLog()
{
  std::vector<PipelineMessage> messages = getStandardOutputMessages();
  int count = messages.size();
  QString log;

  for(int i = 0; i < count; i++)
  {
    log += messages[i].generateStandardOutputString() + "\n";
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
void PipelineListener::createStandardOutputLogFile(QString path)
{
  if(m_StandardOutputLog)
  {
    m_StandardOutputLog->close();
    delete m_StandardOutputLog;
  }

  m_StandardOutputLog = new QFile(path);
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

#pragma once

#include <vector>

#include <QtCore/QFile>

#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/Messages/AbstractMessageHandler.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

class PipelineListenerMessageHandler;

class AbstractErrorMessage;
class AbstractProgressMessage;
class AbstractStatusMessage;
class AbstractWarningMessage;

class SIMPLib_EXPORT PipelineListener : public QObject, public IObserver
{
  Q_OBJECT
public:
  SIMPL_TYPE_MACRO_SUPER(PipelineListener, IObserver)

  PipelineListener(QObject* parent);
  virtual ~PipelineListener();

  friend PipelineListenerMessageHandler;

  void createErrorLogFile(QString path);
  void createWarningLogFile(QString path);
  void createStatusLogFile(QString path);
  void closeFiles();

  std::vector<AbstractMessage*> getMessages();
  std::vector<AbstractErrorMessage*> getErrorMessages();
  std::vector<AbstractWarningMessage*> getWarningMessages();
  std::vector<AbstractStatusMessage*> getStatusMessages();
  std::vector<AbstractProgressMessage*> getProgressMessages();

  QString getErrorLog();
  QString getWarningLog();
  QString getStatusLog();

public slots:
  void processPipelineMessage(const AbstractMessage::Pointer& pm);

private:
  std::vector<AbstractMessage::Pointer> m_SharedMessages;
  std::vector<AbstractMessage*> m_Messages;
  std::vector<AbstractErrorMessage*> m_ErrorMessages;
  std::vector<AbstractStatusMessage*> m_StatusMessages;
  std::vector<AbstractProgressMessage*> m_ProgressMessages;
  std::vector<AbstractWarningMessage*> m_WarningMessages;

  QFile* m_ErrorLog;
  QFile* m_WarningLog;
  QFile* m_StatusLog;
  QFile* m_StandardOutputLog;
};


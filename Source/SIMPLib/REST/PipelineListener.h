#pragma once

#include <vector>

#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/Messages/AbstractMessageHandler.h"

class PipelineListenerMessageHandler;

class AbstractErrorMessage;
class AbstractProgressMessage;
class AbstractStatusMessage;
class AbstractWarningMessage;

class SIMPLib_EXPORT PipelineListener : public QObject, public IObserver
{
  Q_OBJECT
public:
  /**
   * @brief Returns the name of the class for PipelineListener
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for PipelineListener
   */
  static QString ClassName();

  PipelineListener(QObject* parent);
  virtual ~PipelineListener();

  friend PipelineListenerMessageHandler;

  void createErrorLogFile(QString path);
  void createWarningLogFile(QString path);
  void createStatusLogFile(QString path);
  void closeFiles();

  std::vector<const AbstractMessage*> getAllMessages();
  std::vector<const AbstractErrorMessage*> getErrorMessages();
  std::vector<const AbstractWarningMessage*> getWarningMessages();
  std::vector<const AbstractStatusMessage*> getStatusMessages();
  std::vector<const AbstractProgressMessage*> getProgressMessages();

  QString getErrorLog();
  QString getWarningLog();
  QString getStatusLog();

public Q_SLOTS:
  void processPipelineMessage(const AbstractMessage::Pointer& pm) override;

private:
  std::vector<AbstractMessage::Pointer> m_SharedMessages;
  std::vector<const AbstractMessage*> m_AllMessages;
  std::vector<const AbstractErrorMessage*> m_ErrorMessages;
  std::vector<const AbstractStatusMessage*> m_StatusMessages;
  std::vector<const AbstractProgressMessage*> m_ProgressMessages;
  std::vector<const AbstractWarningMessage*> m_WarningMessages;

  QFile* m_ErrorLog;
  QFile* m_WarningLog;
  QFile* m_StatusLog;
  QFile* m_StandardOutputLog;
};

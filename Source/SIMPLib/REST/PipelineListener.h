#pragma once

#include <vector>

#include <QtCore/QFile>

#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

class SIMPLib_EXPORT PipelineListener : public QObject, public IObserver
{
  Q_OBJECT
public:
  SIMPL_TYPE_MACRO_SUPER(PipelineListener, IObserver)

  PipelineListener(QObject* parent);
  virtual ~PipelineListener();

  void createErrorLogFile(QString path);
  void createWarningLogFile(QString path);
  void createStatusLogFile(QString path);
  void createStandardOutputLogFile(QString path);
  void closeFiles();

  std::vector<AbstractMessage> getMessages();
  std::vector<AbstractMessage> getErrorMessages();
  std::vector<AbstractMessage> getWarningMessages();
  std::vector<AbstractMessage> getStatusMessages();
  std::vector<AbstractMessage> getStandardOutputMessages();

  QString getErrorLog();
  QString getWarningLog();
  QString getStatusLog();
  QString getStandardOutputLog();

public slots:
  void processPipelineMessage(const AbstractMessage& pm);

private:
  std::vector<AbstractMessage> m_Messages;

  QFile* m_ErrorLog;
  QFile* m_WarningLog;
  QFile* m_StatusLog;
  QFile* m_StandardOutputLog;
};


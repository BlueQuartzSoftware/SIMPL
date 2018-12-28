#ifndef _pipelinelistener_h_
#define _pipelinelistener_h_

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

  std::vector<PipelineMessage> getMessages();
  std::vector<PipelineMessage> getErrorMessages();
  std::vector<PipelineMessage> getWarningMessages();
  std::vector<PipelineMessage> getStatusMessages();
  std::vector<PipelineMessage> getStandardOutputMessages();

  QString getErrorLog();
  QString getWarningLog();
  QString getStatusLog();
  QString getStandardOutputLog();

public slots:
  void processPipelineMessage(const PipelineMessage& pm);

private:
  std::vector<PipelineMessage> m_Messages;

  QFile* m_ErrorLog;
  QFile* m_WarningLog;
  QFile* m_StatusLog;
  QFile* m_StandardOutputLog;
};

#endif

#pragma once

#include "SIMPLib/Messages/AbstractMessageHandler.h"
#include "SIMPLib/SIMPLib.h"

class QFile;
class PipelineListener;

class SIMPLib_EXPORT PipelineListenerMessageHandler : public AbstractMessageHandler
{
  public:
    PipelineListenerMessageHandler(PipelineListener* listener);

    virtual void processMessage(PipelineErrorMessage* msg) const override;
    virtual void processMessage(PipelineProgressMessage* msg) const override;
    virtual void processMessage(PipelineStatusMessage* msg) const override;
    virtual void processMessage(PipelineWarningMessage* msg) const override;

    virtual void processMessage(FilterErrorMessage* msg) const override;
    virtual void processMessage(FilterProgressMessage* msg) const override;
    virtual void processMessage(FilterStatusMessage* msg) const override;
    virtual void processMessage(FilterWarningMessage* msg) const override;

  private:
    PipelineListener* m_Listener = nullptr;

    void streamToLog(const QString &msgString, QFile* log) const;
};

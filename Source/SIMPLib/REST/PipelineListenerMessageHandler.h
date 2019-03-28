#pragma once

#include "SIMPLib/Messages/AbstractMessageHandler.h"
#include "SIMPLib/SIMPLib.h"

class QFile;
class PipelineListener;

class SIMPLib_EXPORT PipelineListenerMessageHandler : public AbstractMessageHandler
{
  public:
    PipelineListenerMessageHandler(PipelineListener* listener);

    virtual void processMessage(const PipelineErrorMessage* msg) const override;
    virtual void processMessage(const PipelineProgressMessage* msg) const override;
    virtual void processMessage(const PipelineStatusMessage* msg) const override;
    virtual void processMessage(const PipelineWarningMessage* msg) const override;

    virtual void processMessage(const FilterErrorMessage* msg) const override;
    virtual void processMessage(const FilterProgressMessage* msg) const override;
    virtual void processMessage(const FilterStatusMessage* msg) const override;
    virtual void processMessage(const FilterWarningMessage* msg) const override;

  private:
    PipelineListener* m_Listener = nullptr;

    void streamToLog(const QString &msgString, QFile* log) const;
};

#pragma once

#include "SIMPLib/Messages/AbstractMessageHandler.h"
#include "SIMPLib/SIMPLib.h"

class QFile;
class PipelineListener;

/**
 * @brief This message handler is used by the PipelineListener class to stream incoming error, warning,
 * and status messages to the correct logs.  It also stores the incoming messages for later use.
 */
class SIMPLib_EXPORT PipelineListenerMessageHandler : public AbstractMessageHandler
{
  public:
    PipelineListenerMessageHandler(PipelineListener* listener);

    /**
     * @brief Streams incoming PipelineErrorMessages to the error log and stores the message
     * for later use.
     */
    virtual void processMessage(const PipelineErrorMessage* msg) const override;

    /**
     * @brief Stores the incoming PipelineProgressMessages for later use.
     */
    virtual void processMessage(const PipelineProgressMessage* msg) const override;

    /**
     * @brief Streams incoming PipelineStatusMessages to the status log and stores the message
     * for later use.
     */
    virtual void processMessage(const PipelineStatusMessage* msg) const override;

    /**
     * @brief Streams incoming PipelineWarningMessages to the warning log and stores the message
     * for later use.
     */
    virtual void processMessage(const PipelineWarningMessage* msg) const override;

    /**
     * @brief Streams incoming FilterErrorMessages to the error log and stores the message
     * for later use.
     */
    virtual void processMessage(const FilterErrorMessage* msg) const override;

    /**
     * @brief Stores the incoming FilterProgressMessages for later use.
     */
    virtual void processMessage(const FilterProgressMessage* msg) const override;

    /**
     * @brief Streams incoming FilterStatusMessages to the status log and stores the message
     * for later use.
     */
    virtual void processMessage(const FilterStatusMessage* msg) const override;

    /**
     * @brief Streams incoming FilterWarningMessages to the warning log and stores the message
     * for later use.
     */
    virtual void processMessage(const FilterWarningMessage* msg) const override;

  private:
    PipelineListener* m_Listener = nullptr;

    void streamToLog(const QString &msgString, QFile* log) const;
};

/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Messages/AbstractMessageHandler.h"

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

  void streamToLog(const QString& msgString, QFile* log) const;
};

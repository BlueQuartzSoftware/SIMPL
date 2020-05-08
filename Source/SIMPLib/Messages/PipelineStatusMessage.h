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

#include <memory>

#include "SIMPLib/Messages/AbstractStatusMessage.h"

/**
 * @class PipelineStatusMessage PipelineStatusMessage.h SIMPLib/Messages/PipelineStatusMessage.h
 * @brief This class is a pipeline status message class that is responsible for holding all the details
 * of a pipeline status message emitted by a FilterPipeline instance.
 */
class SIMPLib_EXPORT PipelineStatusMessage : public AbstractStatusMessage
{

public:
  using Self = PipelineStatusMessage;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for PipelineStatusMessage
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for PipelineStatusMessage
   */
  static QString ClassName();

  ~PipelineStatusMessage() override;

  /**
   * @brief Setter property for PipelineName
   */
  void setPipelineName(const QString& value);
  /**
   * @brief Getter property for PipelineName
   * @return Value of PipelineName
   */
  QString getPipelineName() const;

  /**
   * @brief New
   * @param humanLabel
   * @param pipelineIndex
   * @param msg
   * @return
   */
  static Pointer New(const QString& pipelineName, const QString& msgText);

  /**
   * @brief This method creates and returns a string for pipeline status messages
   */
  QString generateMessageString() const override;

  /**
   * @brief Method that allows the visitation of a message by a message handler.  This
   * is part of the double-dispatch API that allows observers to be able to perform
   * subclass specific operations on messages that they receive.
   * @param msgHandler The observer's message handler
   */
  void visit(AbstractMessageHandler* msgHandler) const override final;

protected:
  PipelineStatusMessage();
  PipelineStatusMessage(const QString& pipelineName, const QString& msgText);

private:
  QString m_PipelineName = {};
};
Q_DECLARE_METATYPE(PipelineStatusMessage::Pointer)

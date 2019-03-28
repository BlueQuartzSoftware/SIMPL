/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/Messages/VisitableProgressMessage.h"

/**
 * @class PipelineProgressMessage PipelineProgressMessage.h SIMPLib/Messages/PipelineProgressMessage.h
 * @brief This class is a pipeline progress message class that is responsible for holding all the details
 * of a pipeline progress message emitted by a FilterPipeline instance.
 */
class SIMPLib_EXPORT PipelineProgressMessage : public VisitableProgressMessage<PipelineProgressMessage>
{
  public:
    SIMPL_SHARED_POINTERS(PipelineProgressMessage)
    SIMPL_STATIC_NEW_MACRO(PipelineProgressMessage)
    SIMPL_TYPE_MACRO(PipelineProgressMessage)

    virtual ~PipelineProgressMessage();

    SIMPL_INSTANCE_STRING_PROPERTY(PipelineName)

    /**
     * @brief New
     * @param humanLabel
     * @param pipelineIndex
     * @param msg
     * @param progress
     * @return
     */
    static Pointer New(const QString &pipelineName, const QString& prefix, const QString& msgText, int progress);

    /**
     * @brief This method creates and returns a string for pipeline progress messages
     */
    virtual QString generateMessageString() const override;

  protected:
    PipelineProgressMessage();
    PipelineProgressMessage(const QString &pipelineName, const QString& prefix, const QString& msgText, int progress);

  private:

};
Q_DECLARE_METATYPE(PipelineProgressMessage::Pointer)



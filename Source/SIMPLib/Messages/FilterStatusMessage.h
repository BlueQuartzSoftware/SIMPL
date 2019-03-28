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

#include "SIMPLib/Messages/VisitableStatusMessage.h"

/**
 * @class FilterStatusMessage FilterStatusMessage.h SIMPLib/Messages/FilterStatusMessage.h
 * @brief This class is a filter status message class that is responsible for holding all the details
 * of a status message emitted by an AbstractFilter
 */
class SIMPLib_EXPORT FilterStatusMessage : public VisitableStatusMessage<FilterStatusMessage>
{
  public:
    SIMPL_SHARED_POINTERS(FilterStatusMessage)
    SIMPL_STATIC_NEW_MACRO(FilterStatusMessage)
    SIMPL_TYPE_MACRO(FilterStatusMessage)

    virtual ~FilterStatusMessage();

    SIMPL_INSTANCE_STRING_PROPERTY(ClassName)

    SIMPL_INSTANCE_STRING_PROPERTY(HumanLabel)

    SIMPL_INSTANCE_PROPERTY(int, PipelineIndex)

    /**
     * @brief New
     * @param className
     * @param humanLabel
     * @param pipelineIndex
     * @param msg
     * @return
     */
    static Pointer New(const QString& className, const QString& humanLabel, int pipelineIndex, const QString &prefix, const QString& msgText);

    /**
     * @brief This method creates and returns a string for filter status messages
     */
    virtual QString generateMessageString() const override;

  protected:
    FilterStatusMessage();
    FilterStatusMessage(const QString& className, const QString& humanLabel, int pipelineIndex, const QString &prefix, const QString& msgText);

  private:

};
Q_DECLARE_METATYPE(FilterStatusMessage::Pointer)



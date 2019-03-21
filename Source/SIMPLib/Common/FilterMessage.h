/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/Common/AbstractMessage.h"

class IMessageHandler;

/**
 * @class FilterMessage FilterMessage.h DREAM3DLib/Common/FilterMessage.h
 * @brief This class enables the creation of Error, Warning, and Status messages that
 * can be sent up from filters to the DREAM3D GUI.
 */
class SIMPLib_EXPORT FilterMessage : public AbstractMessage
{
  public:
    FilterMessage();

    FilterMessage(const QString& className, const char* msg, int code, MessageType msgType = MessageType::UnknownMessageType, int progress = -1);

    FilterMessage(const QString& className, const QString& msg, int code, MessageType msgType = MessageType::UnknownMessageType, int progress = -1);
    FilterMessage(const QString& className, const QString& humanLabel, const QString& msg, int code, MessageType msgType = MessageType::UnknownMessageType, int progress = -1);

    FilterMessage(const QString& humanLabel, int pipelineIndex, const QString& msg, MessageType msgType = MessageType::UnknownMessageType);

    static FilterMessage CreateErrorMessage(const QString className, const QString humanLabel, const QString msg, int code);

    static FilterMessage CreateStatusMessage(const QString className, const QString humanLabel, const QString msg);

    static FilterMessage CreateWarningMessage(const QString className, const QString humanLabel, const QString msg, int code);

    static FilterMessage CreateStandardOutputMessage(const QString humanLabel, int pipelineIndex, const QString msg);

    SIMPL_TYPE_MACRO(FilterMessage)

    virtual ~FilterMessage();

    bool operator==(const FilterMessage& rhs);

    SIMPL_INSTANCE_STRING_PROPERTY(FilterClassName)

    SIMPL_INSTANCE_STRING_PROPERTY(FilterHumanLabel)

    SIMPL_INSTANCE_PROPERTY(int, PipelineIndex)

    /**
     * @brief This method creates and returns a string for error messages
     */
    virtual QString generateErrorString() const override;

    /**
     * @brief This method creates and returns a string for warning messages
     */
    virtual QString generateWarningString() const override;

    /**
     * @brief This method creates and returns a string for status messages
     */
    virtual QString generateStatusString() const override;

    /**
     * @brief This method creates and returns a string for standard output messages
     */
    virtual QString generateStandardOutputString() const override;

    /**
     * @brief This method generates a status message that includes a progress value.
     * @return
     */
    virtual QString generateProgressString() const override;

    /**
     * @brief visit
     * @param msgHandler
     */
    virtual void visit(IMessageHandler* msgHandler) override;

  private:

};
Q_DECLARE_METATYPE(FilterMessage)



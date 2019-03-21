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

#include <QtCore/QString>
#include <QtCore/QMetaType>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

class IMessageHandler;

/**
 * @class AbstractMessage AbstractMessage.h DREAM3DLib/Common/AbstractMessage.h
 * @brief This class enables the creation of Error, Warning, and Status messages that
 * can be sent up from filters to the DREAM3D GUI.
 */
class SIMPLib_EXPORT AbstractMessage
{
  public:
    SIMPL_TYPE_MACRO(AbstractMessage)

    using EnumType = unsigned int;

    enum class MessageType : EnumType
    {
      Error = 0,
      Warning = 1,
      StatusMessage = 2,
      StandardOutputMessage = 3,
      ProgressValue = 4,
      StatusMessageAndProgressValue = 5,
      UnknownMessageType = 6
    };

    AbstractMessage();

    virtual ~AbstractMessage();

    SIMPL_INSTANCE_STRING_PROPERTY(Prefix)

    SIMPL_INSTANCE_STRING_PROPERTY(Text)

    SIMPL_INSTANCE_PROPERTY(int, Code)

    SIMPL_INSTANCE_PROPERTY(MessageType, Type)

    SIMPL_INSTANCE_PROPERTY(int, ProgressValue)

    /**
     * @brief This method creates and returns a string for error messages
     */
    virtual QString generateErrorString() const;

    /**
     * @brief This method creates and returns a string for warning messages
     */
    virtual QString generateWarningString() const;

    /**
     * @brief This method creates and returns a string for status messages
     */
    virtual QString generateStatusString() const;

    /**
     * @brief This method creates and returns a string for standard output messages
     */
    virtual QString generateStandardOutputString() const;

    /**
     * @brief This method generates a status message that includes a progress value.
     * @return
     */
    virtual QString generateProgressString() const;

    /**
     * @brief visit
     * @param msgHandler
     */
    virtual void visit(IMessageHandler* msgHandler);

  protected:
    AbstractMessage(const QString& prefix, const QString& msg, int code, MessageType msgType = MessageType::UnknownMessageType, int progress = -1);

  private:

};
Q_DECLARE_METATYPE(AbstractMessage)



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

#include "SIMPLib/Messages/AbstractMessage.h"

/**
 * @class AbstractErrorMessage AbstractErrorMessage.h SIMPLib/Messages/AbstractErrorMessage.h
 * @brief This class is an abstract error message superclass.
 */
class SIMPLib_EXPORT AbstractErrorMessage : public AbstractMessage
{
public:
  SIMPL_SHARED_POINTERS(AbstractErrorMessage)
  SIMPL_TYPE_MACRO(AbstractErrorMessage)

  virtual ~AbstractErrorMessage()
  {
  }

  SIMPL_INSTANCE_PROPERTY(int, Code)

  /**
   * @brief This method creates and returns a string for error messages
   */
  virtual QString generateMessageString() const = 0;

  /**
   * @brief Method that allows the visitation of a message by a message handler.  This
   * is part of the double-dispatch API that allows observers to be able to perform
   * subclass specific operations on messages that they receive.
   * @param msgHandler The observer's message handler
   */
  virtual void visit(AbstractMessageHandler* msgHandler) = 0;

protected:
  AbstractErrorMessage()
  : AbstractMessage()
  , m_Code(0)
  {
  }

  AbstractErrorMessage(const QString& prefix, const QString& msgText, int code)
  : AbstractMessage(prefix, msgText)
  , m_Code(code)
  {
  }

private:
};
Q_DECLARE_METATYPE(AbstractErrorMessage::Pointer)

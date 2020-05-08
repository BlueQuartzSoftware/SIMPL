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

#include "SIMPLib/Messages/AbstractWarningMessage.h"

/**
 * @class GenericWarningMessage GenericWarningMessage.h SIMPLib/Messages/GenericWarningMessage.h
 * @brief This class is a generic warning message class that is responsible for holding all the details
 * of a generic warning message emitted by any observable object
 */
class SIMPLib_EXPORT GenericWarningMessage : public AbstractWarningMessage
{

public:
  using Self = GenericWarningMessage;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for GenericWarningMessage
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GenericWarningMessage
   */
  static QString ClassName();

  ~GenericWarningMessage() override;

  /**
   * @brief New
   * @param humanLabel
   * @param msg
   * @param code
   * @return
   */
  static Pointer New(const QString& msgText, int code);

  /**
   * @brief This method creates and returns a string for generic warning messages
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
  GenericWarningMessage();
  GenericWarningMessage(const QString& msgText, int code);

private:
};
Q_DECLARE_METATYPE(GenericWarningMessage::Pointer)

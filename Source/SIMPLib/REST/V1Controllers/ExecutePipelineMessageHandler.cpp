/* ============================================================================
 * Copyright (c) 2017-2019 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ExecutePipelineMessageHandler.h"

#include "QtCore/QJsonArray"

#include "SIMPLib/Messages/FilterErrorMessage.h"
#include "SIMPLib/Messages/FilterWarningMessage.h"
#include "SIMPLib/Messages/PipelineErrorMessage.h"
#include "SIMPLib/Messages/PipelineWarningMessage.h"

#include "REST/V1Controllers/ExecutePipelineController.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExecutePipelineMessageHandler::ExecutePipelineMessageHandler(QJsonArray* errors, QJsonArray* warnings)
: m_Errors(errors)
, m_Warnings(warnings)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineMessageHandler::processMessage(const FilterErrorMessage* msg) const
{
  QJsonObject obj = writeToJson(msg->getCode(), msg->getMessageText());
  obj.insert(SIMPL::JSON::FilterIndex, msg->getPipelineIndex());
  obj.insert(SIMPL::JSON::FilterHumanLabel, msg->getHumanLabel());
  m_Errors->push_back(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineMessageHandler::processMessage(const FilterWarningMessage* msg) const
{
  QJsonObject obj = writeToJson(msg->getCode(), msg->getMessageText());
  obj.insert(SIMPL::JSON::FilterIndex, msg->getPipelineIndex());
  obj.insert(SIMPL::JSON::FilterHumanLabel, msg->getHumanLabel());
  m_Warnings->push_back(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineMessageHandler::processMessage(const PipelineErrorMessage* msg) const
{
  QJsonObject obj = writeToJson(msg->getCode(), msg->getMessageText());
  m_Errors->push_back(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipelineMessageHandler::processMessage(const PipelineWarningMessage* msg) const
{
  QJsonObject obj = writeToJson(msg->getCode(), msg->getMessageText());
  m_Warnings->push_back(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject ExecutePipelineMessageHandler::writeToJson(int code, const QString& msg) const
{
  QJsonObject obj;
  obj.insert(SIMPL::JSON::Code, code);
  obj.insert(SIMPL::JSON::Message, msg);
  return obj;
}

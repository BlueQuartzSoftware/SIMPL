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

class GenericErrorMessage;
class GenericProgressMessage;
class GenericStatusMessage;
class GenericWarningMessage;
class PipelineErrorMessage;
class PipelineProgressMessage;
class PipelineStatusMessage;
class PipelineWarningMessage;
class FilterErrorMessage;
class FilterProgressMessage;
class FilterStatusMessage;
class FilterWarningMessage;

/**
 * @class AbstractMessageHandler AbstractMessageHandler.h SIMPLib/Messages/AbstractMessageHandler.h
 * @brief This is the message handler superclass that is part of the double-dispatch API and enables
 * observers to process messages that they receive from observable objects
 */
class SIMPLib_EXPORT AbstractMessageHandler
{
public:
  virtual void processMessage(const GenericErrorMessage* msg) const;
  virtual void processMessage(const GenericProgressMessage* msg) const;
  virtual void processMessage(const GenericStatusMessage* msg) const;
  virtual void processMessage(const GenericWarningMessage* msg) const;

  virtual void processMessage(const PipelineErrorMessage* msg) const;
  virtual void processMessage(const PipelineProgressMessage* msg) const;
  virtual void processMessage(const PipelineStatusMessage* msg) const;
  virtual void processMessage(const PipelineWarningMessage* msg) const;

  virtual void processMessage(const FilterErrorMessage* msg) const;
  virtual void processMessage(const FilterProgressMessage* msg) const;
  virtual void processMessage(const FilterStatusMessage* msg) const;
  virtual void processMessage(const FilterWarningMessage* msg) const;

protected:
  AbstractMessageHandler();
};

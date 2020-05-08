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

#include "AbstractMessageHandler.h"

#include "SIMPLib/Messages/GenericErrorMessage.h"
#include "SIMPLib/Messages/GenericProgressMessage.h"
#include "SIMPLib/Messages/GenericStatusMessage.h"
#include "SIMPLib/Messages/GenericWarningMessage.h"
#include "SIMPLib/Messages/FilterErrorMessage.h"
#include "SIMPLib/Messages/FilterProgressMessage.h"
#include "SIMPLib/Messages/FilterStatusMessage.h"
#include "SIMPLib/Messages/FilterWarningMessage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractMessageHandler::AbstractMessageHandler() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const GenericErrorMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing generic error messages. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const GenericProgressMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing generic progress messages. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const GenericStatusMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing generic status messages. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const GenericWarningMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing generic warning messages. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const PipelineErrorMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing pipeline error messages. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const PipelineProgressMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing pipeline progress messages. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const PipelineStatusMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing pipeline status messages. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const PipelineWarningMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing pipeline warning messages. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const FilterErrorMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing filter error messages. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const FilterProgressMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing filter progress messages. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const FilterStatusMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing filter status messages. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMessageHandler::processMessage(const FilterWarningMessage* msg) const
{
  /* This is a default method that can be reimplemented in a subclass.  Subclassed message handlers
   * should reimplement this method if they care about processing filter warning messages. */
}

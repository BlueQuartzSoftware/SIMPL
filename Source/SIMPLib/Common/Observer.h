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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QObject>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/Messages/AbstractMessage.h"

/**
 * @class Observer Observer.h DREAM3D/Common/Observer.h
 * @brief This class implements the <b>Observer</b> pattern from the <b>Gang of
 * Four Design Patterns</b> book. There are various methods that are used for
 * notifications. The <b>Observable</b> will decide which method to call.
 *
 * @date September 22, 2011
 * @version 1.0
 */
class SIMPLib_EXPORT Observer : public QObject, public IObserver
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(Observer SUPERCLASS IObserver)
  PYB11_SHARED_POINTERS(Observer)
  PYB11_CREATION()
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  Observer();

  /**
   * @brief Returns the name of the class for Observer
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for Observer
   */
  static QString ClassName();

  ~Observer() override;

public Q_SLOTS:
  void processPipelineMessage(const AbstractMessage::Pointer& pm) override;

public:
  Observer(const Observer&) = delete;            // Copy Constructor Not Implemented
  Observer(Observer&&) = delete;                 // Move Constructor Not Implemented
  Observer& operator=(const Observer&) = delete; // Copy Assignment Not Implemented
  Observer& operator=(Observer&&) = delete;      // Move Assignment Not Implemented

private:
};

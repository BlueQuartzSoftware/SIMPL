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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/AbstractMessage.h"


/**
 * @class Observable Observable.h DREAM3D/Common/Observable.h
 * @brief
 *
 *
 * @date Sep 22, 2011
 * @version 1.0
 */
class SIMPLib_EXPORT Observable : public QObject
{
    Q_OBJECT
    PYB11_CREATE_BINDINGS(Observable)
    PYB11_METHOD(void notifyErrorMessage ARGS humanLabel ss code)

  public:
    SIMPL_TYPE_MACRO(Observable)

    Observable();

    Observable(const Observable& rhs);

    ~Observable() override;

    void operator=(const Observable&);

    // ------------------------------
    // These are convenience methods that construct a @see PipelineMessage object and then 'emit' that object
    // ------------------------------
    virtual void notifyErrorMessage(const QString &prefix, const QString& text, int code);

    virtual void notifyWarningMessage(const QString& text, int code);

    virtual void notifyStatusMessage(const QString& text);

    virtual void notifyStandardOutputMessage(const QString& text);

    virtual void notifyStatusMessage(const QString& prefix, const QString& text);

    virtual void notifyProgressMessage(const QString& prefix, const QString& text, int progress);

  public slots:

    /**
     * @brief This method will cause this object to 'emit' the messageGenerated() signal. This is useful if other
     * classes need the filter to emit an error or warning messge from a class that is not able to emit the proper signals
     * or the class is not connected to anything that would receive the signals
     * @param msg
     */
    void broadcastPipelineMessage(const AbstractMessage& msg);


  signals:

    /**
     * @brief messageGenerated This is a Qt Signal that is used when the filter generates Errors, Warnings, Status and Progress Messages
     * @param msg
     */
    void messageGenerated(const AbstractMessage& msg);
};



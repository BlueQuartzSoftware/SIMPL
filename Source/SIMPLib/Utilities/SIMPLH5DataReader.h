/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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

#ifndef _simplh5datareader_h_
#define _simplh5datareader_h_

#include <QtCore/QStringList>

#include <hdf5.h>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Observable.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"


class IObserver;
class DataContainerArrayProxy;
class SIMPLH5DataReaderRequirements;

/**
 * @brief The SIMPLH5DataReader class
 */
class SIMPLib_EXPORT SIMPLH5DataReader : public Observable
{
    Q_OBJECT

  public:
    SIMPL_SHARED_POINTERS(SIMPLH5DataReader)
    SIMPL_STATIC_NEW_MACRO(SIMPLH5DataReader)

    SIMPLH5DataReader();
    virtual ~SIMPLH5DataReader();

    /**
     * @brief openFile
     * @param filePath
     * @return
     */
    bool openFile(const QString &filePath);

    /**
     * @brief closeFile
     * @return
     */
    bool closeFile();

    /**
     * @brief readDataContainerArrayStructure
     * @param err
     * @return
     */
    DataContainerArrayProxy readDataContainerArrayStructure(SIMPLH5DataReaderRequirements *req, int &err);

    /**
     * @brief readSIMPLDataUsingProxy
     * @param proxy
     * @param preflight
     * @return
     */
    DataContainerArray::Pointer readSIMPLDataUsingProxy(const DataContainerArrayProxy &proxy, bool preflight);

    /**
     * @brief readPipelineJson
     * @param json
     * @return
     */
    bool readPipelineJson(QString &json);

    /**
     * @brief notifyErrorMessage
     * @param humanLabel
     * @param str
     * @param code
     */
    void notifyErrorMessage(const QString& humanLabel, const QString& str, int code);

  signals:
    void errorGenerated(const QString &title, const QString &msg, const int &code);

  private:
    QString                           m_CurrentFilePath = "";
    hid_t                             m_FileId = -1;

    /**
     * @brief readDataContainerBundles
     * @param fileId
     * @param dca
     * @return
     */
    bool readDataContainerBundles(hid_t fileId, DataContainerArray::Pointer dca);

    SIMPLH5DataReader(const SIMPLH5DataReader&) = delete; // Copy Constructor Not Implemented
    void operator=(const SIMPLH5DataReader&) = delete;    // Operator '=' Not Implemented
};


#endif /* _simplh5datareader_h_ */

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
#pragma once

#include <memory>

#include <hdf5.h>

#include "SIMPLib/Common/Observable.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"

class IObserver;
class DataContainerArrayProxy;
class SIMPLH5DataReaderRequirements;
class DataContainerArray;
using DataContainerArrayShPtrType = std::shared_ptr<DataContainerArray>;

/**
 * @brief The SIMPLH5DataReader class
 */
class SIMPLib_EXPORT SIMPLH5DataReader : public Observable
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(SIMPLH5DataReader)
  PYB11_SHARED_POINTERS(SIMPLH5DataReader)
  PYB11_STATIC_NEW_MACRO(SIMPLH5DataReader)
  PYB11_METHOD(bool openFile ARGS filePath)
  PYB11_METHOD(bool closeFile)
  PYB11_METHOD(DataContainerArrayProxy readDataContainerArrayStructure ARGS SIMPLH5DataReaderRequirements err)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = SIMPLH5DataReader;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  SIMPLH5DataReader();
  ~SIMPLH5DataReader() override;

  /**
   * @brief openFile
   * @param filePath
   * @return
   */
  bool openFile(const QString& filePath);

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
  DataContainerArrayProxy readDataContainerArrayStructure(SIMPLH5DataReaderRequirements* req, int& err);

  /**
   * @brief readSIMPLDataUsingProxy
   * @param proxy
   * @param preflight
   * @return
   */
  DataContainerArrayShPtrType readSIMPLDataUsingProxy(DataContainerArrayProxy& proxy, bool preflight);

  /**
   * @brief readPipelineJson
   * @param json
   * @return
   */
  bool readPipelineJson(QString& json);

  /**
   * @brief setErrorCondition
   * @param code
   * @param str
   */
  void setErrorCondition(int code, const QString& str) override;

signals:
  void errorGenerated(const QString& title, const QString& msg, const int& code);

private:
  QString m_CurrentFilePath = "";
  hid_t m_FileId = -1;

  /**
   * @brief readDataContainerBundles
   * @param fileId
   * @param dca
   * @return
   */
  bool readDataContainerBundles(hid_t fileId, const DataContainerArrayShPtrType& dca);

public:
  SIMPLH5DataReader(const SIMPLH5DataReader&) = delete;            // Copy Constructor Not Implemented
  SIMPLH5DataReader(SIMPLH5DataReader&&) = delete;                 // Move Constructor Not Implemented
  SIMPLH5DataReader& operator=(const SIMPLH5DataReader&) = delete; // Copy Assignment Not Implemented
  SIMPLH5DataReader& operator=(SIMPLH5DataReader&&) = delete;      // Move Assignment Not Implemented
};

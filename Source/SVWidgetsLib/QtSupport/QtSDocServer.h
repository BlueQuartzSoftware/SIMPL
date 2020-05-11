/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class ServerSettings;
class HttpListener;
class HttpSessionStore;
class QtSStaticFileController;

class SVWidgetsLib_EXPORT QtSDocServer : public QObject
{
  Q_OBJECT

public:
  // SIMPL_SHARED_POINTERS(QtSDocServer)
  /**
   * @brief Returns the name of the class for QtSDocServer
   */
  virtual QString getNameOfClass() const;
  /**
   * @brief Returns the name of the class for QtSDocServer
   */
  static QString ClassName();

  ~QtSDocServer() override;

  /**
   * @brief Static instance to retrieve the global instance of this class
   * @return
   */
  static QtSDocServer* Instance();

  /**
   * @brief ipAddress
   * @return
   */
  static QString GetIPAddress();

  /**
   * @brief port
   * @return
   */
  static int GetPort();

  /**
   * @brief Returns the root of the help directory
   */
  static QString GetHelpRootDir();

  /**
   * @brief Generates a help URL from the given lowercase name.
   * @param className The name of the filter class that we are generating a URL
   */
  static QUrl GenerateHTMLUrl(const QString& className);

protected:
  explicit QtSDocServer(QObject* parent = nullptr);

  /**
   * @brief initializeDocServer
   */
  void initializeDocServer();

private:
  static QtSDocServer* self;

  std::shared_ptr<ServerSettings> m_ServerSettings;
  std::shared_ptr<HttpListener> m_HttpListener;
  std::shared_ptr<HttpSessionStore> m_HttpSessionStore;
  std::shared_ptr<QtSStaticFileController> m_QtSStaticFileController;

public:
  QtSDocServer(const QtSDocServer&) = delete;            // Copy Constructor Not Implemented
  QtSDocServer(QtSDocServer&&) = delete;                 // Move Constructor Not Implemented
  QtSDocServer& operator=(const QtSDocServer&) = delete; // Copy Assignment Not Implemented
  QtSDocServer& operator=(QtSDocServer&&) = delete;      // Move Assignment Not Implemented
};

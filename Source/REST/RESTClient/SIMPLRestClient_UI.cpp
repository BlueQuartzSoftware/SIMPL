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

#include "SIMPLRestClient_UI.h"

#include <QtWidgets/QFileDialog>

#include "Core/SIMPLRestClient.h"

#include "SIMPLRestClient/SIMPLClientRequest.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLRestClient_UI::SIMPLRestClient_UI(QWidget* parent)
: QMainWindow(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLRestClient_UI::~SIMPLRestClient_UI()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient_UI::setupGui()
{
  // Create the SIMPL Rest client
  m_Client = new SIMPLRestClient();

  m_StandardColor = outputTE->textColor();
  m_StandardFontWeight = outputTE->fontWeight();

  createCommunicationsConnections();

  createModificationConnections();

  start_Btn->setEnabled(validateParameters());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient_UI::on_pipelinePathSelect_Btn_clicked()
{
  QString filters = tr("JSON File (*.json);;All Files(*.*)");
  QString filePath = QFileDialog::getOpenFileName(this, tr("Select Pipeline File"), m_LastOpenDirectory, filters);
  if(filePath.isEmpty())
  {
    return;
  }

  pipelineFilePath_LE->setText(filePath);

  m_LastOpenDirectory = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient_UI::on_start_Btn_clicked()
{
  if(validateParameters() == false)
  {
    return;
  }

  //  QUrl mikeUrl = QUrl("http://10.0.1.142:8080/");
  //  QUrl matthewUrl = QUrl("http://10.0.1.109:8080/");

  m_Client->setServerUrl(serverURL_LE->text());

  //    // "API Not Found" Test
  //    {
  //      client->sendAPINotFoundRequest(30000);
  //    }

  // "Execute Pipeline" Test
  {
    QJsonObject pipelineObj = m_Client->extractPipelineFromFile(pipelineFilePath_LE->text());
    m_Client->sendExecutePipelineRequest(pipelineObj, requestTimeoutSB->value() * 1000);
  }

  //    // "List Filter Parameters" Test
  //    {
  //      QJsonObject filterNameObj;
  //      filterNameObj.insert("ClassName", "DataContainerReader");
  //      client->sendListFilterParametersRequest(filterNameObj, 30000);
  //    }

  //    // "Loaded Plugins" Test
  //    {
  //      client->sendLoadedPluginsRequest(30000);
  //    }

  //    // "Names of Filters" Test
  //    {
  //      client->sendNamesOfFiltersRequest(30000);
  //    }

  //    // "Number Of Filters" Test
  //    {
  //      client->sendNumberOfFiltersRequest(30000);
  //    }

  //    // "Plugin Information" Test
  //    {
  //      client->sendPluginInformationRequest(30000);
  //    }

  //    // "Preflight Pipeline" Test
  //    {
  //      client->sendPreflightPipelineRequest(30000);
  //    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient_UI::createModificationConnections()
{
  QObject::connect(serverURL_LE, &QLineEdit::textChanged, [=] { start_Btn->setEnabled(validateParameters()); });

  QObject::connect(pipelineFilePath_LE, &QLineEdit::textChanged, [=] { start_Btn->setEnabled(validateParameters()); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLRestClient_UI::createCommunicationsConnections()
{
  QObject::connect(m_Client, &SIMPLRestClient::notifyErrorMessage, [=](const QString& msg) {
    outputTE->setTextColor(m_ErrorColor);
    outputTE->setFontWeight(m_BoldFontWeight);
    outputTE->append(msg + "\n");
  });

  QObject::connect(m_Client, &SIMPLRestClient::notifyWarningMessage, [=](const QString& msg) {
    outputTE->setTextColor(m_WarningColor);
    outputTE->setFontWeight(m_BoldFontWeight);
    outputTE->append(msg + "\n");
  });

  QObject::connect(m_Client, &SIMPLRestClient::notifyStatusMessage, [=](const QString& msg) {
    outputTE->setTextColor(m_StandardColor);
    outputTE->setFontWeight(m_StandardFontWeight);
    outputTE->append(msg + "\n");
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLRestClient_UI::validateParameters()
{
  bool valid = true;
  outputTE->clear();

  QUrl serverUrl(serverURL_LE->text());

  if(serverUrl.isEmpty())
  {
    outputTE->setTextColor(m_ErrorColor);
    outputTE->setFontWeight(m_BoldFontWeight);
    outputTE->append("The server URL is empty.  Please enter a valid URL.\n");
    valid = false;
  }
  else if(serverUrl.isValid() == false)
  {
    outputTE->setTextColor(m_ErrorColor);
    outputTE->setFontWeight(m_BoldFontWeight);
    outputTE->append(tr("The server URL, %1, is invalid.  Please enter a valid URL.\n").arg(serverUrl.toString()));
    valid = false;
  }

  QString pipelineFilePath = pipelineFilePath_LE->text();
  QFileInfo fi(pipelineFilePath);
  if(pipelineFilePath.isEmpty())
  {
    outputTE->setTextColor(m_ErrorColor);
    outputTE->setFontWeight(m_BoldFontWeight);
    outputTE->append(tr("The pipeline file path is empty.  Please enter a valid pipeline file path.\n"));
    valid = false;
  }
  else if(fi.exists() == false)
  {
    outputTE->setTextColor(m_ErrorColor);
    outputTE->setFontWeight(m_BoldFontWeight);
    outputTE->append(tr("The pipeline file at path \"%1\" does not exist.  Please enter a valid pipeline file path.\n").arg(pipelineFilePath));
    valid = false;
  }

  return valid;
}

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
#include "ProgressDialog.h"

#include <iostream>

#include "SIMPLib/Messages/AbstractMessageHandler.h"
#include "SIMPLib/Messages/FilterErrorMessage.h"

#include "ui_ProgressDialog.h"

/**
 * @brief This class is the message handler for the Progress Dialog class.  It is responsible
 * for receiving any incoming messages that the progress dialog would like to know about.
 */
class ProgressDialogMessageHandler : public AbstractMessageHandler
{
public:
  explicit ProgressDialogMessageHandler(ProgressDialog* progressDialog)
  : m_ProgressDialog(progressDialog)
  {
  }

  /**
   * @brief Sets the text of the progress dialog's label to the human label of incoming
   * FilterErrorMessages.
   * @param msg
   */
  void processMessage(const FilterErrorMessage* msg) const override
  {
    m_ProgressDialog->m_Ui->label->setText(msg->getHumanLabel());
  }

private:
  ProgressDialog* m_ProgressDialog = nullptr;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProgressDialog::ProgressDialog(QWidget* parent, Qt::WindowFlags f)
: QDialog(parent, f)
, m_Ui(new Ui::ProgressDialog)
{
  m_Ui->setupUi(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProgressDialog::~ProgressDialog() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProgressDialog::setLabelText(const QString& text)
{
  m_Ui->label->setText(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProgressDialog::processPipelineMessage(const AbstractMessage::Pointer& msg)
{
  ProgressDialogMessageHandler msgHandler(this);
  msg->visit(&msgHandler);
}

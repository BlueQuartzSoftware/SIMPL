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

#include "OutputFileWidget.h"

#include <QtCore/QDir>
#include <QtCore/QMetaProperty>

#include <QtWidgets/QFileDialog>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OutputFileWidget::OutputFileWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: AbstractIOFileWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<OutputFileFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "OutputFileWidget can ONLY be used with a OutputFileFilterParameter object");

  setOpenDialogLastFilePath(QDir::homePath());

  setupGui();
  if(filter)
  {
    QString currentPath = filter->property(PROPERTY_NAME_AS_CHAR).toString();
    if(currentPath.isEmpty() == false)
    {
      currentPath = QDir::toNativeSeparators(currentPath);
      // Store the last used directory into the private instance variable
      QFileInfo fi(currentPath);
      setOpenDialogLastFilePath(fi.filePath());
    }
    else
    {
      setOpenDialogLastFilePath(QDir::homePath());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OutputFileWidget::~OutputFileWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputFileWidget::setupGui()
{
  connect(selectBtn, SIGNAL(clicked()), this, SLOT(selectOutputFile()));

  m_LineEdit->setPlaceholderText("Enter Output File Path");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputFileWidget::selectOutputFile()
{
  QString currentPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
  if(currentPath.isEmpty() == true)
  {
    currentPath = getOpenDialogLastFilePath();
  }
  QString Ftype = m_FilterParameter->getFileType();
  QString ext = m_FilterParameter->getFileExtension();
  QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
  QString defaultName = getOpenDialogLastFilePath();
  QString file = QFileDialog::getSaveFileName(this, tr("Save File As"), defaultName, s);

  if(true == file.isEmpty())
  {
    return;
  }
  // bool ok = false;
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  setOpenDialogLastFilePath(fi.filePath());

  m_LineEdit->setText(file);
  on_m_LineEdit_editingFinished();
}

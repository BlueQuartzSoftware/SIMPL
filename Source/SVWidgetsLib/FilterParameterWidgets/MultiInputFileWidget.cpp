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

#include "MultiInputFileWidget.h"

#include <tuple>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

#include "SIMPLib/FilterParameters/MultiInputFileFilterParameter.h"
#include "SIMPLib/Utilities/QtConverter.hpp"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "ui_MultiInputFileWidget.h"

static QString s_LastUsedDir;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiInputFileWidget::MultiInputFileWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_Ui(new Ui::MultiInputFileWidget)
{
  m_FilterParameter = dynamic_cast<MultiInputFileFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "MultiInputFileWidget can ONLY be used with a MultiInputFileFilterParameter object");

  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiInputFileWidget::~MultiInputFileWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &MultiInputFileWidget::beforePreflight);

  // Catch when the filter is finished running the preflight
  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &MultiInputFileWidget::afterPreflight);

  // Catch when the filter wants its values updated
  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &MultiInputFileWidget::filterNeedsInputParameters);

  connect(m_Ui->selectBtn, &QPushButton::clicked, this, &MultiInputFileWidget::selectMultiInputFile);

  s_LastUsedDir = QDir::homePath();

  // Loop through all of the input files, find the first non-empty one and use that.
  if(getFilter() != nullptr && getFilterParameter() != nullptr)
  {
    MultiInputFileFilterParameter::GetterCallbackType getter = m_FilterParameter->getGetterCallback();
    m_SelectedFiles = QtConverter::ToQStringList(getter());
    for(const auto& inputFile : m_SelectedFiles)
    {
      if(!inputFile.isEmpty())
      {
        QFileInfo fi(inputFile);
        s_LastUsedDir = fi.absolutePath();
        break;
      }
    }
    m_Ui->m_HumanLabel->setText(m_FilterParameter->getHumanLabel());
  }

  createSummaryText();
}

// -----------------------------------------------------------------------------
void MultiInputFileWidget::selectMultiInputFile()
{
  QString Ftype = m_FilterParameter->getFileType();
  QString ext = m_FilterParameter->getFileExtension();
  QString s = QString("%1 Files (%2);;All Files (*.*)").arg(Ftype, ext);

  QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Input File(s)"), s_LastUsedDir, s);

  if(files.isEmpty())
  {
    return;
  }

  m_SelectedFiles = files;

  createSummaryText();
}

// -----------------------------------------------------------------------------
void MultiInputFileWidget::createSummaryText()
{
  if(m_SelectedFiles.isEmpty())
  {
    m_Ui->m_SummaryLabel->hide();
    return;
  }
  QString file = m_SelectedFiles.at(0);
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  s_LastUsedDir = fi.absolutePath();
  QString msg;
  QTextStream out(&msg);
  out << s_LastUsedDir << "\nNumber of Files Selected: " << m_SelectedFiles.size();
  m_Ui->m_SummaryLabel->setText(msg);
  m_Ui->m_SummaryLabel->setVisible(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  std::ignore = filter;

  std::vector<std::string> files = QtConverter::FromQStringFileList(m_SelectedFiles);
  MultiInputFileFilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();

  if(setter)
  {
    setter(files);
  }
  else
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::afterPreflight()
{
}

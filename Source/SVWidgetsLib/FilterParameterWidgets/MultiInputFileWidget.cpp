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

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

#include "SIMPLib/FilterParameters/MultiInputFileFilterParameter.h"
#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "ui_MultiInputFileWidget.h"

static QString s_LastUsedDir;

QStringList toStringList(const std::vector<std::string>& inputFiles)
{
  QStringList fileList;
  for(const auto& inputFile : inputFiles)
  {
    fileList << QString::fromStdString(inputFile);
  }
  return fileList;
}

std::vector<std::string> fromStringList(const QStringList& selectedFiles)
{
  std::vector<std::string> files;
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  for(const auto& filePath : selectedFiles)
  {
    files.push_back(validator->convertToAbsolutePath(filePath).toStdString());
  }
  return files;
}

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
void MultiInputFileWidget::setIcon(const QPixmap& path)
{
  m_Icon = path;
  setupMenuField();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPixmap MultiInputFileWidget::getIcon()
{
  return m_Icon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  connect(m_Ui->selectBtn, SIGNAL(clicked()), this, SLOT(selectMultiInputFile()));

  s_LastUsedDir = QDir::homePath();

  // Default to the user's home directory
  m_Ui->m_LineEdit->setText(QDir::homePath());
  setValidFilePath(m_Ui->m_LineEdit->text());

  // Loop through all of the input files, find the first non-empty one and use that.
  if(getFilter() != nullptr && getFilterParameter() != nullptr)
  {
    MultiInputFileFilterParameter::GetterCallbackType getter = m_FilterParameter->getGetterCallback();
    m_SelectedFiles = toStringList(getter());
    for(const auto& inputFile : m_SelectedFiles)
    {
      if(!inputFile.isEmpty())
      {
        QFileInfo fi(inputFile);
        s_LastUsedDir = fi.absolutePath();
        m_Ui->m_LineEdit->setText(fi.absolutePath());
        setValidFilePath(m_Ui->m_LineEdit->text());
        break;
      }
    }
  }

  QtSFileCompleter* com = new QtSFileCompleter(this, false);
  m_Ui->m_LineEdit->setCompleter(com);
  QObject::connect(com, SIGNAL(activated(const QString&)), this, SLOT(on_m_LineEdit_textChanged(const QString&)));

  setupMenuField();
  if(m_SelectedFiles.isEmpty())
  {
    m_Ui->m_SummaryLabel->hide();
  }
  m_Ui->m_LineEdit->setPlaceholderText("Enter Input File Path");

  m_CurrentText = m_Ui->m_LineEdit->text();
  on_m_LineEdit_editingFinished();
}

// -----------------------------------------------------------------------------
void MultiInputFileWidget::selectMultiInputFile()
{
  QString Ftype = m_FilterParameter->getFileType();
  QString ext = m_FilterParameter->getFileExtension();
  QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");

  QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Input File(s)"), s_LastUsedDir, s);

  if(files.isEmpty())
  {
    return;
  }

  m_SelectedFiles = files;

  QString file = files.at(0);
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  s_LastUsedDir = fi.filePath();
  m_Ui->m_LineEdit->setText(fi.absolutePath());
  on_m_LineEdit_editingFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::keyPressEvent(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Escape)
  {
    SVStyle* style = SVStyle::Instance();
    m_Ui->m_LineEdit->setText(m_CurrentText);
    setValidFilePath(m_Ui->m_LineEdit->text());
    style->LineEditClearStyle(m_Ui->m_LineEdit);
    m_Ui->m_LineEdit->setToolTip("");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::setupMenuField()
{
  QFileInfo fi(m_Ui->m_LineEdit->text());

  QMenu* lineEditMenu = new QMenu(m_Ui->m_LineEdit);
  m_Ui->m_LineEdit->setButtonMenu(QtSLineEdit::Left, lineEditMenu);

  m_Ui->m_LineEdit->setButtonVisible(QtSLineEdit::Left, true);

  QPixmap pixmap(8, 8);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  painter.drawPixmap(0, (pixmap.height() - m_Icon.height()) / 2, m_Icon);
  m_Ui->m_LineEdit->setButtonPixmap(QtSLineEdit::Left, pixmap);

  {
    m_ShowFileAction = new QAction(lineEditMenu);
    m_ShowFileAction->setObjectName(QString::fromUtf8("showFileAction"));
#if defined(Q_OS_WIN)
    m_ShowFileAction->setText("Show in Windows Explorer");
#elif defined(Q_OS_MAC)
    m_ShowFileAction->setText("Show in Finder");
#else
    m_ShowFileAction->setText("Show in File System");
#endif
    lineEditMenu->addAction(m_ShowFileAction);
    connect(m_ShowFileAction, SIGNAL(triggered()), this, SLOT(showFileInFileSystem()));
  }

  if(!m_Ui->m_LineEdit->text().isEmpty() && fi.exists())
  {
    m_ShowFileAction->setEnabled(true);
  }
  else
  {
    m_ShowFileAction->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::on_m_LineEdit_editingFinished()
{
  QString msg;
  QTextStream out(&msg);

  out << "Number of Files Selected: " << m_SelectedFiles.size();
  m_Ui->m_SummaryLabel->setText(msg);
  m_Ui->m_SummaryLabel->show();

#if 0
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString path = validator->convertToAbsolutePath(m_Ui->m_LineEdit->text());

  QFileInfo fi(m_Ui->m_LineEdit->text());
  if(fi.isRelative())
  {
    m_Ui->m_AbsPathLabel->setText(path);
    m_Ui->m_AbsPathLabel->show();
  }
  else
  {
    m_Ui->m_AbsPathLabel->hide();
  }

  QtSFileUtils::VerifyPathExists(path, m_Ui->m_LineEdit);
  if(m_Ui->m_LineEdit->text() != m_CurrentText)
  {
    emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
  }
  m_CurrentText = m_Ui->m_LineEdit->text();
  setValidFilePath(m_Ui->m_LineEdit->text());
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::on_m_LineEdit_returnPressed()
{
  on_m_LineEdit_editingFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::on_m_LineEdit_textChanged(const QString& text)
{

#if 0
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);

  QFileInfo fi(text);
  if(fi.isRelative())
  {
    m_Ui->m_SummaryLabel->setText(inputPath);
  }

  if(QtSFileUtils::HasValidFilePath(inputPath))
  {
    m_ShowFileAction->setEnabled(true);
    setValidFilePath(m_Ui->m_LineEdit->text());
  }
  else
  {
    m_ShowFileAction->setDisabled(true);
  }

  SVStyle* style = SVStyle::Instance();

  if(text != m_CurrentText)
  {
    style->LineEditBackgroundErrorStyle(m_Ui->m_LineEdit);
    m_Ui->m_LineEdit->setToolTip("Press the 'Return' key to apply your changes");
  }
  else
  {
    style->LineEditClearStyle(m_Ui->m_LineEdit);
    m_Ui->m_LineEdit->setToolTip("");
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::on_m_LineEdit_fileDropped(const QString& text)
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);

  m_Ui->m_LineEdit->setText(text);
  setValidFilePath(m_Ui->m_LineEdit->text());
  // Set/Remove the red outline if the file does exist
  QtSFileUtils::VerifyPathExists(inputPath, m_Ui->m_LineEdit);

  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiInputFileWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  std::vector<std::string> files = fromStringList(m_SelectedFiles);
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

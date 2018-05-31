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

#include "FileListInfoWidget.h"

//-- Qt Includes
#include <QtCore/QDir>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"
#include "SIMPLib/Utilities/StringOperations.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

#include "FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString FileListInfoWidget::m_OpenDialogLastFilePath = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfoWidget::FileListInfoWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_Ui(new Ui::FileListInfoWidget)
, m_DidCausePreflight(false)
{
  FileListInfoFilterParameter* fli = dynamic_cast<FileListInfoFilterParameter*>(parameter);
  if(nullptr == fli)
  {
    QString msg;
    QTextStream ss(&msg);
    ss << "FileListInfoWidget can ONLY be used with FileListInfoFilterParameter objects. The programmer of the filter has a bug.";
    ss << " The name of the filter was " << filter->getHumanLabel() << " and the name of the Filter Parameter was " << parameter->getHumanLabel();
    ss << " and is trying to get the propery " << parameter->getPropertyName() << " in the filter";
    Q_ASSERT_X(nullptr != fli, msg.toLatin1().constData(), __FILE__);
  }
  m_Ui->setupUi(this);
  setupGui();
  if(m_Ui->inputDir->text().isEmpty())
  {
    setInputDirectory(QDir::homePath());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfoWidget::~FileListInfoWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::setWidgetListEnabled(bool b)
{
  foreach(QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::setupGui()
{
  connectSignalsSlots();

  setupMenuField();

  m_WidgetList << m_Ui->inputDir << m_Ui->inputDirBtn;
  m_WidgetList << m_Ui->fileExt << m_Ui->errorMessage << m_Ui->totalDigits << m_Ui->fileSuffix;
  m_WidgetList << m_Ui->filePrefix << m_Ui->totalSlices << m_Ui->startIndex << m_Ui->endIndex;

  m_Ui->errorMessage->setVisible(false);

  m_OrderingGroup = new QButtonGroup(this);
  m_OrderingGroup->addButton(m_Ui->orderAscending);
  m_OrderingGroup->addButton(m_Ui->orderDescending);

  validateInputFile();
  getGuiParametersFromFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::connectSignalsSlots()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // Connections for the various ui widgets
  connect(m_Ui->inputDirBtn, &QPushButton::clicked, this, &FileListInfoWidget::inputDirBtn_clicked);

  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->inputDir->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &FileListInfoWidget::inputDir_textChanged);
  connect(m_Ui->inputDir, &QtSLineEdit::textChanged, this, &FileListInfoWidget::inputDir_textChanged);

  connect(m_Ui->filePrefix, &QtSLineEdit::textChanged, this, [=] {
    generateExampleInputFile();
    emit parametersChanged();
  });

  connect(m_Ui->fileSuffix, &QtSLineEdit::textChanged, this, [=] {
    generateExampleInputFile();
    emit parametersChanged();
  });

  connect(m_Ui->fileExt, &QtSLineEdit::textChanged, this, [=] {
    generateExampleInputFile();
    emit parametersChanged();
  });

  connect(m_Ui->totalDigits, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] {
    generateExampleInputFile();
    emit parametersChanged();
  });

  connect(m_Ui->startIndex, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] {
    generateExampleInputFile();
    emit parametersChanged();
  });

  connect(m_Ui->endIndex, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] {
    generateExampleInputFile();
    emit parametersChanged();
  });

  connect(m_Ui->increment, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] {
    generateExampleInputFile();
    emit parametersChanged();
  });

  connect(m_Ui->orderAscending, &QRadioButton::toggled, this, [=] {
    generateExampleInputFile();
    emit parametersChanged();
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::keyPressEvent(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Escape)
  {
    m_Ui->inputDir->setText(m_CurrentText);
    m_Ui->inputDir->setStyleSheet("");
    m_Ui->inputDir->setToolTip("");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::setupMenuField()
{
  QFileInfo fi(m_Ui->inputDir->text());

  QMenu* lineEditMenu = new QMenu(m_Ui->inputDir);
  m_Ui->inputDir->setButtonMenu(QtSLineEdit::Left, lineEditMenu);
  QLatin1String iconPath = QLatin1String(":/SIMPL/icons/images/caret-bottom.png");

  m_Ui->inputDir->setButtonVisible(QtSLineEdit::Left, true);

  QPixmap pixmap(8, 8);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  const QPixmap mag = QPixmap(iconPath);
  painter.drawPixmap(0, (pixmap.height() - mag.height()) / 2, mag);
  m_Ui->inputDir->setButtonPixmap(QtSLineEdit::Left, pixmap);

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

  if(m_Ui->inputDir->text().isEmpty() == false && fi.exists())
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
void FileListInfoWidget::getGuiParametersFromFilter()
{
  blockSignals(true);

  FileListInfo_t data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<FileListInfo_t>();

  m_Ui->inputDir->setText(data.InputPath);

  m_Ui->startIndex->setValue(data.StartIndex);
  m_Ui->endIndex->setValue(data.EndIndex);
  m_Ui->increment->setValue(data.IncrementIndex);

  m_Ui->filePrefix->setText(data.FilePrefix);
  m_Ui->fileSuffix->setText(data.FileSuffix);

  m_Ui->fileExt->setText(data.FileExtension);
  m_Ui->totalDigits->setValue(data.PaddingDigits);

  setOrdering(data.Ordering);
  blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::validateInputFile()
{
  FileListInfo_t data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<FileListInfo_t>();

  QString currentPath = data.InputPath;
  QFileInfo fi(currentPath);
  if(currentPath.isEmpty() == false && fi.exists() == false)
  {
    QString defaultName = m_OpenDialogLastFilePath;

    QString title = QObject::tr("Select a replacement input file in filter '%2'").arg(getFilter()->getHumanLabel());

    QString file = QFileDialog::getExistingDirectory(this, title, getInputDirectory(), QFileDialog::ShowDirsOnly);
    if(true == file.isEmpty())
    {
      file = currentPath;
    }
    file = QDir::toNativeSeparators(file);
    // Store the last used directory into the private instance variable
    QFileInfo fi(file);

    setInputDirectory(fi.filePath());

    data.InputPath = file;

    QVariant v;
    v.setValue(data);
    bool ok = getFilter()->setProperty(PROPERTY_NAME_AS_CHAR, v);
    if(!ok)
    {
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::checkIOFiles()
{
  if(true == this->verifyPathExists(m_Ui->inputDir->text(), m_Ui->inputDir))
  {
    findMaxSliceAndPrefix();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::inputDirBtn_clicked()
{
  QString outputFile = QFileDialog::getExistingDirectory(this, tr("Select Input Directory"), getInputDirectory());

  if(!outputFile.isNull())
  {
    m_Ui->inputDir->blockSignals(true);
    m_Ui->inputDir->setText(QDir::toNativeSeparators(outputFile));
    inputDir_textChanged(m_Ui->inputDir->text());
    setOpenDialogLastFilePath(outputFile);
    m_Ui->inputDir->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::inputDir_textChanged(const QString& text)
{
  Q_UNUSED(text)

  if(verifyPathExists(m_Ui->inputDir->text(), m_Ui->inputDir))
  {
    m_ShowFileAction->setEnabled(true);
    findMaxSliceAndPrefix();
    QDir dir(m_Ui->inputDir->text());
    QString dirname = dir.dirName();
    dir.cdUp();

    generateExampleInputFile();
    m_Ui->inputDir->blockSignals(true);
    m_Ui->inputDir->setText(QDir::toNativeSeparators(m_Ui->inputDir->text()));
    m_Ui->inputDir->blockSignals(false);
  }
  else
  {
    m_ShowFileAction->setEnabled(false);
    m_Ui->fileListView->clear();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t FileListInfoWidget::getOrdering()
{
  if(m_Ui->orderAscending->isChecked())
  {
    return SIMPL::RefFrameZDir::LowtoHigh;
  }
  if(m_Ui->orderDescending->isChecked())
  {
    return SIMPL::RefFrameZDir::HightoLow;
  }
  return SIMPL::RefFrameZDir::UnknownRefFrameZDirection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::setOrdering(uint32_t ref)
{
  if(ref == SIMPL::RefFrameZDir::LowtoHigh)
  {
    m_Ui->orderAscending->setChecked(true);
  }
  if(ref == SIMPL::RefFrameZDir::HightoLow)
  {
    m_Ui->orderDescending->setChecked(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::generateExampleInputFile()
{
  QString indexString = StringOperations::GeneratePaddedString(m_Ui->startIndex->value(), m_Ui->totalDigits->value(), '0');
  QString filename = QString("%1%2%3.%4").arg(m_Ui->filePrefix->text()).arg(indexString).arg(m_Ui->fileSuffix->text()).arg(m_Ui->fileExt->text());
  m_Ui->generatedFileNameExample->setText(filename);

  int start = m_Ui->startIndex->value();
  int end = m_Ui->endIndex->value();
  int increment = m_Ui->increment->value();
  bool hasMissingFiles = false;

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(start, end, increment, hasMissingFiles, m_Ui->orderAscending->isChecked(), m_Ui->inputDir->text(), m_Ui->filePrefix->text(),
                                                                  m_Ui->fileSuffix->text(), m_Ui->fileExt->text(), m_Ui->totalDigits->value());
  m_Ui->fileListView->clear();
  QIcon greenDot = QIcon(QString(":/SIMPL/icons/images/bullet_ball_green.png"));
  QIcon redDot = QIcon(QString(":/SIMPL/icons/images/bullet_ball_red.png"));
  for(QVector<QString>::size_type i = 0; i < fileList.size(); ++i)
  {
    QString filePath(fileList.at(i));
    QFileInfo fi(filePath);
    QListWidgetItem* item = new QListWidgetItem(filePath, m_Ui->fileListView);
    if(fi.exists() == true)
    {
      item->setIcon(greenDot);
    }
    else
    {
      hasMissingFiles = true;
      item->setIcon(redDot);
    }
  }

  if(hasMissingFiles == true)
  {
    m_Ui->errorMessage->setVisible(true);
    m_Ui->errorMessage->setText("Alert: Red Dot File(s) on the list do NOT exist on the filesystem. Please make sure all files exist");
  }
  else
  {
    m_Ui->errorMessage->setVisible(true);
    m_Ui->errorMessage->setText("All files exist.");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::findMaxSliceAndPrefix()
{
  if(m_Ui->inputDir->text().length() == 0)
  {
    return;
  }
  QDir dir(m_Ui->inputDir->text());

  // Final check to make sure we have a valid file extension
  if(m_Ui->fileExt->text().isEmpty() == true)
  {
    return;
  }

  QString ext = "." + m_Ui->fileExt->text();
  QStringList filters;
  filters << "*" + ext;
  dir.setNameFilters(filters);
  QFileInfoList angList = dir.entryInfoList();

  int minSlice = 0;
  int maxSlice = 0;
  int currValue = 0;
  bool flag = false;
  bool ok;
  QString fPrefix;
  QRegExp rx("(\\d+)");
  QStringList list;
  int pos = 0;
  int digitStart = 0;
  int digitEnd = 0;
  int totalOimFilesFound = 0;
  int minTotalDigits = 1000;
  foreach(QFileInfo fi, angList)
  {
    if(fi.suffix().compare(ext) && fi.isFile() == true)
    {
      pos = 0;
      list.clear();
      QString fn = fi.baseName();
      QString fns = fn;
      int length = fn.length();
      digitEnd = length - 1;
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
      {
        --digitEnd;
      }
      pos = digitEnd;

      digitStart = pos = rx.indexIn(fn, pos);
      digitEnd = digitStart;
      while((pos = rx.indexIn(fn, pos)) != -1)
      {
        list << rx.cap(0);
        fPrefix = fn.left(pos);
        pos += rx.matchedLength();
      }
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
      {
        ++digitEnd;
      }

      if(digitEnd - digitStart < minTotalDigits)
      {
        minTotalDigits = digitEnd - digitStart;
      }
      m_Ui->totalDigits->setValue(minTotalDigits);
      if(list.size() > 0)
      {
        currValue = list.front().toInt(&ok);
        if(false == flag)
        {
          minSlice = currValue;
          flag = true;
        }
        if(currValue > maxSlice)
        {
          maxSlice = currValue;
        }
        if(currValue < minSlice)
        {
          minSlice = currValue;
        }
      }
      ++totalOimFilesFound;
    }
  }
  this->m_Ui->totalSlices->setText(QString::number(totalOimFilesFound));
  this->m_Ui->filePrefix->setText(fPrefix);
  this->m_Ui->startIndex->setValue(minSlice);
  this->m_Ui->endIndex->setValue(maxSlice);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::widgetChanged(const QString& text)
{
  Q_UNUSED(text)
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  if(nullptr == filter)
  {
    QString ss = QObject::tr("Error Setting FileListStack Gui values to Filter instance. Filter instance was nullptr.").arg(getFilterParameter()->getPropertyName());
    emit errorSettingFilterParameter(ss);
    return;
  }
  bool ok = false;

  FileListInfo_t data;
  data.IncrementIndex = m_Ui->increment->value();
  data.EndIndex = m_Ui->endIndex->value();
  data.FileExtension = m_Ui->fileExt->text();
  data.FilePrefix = m_Ui->filePrefix->text();
  data.FileSuffix = m_Ui->fileSuffix->text();
  data.InputPath = m_Ui->inputDir->text();
  data.Ordering = getOrdering();
  data.PaddingDigits = m_Ui->totalDigits->value();
  data.StartIndex = m_Ui->startIndex->value();

  QVariant v;
  v.setValue(data);
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::beforePreflight()
{
  if(m_DidCausePreflight == false)
  {
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::afterPreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::setInputDirectory(QString val)
{
  m_Ui->inputDir->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FileListInfoWidget::getInputDirectory()
{
  if(m_Ui->inputDir->text().isEmpty())
  {
    return QDir::homePath();
  }
  return m_Ui->inputDir->text();
}

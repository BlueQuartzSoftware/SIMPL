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

#include "AbstractIOFileWidget.h"

#include <QtCore/QDir>
#include <QtCore/QMetaProperty>
#include <QtCore/QFileInfo>

#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>



#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractIOFileWidget::AbstractIOFileWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  setupUi(this);
  setupGui();
  if(filter)
  {
    QString currentPath = filter->property(PROPERTY_NAME_AS_CHAR).toString();
    if(currentPath.isEmpty() == false)
    {
      currentPath = QDir::toNativeSeparators(currentPath);
      // Store the last used directory into the private instance variable
      QFileInfo fi(currentPath);
      m_LineEdit->setText(fi.filePath());
    }
    else
    {
      m_LineEdit->setText(QDir::homePath());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractIOFileWidget::~AbstractIOFileWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractIOFileWidget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its m_LineEdits updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  QtSFileCompleter* com = new QtSFileCompleter(this, false);
  m_LineEdit->setCompleter(com);
  QObject::connect(com, SIGNAL(activated(const QString&)), this, SLOT(on_m_LineEdit_textChanged(const QString&)));

  setupMenuField();

  if(getFilterParameter() != nullptr)
  {
    label->setText(getFilterParameter()->getHumanLabel());

    QString currentPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
    m_LineEdit->setText(currentPath);
    if(verifyPathExists(currentPath, m_LineEdit))
    {
    }
  }

  m_CurrentText = m_LineEdit->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractIOFileWidget::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape)
  {
    m_LineEdit->setText(m_CurrentText);
    m_LineEdit->setStyleSheet("");
    m_LineEdit->setToolTip("");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractIOFileWidget::setupMenuField()
{
  QFileInfo fi(m_LineEdit->text());

  QMenu* lineEditMenu = new QMenu(m_LineEdit);
  m_LineEdit->setButtonMenu(QtSLineEdit::Left, lineEditMenu);
  QLatin1String iconPath = QLatin1String(":/caret-bottom.png");

  m_LineEdit->setButtonVisible(QtSLineEdit::Left, true);

  QPixmap pixmap(8, 8);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  const QPixmap mag = QPixmap(iconPath);
  painter.drawPixmap(0, (pixmap.height() - mag.height()) / 2, mag);
  m_LineEdit->setButtonPixmap(QtSLineEdit::Left, pixmap);

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


  if (m_LineEdit->text().isEmpty() == false && fi.exists())
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
bool AbstractIOFileWidget::verifyPathExists(QString filePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(filePath);
  if(false == fileinfo.exists())
  {
    lineEdit->setStyleSheet("QLineEdit { border: 1px solid red; }");
  }
  else
  {
    lineEdit->setStyleSheet("");
  }
  return fileinfo.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractIOFileWidget::on_m_LineEdit_editingFinished()
{
  m_LineEdit->setStyleSheet(QString(""));
  m_CurrentText = m_LineEdit->text();
  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractIOFileWidget::on_m_LineEdit_returnPressed()
{
  on_m_LineEdit_editingFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractIOFileWidget::on_m_LineEdit_textChanged(const QString& text)
{
  if (hasValidFilePath(text) == true)
  {
    m_ShowFileAction->setEnabled(true);
  }
  else
  {
    m_ShowFileAction->setDisabled(true);
  }

  if (text != m_CurrentText)
  {
    m_LineEdit->setStyleSheet(QString::fromLatin1("QLineEdit { color: rgb(255, 0, 0); }"));
    m_LineEdit->setToolTip("Press the 'Return' key to apply your changes");
  }
  else
  {
    m_LineEdit->setStyleSheet("");
    m_LineEdit->setToolTip("");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractIOFileWidget::on_m_LineEdit_fileDropped(const QString& text)
{
  setOpenDialogLastFilePath(text);
  // Set/Remove the red outline if the file does exist
  verifyPathExists(text, m_LineEdit);

  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractIOFileWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QString text = m_LineEdit->text();
  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, text);
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractIOFileWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractIOFileWidget::afterPreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractIOFileWidget::setOpenDialogLastFilePath(QString val) 
{
  m_LineEdit->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbstractIOFileWidget::getOpenDialogLastFilePath() 
{
  if(m_LineEdit->text().isEmpty())
  {
    return QDir::homePath();
  }
  return m_LineEdit->text();
}

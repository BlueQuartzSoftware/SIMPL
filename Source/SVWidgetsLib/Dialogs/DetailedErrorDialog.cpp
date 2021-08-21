/* ============================================================================
 * Copyright (c) 2021-2021 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DetailedErrorDialog.h"

#include <QtGui/QAccessible>
#include <QtWidgets/QStyle>

#include "ui_DetailedErrorDialog.h"

namespace
{
QPixmap GetPixmap(QDialog& dialog, QMessageBox::Icon iconType)
{
  QStyle* style = dialog.style();
  int iconSize = style->pixelMetric(QStyle::PM_MessageBoxIconSize, nullptr, &dialog);
  QIcon icon;
  switch(iconType)
  {
  case QMessageBox::Information: {
    icon = style->standardIcon(QStyle::SP_MessageBoxInformation, nullptr, &dialog);
    break;
  }
  case QMessageBox::Warning: {
    icon = style->standardIcon(QStyle::SP_MessageBoxWarning, nullptr, &dialog);
    break;
  }
  case QMessageBox::Critical: {
    icon = style->standardIcon(QStyle::SP_MessageBoxCritical, nullptr, &dialog);
    break;
  }
  case QMessageBox::Question: {
    icon = style->standardIcon(QStyle::SP_MessageBoxQuestion, nullptr, &dialog);
    break;
  }
  default: {
    break;
  }
  }
  if(icon.isNull())
  {
    return QPixmap();
  }
  return icon.pixmap(iconSize, iconSize);
}
} // namespace

// -----------------------------------------------------------------------------
DetailedErrorDialog::DetailedErrorDialog(QMessageBox::Icon iconType, const QString& title, const QString& text, const QString& detailedText, QWidget* parent, Qt::WindowFlags f)
: QDialog(parent, f)
, m_Ui(new Ui::DetailedErrorDialog())
{
  m_Ui->setupUi(this);
  setWindowTitle(title);
  setText(text);
  setDetailedText(detailedText);
  QPixmap pixmap = GetPixmap(*this, iconType);
  setIcon(pixmap);
}

// -----------------------------------------------------------------------------
DetailedErrorDialog::~DetailedErrorDialog() = default;

// -----------------------------------------------------------------------------
void DetailedErrorDialog::setText(const QString& text)
{
  m_Ui->text->setText(text);
}

// -----------------------------------------------------------------------------
void DetailedErrorDialog::setDetailedText(const QString& text)
{
  m_Ui->detailedText->setPlainText(text);
}

// -----------------------------------------------------------------------------
void DetailedErrorDialog::setIcon(const QPixmap& pixmap)
{
  m_Ui->iconLabel->setPixmap(pixmap);
}

// -----------------------------------------------------------------------------
void DetailedErrorDialog::showEvent(QShowEvent* event)
{
  QAccessibleEvent accessibleEvent(this, QAccessible::Event::Alert);
  QAccessible::updateAccessibility(&accessibleEvent);
  QDialog::showEvent(event);
}

// -----------------------------------------------------------------------------
int DetailedErrorDialog::critical(QWidget* widget, const QString& title, const QString& text, const QString& detailedText)
{
  DetailedErrorDialog dialog(QMessageBox::Icon::Critical, title, text, detailedText, widget);
  return dialog.exec();
}

// -----------------------------------------------------------------------------
int DetailedErrorDialog::information(QWidget* widget, const QString& title, const QString& text, const QString& detailedText)
{
  DetailedErrorDialog dialog(QMessageBox::Icon::Information, title, text, detailedText, widget);
  return dialog.exec();
}

// -----------------------------------------------------------------------------
int DetailedErrorDialog::question(QWidget* widget, const QString& title, const QString& text, const QString& detailedText)
{
  DetailedErrorDialog dialog(QMessageBox::Icon::Question, title, text, detailedText, widget);
  return dialog.exec();
}

// -----------------------------------------------------------------------------
int DetailedErrorDialog::warning(QWidget* widget, const QString& title, const QString& text, const QString& detailedText)
{
  DetailedErrorDialog dialog(QMessageBox::Icon::Warning, title, text, detailedText, widget);
  return dialog.exec();
}

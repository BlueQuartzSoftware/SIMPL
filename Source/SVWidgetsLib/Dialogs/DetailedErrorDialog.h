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

#pragma once

#include <QtCore/QSharedPointer>
#include <QtCore/QString>

#include <QtGui/QPixmap>

#include <QtWidgets/QDialog>
#include <QtWidgets/QMessageBox>

#include "SVWidgetsLib/SVWidgetsLib.h"

namespace Ui
{
class DetailedErrorDialog;
}

/**
 * @brief This will display a resizable QDialog that shows a detailed error
 */
class SVWidgetsLib_EXPORT DetailedErrorDialog : public QDialog
{
  Q_OBJECT

public:
  DetailedErrorDialog(QMessageBox::Icon iconType, const QString& title, const QString& text, const QString& detailedText, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  ~DetailedErrorDialog() override;

  static int critical(QWidget* widget, const QString& title, const QString& text, const QString& detailedText);
  static int information(QWidget* widget, const QString& title, const QString& text, const QString& detailedText);
  static int question(QWidget* widget, const QString& title, const QString& text, const QString& detailedText);
  static int warning(QWidget* widget, const QString& title, const QString& text, const QString& detailedText);

public Q_SLOTS:
  void setText(const QString& text);
  void setDetailedText(const QString& text);
  void setIcon(const QPixmap& pixmap);

protected:
  void showEvent(QShowEvent* event) override;

private:
  QSharedPointer<Ui::DetailedErrorDialog> m_Ui;
};

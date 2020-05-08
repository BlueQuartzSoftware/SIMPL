/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5280
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtGui/QColor>
#include <QtCore/QRect>
#include <QtWidgets/QLabel>

#include "SIMPLib/DataContainers/DataArrayPath.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

/**
 * @brief The SVDataPathLabel class
 */
class SVWidgetsLib_EXPORT DataPathLabel : public QLabel
{
  Q_OBJECT

public:
  static QColor GetBackgroundColor(DataArrayPathHelper::DataType type);
  static QColor GetInactiveColor();

  Q_PROPERTY(int DataTypei READ getDataTypei)

  DataPathLabel(QWidget* parent = nullptr);
  ~DataPathLabel() override;

  DataArrayPathHelper::DataType getDataType() const;
  void setDataType(DataArrayPathHelper::DataType type);
  int getDataTypei() const;

protected:
  int getYMargin() const;
  QRect getStyledContentsRect() const;
  QRect getStyledBorderRect() const;
  // void paintEvent(QPaintEvent* event) override;

private:
  DataArrayPathHelper::DataType m_DataType = DataArrayPathHelper::DataType::None;
};

/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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

#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @class MontageSelection MontageSelection.h SIMPLib/Utilities/MontageSelection.h
 * @brief The MontageSelection class is a utility class for generating and storing
 * a list of DataContainer names from prefix, suffix, as well as a range of rows
 * and columns.  This class is used in MontageSelectionFilterParameter and the
 * corresponding MontageSelectionWidget.
 */
class SIMPLib_EXPORT MontageSelection
{
  PYB11_CREATE_BINDINGS(MontageSelection)
  PYB11_STATIC_CREATION(Create ARGS std::string std::string int int int int int)
  PYB11_PROPERTY(QString Prefix READ getPrefix WRITE setPrefix)
  PYB11_PROPERTY(QString Suffix READ getSuffix WRITE setSuffix)
  PYB11_PROPERTY(int Padding READ getPadding WRITE setPadding)
  PYB11_PROPERTY(int RowStart READ getRowStart WRITE setRowStart)
  PYB11_PROPERTY(int RowEnd READ getRowEnd WRITE setRowEnd)
  PYB11_PROPERTY(int ColStart READ getColStart WRITE setColStart)
  PYB11_PROPERTY(int ColEnd READ getColEnd WRITE setColEnd)

public:
  MontageSelection();
  MontageSelection(const QString& prefix, const QString& suffix, int padding, int rowStart, int rowEnd, int colStart, int colEnd);
  virtual ~MontageSelection();

  static MontageSelection Create(const std::string& prefix, const std::string& suffix, int padding, int rowStart, int rowEnd, int colStart, int colEnd);

  /**
   * @brief Returns the prefix used for generating DataContainer names.
   * @return
   */
  QString getPrefix() const;

  /**
   * @brief Returns the suffix used for generating DataContainer names.
   * @return
   */
  QString getSuffix() const;

  /**
   * @brief Sets the prefix used for generating DataContainer names.
   * @param prefix
   */
  void setPrefix(const QString& prefix);

  /**
   * @brief Sets the suffix used for generating DataContainer names.
   * @param suffix
   */
  void setSuffix(const QString& suffix);

  /**
   * @brief Returns the padding used for digits in DataContainer names.
   * @return
   */
  int getPadding() const;

  /**
   * @brief Sets the padding used for digits in DataContainer names.
   * The minimum value allowed is 1, and values under that will be bumped up.
   * @param padding
   */
  void setPadding(int padding);

  /**
   * @brief Returns the starting row in the montage DataContainer selection.
   * @return
   */
  int getRowStart() const;

  /**
   * @brief Returns the last row in the montage DataContainer selection.
   * @return
   */
  int getRowEnd() const;

  /**
   * @brief Returns the starting column in the montage DataContainer selection.
   * @return
   */
  int getColStart() const;

  /**
   * @brief Returns the last column in the montage DataContainer selection.
   * @return
   */
  int getColEnd() const;

  /**
   * @brief Sets the starting row in the montage DataContainer selection.
   * The minimum value allowed is 0, and values under that will be bumped up.
   * @param rowStart
   */
  void setRowStart(int rowStart);

  /**
   * @brief Sets the last row in the montage DataContainer selection.
   * The minimum value allowed is the RowStart value, and values under that will be bumped up.
   * @param rowEnd
   */
  void setRowEnd(int rowEnd);

  /**
   * @brief Sets the starting column in the montage DataContainer selection.
   * The minimum value allowed is 0, and values under that will be bumped up.
   * @param colStart
   */
  void setColStart(int colStart);

  /**
   * @brief Sets the last column in the montage DataContainer selection.
   * The minimum value allowed is the ColStart value, and values under that will be bumped up.
   * @param colEnd
   */
  void setColEnd(int colEnd);

  /**
   * @brief Sets the range of rows for the montage DataContainer selection.
   * The minimum value allowed is 0, and the end value must be equal or greater than the start value.
   * Invalid values will be bumped up to their minimum values.
   * @param rowStart
   * @param rowEnd
   */
  void setRowRange(int rowStart, int rowEnd);

  /**
   * @brief Sets the range of columns for the montage DataContainer selection.
   * The minimum value allowed is 0, and the end value must be equal or greater than the start value.
   * Invalid values will be bumped up to their minimum values.
   * @param colStart
   * @param colEnd
   */
  void setColRange(int colStart, int colEnd);

  /**
   * @brief Returns the DataContainer name for the tile at the given row and column.
   * @param row
   * @param col
   * @return
   */
  QString getDataContainerName(int row, int col) const;

  /**
   * @brief Returns a QStringList of the target DataContainer names in comb order.
   * @return
   */
  QStringList getDataContainerNamesCombOrder() const;

  /**
   * @brief Returns a QStringList of the target DataContainer names in snake order.
   * @return
   */
  QStringList getDataContainerNamesSnakeOrder() const;

  /**
   * @brief Returns a QJsonObject corresponding to the current values.
   * @return
   */
  QJsonObject toJson() const;

  /**
   * @brief Loads values from the given QJsonObject.
   * @param json
   */
  void fromJson(const QJsonObject& json);

private:
  QString m_Prefix;
  QString m_Suffix;
  int m_Padding = 0;
  int m_RowStart = 0;
  int m_RowEnd = 0;
  int m_ColStart = 0;
  int m_ColEnd = 0;

public:
  MontageSelection(const MontageSelection&) = default;            // Copy Constructor Default Implemented
  MontageSelection(MontageSelection&&) = default;                 // Move Constructor Default Implemented
  MontageSelection& operator=(const MontageSelection&) = default; // Copy Assignment Default Implemented
  MontageSelection& operator=(MontageSelection&&) = default;      // Move Assignment Default Implemented
};

Q_DECLARE_METATYPE(MontageSelection);

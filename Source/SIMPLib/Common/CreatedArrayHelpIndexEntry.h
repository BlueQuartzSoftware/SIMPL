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

#pragma once

#include <vector>
#include <memory>

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"

class SIMPLib_EXPORT CreatedArrayHelpIndexEntry
{
public:
  using Self = CreatedArrayHelpIndexEntry;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for CreatedArrayHelpIndexEntry
   */
  QString getNameOfClass() const;
  /**
   * @brief Returns the name of the class for CreatedArrayHelpIndexEntry
   */
  static QString ClassName();

  typedef QVector<Pointer> VectorType;

  virtual ~CreatedArrayHelpIndexEntry();

  /**
   * @brief Setter property for FilterName
   */
  void setFilterName(const QString& value);
  /**
   * @brief Getter property for FilterName
   * @return Value of FilterName
   */
  QString getFilterName() const;

  /**
   * @brief Setter property for FilterHumanLabel
   */
  void setFilterHumanLabel(const QString& value);
  /**
   * @brief Getter property for FilterHumanLabel
   * @return Value of FilterHumanLabel
   */
  QString getFilterHumanLabel() const;

  /**
   * @brief Setter property for FilterGroup
   */
  void setFilterGroup(const QString& value);
  /**
   * @brief Getter property for FilterGroup
   * @return Value of FilterGroup
   */
  QString getFilterGroup() const;

  /**
   * @brief Setter property for FilterSubGroup
   */
  void setFilterSubGroup(const QString& value);
  /**
   * @brief Getter property for FilterSubGroup
   * @return Value of FilterSubGroup
   */
  QString getFilterSubGroup() const;

  /**
   * @brief Setter property for ArrayDefaultName
   */
  void setArrayDefaultName(const QString& value);
  /**
   * @brief Getter property for ArrayDefaultName
   * @return Value of ArrayDefaultName
   */
  QString getArrayDefaultName() const;

  /**
   * @brief Setter property for ArrayGroup
   */
  void setArrayGroup(const QString& value);
  /**
   * @brief Getter property for ArrayGroup
   * @return Value of ArrayGroup
   */
  QString getArrayGroup() const;

  /**
   * @brief Setter property for ArrayType
   */
  void setArrayType(const QString& value);
  /**
   * @brief Getter property for ArrayType
   * @return Value of ArrayType
   */
  QString getArrayType() const;

  /**
   * @brief Setter property for ArrayNumComponents
   */
  void setArrayNumComponents(int value);
  /**
   * @brief Getter property for ArrayNumComponents
   * @return Value of ArrayNumComponents
   */
  int getArrayNumComponents() const;

  /**
   * @brief Setter property for ArrayDescription
   */
  void setArrayDescription(const QString& value);
  /**
   * @brief Getter property for ArrayDescription
   * @return Value of ArrayDescription
   */
  QString getArrayDescription() const;

protected:
  CreatedArrayHelpIndexEntry();

public:
  CreatedArrayHelpIndexEntry(const CreatedArrayHelpIndexEntry&) = delete;            // Copy Constructor Not Implemented
  CreatedArrayHelpIndexEntry(CreatedArrayHelpIndexEntry&&) = delete;                 // Move Constructor Not Implemented
  CreatedArrayHelpIndexEntry& operator=(const CreatedArrayHelpIndexEntry&) = delete; // Copy Assignment Not Implemented
  CreatedArrayHelpIndexEntry& operator=(CreatedArrayHelpIndexEntry&&) = delete;      // Move Assignment Not Implemented

private:
  QString m_FilterName = {};
  QString m_FilterHumanLabel = {};
  QString m_FilterGroup = {};
  QString m_FilterSubGroup = {};
  QString m_ArrayDefaultName = {};
  QString m_ArrayGroup = {};
  QString m_ArrayType = {};
  int m_ArrayNumComponents = {};
  QString m_ArrayDescription = {};
};

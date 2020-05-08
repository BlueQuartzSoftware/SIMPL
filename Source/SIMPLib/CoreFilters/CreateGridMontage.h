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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Montages/DataContainerGrid.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The CreateGridMontage class. See [Filter documentation](@ref CreateGridMontage) for details.
 */
class SIMPLib_EXPORT CreateGridMontage : public AbstractFilter
{
  Q_OBJECT

  // clang-format off
  PYB11_BEGIN_BINDINGS(CreateGridMontage SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(CreateGridMontage)
  PYB11_FILTER_NEW_MACRO(CreateGridMontage)

  PYB11_PROPERTY(QString MontageName READ getMontageName WRITE setMontageName)
  PYB11_PROPERTY(DataContainerGrid GridSelection READ getGridSelection WRITE setGridSelection)
  PYB11_PROPERTY(QStringList DataContainerNameList READ getDataContainerNameList WRITE setDataContainerNameList)
  PYB11_END_BINDINGS()
  // clang-format on

public:
  using Self = CreateGridMontage;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer();

  static Pointer New();

  QString getNameOfClass() const override;
  static QString ClassName();
  // static int CreateGridMontage::IsTypeOf(const char* type);

  ~CreateGridMontage() override;

  /**
   * @brief Getter for the MontageName property
   * @return
   */
  QString getMontageName() const;

  /**
   * @brief Setter for the MontageName property
   * @param value
   */
  void setMontageName(const QString& value);

  Q_PROPERTY(QString MontageName READ getMontageName WRITE setMontageName)

  /**
   * @brief Getter for the GridSelection property.
   * @return
   */
  DataContainerGrid getGridSelection() const;

  /**
   * @brief Setter for the GridSelection property.
   * @param value
   */
  void setGridSelection(const DataContainerGrid& value);

  Q_PROPERTY(DataContainerGrid GridSelection READ getGridSelection WRITE setGridSelection)

  /**
   * @brief Getter for the DataContainerNameList property
   * @return
   */
  QStringList getDataContainerNameList() const;

  /**
   * @brief Setter for the DataContainerNameList property
   * @param value
   */
  void setDataContainerNameList(const QStringList& value);

  Q_PROPERTY(QStringList DataContainerNameList READ getDataContainerNameList WRITE setDataContainerNameList)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  CreateGridMontage();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  QString m_MontageName;
  DataContainerGrid m_GridSelection;
  QStringList m_DataContainerNameList;

public:
  CreateGridMontage(const CreateGridMontage&) = delete;            // Copy Constructor Not Implemented
  CreateGridMontage(CreateGridMontage&&) = delete;                 // Move Constructor Not Implemented
  CreateGridMontage& operator=(const CreateGridMontage&) = delete; // Copy Assignment Not Implemented
  CreateGridMontage& operator=(CreateGridMontage&&) = delete;      // Move Assignment Not Implemented
};

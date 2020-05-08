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

#include <memory>

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

/**
 * @class ThresholdExample ThresholdExample.h ExamplePlugin/Code/ExamplePluginFilters/ThresholdExample.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class SIMPLib_EXPORT ThresholdExample : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ThresholdExample SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(ThresholdExample)
  PYB11_FILTER_NEW_MACRO(ThresholdExample)
  PYB11_PROPERTY(ComparisonInputs CellComparisonInputs READ getCellComparisonInputs WRITE setCellComparisonInputs)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ThresholdExample;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;

  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for ThresholdExample
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ThresholdExample
   */
  static QString ClassName();

  ~ThresholdExample() override;
  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const QString& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  QString getDataContainerName() const;

  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  QString getCellAttributeMatrixName() const;

  /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */

  // -----------------------------------------------------------------------------
  /* Each Filter can ONLY have ONE of each of these types of widgets */
  /**
   * @brief Setter property for CellComparisonInputs
   */
  void setCellComparisonInputs(const ComparisonInputs& value);
  /**
   * @brief Getter property for CellComparisonInputs
   * @return Value of CellComparisonInputs
   */
  ComparisonInputs getCellComparisonInputs() const;

  Q_PROPERTY(ComparisonInputs CellComparisonInputs READ getCellComparisonInputs WRITE setCellComparisonInputs)

  /**
   * @brief This returns the group that the filter belonds to. You can select
   * a different group if you want. The string returned here will be displayed
   * in the GUI for the filter
   */
  QString getCompiledLibraryName() const override;
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;
  QString getGroupName() const override;

  /**
   * @brief This returns a string that is displayed in the GUI. It should be readable
   * and understandable by humans.
   */
  QString getHumanLabel() const override;

  /**
   * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
   * a subgroup. It should be readable and understandable by humans.
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief This method will instantiate all the end user settable options/parameters
   * for this filter
   */
  void setupFilterParameters() override;

  /**
   * @brief This method will read the options from a file
   * @param reader The reader that is used to read the options from a file
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  ThresholdExample();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  virtual void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

public:
  ThresholdExample(const ThresholdExample&) = delete;            // Copy Constructor Not Implemented
  ThresholdExample(ThresholdExample&&) = delete;                 // Move Constructor Not Implemented
  ThresholdExample& operator=(const ThresholdExample&) = delete; // Copy Assignment Not Implemented
  ThresholdExample& operator=(ThresholdExample&&) = delete;      // Move Assignment Not Implemented

private:
  QString m_DataContainerName = {};
  QString m_CellAttributeMatrixName = {};
  ComparisonInputs m_CellComparisonInputs = {};
};

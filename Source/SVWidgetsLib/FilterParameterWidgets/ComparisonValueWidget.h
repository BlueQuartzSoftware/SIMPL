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

#include <QtWidgets/QWidget>

#include "SIMPLib/Filtering/ComparisonValue.h"

#include "SVWidgetsLib/FilterParameterWidgets/IComparisonWidget.h"

#include "ui_ComparisonValueWidget.h"

/**
* @brief The ComparisonValueWidget class is used to display and edit ComparisonValues for ComparisonSelectionAdvancedWidget
*/
class SVWidgetsLib_EXPORT ComparisonValueWidget : public IComparisonWidget, private Ui::ComparisonValueWidget
{
  Q_OBJECT

public:
  ComparisonValueWidget(QWidget* parent, ComparisonValue::Pointer comparisonValue = nullptr);
  ~ComparisonValueWidget() override;

  /**
  * @brief Returns the ComparisonValue used by this widget
  * @return
  */
  ComparisonValue::Pointer getComparisonValue();
  /**
  * @brief Sets the ComparisonValue used by this widget
  * @param value New ComparisonValue::Pointer to use
  */
  void setComparisonValue(const ComparisonValue::Pointer &value);

  /**
  * @brief Returns the ComparisonValue used by this widget as an AbstractComparison::Pointer
  * @return
  */
  AbstractComparison::Pointer getComparison() override;

  /**
  * @brief Sets the array of names to use
  * @param names QStringList of all DataArray names available to use
  */
  void setArrayNames(QStringList names) override;

  /**
  * @brief Sets the AttributeMatrix used to specify DataArray names
  * @param am AttributeMatrix to use
  */
  void setAttributeMatrix(AttributeMatrix::Pointer am) override;
  
  /**
  * @brief This method does additional GUI widget connections
  */
  void setupGui();

  /**
  * @brief Updates the comparison array paths based on the renamed DataArrayPath
  * @param renamePath
  */
  void renameDataArrayPath(DataArrayPath::RenameType renamePath) override;

protected:
  /**
  * @brief Updates the DataArray combo box with names from the given AttributeMatrix
  */
  void setupDataArrayComboBox();

protected slots:
  /**
  * @brief Changes the DataArray used by the ComparisonValue
  * @param index Specifies which DataArray name to use
  */
  void dataArrayChanged(int index);
  
  /**
  * @brief Changes the comparison operator for the ComparisonValue
  * @param index Specifies the new comparisonOperator
  */
  void comparisonOperatorChanged(int index);
  /**
  * @brief Changes the value used by the ComparisonValue
  * @param value Specifies the new value to use
  */
  void comparisonValueChanged();

private:
  ComparisonValue::Pointer m_comparisonValue;
};


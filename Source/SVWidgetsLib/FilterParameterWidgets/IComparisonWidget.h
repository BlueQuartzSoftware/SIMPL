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

#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/Filtering/AbstractComparison.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

/**
* @brief The IComparisonWidget is an abstract class used by ComparisonContainerWidget
* to reference both ComparisonValueWidget and ComparisonSetWidget.
*/
class SVWidgetsLib_EXPORT IComparisonWidget : public QWidget
{
  Q_OBJECT

public:
  IComparisonWidget(QWidget* parent);
  ~IComparisonWidget();

  /**
  * @brief Creates a new IComparisonWidget for a given comparison
  * @param comparison Comparison used for creating the new widget
  * @return
  */
  static IComparisonWidget* CreateWidget(AbstractComparison::Pointer comparison);

  /**
  * @brief Returns the comparison used by the widget as an AbstractComparison
  * @return
  */
  virtual AbstractComparison::Pointer getComparison() = 0;

  /**
  * @brief Sets the array of names to use in this widget or any nested IComparisonWidgets
  * @param names QStringList of all DataArray names available to use
  */
  virtual void setArrayNames(QStringList names) = 0;

  /**
  * @brief Returns a list of all DataArray names used in this widget or any nested IComparisonWidgets
  * @return
  */
  virtual QStringList getArrayNames();

  /**
  * @brief Returns the AttributeMatrix used by the widget
  * @return
  */
  AttributeMatrix::Pointer getAttributeMatrix();

  /**
  * @brief Sets the AttributeMatrix used by the widget
  * @param am
  */
  virtual void setAttributeMatrix(AttributeMatrix::Pointer am);

  /**
  * @brief Updates the comparison's DataArray options based on the renamed path
  * @param renamePath
  */
  virtual void renameDataArrayPath(DataArrayPath::RenameType renamePath) = 0;

signals:
  /**
  * @brief Specifies that the comparison used by the widget has changed
  */
  void comparisonChanged();

protected:
  QStringList m_arrayNames;

private:
  AttributeMatrix::Pointer m_attributeMatrix;
};


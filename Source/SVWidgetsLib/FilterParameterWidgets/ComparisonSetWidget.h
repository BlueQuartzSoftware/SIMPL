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
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>

#include <vector>

#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/Filtering/ComparisonSet.h"

#include "SVWidgetsLib/FilterParameterWidgets/IComparisonWidget.h"

#include "ui_ComparisonSetWidget.h"

class ComparisonContainerWidget;

/**
* @brief The ComparisonSetWidget class is used to display and edit ComparisonSets for the ComparisonSelectionAdvancedWidget.
*/
class SVWidgetsLib_EXPORT ComparisonSetWidget : public IComparisonWidget, private Ui::ComparisonSetWidget
{
  Q_OBJECT

public:
  ComparisonSetWidget(QWidget* parent, ComparisonSet::Pointer comparisonSet = nullptr);
  ~ComparisonSetWidget() override;

  /**
  * @brief Returns the comparisons used by the ComparisonSet
  * @return
  */
  QVector<AbstractComparison::Pointer> getComparisons();

  /**
  * @brief Sets the comparisons used by the ComparisonSet
  * @param comparisons New comparisons to assign to the ComparisonSet to replace the current ones
  */
  void setComparisons(QVector<AbstractComparison::Pointer> comparisons);

  /**
  * @brief Adds a comparison to the ComparisonSet
  * @param comparison New comparison to add to the ComparisonSet
  */
  void addComparison(AbstractComparison::Pointer comparison);

  /**
  * @brief Returns the nth comparison used by the ComparisonSet
  * @param index Which comparison to return
  * @return
  */
  AbstractComparison::Pointer getComparison(int index);

  /**
  * @brief Returns the ComparisonSet used by the widget
  * @return
  */
  ComparisonSet::Pointer getComparisonSet();

  /**
  * @brief Specifies a new ComparisonSet to use by the widget
  * @param comparisonSet
  */
  void setComparisonSet(ComparisonSet::Pointer comparisonSet);

  /**
  * @brief returns the ComparisonSet as an AbstractComparison
  * @return
  */
  AbstractComparison::Pointer getComparison() override;

  /**
  * @brief Sets the array of names to use in any nested IComparisonWidgets
  * @param names QStringList of all DataArray names available to use
  */
  void setArrayNames(QStringList names) override;

  /**
  * @brief Sets the AttributeMatrix used for finding DataArrayNames
  * @param am
  */
  void setAttributeMatrix(AttributeMatrix::Pointer am) override;

  /**
  * @brief Updates the ComparisonSet using the ComparisonContainerWidgets stored in this widget
  */
  void updateItems();

  /**
  * @brief This method does additional GUI widget connections
  */
  void setupGui();

  /**
  * @brief Updates all comparisons contained in the set with the updated path
  * @param renamePath
  */
  void renameDataArrayPath(DataArrayPath::RenameType renamePath) override;
  
protected:
  /**
  * @brief Clears the ComparisonSet of all values and subsets
  */
  void clearSet();

  /**
  * @brief Returns the ComparisonWidgets stored in this widget
  * @return
  */
  QVector<IComparisonWidget*> getComparisonWidgets();

  /**
  * @brief Adds a comparison to the ComparisonSet at a given position
  * @param comparison New comparison to add
  * @param index Position to add the new comparison
  */
  void addComparisonWidget(AbstractComparison::Pointer comparison, int index = -1);
  /**
  * @brief Returns the index to insert new items by default
  * @return
  */
  int insertIndex();

  /**
  * @brief Calculates which index to insert a ComparisonWidget from a given point
  * @param pos Position used to calculate the index
  * @param ignore Any ComparisonContainerWidget that should be ignored during calculating the index
  */
  int insertIndexByPoint(QPoint pos, ComparisonContainerWidget* ignore = nullptr);

  /**
  * @brief Handles dragging ComparisonContainerWidgets into the ComparisonSet
  * @param event
  */
  void dragEnterEvent(QDragEnterEvent* event) override;
  
  /**
  * @brief Handles moving ComparisonContainerWidgets in the ComparisonSet
  * @param event
  */
  void dragMoveEvent(QDragMoveEvent* event) override;

  /**
  * @brief Handles dropping ComparisonContainerWidgets in the ComparisonSet
  * @param event
  */
  void dropEvent(QDropEvent* event) override;

protected slots:
  /**
  * @brief Show the context menu at a given point
  * @param pos
  */
  void showContextMenu(QPoint pos);

  /**
  * @brief Shows the context menu at the cursor's location
  */
  void showAddItemMenu();

  /**
  * @brief Creates a new ComparisonSet at the end of the current ComparisonSet
  */
  void createComparisonSet();

  /**
  * @brief Create a new ComparisonValue at the end of the current ComparisonSet
  */
  void createComparisonValue();

  /**
  * @brief Specifies that the results of the ComparisonSet should have their values flipped.
  * @param newInverse Integer value of the Invert QCheckBox
  */
  void setInverse(int newInverse);

private:
  ComparisonSet::Pointer m_comparisonSet;
};


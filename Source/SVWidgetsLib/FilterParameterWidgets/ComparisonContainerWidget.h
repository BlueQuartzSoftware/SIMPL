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

#include <QtWidgets/QFrame>
#include <QtGui/QMouseEvent>

#include "SIMPLib/Filtering/AbstractComparison.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/FilterParameterWidgets/IComparisonWidget.h"

#include "ui_ComparisonContainerWidget.h"

class ComparisonSetWidget;

/**
* @brief ComparisonContainerWidget is used to display widgets for AbstractComparisons, edit the comparison's, union operator,
* and allow the widgets to be dragged and reordered between ComparisonSetWidgets
*/
class SVWidgetsLib_EXPORT ComparisonContainerWidget : public QFrame, private Ui::ComparisonContainerWidget
{
  Q_OBJECT

public:
  ComparisonContainerWidget(QWidget* parent, AbstractComparison::Pointer comparison);
  ~ComparisonContainerWidget();

  /**
  * @brief Returns the union operator for this comparison
  * @return
  */
  int getUnionOperator();
  /**
  * @brief Sets the union operator for this comparison
  * @param unionOperator Union operator to be used by this comparison
  */
  void setUnionOperator(int unionOperator);

  /**
  * @brief Set whether or not the union operator should be shows for this widget
  * @param enabled Specifies whether or not the union operator is shown
  */
  void showUnionOperator(bool enabled = true);
  /**
  * @brief Hides the union operator for this widget
  */
  void hideUnionOperator();

  /**
  * @brief Sets the comparison this widget represents
  * @param comparison AbstractComparison to be represented by this widget
  */
  void setComparison(AbstractComparison::Pointer comparison);
  /**
  * @brief Sets the comparison widget to be shown in this widget
  * @param widget the IComparisonWidget that stores the AbstractComparison shown
  */
  void setComparisonWidget(IComparisonWidget* widget);

  /**
  * @brief Returns the value of the stored AbstractComparison
  * @return
  */
  AbstractComparison::Pointer getCurrentComparison();
  /**
  * @brief Returns the IComparisonWidget displayed
  * @return
  */
  IComparisonWidget* getComparisonWidget();
  
  /**
  * @brief Returns the ComparisonSetWidget this widget belongs to
  * @return
  */
  ComparisonSetWidget* getComparisonSetWidget();
  /**
  * @brief Sets a new ComparisonSetWidget to belong to and changes any signal and slot connections to reflect the change
  * @param comparisonSetWidget The new ComparisonSetWidget to belong to
  */
  void setComparisonSetWidget(ComparisonSetWidget* comparisonSetWidget);

  /**
  * @brief Selects the current widget and draws a border around it to show which ComparisonContainerWidget is being moved
  */
  void select();
  /**
  * @brief Removes any styling given by select() and clears the selection
  */
  void deselect();

  /**
  * @brief This method does additional GUI widget connections
  */
  void setupGui();

public slots:
  /**
  * @brief Delete this and remove the comparison from the current ComparisonSet
  */
  void deleteItem();

signals:
  /**
  * @brief Signifies that the comparison stored has changed
  */
  void comparisonChanged();

protected:
  static ComparisonContainerWidget* SelectedItem; // Currently Selected Widget
  static QString BorderStyleSheet; // Style Sheet Used When Selected

  /**
  * @brief MousePressEvent
  * @param event Move press event
  */
  void mousePressEvent(QMouseEvent* event);

  /**
  * @brief MouseMoveEvent
  * @param event Mouse move event
  */
  void mouseMoveEvent(QMouseEvent* event);

  /**
  * @brief MouseReleaseEvent
  * @param event Mouse release event
  */
  void mouseReleaseEvent(QMouseEvent* event);

protected slots:
  /**
  * @brief Union operator has been changed in the UI and needs to update the comparison
  * @param unionOp New union operator
  */
  void unionOperatorChanged(int unionOp);

private:
  IComparisonWidget* m_comparisonWidget;
  ComparisonSetWidget* m_comparisonSetWidget;
  QString m_BaseName;
  QPoint m_startDragPoint;
};


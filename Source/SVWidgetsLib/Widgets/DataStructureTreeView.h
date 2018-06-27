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

#ifndef _dataBrowserTreeView_h_
#define _dataBrowserTreeView_h_

#include <QtWidgets/QApplication>

#include <QtCore/QJsonObject>

#include <QtCore/QModelIndex>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QTreeView>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/Widgets/DataStructureItemDelegate.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class SVWidgetsLib_EXPORT DataStructureTreeView : public QTreeView
{
  Q_OBJECT

public:
  /**
   * @brief DataStructureTreeView
   * @param parent
   */
  DataStructureTreeView(QWidget* parent = nullptr);

  /**
   * @brief ~DataStructureTreeView()
   */
  virtual ~DataStructureTreeView();

  /**
   * @brief Returns the DataArrayPath for the given QModelIndex
   * @param index
   * @return
   */
  DataArrayPath getDataArrayPath(QModelIndex index);

  /**
   * @brief Updates the active filter for use in preventing created
   * items from being used as filter parameter values.
   * @param filter
   */
  void setActiveFilter(AbstractFilter::Pointer filter);

  /**
  * @brief Sets the filter requirements and forces a repaint
  * @param reqs
  */
  void setViewRequirements(DataContainerSelectionFilterParameter::RequirementType reqs);

  /**
   * @brief Sets the filter requirements and forces a repaint
   * @param reqs
   */
  void setViewRequirements(AttributeMatrixSelectionFilterParameter::RequirementType reqs);

  /**
   * @brief Sets the filter requirements and forces a repaint
   * @param reqs
   */
  void setViewRequirements(DataArraySelectionFilterParameter::RequirementType reqs);

  /**
   * @brief Clears the filter requirements and forces a repaint
   * @param reqs
   */
  void clearViewRequirements();

signals:
  void filterPath(DataArrayPath path);
  void endDataStructureFiltering();
  void applyPathToFilteringParameter(DataArrayPath path);

protected:
  /**
   * @brief mouseMoveEvent
   * @param event
   */
  void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief dragEnterEvent
   * @param event
   */
  void dragEnterEvent(QDragEnterEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief dragMoveEvent
   * @param event
   */
  void dragMoveEvent(QDragMoveEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief dropEvent
   * @param event
   */
  void dropEvent(QDropEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief leaveEvent
   * @param event
   */
  void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief mouseDoubleClickEvent
   * @param event
   */
  void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief Emits the filter path for the given model index
   * @param index
   */
  void emitFilterPath(QModelIndex& index);

  /**
   * @brief End the drag process
   */
  void dragComplete();

private slots:
  /**
   * @brief mousePressEvent
   * @param event
   */
  void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
  QPoint m_StartPos;
  bool m_Dragging = false;
  AbstractFilter::Pointer m_Filter = nullptr;
  DataStructureItemDelegate* m_Delegate = nullptr;

  /**
   * @brief performDrag
   */
  void performDrag();
};

#endif /* _dataBrowserTreeView_H_ */

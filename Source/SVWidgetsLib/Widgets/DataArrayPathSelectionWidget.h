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

#include <QtCore/QPoint>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDropEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolButton>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

/**
 * @class DataArrayPathSelectionWidget DataArrayPathSelectionWidget.h
 * SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h
 * @brief This class handles DataArrayPath selection and mouse over signals for filter parameter widgets.
 */
class DataArrayPathSelectionWidget : public QToolButton
{
  Q_OBJECT

public:
  enum class State
  {
    Normal,
    Active,
    NotFound,
    DragEnabled,
    DragDisabled
  };
  Q_ENUM(State)

  static const QPixmap CreateDragIcon(const DataArrayPath& path);
  static const QPixmap CreateDragIcon(const QString& text, DataArrayPathHelper::DataType dataType);
  static const QPixmap CreateDragIcon(const QString& text, const QColor& backgroundColor);
  static const QColor GetActiveColor(DataArrayPathHelper::DataType type);
  static const QColor GetCheckedColor(DataArrayPathHelper::DataType type);
  static bool CheckPathRequirements(AbstractFilter* filter, const DataArrayPath& path, const DataContainerSelectionFilterParameter::RequirementType& reqs);
  static bool CheckPathRequirements(AbstractFilter* filter, const DataArrayPath& path, const AttributeMatrixSelectionFilterParameter::RequirementType& reqs);
  static bool CheckPathRequirements(AbstractFilter* filter, const DataArrayPath& path, const DataArraySelectionFilterParameter::RequirementType& reqs);

  DataArrayPathSelectionWidget(QWidget* parent = nullptr);
  ~DataArrayPathSelectionWidget() override = default;

  Q_PROPERTY(DataArrayPathHelper::DataType PathType READ getDataType)
  Q_PROPERTY(State State READ getState)

  /**
   * @brief Returns the type of DataArrayPath this widget can handle
   * @return
   */
  DataArrayPathHelper::DataType getDataType() const;

  /**
   * @brief Sets the DataContainer requirements and sets the DataType to DataContainer
   * @param dcReqs
   */
  void setDataContainerRequirements(const DataContainerSelectionFilterParameter::RequirementType& dcReqs);

  /**
   * @brief Sets the AttributeMatrix requirements and sets the DataType to AttributeMatrix
   * @param amReqs
   */
  void setAttrMatrixRequirements(const AttributeMatrixSelectionFilterParameter::RequirementType& amReqs);

  /**
   * @brief Sets the DataArray requirements and sets the DataType to DataArray
   * @param daReqs
   */
  void setDataArrayRequirements(const DataArraySelectionFilterParameter::RequirementType& daReqs);

  /**
   * @brief Returns the DataContainer requirements
   * @return
   */
  DataContainerSelectionFilterParameter::RequirementType getDataContainerRequirements() const;

  /**
   * @brief Returns the AttributeMatrix requirements
   * @return
   */
  AttributeMatrixSelectionFilterParameter::RequirementType getAttrMatrixRequirements() const;

  /**
   * @brief Returns the DataArray requirements
   * @return
   */
  DataArraySelectionFilterParameter::RequirementType getDataArrayRequirements() const;

  /**
   * @brief Checks the requirements for the current path
   * @return
   */
  bool checkCurrentPath() const;

  /**
   * @brief Checks requirements against the given DataArrayPath
   * @param path
   * @return
   */
  bool checkPathReqs(const DataArrayPath& path) const;

  /**
   * @brief Checks DataContainer requirements against the given DataArrayPath
   * @param path
   * @return
   */
  bool checkDataContainerReqs(const DataArrayPath& path) const;

  /**
   * @brief Checks AttributeMatrix requirements against the given DataArrayPath
   * @param path
   * @return
   */
  bool checkAttributeMatrixReqs(const DataArrayPath& path) const;

  /**
   * @brief Sets the DataArrayPath without checking requirements
   * @param dap
   */
  void setDataArrayPath(const DataArrayPath& dap);

  /**
   * @brief Returns the DataArrayPath
   * @return
   */
  DataArrayPath getDataArrayPath() const;

  /**
   * @brief Sets the filter the selection widget operates on
   * @param filter
   */
  void setFilter(AbstractFilter* filter);

  /**
   * @brief Handles operations performed before Preflight()
   */
  void beforePreflight();

  /**
   * @brief Checks path validity
   */
  void afterPreflight();

  /**
   * @brief Override the minimumSizeHint for extending the widget enough to paint the active color
   */
  QSize minimumSizeHint() const override;

  /**
   * @brief Override the minimumSizeHint for extending the widget enough to paint the active color
   */
  QSize sizeHint() const override;

  /**
   * @brief Returns the property name set for this widget
   * @return
   */
  QString getPropertyName() const;

  /**
   * @brief Sets the property name
   * @param propName
   */
  void setPropertyName(QString propName);

public Q_SLOTS:
  /**
   * @brief Sets whether or not filtering is forced active or not
   * @param active
   */
  void setPathFiltering(bool active);

  /**
   * @brief Resets the styling and clears the check state
   */
  void resetStyle();

  /**
   * @brief Checks if the input path is compatible with the requirements and disables the widget if it does not.
   * @param inputPath
   */
  void checkDragPath(DataArrayPath inputPath);

  /**
   * @brief Clears the enable / disable state from checkDragPath
   */
  void clearPathFiltering();

  /**
   * @brief Clears the enable / disable state but leaves the checked state.  This is used for filtering from the DataStructure.
   */
  void endExternalFiltering();

  /**
   * @brief Sets the DataArrayPath if this widget is being used to filter the DataStructure and the path matches.
   * @param path
   */
  void setFilteredDataArrayPath(DataArrayPath path);

  /**
   * @brief Disables filtering the DataStructure while another DataArrayPathSelectionWidget is locked.
   * @param selection
   */
  void selectionWidgetLocked(QToolButton* selection);

  /**
   * @brief Enables filtering the DataStructure due to the current locking DataArrayPathSelectionWidget is unchecked
   * @param selection
   */
  void selectionWidgetUnlocked(QToolButton* selection);

Q_SIGNALS:
  void filterPath(DataArrayPath path);
  void viewPathsMatchingReqs(DataContainerSelectionFilterParameter::RequirementType dcReqs);
  void viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType amReqs);
  void viewPathsMatchingReqs(DataArraySelectionFilterParameter::RequirementType daReqs);
  void endViewPaths();
  void pathChanged();
  void dataArrayPathSelectionLocked(QToolButton* selection);
  void dataArrayPathSelectionUnlocked(QToolButton* selection);

protected:
  /**
   * @brief Performs initial setup for the GUI
   */
  void setupGui();

  /**
   * @brief Returns the current State
   * @return
   */
  State getState() const;

  /**
   * @brief Change the stylesheet based on the widget state
   * @param styleType
   */
  void setState(State styleType);

  /**
   * @brief emits the appropriate requirements and sets the state to active
   */
  void emitRequirements();

  /**
   * @brief Returns the X margin
   * @return
   */
  int getXMargin() const;

  /**
   * @brief Returns the Y margin
   * @return
   */
  int getYMargin() const;

  /**
   * @brief Returns the contents rect after styling.
   * @return
   */
  QRect getStyledContentsRect() const;

  /**
   * @brief Returns the contents rect after styling.
   * @return
   */
  QRect getStyledBorderRect() const;

  /**
   * @brief Returns the updated size hint based on the superclass's size hint
   */
  QSize updatedSizeHint(QSize sizeHint) const;

  /**
   * @brief Handles the mouse-enter event and emits the appropriate viewPaths signal
   * @param event
   */
  void enterEvent(QEvent* event) override;

  /**
   * @brief Handles the mouse-leave event and emits the endViewPaths signal
   * @param event
   */
  void leaveEvent(QEvent* event) override;

  /**
   * @brief Checks DataArray requirements against the given DataArrayPath
   * @param path
   * @return
   */
  bool checkDataArrayReqs(const DataArrayPath& path) const;

  /**
   * @brief dragEnterEvent
   * @param event
   */
  void dragEnterEvent(QDragEnterEvent* event) override;

  /**
   * @brief dragLeaveEvent
   * @param event
   */
  void dragLeaveEvent(QDragLeaveEvent* event) override;

  /**
   * @brief dropEvent
   * @param event
   */
  void dropEvent(QDropEvent* event) override;

  /**
   * @brief Creates and returns the header part of the tooltip HTML
   * @return
   */
  QString createTooltipHeader() const;

  /**
   * @brief Creates and returns the closing part of the tooltip HTML
   * @return
   */
  QString createTooltipFooter() const;

  /**
   * @brief Create the IGeometry::Type requirements part of the tooltip
   * @param geomTypes
   * @return
   */
  QString createGeomReqString(const IGeometry::Types& geomTypes) const;

  /**
   * @brief Create the AttributeMatrix::Type requirements part of the tooltip
   * @param amTypes
   * @return
   */
  QString createAttrMatrixReqString(const AttributeMatrix::Types& amTypes) const;

  /**
   * @brief Create the DataArray type requirements part of the tooltip
   * @param daTypes
   * @return
   */
  QString createDataArrayTypeString(const std::vector<QString>& daTypes) const;

  /**
   * @brief Create the component requirements part of the tooltip
   * @param compDims
   * @return
   */
  QString createComponentReqString(std::vector<std::vector<size_t>> compDims) const;

  /**
   * @brief mouseMoveEvent
   * @param event
   */
  void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

  /**
   * @brief Returns the color code for the given State
   * @param state
   */
  const QColor getBorderColor(State state) const;

  /**
   * @brief Override the paint event to mark the DataType required
   * @param event
   */
  void paintEvent(QPaintEvent* event) override;

  /**
   * @brief Creates the legacy selection menu
   * @return
   */
  QMenu* createSelectionMenu();

  /**
   * @brief Returns true if the given path is created by this filter. Returns false otherwise.
   * @param path
   * @return
   */
  bool isCreatedPath(const DataArrayPath& path) const;

protected Q_SLOTS:
  /**
   * @brief Show the legacy menu when right-clicking
   * @param pos
   */
  void showContextMenu(const QPoint& pos);

  /**
   * @brief Emits the appropriate signal when the check state is changed
   * @param checked
   */
  void updateCheckState(bool checked);

private Q_SLOTS:
  /**
   * @brief mousePressEvent
   * @param event
   */
  void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
  DataArrayPathHelper::DataType m_DataType = DataArrayPathHelper::DataType::None;
  State m_State = State::Normal;
  AbstractFilter* m_Filter = nullptr;
  DataContainerSelectionFilterParameter::RequirementType m_DataContainerReqs;
  AttributeMatrixSelectionFilterParameter::RequirementType m_AttrMatrixReqs;
  DataArraySelectionFilterParameter::RequirementType m_DataArrayReqs;
  QPoint m_StartPos;
  QString m_PropName;
  QMenu* m_SelectionMenu = nullptr;
  bool m_FilteringPassed = false;
  DataArrayPathSelectionWidget* m_LockedSelection = nullptr;

  void performDrag();
};

Q_DECLARE_METATYPE(DataArrayPathSelectionWidget::State)

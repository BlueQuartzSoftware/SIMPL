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
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolButton>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDropEvent>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"

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
  static const QPixmap CreateDragIcon(DataArrayPath path);
  static const QPixmap CreateDragIcon(QString text, DataArrayPath::DataType dataType);
  static const QString GetActiveColor(DataArrayPath::DataType type);
  static bool CheckPathRequirements(AbstractFilter* filter, DataArrayPath path, DataContainerSelectionFilterParameter::RequirementType reqs);
  static bool CheckPathRequirements(AbstractFilter* filter, DataArrayPath path, AttributeMatrixSelectionFilterParameter::RequirementType reqs);
  static bool CheckPathRequirements(AbstractFilter* filter, DataArrayPath path, DataArraySelectionFilterParameter::RequirementType reqs);

  DataArrayPathSelectionWidget(QWidget* parent = nullptr);
  virtual ~DataArrayPathSelectionWidget() = default;

  /**
  * @brief Returns the type of DataArrayPath this widget can handle
  * @return
  */
  DataArrayPath::DataType getDataType();

  /**
  * @brief Sets the DataContainer requirements and sets the DataType to DataContainer
  * @param dcReqs
  */
  void setDataContainerRequirements(DataContainerSelectionFilterParameter::RequirementType dcReqs);

  /**
  * @brief Sets the AttributeMatrix requirements and sets the DataType to AttributeMatrix
  * @param amReqs
  */
  void setAttrMatrixRequirements(AttributeMatrixSelectionFilterParameter::RequirementType amReqs);

  /**
  * @brief Sets the DataArray requirements and sets the DataType to DataArray
  * @param daReqs
  */
  void setDataArrayRequirements(DataArraySelectionFilterParameter::RequirementType daReqs);

  /**
  * @brief Returns the DataContainer requirements
  * @return
  */
  DataContainerSelectionFilterParameter::RequirementType getDataContainerRequirements();

  /**
  * @brief Returns the AttributeMatrix requirements
  * @return
  */
  AttributeMatrixSelectionFilterParameter::RequirementType getAttrMatrixRequirements();

  /**
  * @brief Returns the DataArray requirements
  * @return
  */
  DataArraySelectionFilterParameter::RequirementType getDataArrayRequirements();

  /**
  * @brief Checks the requirements for the current path
  * @return
  */
  bool checkCurrentPath();

  /**
  * @brief Checks requirements against the given DataArrayPath
  * @param path
  * @return
  */
  bool checkPathReqs(DataArrayPath path);

  /**
  * @brief Checks DataContainer requirements against the given DataArrayPath
  * @param path
  * @return
  */
  bool checkDataContainerReqs(DataArrayPath path);

  /**
  * @brief Checks AttributeMatrix requirements against the given DataArrayPath
  * @param path
  * @return
  */
  bool checkAttributeMatrixReqs(DataArrayPath path);

  /**
  * @brief Sets the DataArrayPath without checking requirements
  * @param dap
  */
  void setDataArrayPath(DataArrayPath dap);

  /**
  * @brief Returns the DataArrayPath
  * @retrurn
  */
  DataArrayPath getDataArrayPath();

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
  * @brief Returns the property name set for this widget
  * @return
  */
  QString getPropertyName();

  /**
  * @brief Sets the property name
  * @param propName
  */
  void setPropertyName(QString propName);

public slots:
  /**
  * @brief Sets whether or not filtering is forced active or not
  * @param active
  */
  void setPathFiltering(bool active);

  /**
  * @brief Resets the styling and clears the check state
  */
  void resetStyle();

signals:
  void filterPath(DataArrayPath path);
  void viewPathsMatchingReqs(DataContainerSelectionFilterParameter::RequirementType dcReqs);
  void viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType amReqs);
  void viewPathsMatchingReqs(DataArraySelectionFilterParameter::RequirementType daReqs);
  void endViewPaths();
  void pathChanged();

protected:
  enum class Style
  {
    Normal,
    Active,
    NotFound,
    DragEnabled,
    DragDisabled
  };

  void setupGui();

  /**
  * @brief Change the stylesheet based on the widget state
  * @param styleType
  */
  void changeStyleSheet(Style styleType);

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
  bool checkDataArrayReqs(DataArrayPath path);

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
  QString createTooltipHeader();

  /**
   * @brief Creates and returns the closing part of the tooltip HTML
   * @return
   */
  QString createTooltipFooter();

  /**
  * @brief Create the IGeometry::Type requirements part of the tooltip
  * @param geomTypes
  * @return
  */
  QString createGeomReqString(QVector<IGeometry::Type> geomTypes);

  /**
  * @brief Create the AttributeMatrix::Type requirements part of the tooltip
  * @param amTypes
  * @return
  */
  QString createAttrMatrixReqString(QVector<AttributeMatrix::Type> amTypes);

  /**
  * @brief Create the DataArray type requirements part of the tooltip
  * @param daTypes
  * @return
  */
  QString createDataArrayTypeString(QVector<QString> daTypes);

  /**
  * @brief Create the component requirements part of the tooltip
  * @param compDims
  * @return
  */
  QString createComponentReqString(QVector<QVector<size_t>> compDims);

  /**
  * @brief mouseMoveEvent
  * @param event
  */
  void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

  /**
  * @brief Returns the color code for the given Style
  * @param style
  */
  const QString getColor(Style style);

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
  bool isCreatedPath(DataArrayPath path);

protected slots:
  /**
  * @brief Show the legacy menu when right-clicking
  * @param pos
  */
  void showContextMenu(const QPoint& pos);

private slots:
  /**
  * @brief mousePressEvent
  * @param event
  */
  void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
  DataArrayPath::DataType m_DataType = DataArrayPath::DataType::None;
  Style m_Style = Style::Normal;
  AbstractFilter* m_Filter = nullptr;
  DataContainerSelectionFilterParameter::RequirementType m_DataContainerReqs;
  AttributeMatrixSelectionFilterParameter::RequirementType m_AttrMatrixReqs;
  DataArraySelectionFilterParameter::RequirementType m_DataArrayReqs;
  QPoint m_StartPos;
  QString m_PropName;
  QMenu* m_SelectionMenu = nullptr;

  void performDrag();
};

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

#include <QtWidgets/QToolButton>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"

/**
* @class DataArrayPathSelectionWidget DataArrayPathSelectionWidget.h 
* SVWidgetsLib/Widgets/DataArrayPathSelectionWidget.h
* @brief This class handles DataArrayPath selection and mouse over signals for filter parameter widgets.
*/
class DataArrayPathSelectionWidget : public QToolButton
{
  Q_OBJECT

public:
  enum class DataType
  {
    DataContainer,
    AttributeMatrix,
    DataArray,
    None
  };

  DataArrayPathSelectionWidget(QWidget* parent = nullptr);
  virtual ~DataArrayPathSelectionWidget() = default;

  /**
  * @brief Returns the type of DataArrayPath this widget can handle
  * @return
  */
  DataType getDataType();

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
  * @brief Sets the DataArrayPath without checking requirements
  * @param dap
  */
  void setPath(DataArrayPath dap);

  /**
  * @brief Returns the DataArrayPath
  * @retrurn
  */
  DataArrayPath getPath();

signals:
  void viewPathsMatchingReqs(DataContainerSelectionFilterParameter::RequirementType dcReqs);
  void viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType amReqs);
  void viewPathsMatchingReqs(DataArraySelectionFilterParameter::RequirementType daReqs);
  void endViewPaths();

protected:
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

private:
  DataType m_DataType = DataType::None;
  DataContainerSelectionFilterParameter::RequirementType m_DataContainerReqs;
  AttributeMatrixSelectionFilterParameter::RequirementType m_AttrMatrixReqs;
  DataArraySelectionFilterParameter::RequirementType m_DataArrayReqs;
};

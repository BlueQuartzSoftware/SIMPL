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

#include <QtCore/QModelIndex>

#include <QtWidgets/QStyledItemDelegate>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class DataStructureItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  explicit DataStructureItemDelegate(QObject* parent = 0);

  ~DataStructureItemDelegate() override;

  /**
   * @brief Sets the active filter for determining which paths are newly created
   * and getting the DataContainerArray to deal with requirements
   * @param filter
   */
  void setActiveFilter(AbstractFilter::Pointer filter);

  /**
   * @brief Sets the filter requirements
   * @param reqs
   */
  void setViewRequirements(DataContainerSelectionFilterParameter::RequirementType reqs);

  /**
   * @brief Sets the filter requirements
   * @param reqs
   */
  void setViewRequirements(AttributeMatrixSelectionFilterParameter::RequirementType reqs);

  /**
   * @brief Sets the filter requirements
   * @param reqs
   */
  void setViewRequirements(DataArraySelectionFilterParameter::RequirementType reqs);

  /**
   * @brief Clears the filter requirements
   */
  void clearRequirements();

  /**
   * @brief Returns true if the view is being filtered.  Returns false otherwise.
   * @return
   */
  bool isFiltered();

  /**
   * @brief Returns true if the given path matches the current requirements. Returns false otherwise.
   * @param path
   * @return
   */
  bool pathMatchesReqs(DataArrayPath path) const;

  /**
   * @brief Returns true if the given index matches the current requirements. Returns false otherwise.
   * @param index
   * @return
   */
  bool indexMatchesReqs(QModelIndex index) const;

protected:
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
  void setEditorData(QWidget* editor, const QModelIndex& index) const Q_DECL_OVERRIDE;
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const Q_DECL_OVERRIDE;
  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;

  /**
   * @brief Returns the DataArrayPath for the given index
   * @param index
   * @return
   */
  DataArrayPath getDataArrayPath(const QModelIndex& index) const;

  /**
  * @brief Create colored icons for new DataArrayPath from the base image
  */
  void createNewPathIcons();

private:
  DataStructureItemDelegate(const DataStructureItemDelegate&) = delete; // Copy Constructor Not Implemented
  void operator=(const DataStructureItemDelegate&) = delete;            // Move assignment Not Implemented

  AbstractFilter::Pointer m_Filter = nullptr;
  std::list<DataArrayPath> m_CreatedPaths;
  DataArrayPath::DataType m_ReqType = DataArrayPath::DataType::None;
  DataContainerSelectionFilterParameter::RequirementType m_DcReqs;
  AttributeMatrixSelectionFilterParameter::RequirementType m_AmReqs;
  DataArraySelectionFilterParameter::RequirementType m_DaReqs;
  QIcon m_CreatedDcIcon;
  QIcon m_CreatedAmIcon;
  QIcon m_CreatedDaIcon;
  QIcon m_CreatedInvalidIcon;
};


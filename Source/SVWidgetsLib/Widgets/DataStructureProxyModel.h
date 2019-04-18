/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5280
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QSortFilterProxyModel>

#include "SVWidgetsLib/SVWidgetsLib.h"

/**
 * @class DataStructureProxyModel DataStructureProxyModel.h SVWidgetsLib/Widgets/DataStructureProxyModel.h
 * @brief The DataStructureProxyModel class is used for searching the Data Structure.
 * Items are shown if they match the requirements or one of their children do.
 * This is done so that if the user is searching for a DataArray, the DataContainer
 * and AttributeMatrix the DataArray is nested within are visible.  If any of its
 * parent containers are hidden, so is the target DataArray.  This behavior is not
 * consistent with QSortFilterProxyModel from which this class is derived.
 */
class SVWidgetsLib_EXPORT DataStructureProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  DataStructureProxyModel(QObject* parent = nullptr);
  ~DataStructureProxyModel() override;

protected:
  /**
   * @brief Determines whether or not the given row should be displayed in the view.
   * The row and parent provided should be from the source model.
   * @param row
   * @param parent
   * @return
   */
  bool filterAcceptsRow(int row, const QModelIndex& parent) const override;
};

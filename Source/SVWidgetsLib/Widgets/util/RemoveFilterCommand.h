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
#include <QtCore/QUuid>
#include <QtCore/QVariant>

#include <QtWidgets/QUndoCommand>

#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class PipelineModel;
class SVPipelineView;

class SVWidgetsLib_EXPORT RemoveFilterCommand : public QUndoCommand
{
public:
  RemoveFilterCommand(AbstractFilter::Pointer filter, SVPipelineView* view, QString actionText, bool useAnimationOnFirstRun = true, QUndoCommand* parent = 0);
  RemoveFilterCommand(std::vector<AbstractFilter::Pointer> filters, SVPipelineView* view, QString actionText, bool useAnimationOnFirstRun = true, QUndoCommand* parent = 0);
  ~RemoveFilterCommand() override;

  void undo() override;

  void redo() override;

private:
  SVPipelineView* m_PipelineView = nullptr;
  std::vector<AbstractFilter::Pointer> m_Filters;
  std::vector<int> m_FilterRows;
  bool m_FirstRun = true;
  bool m_UseAnimationOnFirstRun = true;

  /**
   * @brief addFilter
   * @param filter
   * @param insertionIndex
   */
  void addFilter(AbstractFilter::Pointer filter, int insertionIndex = -1);

  /**
   * @brief removeFilter
   * @param row
   */
  void removeFilter(AbstractFilter::Pointer filter);

  /**
   * @brief connectFilterSignalsSlots
   * @param filter
   */
  void connectFilterSignalsSlots(AbstractFilter::Pointer filter);

  /**
   * @brief disconnectFilterSignalsSlots
   * @param filter
   */
  void disconnectFilterSignalsSlots(AbstractFilter::Pointer filter);

public:
  RemoveFilterCommand(const RemoveFilterCommand&) = delete; // Copy Constructor Not Implemented
  RemoveFilterCommand(RemoveFilterCommand&&) = delete;      // Move Constructor Not Implemented
  RemoveFilterCommand& operator=(const RemoveFilterCommand&) = delete; // Copy Assignment Not Implemented
  RemoveFilterCommand& operator=(RemoveFilterCommand&&) = delete;      // Move Assignment Not Implemented
};


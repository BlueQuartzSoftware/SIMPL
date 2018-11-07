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

#include <QtCore/QVariant>

#include <QtWidgets/QUndoCommand>

#include <SIMPLib/Filtering/AbstractFilter.h>

#include "SVWidgetsLib/SVWidgetsLib.h"

class SVPipelineView;
class PipelineModel;

/**
 * @brief The MoveFilterCommand class
 */
class SVWidgetsLib_EXPORT MoveFilterCommand : public QUndoCommand
{
public:

    /**
   * @brief MoveFilterCommand
   * @param filter
   * @param view
   * @param insertIndex
   * @param parent
   */
  MoveFilterCommand(AbstractFilter::Pointer filter, SVPipelineView* view, int insertIndex, QUndoCommand* parent = nullptr);

  /**
   * @brief MoveFilterCommand
   * @param filters
   * @param view
   * @param insertIndex
   * @param parent
   */
  MoveFilterCommand(std::vector<AbstractFilter::Pointer> filters, SVPipelineView* view, int insertIndex, QUndoCommand* parent = nullptr);

  ~MoveFilterCommand() override;

  /**
   * @brief undo
   */
  void undo() override;

  /**
   * @brief redo
   */
  void redo() override;

private:
  std::vector<AbstractFilter::Pointer> m_Filters;
  SVPipelineView* m_PipelineView = nullptr;
  int m_InsertIndex;
  bool m_FirstRun = true;

  /**
   * @brief addFilter
   * @param filter
   * @param insertionIndex
   */
  void addFilter(AbstractFilter::Pointer filter, int insertionIndex);

  /**
   * @brief removeFilter
   * @param filterIndex
   */
  void removeFilter(int filterIndex);

public:
  MoveFilterCommand(const MoveFilterCommand&) = delete; // Copy Constructor Not Implemented
  MoveFilterCommand(MoveFilterCommand&&) = delete;      // Move Constructor Not Implemented
  MoveFilterCommand& operator=(const MoveFilterCommand&) = delete; // Copy Assignment Not Implemented
  MoveFilterCommand& operator=(MoveFilterCommand&&) = delete;      // Move Assignment Not Implemented
};


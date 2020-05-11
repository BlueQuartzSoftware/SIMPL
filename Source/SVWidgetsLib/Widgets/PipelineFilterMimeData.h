/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QMimeData>

#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class SVWidgetsLib_EXPORT PipelineFilterMimeData : public QMimeData
{
  Q_OBJECT

public:
  PipelineFilterMimeData();
  ~PipelineFilterMimeData() override;

  using FilterDragMetadata = std::pair<AbstractFilter::Pointer, int>;
  using FilterDragMetadataContainer = std::vector<PipelineFilterMimeData::FilterDragMetadata>;

  /**
   * @brief Setter property for FilterDragData
   */
  void setFilterDragData(const PipelineFilterMimeData::FilterDragMetadataContainer& value);
  /**
   * @brief Getter property for FilterDragData
   * @return Value of FilterDragData
   */
  PipelineFilterMimeData::FilterDragMetadataContainer getFilterDragData() const;

public:
  PipelineFilterMimeData(const PipelineFilterMimeData&) = delete;            // Copy Constructor Not Implemented
  PipelineFilterMimeData(PipelineFilterMimeData&&) = delete;                 // Move Constructor Not Implemented
  PipelineFilterMimeData& operator=(const PipelineFilterMimeData&) = delete; // Copy Assignment Not Implemented
  PipelineFilterMimeData& operator=(PipelineFilterMimeData&&) = delete;      // Move Assignment Not Implemented

private:
  PipelineFilterMimeData::FilterDragMetadataContainer m_FilterDragData = {};
};

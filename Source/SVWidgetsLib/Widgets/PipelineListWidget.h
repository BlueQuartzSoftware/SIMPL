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

#include <QtWidgets/QFrame>

#include "SVWidgetsLib/SVWidgetsLib.h"

#include "ui_PipelineListWidget.h"

class SVWidgetsLib_EXPORT PipelineListWidget : public QFrame, private Ui::PipelineListWidget
{
    Q_OBJECT

  public:
    PipelineListWidget(QWidget* parent = nullptr);
    ~PipelineListWidget() override;

    /**
     * @brief getPipelineView
     * @return
     */
    SVPipelineView* getPipelineView();

    /**
     * @brief setProgressValue
     * @param percent
     */
    void setProgressValue(float percent);

  public slots:
    /**
     * @brief on_startPipelineBtn_clicked
     */
    void on_startPipelineBtn_clicked();

    /**
     * @brief preflightFinished
     */
    void preflightFinished(FilterPipeline::Pointer pipeline, int err);

    /**
     * @brief pipelineFinished
     */
    void pipelineFinished();

  protected:

    /**
     * @brief setupGui
     */
    void setupGui();

  signals:
    void pipelineCanceled(const QModelIndex &pipelineIndex);

  private:

    /**
     * @brief getStartPipelineIdleStyle
     * @return
     */
    QString getStartPipelineIdleStyle();

    /**
     * @brief getStartPipelineInProgressStyle
     * @param percent
     * @return
     */
    QString getStartPipelineInProgressStyle(float percent);

    /**
     * @brief Sets the PipelineState and updates the UI to match
     * @param state
     */
    void setPipelineState(SVPipelineView::PipelineViewState state);

  public:
    PipelineListWidget(const PipelineListWidget&) = delete; // Copy Constructor Not Implemented
    PipelineListWidget(PipelineListWidget&&) = delete;      // Move Constructor Not Implemented
    PipelineListWidget& operator=(const PipelineListWidget&) = delete; // Copy Assignment Not Implemented
    PipelineListWidget& operator=(PipelineListWidget&&) = delete;      // Move Assignment Not Implemented
};


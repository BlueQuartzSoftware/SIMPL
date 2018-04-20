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
* Neither the name of BlueQuartz Software nor the names of its
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
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _simplcontroller_h_
#define _simplcontroller_h_

#include <QtCore/QObject>
#include <QtCore/QStack>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QSignalMapper>

#include <QtWidgets/QUndoStack>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class PipelineModel;

class SVWidgetsLib_EXPORT SIMPLController : public QObject
{
    Q_OBJECT

  public:
    SIMPL_TYPE_MACRO(SIMPLController)

    SIMPLController(QObject* parent = 0);
    ~SIMPLController();

    /**
     * @brief Determines whether or not the pipeline that begins at the specified index is currently running
     * @param pipelineIndex
     * @return
     */
    bool isPipelineCurrentlyRunning(const QModelIndex &pipelineIndex);

  public slots:
    /**
     * @brief preflightPipeline
     * @param pipelineIndex
     */
    void preflightPipeline(const QModelIndex &pipelineIndex, PipelineModel *model);

    /**
     * @brief runPipeline
     * @param pipelineIndex
     * @param model
     */
    void runPipeline(const QModelIndex &pipelineIndex, PipelineModel *model);

    /**
     * @brief cancelPipeline
     * @param pipelineIndex
     */
    void cancelPipeline(const QModelIndex &pipelineIndex);

    /**
     * @brief finishPipeline
     * @param pipelineIndex
     */
    void finishPipeline(const QModelIndex &pipelineIndex);

    /**
     * @brief Should be block this class from either emitting a preflight signal or otherwise running a preflight.
     * @param b
     */
    void blockPreflightSignals(bool b);

  protected slots:
    /**
     * @brief processPipelineMessage
     * @param msg
     */
    void processPipelineMessage(const PipelineMessage& msg);

  signals:
    void statusMessageGenerated(const QString &msg);
    void standardOutputMessageGenerated(const QString &msg);

    void undoActionGenerated(QAction* actionUndo);
    void redoActionGenerated(QAction* actionRedo);

    void displayIssuesTriggered();
    void clearIssuesTriggered();

    void writeSIMPLViewSettingsTriggered();

    void pipelineReady(const QModelIndex &pipelineIndex);

    void pipelineMessageGenerated(const PipelineMessage& msg);

    void filtersAddedToModel(QModelIndexList filterIndices);

    void preflightFinished(const QModelIndex &pipelineIndex, int err);

    void pipelineStarted(const QModelIndex &pipelineIndex);
    void pipelineFinished(const QModelIndex &pipelineIndex);

  private:
    QThread*                                          m_WorkerThread = nullptr;
    QModelIndexList                                   m_CurrentlyRunningPipelines;
    FilterPipeline::Pointer                           m_PipelineInFlight;
    QVector<DataContainerArray::Pointer>              m_PreflightDataContainerArrays;
    QSignalMapper*                                    m_PipelineSignalMapper;
    bool                                              m_BlockPreflight = false;
    QStack<bool>                                      m_BlockPreflightStack;

    QString                                           m_CurrentUndoText = "";
    QString                                           m_CurrentRedoText = "";
    QString                                           m_PreviousUndoText = "";
    QString                                           m_PreviousRedoText = "";

    SIMPLController(const SIMPLController&);    // Copy Constructor Not Implemented
    void operator=(const SIMPLController&);  // Operator '=' Not Implemented
};

#endif // _simplcontroller_h_

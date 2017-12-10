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
     * @brief setupUndoStack
     */
    void setupUndoStack();

    /**
     * @brief getFilterPipeline
     * @param pipelineIndex
     * @param model
     * @return
     */
    FilterPipeline::Pointer getFilterPipeline(const QModelIndex &pipelineIndex, PipelineModel *model);

    /**
     * @brief addPipelineMessageObserver
     * @param pipelineMessageObserver
     */
    void addPipelineMessageObserver(QObject* pipelineMessageObserver);

    /**
     * @brief addFilterToModel
     * @param filter
     * @param model
     * @param parentIndex
     */
    void addFilterToModel(AbstractFilter::Pointer filter, PipelineModel* model, const QModelIndex &parentIndex = QModelIndex(), int insertionIndex = -1);

    /**
     * @brief addPipelineToModel
     * @param pipelineName
     * @param pipeline
     * @param model
     * @param setAsActive
     */
    void addPipelineToModel(const QString &pipelineName, FilterPipeline::Pointer pipeline, PipelineModel *model, bool setAsActive = false, QModelIndex parentIndex = QModelIndex(), int insertionIndex = -1);

    /**
     * @brief fromJsonObject
     * @param modelObject
     * @param model
     * @return
     */
    PipelineModel* fromJsonObject(QJsonObject modelObject, PipelineModel* model);

    /**
     * @brief toJsonObject
     * @param model
     * @return
     */
    QJsonObject toJsonObject(PipelineModel *model);

    /**
     * @brief getActivePipelineIndex
     * @return
     */
    QModelIndex getActivePipelineIndex();

  public slots:

    /**
     * @brief addPipelineToModelFromFile
     * @param filePath
     * @param model
     * @param parentIndex
     * @param insertionIndex
     * @return
     */
    int addPipelineToModelFromFile(const QString& filePath, PipelineModel* model, const QModelIndex &parentIndex = QModelIndex(), int insertionIndex = -1);

    /**
     * @brief preflightPipeline
     * @param pipelineIndex
     */
    void preflightPipeline(const QModelIndex &pipelineIndex, PipelineModel* model);

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
     * @brief updateActivePipeline
     * @param pipelineIdx
     * @param model
     */
    void updateActivePipeline(const QModelIndex &pipelineIdx, PipelineModel *model);

    /**
     * @brief Should be block this class from either emitting a preflight signal or otherwise running a preflight.
     * @param b
     */
    void blockPreflightSignals(bool b);

    /**
     * @brief addUndoCommand
     * @param cmd
     */
    void addUndoCommand(QUndoCommand* cmd);

    /**
     * @brief undo
     */
    void undo();

    /**
     * @brief redo
     */
    void redo();

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

    void clearIssuesTriggered();
    void displayIssuesTriggered();

    void writeSIMPLViewSettingsTriggered();

    void pipelineEnteringReadyState(const QModelIndex &pipelineIndex);
    void pipelineEnteringRunningState(const QModelIndex &pipelineIndex);
    void pipelineEnteringStoppedState(const QModelIndex &pipelineIndex);

    void pipelineMessageGenerated(const PipelineMessage& msg);

    void filtersAddedToModel(QModelIndexList filterIndices);

    void preflightFinished(int err);

    void pipelineCanceled();
    void pipelineFinished();

  private:
    QThread*                                          m_WorkerThread = nullptr;
    FilterPipeline::Pointer                           m_PipelineInFlight;
    QVector<DataContainerArray::Pointer>              m_PreflightDataContainerArrays;
    QSignalMapper*                                    m_PipelineSignalMapper;
    bool                                              m_BlockPreflight = false;
    QStack<bool>                                      m_BlockPreflightStack;
    QList<QObject*>                                   m_PipelineMessageObservers;

    QSharedPointer<QUndoStack>                        m_UndoStack;
    QString                                           m_CurrentUndoText = "";
    QString                                           m_CurrentRedoText = "";
    QString                                           m_PreviousUndoText = "";
    QString                                           m_PreviousRedoText = "";

    QPersistentModelIndex                             m_ActivePipelineIndex;

    /**
     * @brief getPipelineFromFile
     * @param filePath
     * @return
     */
    FilterPipeline::Pointer getPipelineFromFile(const QString& filePath);

    /**
     * @brief wrapModel
     * @param index
     * @return
     */
    QJsonObject wrapModel(QModelIndex index, PipelineModel* model);

    /**
     * @brief unwrapModel
     * @param objectName
     * @param object
     * @param model
     * @param parentIndex
     */
    void unwrapModel(QString objectName, QJsonObject object, PipelineModel* model, QModelIndex parentIndex);

    SIMPLController(const SIMPLController&);    // Copy Constructor Not Implemented
    void operator=(const SIMPLController&);  // Operator '=' Not Implemented
};

#endif // _simplcontroller_h_

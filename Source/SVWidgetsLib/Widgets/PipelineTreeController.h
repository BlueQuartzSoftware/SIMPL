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

#ifndef _PipelineTreeController_h_
#define _PipelineTreeController_h_

#include <QtCore/QObject>
#include <QtCore/QStack>
#include <QtCore/QPersistentModelIndex>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class PipelineTreeModel;

class SVWidgetsLib_EXPORT PipelineTreeController : public QObject
{
    Q_OBJECT

  public:
    SIMPL_TYPE_MACRO(PipelineTreeController)

    PipelineTreeController(QObject* parent = 0);
    ~PipelineTreeController();

    /**
     * @brief getFilterPipeline
     * @param pipelineIndex
     * @param model
     * @return
     */
    FilterPipeline::Pointer getFilterPipeline(const QModelIndex &pipelineIndex, PipelineTreeModel *model);

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
    void addFilterToModel(AbstractFilter::Pointer filter, PipelineTreeModel* model, const QModelIndex &parentIndex = QModelIndex(), int insertionIndex = -1);

    /**
     * @brief addPipelineToModel
     * @param pipelineName
     * @param pipeline
     * @param model
     * @param setAsActive
     */
    void addPipelineToModel(const QString &pipelineName, FilterPipeline::Pointer pipeline, PipelineTreeModel *model, bool setAsActive = false, QModelIndex parentIndex = QModelIndex(), int insertionIndex = -1);

    /**
     * @brief fromJsonObject
     * @param modelObject
     * @param model
     * @return
     */
    PipelineTreeModel* fromJsonObject(QJsonObject modelObject, PipelineTreeModel* model);

    /**
     * @brief toJsonObject
     * @param model
     * @return
     */
    QJsonObject toJsonObject(PipelineTreeModel *model);

  public slots:

    /**
     * @brief addPipelineToModelFromFile
     * @param filePath
     * @param model
     * @param parentIndex
     * @param insertionIndex
     * @return
     */
    int addPipelineToModelFromFile(const QString& filePath, PipelineTreeModel* model, const QModelIndex &parentIndex = QModelIndex(), int insertionIndex = -1);

    /**
     * @brief preflightPipeline
     * @param pipelineIndex
     */
    void preflightPipeline(const QModelIndex &pipelineIndex, PipelineTreeModel* model);

    /**
     * @brief updateActivePipeline
     * @param pipelineIdx
     * @param model
     */
    void updateActivePipeline(const QModelIndex &pipelineIdx, PipelineTreeModel *model);

    /**
     * @brief Should be block this class from either emitting a preflight signal or otherwise running a preflight.
     * @param b
     */
    void blockPreflightSignals(bool b);

  signals:
    void statusMessageGenerated(const QString &msg);
    void standardOutputMessageGenerated(const QString &msg);

    void pipelineIssuesCleared();

    void filtersAddedToModel(QModelIndexList filterIndices);

    void preflightFinished(int err);

  private:
    bool                                              m_BlockPreflight = false;
    QStack<bool>                                      m_BlockPreflightStack;
    QList<QObject*>                                   m_PipelineMessageObservers;

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
    QJsonObject wrapModel(QModelIndex index, PipelineTreeModel* model);

    /**
     * @brief unwrapModel
     * @param objectName
     * @param object
     * @param model
     * @param parentIndex
     */
    void unwrapModel(QString objectName, QJsonObject object, PipelineTreeModel* model, QModelIndex parentIndex);

    PipelineTreeController(const PipelineTreeController&);    // Copy Constructor Not Implemented
    void operator=(const PipelineTreeController&);  // Operator '=' Not Implemented
};

#endif // _PipelineTreeController_h_

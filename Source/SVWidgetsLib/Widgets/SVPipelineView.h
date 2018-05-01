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

#include <vector>
#include <stack>

#include <QtCore/QSharedPointer>

#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QListView>
#include <QtGui/QPainter>

#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersWriter.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/PipelineView.h"
#include "SVWidgetsLib/Widgets/SVPipelineFilterWidget.h"
#include "SVWidgetsLib/Widgets/SVPipelineFilterOutlineWidget.h"

#include "SVWidgetsLib/QtSupport/QtSFileDragMessageBox.h"

class QScrollArea;
class QContextMenuEvent;
class QLabel;
class QEvent;
class QMenu;
class QAction;
class PipelineFilterObject;
class DataStructureWidget;
class PipelineModel;
class QSignalMapper;

/*
 *
 */
class SVWidgetsLib_EXPORT SVPipelineView : public QListView, public PipelineView
{
    Q_OBJECT

  public:
    typedef std::pair<int, PipelineFilterObject*> IndexedFilterObject;

    SIMPL_INSTANCE_PROPERTY(bool, PipelineIsRunning)

    SIMPL_GET_PROPERTY(QAction*, ActionEnableFilter)
    SIMPL_GET_PROPERTY(QAction*, ActionCut)
    SIMPL_GET_PROPERTY(QAction*, ActionCopy)
    SIMPL_GET_PROPERTY(QAction*, ActionPaste)
    SIMPL_GET_PROPERTY(QAction*, ActionClearPipeline)

    SVPipelineView(QWidget* parent = 0);
    virtual ~SVPipelineView();

    /**
     * @brief addPipelineMessageObserver
     * @param pipelineMessageObserver
     */
    void addPipelineMessageObserver(QObject* pipelineMessageObserver);

    /**
     * @brief filterCount
     * @return
     */
    int filterCount();

    /**
     * @brief openPipeline
     * @param filePath
     * @return
     */
    int openPipeline(const QString& filePath, int insertIndex = -1);

    /**
     * @brief readPipelineFromFile
     * @param filePath
     * @return FilterPipeline::Pointer
     */
    FilterPipeline::Pointer readPipelineFromFile(const QString& filePath);

    /**
     * @brief resetLayout
     */
    void resetLayout();

    /**
     * @brief setModel
     * @param model
     */
    void setModel(QAbstractItemModel* model) override;

    virtual QAction* getActionEnableFilter();

    /**
     * @brief getPipelineModel
     * @return
     */
    PipelineModel* getPipelineModel();

    /**
     * @brief getFilterPipeline
     * @param pipelineIndex
     * @return
     */
    FilterPipeline::Pointer getFilterPipeline();

    /**
     * @brief writePipeline
     * @param outputPath
     * @return
     */
    int writePipeline(const QString &outputPath);

    /**
     * @brief getPipelineMessageObservers
     * @return
     */
    QList<QObject*> getPipelineMessageObservers();

    /**
     * @brief isPipelineCurrentlyRunning
     * @return
     */
    bool isPipelineCurrentlyRunning();

  public slots:
    /**
     * @brief Adds a filter with the specified filterClassName to the current model
     * @param filterClassName
     */
    void addFilterFromClassName(const QString &filterClassName, int insertIndex = -1);

    /**
     * @brief Adds a filter to the current model at insertIndex.  If insertIndex is < 0,
     * the filter gets appended to the end of the model
     * @param filter
     */
    void addFilter(AbstractFilter::Pointer filter, int insertIndex = -1);

    /**
     * @brief Adds multiple filters to the current model.  If insertIndex is < 0,
     * the filters get appended to the end of the model
     * @param filters
     */
    void addFilters(std::vector<AbstractFilter::Pointer> filters, int insertIndex = -1);

    /**
     * @brief Moves a filter to insertIndex in the current model.
     * @param filter
     */
    void moveFilter(AbstractFilter::Pointer filter, int insertIndex);

    /**
     * @brief Moves multiple filters to insertIndex in the current model.
     * @param filters
     */
    void moveFilters(std::vector<AbstractFilter::Pointer> filters, int insertIndex);

    /**
     * @brief Removes filter from the current model
     * @param filter
     */
    void removeFilter(AbstractFilter::Pointer filter);

    /**
     * @brief Removes multiple filters from the current model
     * @param filters
     */
    void removeFilters(std::vector<AbstractFilter::Pointer> filters);

    /**
     * @brief Cuts filter from the current model
     * @param filter
     */
    void cutFilter(AbstractFilter::Pointer filter);

    /**
     * @brief Cuts multiple filters from the current model
     * @param filters
     */
    void cutFilters(std::vector<AbstractFilter::Pointer> filters);

    /**
     * @brief Copies the currently selected filters from the current model into the system clipboard
     */
    void copySelectedFilters();

    /**
     * @brief Pastes filter from the system clipboard to the current model
     * @param filter
     */
    void pasteFilter(AbstractFilter::Pointer filter);

    /**
     * @brief Pastes multiple filters from the system clipboard to the current model
     * @param filters
     */
    void pasteFilters(std::vector<AbstractFilter::Pointer> filters);

    /**
     * @brief preflightPipeline
     * @param pipelineIndex
     */
    void preflightPipeline();

    /**
     * @brief runPipeline
     * @param pipelineIndex
     * @param model
     */
    void executePipeline();

    /**
     * @brief cancelPipeline
     * @param pipelineIndex
     */
    void cancelPipeline();

    /**
     * @brief Should be block this class from either emitting a preflight signal or otherwise running a preflight.
     * @param b
     */
    void blockPreflightSignals(bool b);

    /**
    * @brief clearPipeline
    */
    void clearPipeline();

    /**
     * @brief toReadyState
     */
    void toReadyState();

    /**
     * @brief toRunningState
     */
    void toRunningState();

    /**
     * @brief toStoppedState
     */
    void toStoppedState();

  signals:
    void pipelineDropped(const QString &filePath, PipelineModel* model, const QModelIndex &parentIndex, int insertionIndex);

    void displayIssuesTriggered();
    void clearIssuesTriggered();
    void clearDataStructureWidgetTriggered();

    void writeSIMPLViewSettingsTriggered();

    void addPlaceHolderFilter(QPoint p);
    void removePlaceHolderFilter();

    void preflightFinished(FilterPipeline::Pointer pipeline, int err);
    void preflightHasMessage(PipelineMessage msg);

    void filterParametersChanged(AbstractFilter::Pointer filter);

    void pipelineStarted();
    void pipelineFinished();

    void pipelineHasMessage(const PipelineMessage &msg);
    void pipelineHasErrorsSignal();
    void pipelineHasNoErrors();
    void pipelineFilePathUpdated(const QString &name);
    void pipelineChanged();
    void filePathOpened(const QString &filePath);

    void pipelineFilterObjectSelected(PipelineFilterObject* object);
    void filterInputWidgetNeedsCleared();

    void filterInputWidgetEdited();

    void filterWidgetsDropped(int insertIndex, Qt::KeyboardModifiers modifiers);

    void filterEnabledStateChanged();

    void deleteKeyPressed();

    void contextMenuRequested(SVPipelineView* widget, const QPoint &pos);

    void statusMessage(const QString& message);
    void stdOutMessage(const QString& message);

  protected:
    void setupGui();

    void connectSignalsSlots();

    /**
     * @brief startDrag
     * @param supportedActions
     */
    void startDrag(Qt::DropActions supportedActions) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void setFiltersEnabled(QModelIndexList indexes, bool enabled);
    void setSelectedFiltersEnabled(bool enabled);

    void updateActionEnableFilter();

  protected slots:
    void handleFilterParameterChanged(QUuid id);

    void requestContextMenu(const QPoint& pos);

    /**
     * @brief updatePasteAvailability
     */
    void updatePasteAvailability();

    /**
     * @brief Slot that executes when the delete key gets pressed
     */
    void listenDeleteKeyTriggered();

    void listenCutTriggered();
    void listenCopyTriggered();
    void listenPasteTriggered();
    void listenClearPipelineTriggered();

  private slots:
    /**
     * @brief finishPipeline
     * @param pipelineIndex
     */
    void finishPipeline();

    /**
     * @brief processPipelineMessage
     * @param msg
     */
    void processPipelineMessage(const PipelineMessage& msg);

  private:
    QThread*                                          m_WorkerThread = nullptr;
    FilterPipeline::Pointer                           m_PipelineInFlight;
    QVector<DataContainerArray::Pointer>              m_PreflightDataContainerArrays;
    QList<QObject*>                                   m_PipelineMessageObservers;

    bool                                              m_PipelineRunning = false;

    QUndoCommand*                                     m_DragCommand = nullptr;
    QPoint                                            m_DragStartPosition;
    SVPipelineFilterOutlineWidget*                    m_FilterOutlineWidget = nullptr;
    QPersistentModelIndex                             m_CurrentHoveringIndex;
    bool                                              m_BlockPreflight = false;
    std::stack<bool>                                  m_BlockPreflightStack;
    bool                                              m_LoadingJson = false;

    QAction*                                          m_ActionEnableFilter = nullptr;
    QAction*                                          m_ActionCut = nullptr;
    QAction*                                          m_ActionCopy = nullptr;
    QAction*                                          m_ActionPaste = nullptr;
    QAction*                                          m_ActionClearPipeline = new QAction("Clear Pipeline", this);

    /**
     * @brief Gets the currently selected filters
     * @return
     */
    std::vector<AbstractFilter::Pointer> getSelectedFilters();

    /**
     * @brief requestFilterContextMenu
     * @param pos
     * @param index
     */
    void requestFilterItemContextMenu(const QPoint &pos, const QModelIndex &index);

    /**
     * @brief requestPipelineContextMenu
     * @param pos
     */
    void requestPipelineItemContextMenu(const QPoint &pos);

    /**
     * @brief requestSinglePipelineContextMenu
     * @param menu
     */
    void requestSinglePipelineContextMenu(QMenu &menu);

    /**
     * @brief requestDefaultContextMenu
     * @param pos
     */
    void requestDefaultContextMenu(const QPoint &pos);

    SVPipelineView(const SVPipelineView&) = delete; // Copy Constructor Not Implemented
    void operator=(const SVPipelineView&) = delete;       // Move assignment Not Implemented
};


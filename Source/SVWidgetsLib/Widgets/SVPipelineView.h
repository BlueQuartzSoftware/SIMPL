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

/*
 *
 */
class SVWidgetsLib_EXPORT SVPipelineView : public QListView
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

    /**
     * @brief filterCount
     * @return
     */
    int filterCount();

    /**
     * @brief createFilterObjectFromFilter
     * @param filter
     * @return
     */
    PipelineFilterObject* createFilterObjectFromFilter(AbstractFilter::Pointer filter);

    /**
     * @brief readPipelineFromFile
     * @param filePath
     * @return FilterPipeline::Pointer
     */
    FilterPipeline::Pointer readPipelineFromFile(const QString& filePath);

    /**
     * @brief newEmptyPipelineViewLayout
     */
    void newEmptyPipelineViewLayout();

    /**
     * @brief resetLayout
     */
    void resetLayout();

    /**
     * @brief setDataStructureWidget
     * @param w
     */
    virtual void setDataStructureWidget(DataStructureWidget* w);

    /**
     * @brief getDataStructureWidget
     * @return
     */
    virtual DataStructureWidget* getDataStructureWidget();

    virtual QAction* getActionEnableFilter();

    /**
     * @brief getPipelineModel
     * @return
     */
    PipelineModel* getPipelineModel();

  public slots:
    /**
     * @brief Should be block this class from either emitting a preflight signal or otherwise running a preflight.
     * @param b
     */
    void blockPreflightSignals(bool b);

    /**
     * @brief setSelectedFilterObject
     * @param w
     * @param modifiers
     */
    void setSelectedFilterObject(PipelineFilterObject* w, Qt::KeyboardModifiers modifiers);

    /**
    * @brief clearPipeline
    */
    void clearPipeline(bool addToUndoStack = true);

    /**
     * @brief reindexWidgetTitles
     */
    void reindexWidgetTitles();

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
    void pipelineIssuesCleared();
    void preflightTriggered(const QModelIndex &pipelineIndex, PipelineModel* model);

    void addPlaceHolderFilter(QPoint p);
    void removePlaceHolderFilter();
    void preflightHasMessage(PipelineMessage msg);

    void pipelineHasErrorsSignal();
    void pipelineHasNoErrors();
    void pipelineTitleUpdated(QString name);
    void windowTitleNeedsRefresh();

    void pipelineFilterObjectSelected(PipelineFilterObject* object);
    void filterInputWidgetChanged(FilterInputWidget* widget);
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

    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void setFiltersEnabled(QModelIndexList indexes, bool enabled);
    void setSelectedFiltersEnabled(bool enabled);

    void updateActionEnableFilter();

  protected slots:
    void startDrag(QMouseEvent* event, SVPipelineFilterWidget *fw);

    void handleFilterParameterChanged(QUuid id);

    void on_focusInEventStarted(QFocusEvent* event);
    void on_focusOutEventStarted(QFocusEvent* event);

    void requestContextMenu(const QPoint& pos);

    /**
     * @brief Slot that executes when the delete key gets pressed
     */
    void listenDeleteKeyTriggered();

    void listenCutTriggered();
    void listenCopyTriggered();
    void listenPasteTriggered();
    void listenClearPipelineTriggered();

  private:
    SVPipelineFilterOutlineWidget*                    m_FilterOutlineWidget = nullptr;
    QLabel*                                           m_EmptyPipelineLabel = nullptr;
    bool                                              m_BlockPreflight = false;
    std::stack<bool>                                  m_BlockPreflightStack;
    DataStructureWidget*                              m_DataStructureWidget = nullptr;
    bool                                              m_LoadingJson = false;
    QUndoStack*                                       m_UndoStack = nullptr;
    QAction*                                          m_ActionUndo = nullptr;
    QAction*                                          m_ActionRedo = nullptr;

    QAction*                                          m_ActionEnableFilter = new QAction("Enable", this);
    QAction*                                          m_ActionCut = new QAction("Cut", this);
    QAction*                                          m_ActionCopy = new QAction("Copy", this);
    QAction*                                          m_ActionPaste = new QAction("Paste", this);
    QAction*                                          m_ActionClearPipeline = new QAction("Clear Pipeline", this);

    /**
     * @brief setupUndoStack
     */
    void setupUndoStack();

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


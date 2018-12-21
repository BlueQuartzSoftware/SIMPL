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

#include <stack>
#include <vector>

#include <QtCore/QSharedPointer>

#include <QtGui/QPainter>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersWriter.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/PipelineView.h"

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
class SVWidgetsLib_EXPORT SVPipelineView : public QListView, public PipelineView, public IObserver
{
  Q_OBJECT

public:
  enum class PipelineViewState : int
  {
    Idle = 0,
    Running,
    Cancelling
  };

  using IndexedFilterObject = std::pair<int, PipelineFilterObject*>;

  SIMPL_INSTANCE_PROPERTY(PipelineViewState, PipelineState)

  SIMPL_GET_PROPERTY(QAction*, ActionEnableFilter)
  SIMPL_GET_PROPERTY(QAction*, ActionCut)
  SIMPL_GET_PROPERTY(QAction*, ActionCopy)
  SIMPL_GET_PROPERTY(QAction*, ActionPaste)
  SIMPL_GET_PROPERTY(QAction*, ActionClearPipeline)

  SVPipelineView(QWidget* parent = nullptr);
  ~SVPipelineView() override;

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
   * @brief setModel
   * @param model
   */
  void setModel(QAbstractItemModel* model) override;

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
  int writePipeline(const QString& outputPath);

  /**
   * @brief isPipelineCurrentlyRunning
   * @return
   */
  bool isPipelineCurrentlyRunning();

  /**
   * @brief Returns the regular disable button pixmap in the current highlighted text color
   * @return
   */
  QPixmap getDisableBtnPixmap(bool highlighted = false);

  /**
   * @brief Returns the high-dpi disable button pixmap in the current highlighted text color
   * @return
   */
  QPixmap getHighDPIDisableBtnPixmap(bool highlighted = false);

  /**
   * @brief Returns the regular activated disable button pixmap in the current highlighted text color
   * @return
   */
  QPixmap getDisableBtnActivatedPixmap(bool highlighted = false);

  /**
   * @brief Returns the high-dpi activated disable button pixmap in the current highlighted text color
   * @return
   */
  QPixmap getHighDPIDisableBtnActivatedPixmap(bool highlighted = false);

  /**
   * @brief Returns the regular hovered disable button pixmap in the current highlighted text color
   * @return
   */
  QPixmap getDisableBtnHoveredPixmap(bool highlighted = false);

  /**
   * @brief Returns the high-dpi hovered disable button pixmap in the current highlighted text color
   * @return
   */
  QPixmap getHighDPIDisableBtnHoveredPixmap(bool highlighted = false);

  /**
   * @brief Returns the regular delete button pixmap in the current highlighted text color
   * @return
   */
  QPixmap getDeleteBtnPixmap(bool highlighted = false);

  /**
   * @brief Returns the high-dpi delete button pixmap in the current highlighted text color
   * @return
   */
  QPixmap getHighDPIDeleteBtnPixmap(bool highlighted = false);

  /**
   * @brief Returns the regular hovered delete button pixmap in the current highlighted text color
   * @return
   */
  QPixmap getDeleteBtnHoveredPixmap(bool highlighted = false);

  /**
   * @brief Returns the high-dpi hovered delete button pixmap in the current highlighted text color
   * @return
   */
  QPixmap getHighDPIDeleteBtnHoveredPixmap(bool highlighted = false);

public slots:
  /**
   * @brief Adds a filter with the specified filterClassName to the current model
   * @param filterClassName
   */
  void addFilterFromClassName(const QString& filterClassName, int insertIndex = -1, bool useAnimationOnFirstRun = true);

  /**
   * @brief Adds a filter to the current model at insertIndex.  If insertIndex is < 0,
   * the filter gets appended to the end of the model
   * @param filter
   */
  void addFilter(const AbstractFilter::Pointer& filter, int insertIndex = -1, bool useAnimationOnFirstRun = true);

  /**
   * @brief Adds multiple filters to the current model.  If insertIndex is < 0,
   * the filters get appended to the end of the model
   * @param filters
   */
  void addFilters(const std::vector<AbstractFilter::Pointer>& filters, int insertIndex = -1, bool useAnimationOnFirstRun = true);

  /**
   * @brief Removes filter from the current model
   * @param filter
   */
  void removeFilter(const AbstractFilter::Pointer& filter, bool useAnimationOnFirstRun = true);

  /**
   * @brief Removes multiple filters from the current model
   * @param filters
   */
  void removeFilters(const std::vector<AbstractFilter::Pointer>& filters, bool useAnimationOnFirstRun = true);

  /**
   * @brief Cuts filter from the current model
   * @param filter
   */
  void cutFilter(const AbstractFilter::Pointer& filter, bool useAnimationOnFirstRun = true);

  /**
   * @brief Cuts multiple filters from the current model
   * @param filters
   */
  void cutFilters(const std::vector<AbstractFilter::Pointer>& filters, bool useAnimationOnFirstRun = true);

  /**
   * @brief Copies the currently selected filters from the current model into the system clipboard
   */
  void copySelectedFilters();

  /**
   * @brief Pastes multiple filters from the system clipboard to the current model
   * @param insertIndex
   */
  void pasteFilters(int insertIndex = -1, bool useAnimationOnFirstRun = true);

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
   * @brief Update the indices of all FilterInputWidgets
   */
  void updateFilterInputWidgetIndices();

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

  /**
   * @brief processPipelineMessage
   * @param pm
   */
  void processPipelineMessage(const PipelineMessage& pm) override;

signals:
  void displayIssuesTriggered();
  void clearIssuesTriggered();
  void clearDataStructureWidgetTriggered();

  void writeSIMPLViewSettingsTriggered();

  void addPlaceHolderFilter(QPoint p);
  void removePlaceHolderFilter();

  void preflightFinished(FilterPipeline::Pointer pipeline, int err);

  void filterParametersChanged(AbstractFilter::Pointer filter);

  void pipelineStarted();
  void pipelineFinished();

  void pipelineHasMessage(const PipelineMessage& msg);
  void pipelineFilePathUpdated(const QString& name);
  void pipelineChanged();
  void filePathOpened(const QString& filePath);

  void filterInputWidgetNeedsCleared();

  void filterEnabledStateChanged();

  void deleteKeyPressed();

  void statusMessage(const QString& message);
  void stdOutMessage(const QString& message);

protected:
  void setupGui();

  void connectSignalsSlots();

  /**
   * @brief beginDrag
   * @param event
   */
  void beginDrag(QMouseEvent* event);

  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dropEvent(QDropEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

  void setFiltersEnabled(QModelIndexList indexes, bool enabled);
  void setSelectedFiltersEnabled(bool enabled);

protected slots:
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
   * @brief listenFilterInProgress
   * @param filter
   */
  void listenFilterInProgress(AbstractFilter* filter);

  /**
   * @brief listenFilterCompleted
   */
  void listenFilterCompleted(AbstractFilter* filter);

  /**
   * @brief finishPipeline
   * @param pipelineIndex
   */
  void finishPipeline();

private:
  QThread* m_WorkerThread = nullptr;
  FilterPipeline::Pointer m_PipelineInFlight;
  QVector<DataContainerArray::Pointer> m_PreflightDataContainerArrays;
  QList<QObject*> m_PipelineMessageObservers;

  QUndoCommand* m_MoveCommand = nullptr;
  QPoint m_DragStartPosition;
  QModelIndex m_DropIndicatorIndex;
  bool m_BlockPreflight = false;
  std::stack<bool> m_BlockPreflightStack;

  QAction* m_ActionEnableFilter = nullptr;
  QAction* m_ActionCut = nullptr;
  QAction* m_ActionCopy = nullptr;
  QAction* m_ActionPaste = nullptr;
  QAction* m_ActionClearPipeline = new QAction("Clear Pipeline", this);

  QPixmap m_DisableBtnPixmap;
  QPixmap m_DisableHighlightedPixmap;
  QColor m_DisableBtnColor = QColor(Qt::black);

  QPixmap m_DisableBtnPixmap2x;
  QPixmap m_DisableBtnHighlightedPixmap2x;
  QColor m_DisableBtn2xColor = QColor(Qt::black);

  QPixmap m_DisableBtnActivatedPixmap;
  QPixmap m_DisableBtnActivatedPixmap2x;

  QPixmap m_DisableBtnHoveredPixmap;
  QPixmap m_DisableBtnHoveredHighlightedPixmap;
  QColor m_DisableBtnHoveredColor = QColor(Qt::black);

  QPixmap m_DisableBtnHoveredPixmap2x;
  QPixmap m_DisableBtnHoveredHighlightedPixmap2x;
  QColor m_DisableBtnHovered2xColor = QColor(Qt::black);

  QPixmap m_DeleteBtnPixmap;
  QPixmap m_DeleteBtnHighlightedPixmap;
  QColor m_DeleteBtnColor = QColor(Qt::black);

  QPixmap m_DeleteBtnPixmap2x;
  QPixmap m_DeleteBtnHighlightedPixmap2x;
  QColor m_DeleteBtn2xColor = QColor(Qt::black);

  QPixmap m_DeleteBtnHoveredPixmap;
  QPixmap m_DeleteBtnHoveredHighlightedPixmap;
  QColor m_DeleteBtnHoveredColor = QColor(Qt::black);

  QPixmap m_DeleteBtnHoveredPixmap2x;
  QPixmap m_DeleteBtnHoveredHighlightedPixmap2x;
  QColor m_DeleteBtnHovered2xColor = QColor(Qt::black);

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
  void requestFilterItemContextMenu(const QPoint& pos, const QModelIndex& index);

  /**
   * @brief requestPipelineContextMenu
   * @param pos
   */
  void requestPipelineItemContextMenu(const QPoint& pos);

  /**
   * @brief requestSinglePipelineContextMenu
   * @param menu
   */
  void requestSinglePipelineContextMenu(QMenu& menu);

  /**
   * @brief requestErrorHandlingContextMenu
   * @param menu
   */
  void requestErrorHandlingContextMenu(QMenu& menu);

  /**
   * @brief requestDefaultContextMenu
   * @param pos
   */
  void requestDefaultContextMenu(const QPoint& pos);

  /**
   * @brief addDropIndicator
   * @param text
   * @param insertIndex
   */
  void addDropIndicator(const QString& text, int insertIndex);

  /**
   * @brief removeDropIndicator
   */
  void removeDropIndicator();

  /**
   * @brief findNextRow
   * @param pos
   * @return
   */
  int findNextRow(const QPoint& pos);

  /**
   * @brief findPreviousRow
   * @param pos
   * @return
   */
  int findPreviousRow(const QPoint& pos);

  /**
   * @brief getDraggingPixmap
   * @param indexes
   * @return
   */
  QPixmap getDraggingPixmap(QModelIndexList indexes);

  /**
   * @brief setPixmapColor
   * @param pixmap
   * @param pixmapColor
   */
  QPixmap setPixmapColor(const QPixmap& pixmap, const QColor& pixmapColor);

public:
  SVPipelineView(const SVPipelineView&) = delete; // Copy Constructor Not Implemented
  SVPipelineView(SVPipelineView&&) = delete;      // Move Constructor Not Implemented
  SVPipelineView& operator=(const SVPipelineView&) = delete; // Copy Assignment Not Implemented
  SVPipelineView& operator=(SVPipelineView&&) = delete;      // Move Assignment Not Implemented
};

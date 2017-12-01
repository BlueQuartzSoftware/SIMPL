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


#ifndef _svpipelineviewwidget_h_
#define _svpipelineviewwidget_h_

#include <vector>
#include <stack>

#include <QtCore/QSharedPointer>
#include <QtCore/QTimer>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QStatusBar>
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
/*
 *
 */
class SVWidgetsLib_EXPORT SVPipelineViewWidget : public QFrame, public PipelineView
{
    Q_OBJECT

  public:
    SIMPL_INSTANCE_PROPERTY(bool, PipelineIsRunning)

    SVPipelineViewWidget(QWidget* parent = 0);
    virtual ~SVPipelineViewWidget();

    /**
     * @brief filterCount
     * @return
     */
    int filterCount(QUuid startId = QUuid()) override;

    /**
     * @brief filterObjectAt
     * @param index
     * @return
     */
    PipelineFilterObject* filterObjectAt(QVariant value) override;

    /**
    * @brief containsFilterWidget
    * @param filterWidget
    * @return
    */
    bool containsFilterWidget(PipelineFilterObject* filterWidget) override;

    /**
    * @brief valueOfFilterWidget
    * @param filterWidget
    * @return
    */
    QVariant valueOfFilterWidget(PipelineFilterObject* filterObject) override;

    /**
     * @brief getFilterPipeline
     * @return
     */
    FilterPipeline::Pointer getFilterPipeline(QUuid startId = QUuid()) override;

    /**
     * @brief Returns a FilterPipeline Object with a new filter instance that has the input parameters copied
     * from the filter instance that is embedded in the SVPipelineFilterWidget instance. This function does NOT perform
     * a DEEP copy of the filter.
     * @return
     */
    FilterPipeline::Pointer getCopyOfFilterPipeline() override;

    /**
    * @brief clearSelectedFilterObjects
    */
    void clearSelectedFilterObjects() override;

    /**
     * @brief populatePipelineView
     * @param jsonString
     */
    void populatePipelineView(QString jsonString, QVariant value) override;

    /**
    * @brief getSelectedFilterWidgets
    * @return
    */
    QList<PipelineFilterObject*> getSelectedFilterObjects() override;

    /**
    * @brief getSelectedIndexedFilterObject
    * @return
    */
    QList<IndexedFilterObject> getSelectedIndexedFilterObjects() override;

    /**
     * @brief recheckWindowTitleAndModification
     */
    void recheckWindowTitleAndModification() override;

    /**
     * @brief eventFilter
     * @return
     */
    bool eventFilter(QObject*, QEvent*) override;

    /**
     * @brief createFilterObjectFromFilter
     * @param filter
     * @return
     */
    PipelineFilterObject* createFilterObjectFromFilter(AbstractFilter::Pointer filter) override;

    /**
     * @brief readPipelineFromFile
     * @param filePath
     * @return FilterPipeline::Pointer
     */
    FilterPipeline::Pointer readPipelineFromFile(const QString& filePath);

    /**
     * @brief getJsonFromFile
     * @param filePath
     * @return QString
     */
    QString getJsonFromFile(const QString& filePath);

    /**
    * @brief Write pipeline to a file
    */
    int writePipeline(QString filePath);

    /**
     * @brief setPipelineMessageObserver
     * @param pipelineMessageObserver
     */
    void addPipelineMessageObserver(QObject* pipelineMessageObserver);

    /**
     * @brief setScrollArea
     * @param sa
     */
    void setScrollArea(QScrollArea* sa);

    /**
     * @brief setAutoScroll
     * @param scroll
     */
    void setAutoScroll(bool scroll) {m_AutoScroll = scroll;}

    /**
     * @brief hasAutoScroll
     * @return
     */
    bool hasAutoScroll() {return m_AutoScroll;}

    /**
     * @brief setAutoScrollMargin
     * @param margin
     */
    void setAutoScrollMargin(int margin) {m_AutoScrollMargin = margin;}

    /**
     * @brief getAutoScrollMargin
     * @return
     */
    int getAutoScrollMargin() {return m_AutoScrollMargin;}

    /**
     * @brief stopAutoScroll Stops the time so the Auto Scrolling will cease.
     */
    void stopAutoScroll();

    /**
     * @brief startAutoScroll Starts the QTimer in charge of auto scrolling the view
     */
    void startAutoScroll();

    /**
     * @brief shouldAutoScroll Figures out if the mouse position is in the margin area that would trigger an
     * autoscroll
     * @param pos
     * @return
     */
    bool shouldAutoScroll(const QPoint& pos);

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
    virtual void setDataStructureWidget(DataStructureWidget* w) override;

    /**
     * @brief getDataStructureWidget
     * @return
     */
    virtual DataStructureWidget* getDataStructureWidget();

    virtual QAction* getActionEnableFilter();

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
    void setSelectedFilterObject(PipelineFilterObject* w, Qt::KeyboardModifiers modifiers) override;

    /**
     * @brief preflightPipeline
     */
    void preflightPipeline(QUuid id = QUuid()) override;

    /**
    * @brief Open pipeline to a file
    */
    int openPipeline(const QString& filePath, QVariant value, const bool& setOpenedFilePath, const bool& changeTitle) override;

    /**
     * @brief addSIMPLViewReaderFilter
     * @param filePath
     */
    void addSIMPLViewReaderFilter(const QString& filePath, QVariant value) override;

    /**
    * @brief clearWidgets
    */
    void clearFilterWidgets() override;

    /**
    * @brief addUndoCommand
    */
    void addUndoCommand(QUndoCommand* cmd) override;

    /**
     * @brief reindexWidgetTitles
     */
    void reindexWidgetTitles() override;

    /**
     * @brief doAutoScroll This does the actual scrolling of the Widget
     */
    void doAutoScroll();

    /**
    * @brief toRunningState
    */
    void toRunningState();

    /**
    * @brief toIdleState
    */
    void toIdleState();

    /**
     * @brief getActionRedo
     */
    QAction* getActionRedo();

    /**
     * @brief getActionUndo
     */
    QAction* getActionUndo();

  signals:
    void addPlaceHolderFilter(QPoint p);
    void removePlaceHolderFilter();
    void preflightHasMessage(PipelineMessage msg);

    void pipelineOpened(QString& file, const bool& setOpenedFilePath, const bool& changeTitle);
    void pipelineHasErrorsSignal();
    void pipelineHasNoErrors();
    void pipelineIssuesCleared();
    void pipelineTitleUpdated(QString name);
    void windowNeedsRecheck();

    void pipelineFilterObjectSelected(PipelineFilterObject* object);
    void filterInputWidgetChanged(FilterInputWidget* widget);
    void filterInputWidgetNeedsCleared();

    void filterInputWidgetEdited();
    void preflightPipelineComplete();
    void preflightFinished(int err);

    void filterWidgetsDropped(int insertIndex, Qt::KeyboardModifiers modifiers);

    void filterEnabledStateChanged();

    void deleteKeyPressed(SVPipelineViewWidget* viewWidget);

    void contextMenuRequested(SVPipelineViewWidget* widget, const QPoint &pos);

    void statusMessage(const QString& message);
    void stdOutMessage(const QString& message);

  protected:
    void setupGui();

    int getIndexAtPoint(const QPoint& point);
    SVPipelineFilterWidget* getFilterWidgetAtIndex(int index);

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

    void setFiltersEnabled(QList<PipelineFilterObject*> widgets, bool enabled);
    void setSelectedFiltersEnabled(bool enabled);

    void updateActionEnableFilter();

  protected slots:
    void startDrag(QMouseEvent* event, SVPipelineFilterWidget *fw);

    void handleFilterParameterChanged(QUuid id);

    void on_focusInEventStarted(QFocusEvent* event);
    void on_focusOutEventStarted(QFocusEvent* event);

    void requestContextMenu(const QPoint& pos);

  private slots:
    /**
     * @brief removeFilterWidget
     * @param filterWidget
     */
    void removeFilterObject(PipelineFilterObject* filterObject, bool deleteWidget) override;

    /**
     * @brief removeFilterObject
     * @param filterObject
     */
    void removeFilterObject(PipelineFilterObject* filterObject);

    /**
     * @brief updateCurrentUndoText
     * @param text
     */
    void updateCurrentUndoText(const QString &text);

    /**
     * @brief updateCurrentRedoText
     * @param text
     */
    void updateCurrentRedoText(const QString &text);

    /**
     * @brief actionUndo_triggered
     */
    void actionUndo_triggered();

    /**
     * @brief actionRedo_triggered
     */
    void actionRedo_triggered();

  private:
    SVPipelineFilterWidget*                           m_ShiftStart = nullptr;
    QVBoxLayout*                                      m_FilterWidgetLayout = nullptr;
    int                                               m_FilterOrigPos;
    SVPipelineFilterOutlineWidget*                    m_FilterOutlineWidget = nullptr;
    QLabel*                                           m_EmptyPipelineLabel = nullptr;
    QList<QObject*>                                   m_PipelineMessageObservers;
    QScrollArea*                                      m_ScrollArea = nullptr;
    QTimer                                            m_autoScrollTimer;
    bool                                              m_AutoScroll;
    int                                               m_AutoScrollMargin;
    int                                               m_autoScrollCount;
    QMenu*                                            m_ContextMenu = nullptr;
    QSharedPointer<QUndoStack>                        m_UndoStack;
    QAction*                                          m_ActionUndo = nullptr;
    QAction*                                          m_ActionRedo = nullptr;
    QString                                           m_CurrentUndoText = "";
    QString                                           m_CurrentRedoText = "";
    QString                                           m_PreviousUndoText = "";
    QString                                           m_PreviousRedoText = "";
    bool                                              m_BlockPreflight = false;
    std::stack<bool>                                  m_BlockPreflightStack;
    DataStructureWidget*                              m_DataStructureWidget = nullptr;
    bool                                              m_LoadingJson = false;
    QAction*                                          m_ActionEnableFilter = nullptr;

    /**
     * @brief addFilterObject
     * @param filterObject
     * @param value
     */
    virtual void addFilterObject(PipelineFilterObject* filterObject, QVariant value) override;

    /**
     * @brief removeFilterObjects
     * @param filterObjects
     */
    void removeFilterObjects(QList<PipelineFilterObject*> filterObjects);

    QMenu* createPipelineFilterWidgetMenu(SVPipelineFilterWidget* filterWidget);
    void createPipelineViewWidgetMenu();

    SVPipelineViewWidget(const SVPipelineViewWidget&) = delete; // Copy Constructor Not Implemented
    void operator=(const SVPipelineViewWidget&) = delete;       // Operator '=' Not Implemented
};

#endif /* _svpipelineviewwidget_H */


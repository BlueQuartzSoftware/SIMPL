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

#include <QtCore/QTimer>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QStatusBar>
#include <QtGui/QPainter>


#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersWriter.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/PipelineView.h"
#include "SVWidgetsLib/Widgets/SVPipelineFilterWidget.h"
#include "SVWidgetsLib/Widgets/DropBoxWidget.h"



#include "SVWidgetsLib/QtSupport/QtSFileDragMessageBox.h"

class QScrollArea;
class QContextMenuEvent;
class QLabel;
class QEvent;
class QMenu;
class QAction;

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
     * @param pipeline
     */
    void populatePipelineView(FilterPipeline::Pointer pipeline, QVariant value, bool connectToStart = false) override;

    /**
    * @brief getSelectedFilterWidgets
    * @return
    */
    QList<PipelineFilterObject*> getSelectedFilterObjects() override;

    /**
     * @brief eventFilter
     * @return
     */
    bool eventFilter(QObject*, QEvent*) override;

    /**
     * @brief readPipelineFromFile
     * @param filePath
     * @return FilterPipeline::Pointer
     */
    FilterPipeline::Pointer readPipelineFromFile(const QString& filePath);

    /**
    * @brief Write pipeline to a file
    */
    int writePipeline(QString filePath);

    /**
     * @brief setPipelineMessageObserver
     * @param pipelineMessageObserver
     */
    void setPipelineMessageObserver(QObject* pipelineMessageObserver);

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

  public slots:

    /**
     * @brief addFilter
     * @param filter
     * @param index
     * @param allowUndo
     */
    void addFilter(AbstractFilter::Pointer filter, QVariant value, bool allowUndo = true, bool connectToStart = false) override;

    /**
     * @brief addFilter
     * @param filterClassName
     * @param index
     * param allowUndo
     */
    void addFilter(const QString& filterClassName, QVariant value, bool allowUndo = true, bool connectToStart = false) override;

    /**
     * @brief addFilter
     * @param filter
     * @param index
     * @param allowUndo
     */
    void addFilters(QList<AbstractFilter::Pointer> filters, QVariant value, bool allowUndo = true, bool connectToStart = false) override;

    /**
     * @brief addFilterWidget
     * @param fw
     * @param index
     * @param allowUndo
     */
    void addFilterWidget(PipelineFilterObject* filterObject, QVariant value, bool allowUndo = true) override;

    /**
     * @brief addFilterWidget
     * @param fw
     * @param index
     * @param allowUndo
     */
    void addFilterWidgets(QList<PipelineFilterObject*> filterWidgets, QVariant value, bool allowUndo = true) override;

    /**
     * @brief moveFilterWidget
     * @param fw
     * @param origin
     * @param destination
     */
    void moveFilterWidget(PipelineFilterObject* fw, QVariant origin, QVariant destination, bool allowUndo = true) override;

    /**
     * @brief pasteFilters
     * @param filters
     */
    void pasteFilters(QList<AbstractFilter::Pointer> filters, bool allowUndo = true) override;

    /**
     * @brief pasteFilterWidgets
     * @param jsonString
     * @param index
     * @param allowUndo
     */
    void pasteFilterWidgets(const QString &jsonString, QVariant value, bool allowUndo = true) override;

    /**
     * @brief removeFilterWidget
     * @param filterWidget
     * @param allowUndo
     */
    void removeFilterWidget(PipelineFilterObject* filterObject, bool allowUndo = true, bool deleteWidget = true) override;

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
    int openPipeline(const QString& filePath, QVariant value, const bool& setOpenedFilePath, const bool& changeTitle, bool connectToStart = false) override;

    /**
     * @brief addSIMPLViewReaderFilter
     * @param filePath
     */
    void addSIMPLViewReaderFilter(const QString& filePath, QVariant value) override;

    /**
    * @brief clearWidgets
    */
    void clearWidgets(bool allowUndo = false) override;

    /**
     * @brief reindexWidgetTitles
     */
    void reindexWidgetTitles();

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

  signals:
    void addPlaceHolderFilter(QPoint p);
    void removePlaceHolderFilter();
    void preflightHasMessage(PipelineMessage msg);

    void pipelineOpened(QString& file, const bool& setOpenedFilePath, const bool& changeTitle);
    void pipelineHasErrorsSignal();
    void pipelineHasNoErrors();
    void pipelineIssuesCleared();
    void pipelineTitleUpdated(QString name);
    void pipelineChanged();

    void filterInputWidgetChanged(FilterInputWidget* widget);
    void filterInputWidgetNeedsCleared();

    void filterInputWidgetEdited();
    void preflightPipelineComplete();
    void preflightFinished(int err);

    void filterWidgetsDropped(int insertIndex, Qt::KeyboardModifiers modifiers);

    void deleteKeyPressed(SVPipelineViewWidget* viewWidget);

    void contextMenuRequested(SVPipelineViewWidget* widget, const QPoint &pos);

  protected:
    void setupGui();

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

  protected slots:
    void startDrag(QMouseEvent* event, SVPipelineFilterWidget *fw);

    void handleFilterParameterChanged(QUuid id);

    void on_focusInEventStarted(QFocusEvent* event);
    void on_focusOutEventStarted(QFocusEvent* event);

    void requestContextMenu(const QPoint& pos);
    void on_actionPaste_triggered();
    void on_actionClearPipeline_triggered();

  private:
    SVPipelineFilterWidget*             m_ShiftStart;
    QVBoxLayout*                        m_FilterWidgetLayout;
    int                                 m_FilterOrigPos;
    DropBoxWidget*                      m_DropBox;
    int                                 m_DropIndex;
    QLabel*                             m_EmptyPipelineLabel;
    QObject*                            m_PipelineMessageObserver;
    QScrollArea*                        m_ScrollArea;
    QTimer                              m_autoScrollTimer;
    bool                                m_AutoScroll;
    int                                 m_AutoScrollMargin;
    int                                 m_autoScrollCount;
    QWidget*                            m_InputParametersWidget;
    QMenu*                              m_ContextMenu;

    SVPipelineViewWidget(const SVPipelineViewWidget&); // Copy Constructor Not Implemented
    void operator=(const SVPipelineViewWidget&); // Operator '=' Not Implemented
};

#endif /* _svpipelineviewwidget_H */


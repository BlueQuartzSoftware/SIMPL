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

#ifndef _svpipelinefilterwidget_h_
#define _svpipelinefilterwidget_h_

#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

#include <QtWidgets/QFrame>
#include <QtWidgets/QMenu>

#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/FilterInputWidget.h"
#include "SVWidgetsLib/Widgets/PipelineFilterObject.h"

#include "SVWidgetsLib/ui_SVPipelineFilterWidget.h"

// This needs to be defined
class QMouseEvent;
class AbstractFilter;
class QFormLayout;
class QLabel;
class QComboBox;
class IObserver;

/**
 * @class SVPipelineFilterWidget SVPipelineFilterWidget.h FilterWidgets/SVPipelineFilterWidget.h
 * @brief  This class is a subclass of the QFrame class and is used to display
 * Filter Options that the user can set. This class is capable of constructing a
 * default GUI widget set for each type of Filter Option that is available. If
 * the programmer needs more specialized widgets then they can simply subclass
 * this class and over ride or implement their custom code.
 *
 * @date Jan 6, 2012
 * @version 1.0
 */
class SVWidgetsLib_EXPORT SVPipelineFilterWidget : public QFrame, public PipelineFilterObject, private Ui::SVPipelineFilterWidget
{
    Q_OBJECT
  public:
    SVPipelineFilterWidget(QWidget* parent = nullptr);
    SVPipelineFilterWidget(AbstractFilter::Pointer filter, IObserver* observer = nullptr, QWidget* parent = nullptr);
    virtual ~SVPipelineFilterWidget();

    SIMPL_INSTANCE_PROPERTY(QString, BorderColorStyle)

    bool hasRightClickTarget();
    void setHasRightClickTarget(bool value);

    void setFilterTitle(const QString title) override;
    void setFilterIndex(int i, int max) override;

    virtual void getGuiParametersFromFilter(AbstractFilter* filt);

    /**
     * @brief deepCopy
     * @return
     */
    PipelineFilterObject* deepCopy() override;

    /**
     * @brief setSelected
     * @param s
     */
    void setSelected(bool s) override;

    /**
     * @brief paintEvent
     * @param event
     */
    void paintEvent(QPaintEvent* event) override;

  public slots:

    /**
     * @brief setIsEnabled
     * @param enabled
     */
    void setIsEnabled(bool enabled) override;

    /**
     * @brief changeStyle
     */
    virtual void changeStyle() override;

    /**
     * @brief displayFilterParameterWidgetError
     * @param msg
     */
    void displayFilterParameterWidgetError(const QString& msg);

    /**
     * @brief adjustLayout
     * @param w
     * @param state
     */
    void adjustLayout(QWidget* w, int state);

    /**
    * @brief launchHelpForItem
    */
    void launchHelpForItem();

    /**
    * @brief toIdleState
    */
    virtual void toReadyState() override;

    /**
    * @brief toRunningState
    */
    virtual void toExecutingState() override;

    /**
     * @brief toCompletedState
     */
    virtual void toCompletedState() override;

    /**
     * @brief toDisabledState
     */
    virtual void toDisabledState() override;

    /**
     * @brief toActiveState
     */
    virtual void toRunningState() override;

    /**
     * @brief toInactiveState
     */
    virtual void toStoppedState() override;

    /**
     * @brief toSelectedState
     */
    virtual void toPausedState() override;

    /**
     * @brief toOkState
     */
    virtual void toOkState() override;

    /**
     * @brief toErrorState
     */
    virtual void toErrorState() override;

    /**
     * @brief toWarningState
     */
    virtual void toWarningState() override;

  protected slots:

    /**
     * @brief on_deleteBtn_clicked
     */
    void on_deleteBtn_clicked();

    /**
     * @brief on_disableBtn_clicked
     */
    void on_disableBtn_clicked(bool state = false);

    /**
     * @brief filterInputWidget_filterParametersChanged
     */
    void filterInputWidget_filterParametersChanged();

    /**
     * @brief updateBorderThickness
     */
    void updateBorderThickness();

  signals:

    /**
     * @brief dragStarted
     */
    void dragStarted(QMouseEvent* event, SVPipelineFilterWidget* fw);

    /**
     * @brief parametersChanged
     */
    void parametersChanged(QUuid id);

    /**
     * @brief parametersChanged
     * @param obj
     */
    void parametersChanged1(PipelineFilterObject* obj);

    /**
     * @brief filterWidgetPressed
     */
    void filterWidgetPressed(PipelineFilterObject* fw, Qt::KeyboardModifiers modifiers);

    /**
    * @brief filterWidgetCut
    */
    void filterWidgetCut();

    /**
    * @brief filterWidgetCopied
    */
    void filterWidgetCopied();

    /**
    * @brief filterWidgetPasted
    */
    void filterWidgetPasted();

    /**
     * @brief filterWidgetRemoved
     * @param widget
     */
    void filterWidgetRemoved(PipelineFilterObject* widget);


    /**
    * @brief focusInEventStarted
    */
    void focusInEventStarted(QFocusEvent* event);

    /**
    * @brief focusOutEventStarted
    */
    void focusOutEventStarted(QFocusEvent* event);

  protected:
    /**
     * @brief mousePressEvent
     * @param event
     */
    virtual void mousePressEvent( QMouseEvent* event ) override;

    /**
     * @brief mouseReleaseEvent
     * @param event
     */
    virtual void mouseReleaseEvent( QMouseEvent* event ) override;

    /**
     * @brief mouseMoveEvent
     * @param event
     */
    virtual void mouseMoveEvent( QMouseEvent* event ) override;

    /**
     * @brief enterEvent
     * @param event
     */
    virtual void enterEvent(QEvent* event) override;

    /**
     * @brief leaveEvent
     * @param event
     */
    virtual void leaveEvent(QEvent* event) override;

    /**
    * @brief getHorizontalLayout
    */
    QHBoxLayout* getHorizontalLayout();

    /**
    * @brief focusInEvent
    */
    void focusInEvent(QFocusEvent* event) override;

    /**
    * @brief focusOutEvent
    */
    void focusOutEvent(QFocusEvent* event) override;
    
    /**
     * @brief eventFilter
     * @param watched
     * @param event
     * @return 
     */
    virtual bool eventFilter(QObject* watched, QEvent* event) Q_DECL_OVERRIDE;
    

  private:
    QPoint                            m_DragStartPosition;
    IObserver* m_Observer = nullptr;
    bool m_HasRightClickTarget = false;
    bool m_HoverState = false;
    int m_MaxFilterCount = 0;
    int m_IndexBoxWidth = 35;
    int m_TextMargin = 6;
    qreal m_SelectionBorderWidth = 4.0;

    QString m_PaddedIndex;
    QString m_FilterHumanLabel;

    QTimer m_AnimationTimer;
    qreal m_BorderThickness = 0.0;
    qreal m_BorderIncrement = 1.0;

    /**
     * @brief initialize Calls all the necessary initialization code for the widget
     * @param filter
     */
    void initialize();

    SVPipelineFilterWidget(const SVPipelineFilterWidget&) = delete; // Copy Constructor Not Implemented
    void operator=(const SVPipelineFilterWidget&) = delete;         // Operator '=' Not Implemented
};

#endif /* _svpipelinefilterwidget_h_ */


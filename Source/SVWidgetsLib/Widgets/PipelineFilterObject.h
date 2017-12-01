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

#ifndef _pipelinefilterobject_h_
#define _pipelinefilterobject_h_

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>

#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/FilterInputWidget.h"

class IObserver;

class SVWidgetsLib_EXPORT PipelineFilterObject
{
  public:
    PipelineFilterObject();
    PipelineFilterObject(AbstractFilter::Pointer filter, IObserver* observer = nullptr);
    virtual ~PipelineFilterObject();

    void setFilter(AbstractFilter::Pointer filter);
    AbstractFilter::Pointer getFilter();

    bool isFocused();
    bool getHasPreflightErrors();
    bool getHasPreflightWarnings();

    // These are convenience functions that just pass through to the filter instance
    QString getHumanLabel();
    QString getFilterGroup();
    QString getFilterSubGroup();
    QString getFilterClassName();
    QString getCompiledLibraryName();
    QColor getGroupColor();
    QIcon getGroupIcon();


    virtual void setFilterTitle(const QString title);
    virtual void setFilterIndex(int i, int max);

    FilterInputWidget* getFilterInputWidget();

    virtual PipelineFilterObject* deepCopy();

    void setHasFocus(bool hasFocus);

    SIMPL_BOOL_PROPERTY(Selected)
    SIMPL_BOOL_PROPERTY(FilterEnabled)

    QWidget* getBasicInputsWidget();
    QWidget* getCurrentStructureWidget();

    void setHasPreflightErrors(bool hasPreflightErrors);
    void setHasPreflightWarnings(bool hasPreflightWarnings);

    virtual void setIsEnabled(bool enabled);

    using EnumType = unsigned int;

    enum class WidgetState : EnumType
    {
      Ready = 0,      //!<
      Executing = 1, //!<
      Completed = 2, //!<
      Disabled = 3
    };
    SIMPL_INSTANCE_PROPERTY(WidgetState, WidgetState)

    enum class PipelineState : EnumType
    {
      Running = 0,
      Stopped = 1,
      Paused = 4,
    };
    SIMPL_INSTANCE_PROPERTY(PipelineState, PipelineState)

    enum class ErrorState : EnumType
    {
        Ok = 0,
        Error = 1,
        Warning = 2,
    };
    SIMPL_INSTANCE_PROPERTY(ErrorState, ErrorState)


    /**
     * @brief changeStyle
     */
    virtual void changeStyle();


    /**
    * @brief toIdleState
    */
    virtual void toReadyState();

    /**
    * @brief toRunningState
    */
    virtual void toExecutingState();

    /**
     * @brief toCompletedState
     */
    virtual void toCompletedState();

    /**
    * @brief toDisabledState
    */
    virtual void toDisabledState();


    /**
     * @brief toActiveState
     */
    virtual void toRunningState();

    /**
     * @brief toInactiveState
     */
    virtual void toStoppedState();

    /**
     * @brief toSelectedState
     */
    virtual void toPausedState();

    /**
     * @brief toOkState
     */
    virtual void toOkState();

    /**
     * @brief toErrorState
     */
    virtual void toErrorState();

    /**
     * @brief toWarningState
     */
    virtual void toWarningState();

  private:
    AbstractFilter::Pointer           m_Filter;
    FilterInputWidget*                m_FilterInputWidget;
    bool                              m_IsFocused;
    bool                              m_HasPreflightErrors;
    bool                              m_HasPreflightWarnings;
    QColor                            m_GroupColor;
    QIcon                             m_GroupIcon;

    /**
    * @brief setupFilterInputWidget Creates and initializes the filter input widget.
    */
    void setupFilterInputWidget();

    PipelineFilterObject(const PipelineFilterObject&) = delete; // Copy Constructor Not Implemented
    void operator=(const PipelineFilterObject&) = delete;       // Operator '=' Not Implemented
};

#endif /* _pipelinefilterobject_h_ */


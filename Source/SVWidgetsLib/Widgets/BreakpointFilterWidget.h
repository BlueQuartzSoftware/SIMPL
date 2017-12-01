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

#ifndef _breakpointfilterwidget_h_
#define _breakpointfilterwidget_h_

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/SVPipelineFilterWidget.h"

#include "SIMPLib/CoreFilters/Breakpoint.h"

/**
 * @class BreakpointFilterWidget BreakpointFilterWidget.h FilterWidgets/BreakpointFilterWidget.h
 * @brief  This class is a subclass of the QGroupBox class and is used to display
 * Filter Options that the user can set. This class is capable of constructing a
 * default GUI widget set for each type of Filter Option that is available. If
 * the programmer needs more specialized widgets then they can simply subclass
 * this class and over ride or implement their custom code.
 *
 * @date Jan 6, 2012
 * @version 1.0
 */
class SVWidgetsLib_EXPORT BreakpointFilterWidget : public SVPipelineFilterWidget
{
    Q_OBJECT
  public:
    BreakpointFilterWidget(QWidget* parent = nullptr);
    BreakpointFilterWidget(AbstractFilter::Pointer filter, IObserver* observer = nullptr, QWidget* parent = nullptr);
    virtual ~BreakpointFilterWidget();

    virtual Breakpoint::Pointer getBreakpointFilter();

    void setupGui();

  protected slots:
    void resumeBtnPressed();
    void showResumeBtn();
    void hideResumeBtn();

  private:
    Breakpoint::Pointer                                     m_Filter;
    QPushButton*                                            m_ResumeBtn;

    BreakpointFilterWidget(const BreakpointFilterWidget&) = delete; // Copy Constructor Not Implemented
    void operator=(const BreakpointFilterWidget&) = delete;         // Operator '=' Not Implemented
};

#endif /* _breakpointfilterwidget_h_ */


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

#include "BreakpointFilterWidget.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BreakpointFilterWidget::BreakpointFilterWidget(QWidget* parent)
: SVPipelineFilterWidget(parent)
, m_Filter(nullptr)
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BreakpointFilterWidget::BreakpointFilterWidget(AbstractFilter::Pointer filter, IObserver* observer, QWidget* parent)
: SVPipelineFilterWidget(filter, observer, parent)
, m_Filter(nullptr)
{
  m_Filter = std::dynamic_pointer_cast<Breakpoint>(SVPipelineFilterWidget::getFilter());

  if(nullptr != m_Filter)
  {
    connect(m_Filter.get(), SIGNAL(pipelineHasPaused()), this, SLOT(showResumeBtn()));
    connect(m_Filter.get(), SIGNAL(pipelineHasResumed()), this, SLOT(hideResumeBtn()));
  }

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BreakpointFilterWidget::~BreakpointFilterWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BreakpointFilterWidget::setupGui()
{
  m_ResumeBtn = new QPushButton("Resume");
  m_ResumeBtn->hide();

  connect(m_ResumeBtn, SIGNAL(clicked()), this, SLOT(resumeBtnPressed()));

  getHorizontalLayout()->addWidget(m_ResumeBtn);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BreakpointFilterWidget::resumeBtnPressed()
{
  m_Filter->resumePipeline();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BreakpointFilterWidget::showResumeBtn()
{
  m_ResumeBtn->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BreakpointFilterWidget::hideResumeBtn()
{
  m_ResumeBtn->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Breakpoint::Pointer BreakpointFilterWidget::getBreakpointFilter()
{
  return m_Filter;
}

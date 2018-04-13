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

#include "PipelineFilterObject.h"

#include <QtWidgets/QGroupBox>

#include "SIMPLib/Common/IObserver.h"

#include "SVWidgetsLib/Core/FilterWidgetManager.h"

#include "QtSupport/QtSStyles.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject::PipelineFilterObject()
: m_Selected(false)
, m_WidgetState(PipelineFilterObject::WidgetState::Ready)
, m_PipelineState(PipelineFilterObject::PipelineState::Stopped)
, m_ErrorState(PipelineFilterObject::ErrorState::Ok)
, m_Filter(AbstractFilter::NullPointer())
, m_FilterInputWidget(nullptr)
, m_IsFocused(false)
, m_HasPreflightErrors(false)
, m_HasPreflightWarnings(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject::PipelineFilterObject(AbstractFilter::Pointer filter, IObserver* observer)
: m_Selected(false)
, m_WidgetState(PipelineFilterObject::WidgetState::Ready)
, m_PipelineState(PipelineFilterObject::PipelineState::Stopped)
, m_ErrorState(PipelineFilterObject::ErrorState::Ok)
, m_Filter(filter)
, m_FilterInputWidget(nullptr)
, m_IsFocused(false)
, m_HasPreflightErrors(false)
, m_HasPreflightWarnings(false)
{
  if(filter != AbstractFilter::NullPointer())
  {
    setupFilterInputWidget();

    if(false == filter->getEnabled())
    {
      toDisabledState();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject::~PipelineFilterObject()
{
  if(m_Filter != AbstractFilter::NullPointer())
  {
    m_Filter->setPreviousFilter(AbstractFilter::NullPointer());
    m_Filter->setNextFilter(AbstractFilter::NullPointer());
    m_Filter = AbstractFilter::NullPointer();
  }

  if(nullptr != m_FilterInputWidget)
  {
    delete m_FilterInputWidget;
    m_FilterInputWidget = nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::setupFilterInputWidget()
{
  QString grpName = m_Filter->getGroupName();

  m_GroupColor = QtSStyles::ColorForFilterGroup(grpName);

  m_GroupIcon = QtSStyles::IconForGroup(grpName);

  // Instantiate the filter input widget object
  if(nullptr != m_FilterInputWidget)
  {
    m_FilterInputWidget->deleteLater();
  }
  m_FilterInputWidget = new FilterInputWidget(m_Filter->getNameOfClass(), this, nullptr);

  // Initialize the filter input widget with values
  m_FilterInputWidget->displayFilterParameters(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon PipelineFilterObject::getGroupIcon()
{
  return m_GroupIcon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor PipelineFilterObject::getGroupColor()
{
  return m_GroupColor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PipelineFilterObject::getFilter()
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::setFilter(AbstractFilter::Pointer filter)
{
  m_Filter = filter;
  if(m_Filter != AbstractFilter::NullPointer())
  {
    setupFilterInputWidget();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineFilterObject::isFocused()
{
  return m_IsFocused;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineFilterObject::getHasPreflightErrors()
{
  return m_HasPreflightErrors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineFilterObject::getHasPreflightWarnings()
{
  return m_HasPreflightWarnings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterObject::getHumanLabel()
{
  if(nullptr != m_Filter.get())
  {
    return m_Filter->getHumanLabel();
  }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterObject::getFilterClassName()
{
  if(nullptr != m_Filter.get())
  {
    return m_Filter->getNameOfClass();
  }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterObject::getCompiledLibraryName()
{
  if(nullptr != m_Filter.get())
  {
    return m_Filter->getCompiledLibraryName();
  }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterObject::getFilterGroup()
{
  if(nullptr != m_Filter.get())
  {
    return m_Filter->getGroupName();
  }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterObject::getFilterSubGroup()
{
  if(nullptr != m_Filter.get())
  {
    return m_Filter->getSubGroupName();
  }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::setFilterTitle(const QString &title)
{
  Q_UNUSED(title)

  // This should be implemented in the subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::setFilterIndex(int i, int numFilters)
{
  Q_UNUSED(i)

  // This should be implemented in the subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterInputWidget* PipelineFilterObject::getFilterInputWidget()
{
  return m_FilterInputWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject* PipelineFilterObject::deepCopy()
{
  // This should be implemented in the subclasses
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::setHasFocus(bool hasFocus)
{
  m_IsFocused = hasFocus;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::changeStyle()
{
  // This should be implemented in the subclasses
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::setHasPreflightErrors(bool hasErrors)
{
  if(hasErrors)
  {
    setErrorState(ErrorState::Error);
  }
  else
  {
    setErrorState(ErrorState::Ok);
  }
  m_HasPreflightErrors = hasErrors;
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::setHasPreflightWarnings(bool hasWarnings)
{
  if(hasWarnings)
  {
    setErrorState(ErrorState::Warning);
  }
  else
  {
    setErrorState(ErrorState::Ok);
  }
  m_HasPreflightWarnings = hasWarnings;
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::setIsEnabled(bool enabled)
{
  setFilterEnabled(enabled);
  // Return if no changes to Enabled
  if(enabled == m_Filter->getEnabled())
  {
    return;
  }

  if(enabled)
  {
    toReadyState();
  }
  else
  {
    toDisabledState();
  }
  m_Filter->setEnabled(enabled);
  changeStyle();

  if(enabled)
  {
    toStoppedState();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::toReadyState()
{
  // This should be implemented in the subclasses
  setWidgetState(WidgetState::Ready);
  setFilterEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::toExecutingState()
{
  // This should be implemented in the subclasses
  setWidgetState(WidgetState::Executing);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::toCompletedState()
{
  // This should be implemented in the subclasses
  setWidgetState(WidgetState::Completed);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::toDisabledState()
{
  // This should be implemented in the subclasses
  setWidgetState(WidgetState::Disabled);
  setFilterEnabled(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::toRunningState()
{
  setPipelineState(PipelineState::Running);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::toStoppedState()
{
  setPipelineState(PipelineState::Stopped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::toPausedState()
{
  setPipelineState(PipelineState::Paused);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::toOkState()
{
  setErrorState(ErrorState::Ok);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::toErrorState()
{
  setErrorState(ErrorState::Error);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterObject::toWarningState()
{
  setErrorState(ErrorState::Warning);
}

/* ============================================================================
 * Copyright (c) 2021 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PythonFilter.h"

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"

// undef slots since a Python header uses slots
#undef slots

#include <pybind11/embed.h>

namespace py = pybind11;

// -----------------------------------------------------------------------------
PythonFilter::PythonFilter(PythonSupport::FilterPyObject object)
: AbstractFilter()
, m_Filter(object)
{
  py::gil_scoped_acquire gil_acquire_guard{};
  setupFilterParameters();
  m_CompiledLibraryName = QString::fromStdString(m_Filter.compiled_lib_name());
  m_FilterVersion = QString::fromStdString(m_Filter.version());
  m_GroupName = QString::fromStdString(m_Filter.group_name());
  m_SubGroupName = QString::fromStdString(m_Filter.sub_group_name());
  m_HumanLabel = QString::fromStdString(m_Filter.human_label());
  m_ClassName = QString::fromStdString(m_Filter.class_name());
  m_Uuid = QString::fromStdString(m_Filter.uuid());
}

// -----------------------------------------------------------------------------
PythonFilter::~PythonFilter() = default;

// -----------------------------------------------------------------------------
void PythonFilter::setupFilterParameters()
{
  py::gil_scoped_acquire gil_acquire_guard{};

  FilterParameterVectorType parameters = m_Filter.setup_parameters();

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void PythonFilter::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  DataContainerArray::Pointer dca = getDataContainerArray();

  py::gil_scoped_acquire gil_acquire_guard{};

  try
  {
    auto&& [status, message] = m_Filter.data_check(dca, PythonSupport::FilterDelegate{this});
    if(status < 0)
    {
      setErrorCondition(status, QString::fromStdString(message));
    }
  } catch(const py::error_already_set& exception)
  {
    setErrorCondition(-1, QString("Python failed with the following error: \n %1").arg(exception.what()));
  } catch(const std::exception& exception)
  {
    setErrorCondition(-2, QString("Caught the following exception: \n %1").arg(exception.what()));
  }
}

// -----------------------------------------------------------------------------
void PythonFilter::execute()
{
  clearErrorCode();
  clearWarningCode();

  if(getCancel())
  {
    return;
  }

  DataContainerArray::Pointer dca = getDataContainerArray();

  if(dca == nullptr)
  {
    throw std::runtime_error("DataContainerArray is nullptr");
  }

  py::gil_scoped_acquire gil_acquire_guard{};

  try
  {
    auto&& [status, message] = m_Filter.execute(dca, PythonSupport::FilterDelegate{this});
    if(status < 0)
    {
      setErrorCondition(status, QString::fromStdString(message));
    }
  } catch(const py::error_already_set& exception)
  {
    setErrorCondition(-3, QString("Python failed with the following error: \n %1").arg(exception.what()));
  } catch(const std::exception& exception)
  {
    setErrorCondition(-4, QString("Caught the following exception: \n %1").arg(exception.what()));
  }
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer PythonFilter::newFilterInstance(bool copyFilterParameters) const
{
  py::gil_scoped_acquire gil_acquire_guard{};

  PythonSupport::FilterPyObject newFilter = m_Filter.create_new();

  PythonFilter::Pointer filter = PythonFilter::New(newFilter);
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString PythonFilter::getCompiledLibraryName() const
{
  return m_CompiledLibraryName;
}

// -----------------------------------------------------------------------------
QString PythonFilter::getBrandingString() const
{
  return "Python Filter";
}

// -----------------------------------------------------------------------------
QString PythonFilter::getFilterVersion() const
{
  return m_FilterVersion;
}

// -----------------------------------------------------------------------------
QString PythonFilter::getGroupName() const
{
  return m_GroupName;
}

// -----------------------------------------------------------------------------
QString PythonFilter::getSubGroupName() const
{
  return m_SubGroupName;
}

// -----------------------------------------------------------------------------
QString PythonFilter::getHumanLabel() const
{
  return m_HumanLabel;
}

// -----------------------------------------------------------------------------
QUuid PythonFilter::getUuid() const
{
  return m_Uuid;
}

// -----------------------------------------------------------------------------
PythonFilter::Pointer PythonFilter::NullPointer()
{
  return nullptr;
}

// -----------------------------------------------------------------------------
PythonFilter::Pointer PythonFilter::New(PythonSupport::FilterPyObject filterObject)
{
  py::gil_scoped_acquire gil_acquire_guard{};

  auto filter = PythonFilter::Pointer(new PythonFilter(filterObject));

  return filter;
}

// -----------------------------------------------------------------------------
QString PythonFilter::getNameOfClass() const
{
  return m_ClassName;
}

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

  DataContainerArray::Pointer dca = getDataContainerArray();

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

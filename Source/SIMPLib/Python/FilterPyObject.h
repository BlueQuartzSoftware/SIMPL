#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

// undef slots since a Python header uses slots
#undef slots

#include <pybind11/embed.h>

namespace PythonSupport
{
struct FilterDelegate
{
  AbstractFilter* filter;

  void notifyStatusMessage(const QString& message) const
  {
    if(filter == nullptr)
    {
      throw std::runtime_error("filter is nullptr");
    }
    filter->notifyStatusMessage(message);
  }

  void notifyProgressMessage(int progress, const QString& message) const
  {
    if(filter == nullptr)
    {
      throw std::runtime_error("filter is nullptr");
    }
    filter->notifyProgressMessage(progress, message);
  }

  void setWarningCondition(int code, const QString& message)
  {
    if(filter == nullptr)
    {
      throw std::runtime_error("filter is nullptr");
    }
    filter->setWarningCondition(code, message);
  }
};

class FilterPyObject
{
public:
  FilterPyObject() = default;
  FilterPyObject(pybind11::object object)
  : m_Object(object)
  {
  }
  ~FilterPyObject() = default;

  FilterPyObject(const FilterPyObject&) = default;
  FilterPyObject(FilterPyObject&&) = default;

  FilterPyObject& operator=(const FilterPyObject&) = default;
  FilterPyObject& operator=(FilterPyObject&&) = default;

  std::pair<int32_t, std::string> data_check(DataContainerArray::Pointer dca, FilterDelegate cppFilter)
  {
    pybind11::tuple result = m_Object.attr("data_check")(dca, cppFilter).cast<pybind11::tuple>();
    auto status = result[0].cast<int32_t>();
    auto message = result[1].cast<std::string>();
    return {status, std::move(message)};
  }

  std::pair<int32_t, std::string> execute(DataContainerArray::Pointer dca, FilterDelegate cppFilter)
  {
    pybind11::tuple result = m_Object.attr("execute")(dca, cppFilter).cast<pybind11::tuple>();
    auto status = result[0].cast<int32_t>();
    auto message = result[1].cast<std::string>();
    return {status, std::move(message)};
  }

  FilterParameterVectorType setup_parameters()
  {
    pybind11::list pyParameters = m_Object.attr("setup_parameters")();
    FilterParameterVectorType parameters{};
    parameters.reserve(pyParameters.size());

    for(pybind11::handle item : pyParameters)
    {
      parameters.push_back(item.cast<FilterParameter::Pointer>());
    }

    return parameters;
  }

  std::string name() const
  {
    return m_Object.attr("name")().cast<std::string>();
  }

  std::string class_name() const
  {
    return m_Object.attr("__class__").attr("__name__").cast<std::string>();
  }

  std::string uuid() const
  {
    return m_Object.attr("uuid")().cast<std::string>();
  }

  std::string group_name() const
  {
    return m_Object.attr("group_name")().cast<std::string>();
  }

  std::string sub_group_name() const
  {
    return m_Object.attr("sub_group_name")().cast<std::string>();
  }

  std::string human_label() const
  {
    return m_Object.attr("human_label")().cast<std::string>();
  }

  std::string version() const
  {
    return m_Object.attr("version")().cast<std::string>();
  }

  std::string compiled_lib_name() const
  {
    return m_Object.attr("compiled_lib_name")().cast<std::string>();
  }

  FilterPyObject create_new() const
  {
    pybind11::object typeObject = pybind11::type::of(m_Object);
    pybind11::object newObject = typeObject();
    return FilterPyObject(newObject);
  }

private:
  pybind11::object m_Object{};
};
} // namespace PythonSupport

#define slots Q_SLOTS

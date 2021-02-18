#pragma once

#include "SIMPLib/Filtering/IFilterFactory.hpp"

#include "SIMPLib/Python/PythonFilter.h"

class PythonFilterFactory : public IFilterFactory
{
public:
  using Self = PythonFilterFactory;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  
  static Pointer NullPointer()
  {
    return nullptr;
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  QString getNameOfClass() const override
  {
    return ClassName();
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return "PythonFilterFactory";
  }

  static Pointer New(pybind11::object typeObject)
  {
    return Pointer(new PythonFilterFactory(typeObject));
  }

  /**
   * @brief Creates a new for this filter. The Calling method MUST set
   * a parent OR take responsibility for deleting this object.
   * @return
   */
  AbstractFilter::Pointer create() const override
  {
    pybind11::gil_scoped_acquire gil_acquire_guard{};

    return PythonFilter::New(m_TypeObject());
  }

  QString getFilterClassName() const override
  {
    return m_FilterClassName;
  }

  QString getFilterGroup() const override
  {
    return m_GroupName;
  }

  QString getFilterSubGroup() const override
  {
    return m_SubGroupName;
  }

  QString getFilterHumanLabel() const override
  {
    return m_HumanName;
  }

  QString getBrandingString() const override
  {
    return m_BrandingString;
  }

  QString getCompiledLibraryName() const override
  {
    return m_CompiledLibraryName;
  }

  QString getFilterHtmlSummary() const override
  {
    return m_HtmlSummary;
  }

  QUuid getUuid() const override
  {
    return m_Uuid;
  }

protected:
  PythonFilterFactory(pybind11::object typeObject)
  : m_TypeObject(typeObject)
  {
    auto filter = PythonFilter::New(m_TypeObject());
    m_FilterClassName = filter->getNameOfClass();
    m_GroupName = filter->getGroupName();
    m_SubGroupName = filter->getSubGroupName();
    m_HumanName = filter->getHumanLabel();
    m_BrandingString = filter->getBrandingString();
    m_CompiledLibraryName = filter->getCompiledLibraryName();
    m_Uuid = filter->getUuid();
    m_HtmlSummary = filter->generateHtmlSummary();
  }

private:
  pybind11::object m_TypeObject;
  QString m_FilterClassName;
  QString m_GroupName;
  QString m_SubGroupName;
  QString m_HumanName;
  QString m_BrandingString;
  QString m_CompiledLibraryName;
  QString m_HtmlSummary;
  QUuid m_Uuid;

public:
  PythonFilterFactory(const PythonFilterFactory&) = delete;            // Copy Constructor Not Implemented
  PythonFilterFactory(PythonFilterFactory&&) = delete;                 // Move Constructor Not Implemented
  PythonFilterFactory& operator=(const PythonFilterFactory&) = delete; // Copy Assignment Not Implemented
  PythonFilterFactory& operator=(PythonFilterFactory&&) = delete; // Move Assignment Not Implemented
};

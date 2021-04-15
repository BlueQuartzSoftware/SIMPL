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

#pragma once

#include "SIMPLib/Filtering/IFilterFactory.hpp"

#include "SIMPLib/Python/PythonFilter.h"

#ifdef __GNUG__
#define SIMPL_PY_VISIBILITY __attribute__((visibility("hidden")))
#else
#define SIMPL_PY_VISIBILITY
#endif

class SIMPL_PY_VISIBILITY PythonFilterFactory : public IFilterFactory
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
  PythonFilterFactory& operator=(PythonFilterFactory&&) = delete;      // Move Assignment Not Implemented
};

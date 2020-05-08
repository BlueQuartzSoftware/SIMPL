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

#pragma once

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/IFilterFactory.hpp"
#include "SIMPLib/SIMPLib.h"

template <class T>
class FilterFactory : public IFilterFactory
{
public:
  using Self = FilterFactory<T>;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  QString getNameOfClass() const override
  {
    return QString("FilterFactory<T>");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("FilterFactory<T>");
  }

  static Pointer New()
  {
    Pointer sharedPtr(new(Self));
    return sharedPtr;
  }
  /**
   * @brief Creates a new  for this filter. The Calling method MUST set
   * a parent  OR take responsibility for deleting this object.
   * @return
   */
  AbstractFilter::Pointer create() const override
  {
    typename T::Pointer w = T::New();
    return w;
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
  FilterFactory()
  {
    typename T::Pointer w = T::New();
    m_FilterClassName = w->getNameOfClass();
    m_GroupName = w->getGroupName();
    m_SubGroupName = w->getSubGroupName();
    m_HumanName = w->getHumanLabel();
    m_BrandingString = w->getBrandingString();
    m_CompiledLibraryName = w->getCompiledLibraryName();
    m_Uuid = w->getUuid();
    m_HtmlSummary = w->generateHtmlSummary();
  }

private:
  QString m_FilterClassName;
  QString m_GroupName;
  QString m_SubGroupName;
  QString m_HumanName;
  QString m_BrandingString;
  QString m_CompiledLibraryName;
  QString m_HtmlSummary;
  QUuid m_Uuid;

public:
  FilterFactory(const FilterFactory&) = delete;            // Copy Constructor Not Implemented
  FilterFactory(FilterFactory&&) = delete;                 // Move Constructor Not Implemented
  FilterFactory& operator=(const FilterFactory&) = delete; // Copy Assignment Not Implemented
  FilterFactory& operator=(FilterFactory&&) = delete;      // Move Assignment Not Implemented
};

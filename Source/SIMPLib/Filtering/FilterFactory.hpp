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

#ifndef _filterfactory_H_
#define _filterfactory_H_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/IFilterFactory.hpp"
#include "SIMPLib/SIMPLib.h"

template <class T> class FilterFactory : public IFilterFactory
{
public:
  SIMPL_SHARED_POINTERS(FilterFactory<T>)
  SIMPL_TYPE_MACRO_SUPER(FilterFactory<T>, IFilterFactory)
  SIMPL_STATIC_NEW_MACRO(FilterFactory<T>)

  /**
   * @brief Creates a new  for this filter. The Calling method MUST set
   * a parent  OR take responsibility for deleting this object.
   * @return
   */
  AbstractFilter::Pointer create() const
  {
    typename T::Pointer w = T::New();
    return w;
  }

  QString getFilterClassName() const
  {
    return m_FilterClassName;
  }

  QString getFilterGroup() const
  {
    return m_GroupName;
  }

  QString getFilterSubGroup() const
  {
    return m_SubGroupName;
  }

  QString getFilterHumanLabel() const
  {
    return m_HumanName;
  }

  QString getBrandingString() const
  {
    return m_BrandingString;
  }

  QString getCompiledLibraryName() const
  {
    return m_CompiledLibraryName;
  }
  
  QUuid getUuid() const
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
  }

private:
  QString m_FilterClassName;
  QString m_GroupName;
  QString m_SubGroupName;
  QString m_HumanName;
  QString m_BrandingString;
  QString m_CompiledLibraryName;
  QUuid m_Uuid;

  FilterFactory(const FilterFactory&);  // Copy Constructor Not Implemented
  void operator=(const FilterFactory&); // Operator '=' Not Implemented
};
#endif /* FilterFACTORY_H_ */

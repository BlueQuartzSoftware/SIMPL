/*=========================================================================
*
*  Copyright Insight Software Consortium
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/
/*=========================================================================
*
*  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
*
*  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
*
*  For complete copyright, license and disclaimer of warranty information
*  please refer to the NOTICE file at the top of the ITK source tree.
*
*=========================================================================*/
#pragma once

#include "itkImportDream3DImageContainer.h"

namespace itk
{

template <typename TElementIdentifier, typename TElement>
ImportDream3DImageContainer<TElementIdentifier, TElement>
::ImportDream3DImageContainer()
{
}


template <typename TElementIdentifier, typename TElement>
ImportDream3DImageContainer<TElementIdentifier, TElement>
::~ImportDream3DImageContainer()
{
  DeallocateManagedMemory();
}


template <typename TElementIdentifier, typename TElement>
typename ImportDream3DImageContainer<TElementIdentifier, TElement>::Element*
ImportDream3DImageContainer<TElementIdentifier, TElement>
::AllocateElements(ElementIdentifier size, bool UseDefaultConstructor) const
{
  // Encapsulate all image memory allocation here to throw an
  // exception when memory allocation fails even when the compiler
  // does not do this by default.
  Element* data;

  try
  {
    data = (Element*)(malloc(sizeof(TElement) * size));
    if(UseDefaultConstructor)
    {
      new(data) Element(); // POD types initialized to 0, others use default constructor.
    }
  } catch(...)
  {
    data = nullptr;
  }
  if(!data)
  {
    // We cannot construct an error string here because we may be out
    // of memory.  Do not use the exception macro.
    throw MemoryAllocationError(__FILE__, __LINE__, "Failed to allocate memory for image.", ITK_LOCATION);
  }
  return data;
}


template <typename TElementIdentifier, typename TElement>
void
ImportDream3DImageContainer<TElementIdentifier, TElement>
::DeallocateManagedMemory()
{
  // Encapsulate all image memory deallocation here
  if(this->GetContainerManageMemory())
  {
    Element* data = this->GetBufferPointer();
    data->~Element();
    free(data);
    this->SetImportPointer(nullptr);
  }
  Superclass::DeallocateManagedMemory();
}


template <typename TElementIdentifier, typename TElement>
void
ImportDream3DImageContainer<TElementIdentifier, TElement>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // end namespace itk


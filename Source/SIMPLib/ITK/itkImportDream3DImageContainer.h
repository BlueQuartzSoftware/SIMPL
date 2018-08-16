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
#pragma once

#include "itkImportImageContainer.h"

namespace itk
{
/** \class ImportDream3DImageContainer
 *  \brief Defines an itk::Image front-end to a standard C-array.
 *
 * Defines an itk::Image front-end to a standard C-array. This container
 * conforms to the ImageContainerInterface. This is a full-fleged Object,
 * so there is modification time, debug, and reference count information.
 * Memory is manage with malloc and free instead of respectively new and
 * delete. This is useful if one needs to import the data container from
 * or export the data to a section of code that manages memory with malloc
 * and free. The behavior of mixing malloc/free and new/delete is undefined
 * and should be avoided.
 *
 * \tparam TElementIdentifier An INTEGRAL type for use in indexing the
 * imported buffer.
 *
 * \tparam TElement The element type stored in the container.
 *
 * \ingroup ImageObjects
 * \ingroup IOFilters
 * \ingroup ITKCommon
 */

template <typename TElementIdentifier, typename TElement> class ImportDream3DImageContainer : public ImportImageContainer<TElementIdentifier, TElement>
{
public:
  /** Standard class typedefs. */
  typedef ImportDream3DImageContainer Self;
  typedef ImportImageContainer<TElementIdentifier, TElement> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Save the template parameters. */
  typedef typename Superclass::ElementIdentifier ElementIdentifier;
  typedef typename Superclass::Element Element;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Standard part of every itk Object. */
  itkTypeMacro(ImportDream3DImageContainer, ImportImageContainer);

protected:
  ImportDream3DImageContainer();
  virtual ~ImportDream3DImageContainer();

  /** PrintSelf routine. Normally this is a protected internal method. It is
   * made public here so that Image can call this method.  Users should not
   * call this method but should call Print() instead. */
  virtual void PrintSelf(std::ostream& os, Indent indent) const override;

  /**
   * Allocates elements of the array.  If UseDefaultConstructor is true, then
   * the default constructor is used to initialize each element.  POD date types
   * initialize to zero.
   */
  virtual Element* AllocateElements(ElementIdentifier size, bool UseDefaultConstructor = false) const override;

  virtual void DeallocateManagedMemory() override;

private:
  ImportDream3DImageContainer(const Self&) = delete;
  void operator=(const Self&) = delete;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImportDream3DImageContainer.hxx"
#endif


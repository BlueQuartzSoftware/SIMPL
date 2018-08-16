/* ============================================================================
* Copyright (c) 2009-2018 BlueQuartz Software, LLC
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
*    United States Air Force Prime Contract FA8650-15-D-5231
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "itkProcessObject.h"
#include "itkSimpleDataObjectDecorator.h"

#include "SIMPLib/Geometry/TransformContainer.h"

namespace itk
{
template <typename ITKTransformType> class Dream3DTransformContainerToTransform : public ProcessObject
{
public:
  /** Standard class typedefs. */
  using Self = Dream3DTransformContainerToTransform;
  using Pointer = SmartPointer<Self>;
  using Superclass = ProcessObject;
  using DecoratorType = typename itk::SimpleDataObjectDecorator<typename ITKTransformType::Pointer>;
  using TParametersValueType = typename ITKTransformType::ParametersType::ValueType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  itkTypeMacro(Dream3DTransformToTransform, ProcessObject);

  using Superclass::SetInput;
  virtual void SetInput(::TransformContainer::Pointer transformContainer);
  DecoratorType* GetOutput();

protected:
  Dream3DTransformContainerToTransform();
  virtual ~Dream3DTransformContainerToTransform();

  virtual void VerifyPreconditions() override;

  virtual void GenerateData() override;
  ::TransformContainer::Pointer m_TransformContainer;
  ProcessObject::DataObjectPointer MakeOutput(ProcessObject::DataObjectPointerArraySizeType) override;

private:
  Dream3DTransformContainerToTransform(const Dream3DTransformContainerToTransform&) = delete; // Copy Constructor Not Implemented
  void operator=(const Dream3DTransformContainerToTransform&) = delete;                       // Move assignment Not Implemented
};
} // end of itk namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDream3DTransformContainerToTransform.hxx"
#endif

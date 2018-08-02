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

#include "itkDream3DTransformContainerToTransform.h"

namespace itk
{

template <typename ITKTransformType>
Dream3DTransformContainerToTransform<ITKTransformType>
::Dream3DTransformContainerToTransform()
{
  // Create the output. We use static_cast<> here because we know the default
  // output must be of type DecoratorType
  typename DecoratorType::Pointer output = static_cast<DecoratorType*>(this->MakeOutput(0).GetPointer());
  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, output.GetPointer());
  m_TransformContainer = ::TransformContainer::NullPointer();
}


template <typename ITKTransformType>
Dream3DTransformContainerToTransform<ITKTransformType>
::~Dream3DTransformContainerToTransform() = default;

template <typename ITKTransformType>
void
Dream3DTransformContainerToTransform<ITKTransformType>
::SetInput(::TransformContainer::Pointer transformContainer)
{
  if(transformContainer != m_TransformContainer)
  {
    m_TransformContainer = transformContainer;
    this->Modified();
  }
}


template <typename ITKTransformType>
ProcessObject::DataObjectPointer
Dream3DTransformContainerToTransform<ITKTransformType>
::MakeOutput(ProcessObject::DataObjectPointerArraySizeType)
{
  typename DecoratorType::Pointer output = DecoratorType::New();
  typename ITKTransformType::Pointer transform = ITKTransformType::New();
  output->Set(transform);
  return output.GetPointer();
}


template <typename ITKTransformType>
void
Dream3DTransformContainerToTransform<ITKTransformType>
::VerifyPreconditions()
{
  if(!m_TransformContainer)
  {
    itkExceptionMacro("Input transform container is empty");
  }
  DecoratorType* outputPtr = this->GetOutput();
  typename ITKTransformType::Pointer transform = outputPtr->Get();
  // Verifies that the number of Parameters and Fixed Parameters in the transform
  // match the expected number based on the transform type.
  if(transform->GetNumberOfParameters() != m_TransformContainer->getParameters().size())
  {
    itkExceptionMacro("Transform container parameter size does not match ITK transform parameter size");
  }
  if(transform->GetNumberOfFixedParameters() != m_TransformContainer->getFixedParameters().size())
  {
    itkExceptionMacro("Transform container fixed parameter size does not match ITK transform fixed parameter size");
  }
  if(transform->GetTransformTypeAsString() != m_TransformContainer->getTransformTypeAsString())
  {
    itkExceptionMacro("Transform types do not match. Transform container is " + m_TransformContainer->getTransformTypeAsString() + ". ITK transform is " + transform->GetTransformTypeAsString() +
                      " .");
  }
  Superclass::VerifyPreconditions();
}


template <typename ITKTransformType>
void
Dream3DTransformContainerToTransform<ITKTransformType>
::GenerateData()
{
  DecoratorType* outputPtr = this->GetOutput();
  typename ITKTransformType::Pointer transform = outputPtr->Get();

  auto dream3DParameters = m_TransformContainer->getParameters();
  auto dream3DFixedParameters = m_TransformContainer->getFixedParameters();

  typename ITKTransformType::ParametersType itkParameters(dream3DParameters.size());
  typename ITKTransformType::FixedParametersType itkFixedParameters(dream3DFixedParameters.size());
  for(size_t p = 0; p < dream3DParameters.size(); p++)
  {
    itkParameters[p] = static_cast<TParametersValueType>(dream3DParameters[p]);
  }
  for(size_t p = 0; p < dream3DFixedParameters.size(); p++)
  {
    itkFixedParameters[p] = static_cast<TParametersValueType>(dream3DFixedParameters[p]);
  }

  transform->SetParameters(itkParameters);
  transform->SetFixedParameters(itkFixedParameters);
}

template <typename ITKTransformType>
typename Dream3DTransformContainerToTransform<ITKTransformType>::DecoratorType*
Dream3DTransformContainerToTransform<ITKTransformType>
::GetOutput()
{
  return itkDynamicCastInDebugMode<DecoratorType*>(this->GetPrimaryOutput());
}

} // end of itk namespace

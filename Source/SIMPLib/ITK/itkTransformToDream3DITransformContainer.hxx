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

#include "itkTransformToDream3DITransformContainer.h"

#include "itkCompositeTransform.h"

namespace itk
{

template <typename TParametersValueType, unsigned int CompositeNDimensions>
TransformToDream3DITransformContainer<TParametersValueType, CompositeNDimensions>
::TransformToDream3DITransformContainer()
{
  // Create the output. We use static_cast<> here because we know the default
  // output must be of type DecoratorType
  typename DecoratorType::Pointer output = static_cast<DecoratorType*>(this->MakeOutput(0).GetPointer());
  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, output.GetPointer());
  m_Transform = nullptr;
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
TransformToDream3DITransformContainer<TParametersValueType, CompositeNDimensions>
::~TransformToDream3DITransformContainer() = default;


template <typename TParametersValueType, unsigned int CompositeNDimensions>
void
TransformToDream3DITransformContainer<TParametersValueType, CompositeNDimensions>
::SetInput(const ITKTransformType * transform)
{
  if(!(transform == m_Transform.GetPointer()))
  {
    m_Transform = transform;
    this->Modified();
  }
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
ProcessObject::DataObjectPointer
TransformToDream3DITransformContainer<TParametersValueType, CompositeNDimensions>
::MakeOutput(ProcessObject::DataObjectPointerArraySizeType)
{
  typename DecoratorType::Pointer output = DecoratorType::New();
  return output.GetPointer();
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
void
TransformToDream3DITransformContainer<TParametersValueType, CompositeNDimensions>
::VerifyPreconditions()
{
  if(!m_Transform)
  {
    itkExceptionMacro("Input transform is empty");
  }
  Superclass::VerifyPreconditions();
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
void
TransformToDream3DITransformContainer<TParametersValueType, CompositeNDimensions>
::GenerateData(::ITransformContainer::Pointer& iTransformContainer, const ITKTransformType * transform)
{
  using CompositeType = itk::CompositeTransform<TParametersValueType, CompositeNDimensions>;
  typename CompositeType::ConstPointer itkCompositeTransform = dynamic_cast<const CompositeType*>(transform);
  if(itkCompositeTransform)
  {
    typename ::CompositeTransformContainer::Pointer cTransform = ::CompositeTransformContainer::New();
    for(auto& t : itkCompositeTransform->GetTransformQueue())
    {
      typename ::ITransformContainer::Pointer transformContainer;
      GenerateData(transformContainer, t);
      cTransform->addTransformContainer(transformContainer);
    }
    iTransformContainer = cTransform;
  }
  else
  {
    ::TransformContainer::Pointer transformContainer = ::TransformContainer::New();
    auto itkParameters = transform->GetParameters();
    auto itkFixedParameters = transform->GetFixedParameters();

    typename ::TransformContainer::TransformParametersType dream3DParameters(itkParameters.size());
    typename ::TransformContainer::TransformFixedParametersType dream3DFixedParameters(itkFixedParameters.size());

    for(size_t p = 0; p < itkParameters.size(); p++)
    {
      dream3DParameters[p] = static_cast<typename ::TransformContainer::ParametersValueType>(itkParameters[p]);
    }
    for(size_t p = 0; p < itkFixedParameters.size(); p++)
    {
      dream3DFixedParameters[p] = static_cast<typename ::TransformContainer::ParametersValueType>(itkFixedParameters[p]);
    }

    transformContainer->setParameters(dream3DParameters);
    transformContainer->setFixedParameters(dream3DFixedParameters);
    transformContainer->setTransformTypeAsString(transform->GetTransformTypeAsString());
    iTransformContainer = transformContainer;
  }
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
void
TransformToDream3DITransformContainer<TParametersValueType, CompositeNDimensions>::GenerateData()
{
  DecoratorType* outputPtr = this->GetOutput();
  typename ::ITransformContainer::Pointer iTransformContainer;
  this->GenerateData(iTransformContainer, m_Transform);
  outputPtr->Set(iTransformContainer);
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
typename TransformToDream3DITransformContainer<TParametersValueType, CompositeNDimensions>::DecoratorType*
TransformToDream3DITransformContainer<TParametersValueType, CompositeNDimensions>
::GetOutput()
{
  return itkDynamicCastInDebugMode<DecoratorType*>(this->GetPrimaryOutput());
}

} // end of itk namespace

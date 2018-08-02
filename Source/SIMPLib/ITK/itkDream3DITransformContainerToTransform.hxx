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

#include "itkDream3DITransformContainerToTransform.h"

#include "itkTransform.h"
#include "itkTransformFactoryBase.h"

#include "SIMPLib/Geometry/CompositeTransformContainer.h"
#include "SIMPLib/Geometry/TransformContainer.h"

namespace itk
{

template <typename TParametersValueType, unsigned int CompositeNDimensions>
Dream3DITransformContainerToTransform<TParametersValueType, CompositeNDimensions>
::Dream3DITransformContainerToTransform()
{
  // Create the output. We use static_cast<> here because we know the default
  // output must be of type DecoratorType
  typename DecoratorType::Pointer output = static_cast<DecoratorType*>(this->MakeOutput(0).GetPointer());
  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, output.GetPointer());
  m_ITransformContainer = ::ITransformContainer::NullPointer();
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
Dream3DITransformContainerToTransform<TParametersValueType, CompositeNDimensions>
::~Dream3DITransformContainerToTransform() = default;

template <typename TParametersValueType, unsigned int CompositeNDimensions>
void
Dream3DITransformContainerToTransform<TParametersValueType, CompositeNDimensions>
::SetInput(const ITransformContainer::Pointer & transformContainer)
{
  if(transformContainer != m_ITransformContainer)
  {
    m_ITransformContainer = transformContainer;
    this->Modified();
  }
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
ProcessObject::DataObjectPointer
Dream3DITransformContainerToTransform<TParametersValueType, CompositeNDimensions>
::MakeOutput(ProcessObject::DataObjectPointerArraySizeType)
{
  typename DecoratorType::Pointer output = DecoratorType::New();
  return output.GetPointer();
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
void
Dream3DITransformContainerToTransform<TParametersValueType, CompositeNDimensions>
::VerifyPreconditions()
{
  if(!m_ITransformContainer)
  {
    itkExceptionMacro("Input transform container is empty");
  }
  Superclass::VerifyPreconditions();
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
void
Dream3DITransformContainerToTransform<TParametersValueType, CompositeNDimensions>
::CreateTransform(ITKTransformPointer& ptr, const std::string& ClassName)
{
  // The logic of this function is shamelessly copied from itkTransformIOBase.hxx in ITK.

  // call to GetFactory has side effect of initializing the
  // TransformFactory overrides
  TransformFactoryBase* theFactory = TransformFactoryBase::GetFactory();

  // Instantiate the transform
  itkDebugMacro("About to call ObjectFactory");
  LightObject::Pointer i = ObjectFactoryBase::CreateInstance(ClassName.c_str());
  itkDebugMacro("After call ObjectFactory");
  ptr = dynamic_cast<ITKTransformType*>(i.GetPointer());
  if(ptr.IsNull())
  {
    std::ostringstream msg;
    msg << "Could not create an instance of \"" << ClassName << "\"" << std::endl
        << "The usual cause of this error is not registering the "
        << "transform with TransformFactory" << std::endl;
    msg << "Currently registered Transforms: " << std::endl;
    std::list<std::string> names = theFactory->GetClassOverrideWithNames();
    for(auto& name : names)
    {
      msg << "\t\"" << name << "\"" << std::endl;
    }
    itkExceptionMacro(<< msg.str());
  }
  // Correct extra reference count from CreateInstance()
  ptr->UnRegister();
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
void
Dream3DITransformContainerToTransform<TParametersValueType, CompositeNDimensions>
::TransformGenerateData(ITKTransformPointer& itktransform, ::TransformContainer* transformContainer)
{
  CreateTransform(itktransform, transformContainer->getTransformTypeAsString().c_str());
  auto dream3DParameters = transformContainer->getParameters();
  auto dream3DFixedParameters = transformContainer->getFixedParameters();

  // Kernel transforms need an additional initialization. We do not handle that and
  // therefore Kernel transforms are not currently supported.
  std::string transformTypeName = itktransform->GetNameOfClass();
  const size_t len = strlen("KernelTransform"); // Computed at compile time in most cases
  if(transformTypeName.size() >= len && !transformTypeName.compare(transformTypeName.size() - len, len, "KernelTransform"))
  {
    itkExceptionMacro("KernelTransforms are not currently supported.");
  }

  if(dream3DParameters.size() != itktransform->GetParameters().size() || dream3DFixedParameters.size() != itktransform->GetFixedParameters().size())
  {
    std::ostringstream msg;
    msg << "Size mismatch in Dream3D transform container parameters and ITK transform parameters." << std::endl
        << "There must be an error in the transform name saved in TransformTypeAsString." << std::endl;

    itkExceptionMacro(<< msg.str());
  }
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
  itktransform->SetParameters(itkParameters);
  itktransform->SetFixedParameters(itkFixedParameters);
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
void
Dream3DITransformContainerToTransform<TParametersValueType, CompositeNDimensions>
::GenerateData(ITKTransformPointer& itktransform, const ITransformContainer::Pointer& mTransformContainer)
{
  ::TransformContainer* transformContainer = dynamic_cast<::TransformContainer*>(mTransformContainer.get());
  if(transformContainer)
  {
    this->TransformGenerateData(itktransform, transformContainer);
  }
  else
  {
    ::CompositeTransformContainer* ctransformContainer = dynamic_cast<::CompositeTransformContainer*>(mTransformContainer.get());
    if(ctransformContainer)
    {
      using cTransformType = Transform<TParametersValueType, CompositeNDimensions, CompositeNDimensions>;

      typename CompositeType::Pointer itkcompositetransform = CompositeType::New();

      for(auto& t : ctransformContainer->getTransformContainers())
      {
        ITKTransformPointer itkctransform;
        this->GenerateData(itkctransform, t);
        cTransformType* itkDimTransform = dynamic_cast<cTransformType*>(itkctransform.GetPointer());
        if(itkDimTransform)
        {
          itkcompositetransform->AddTransform(itkDimTransform);
        }
        else
        {
          std::ostringstream msg;
          msg << "Transform dimension mismatch between given dimension (Input=" << CompositeNDimensions << ", Output=" << CompositeNDimensions << ")" << std::endl
              << "and dimension of internal transform (Input=" << itktransform->GetInputSpaceDimension() << ", Output=" << itktransform->GetOutputSpaceDimension() << ")." << std::endl;
          itkExceptionMacro(<< msg.str());
        }
      }
      itktransform = itkcompositetransform;
    }
    else
    {
      // Should never arrive here
      itkExceptionMacro("Input transform is neither a TransformContainer nor a CompositeTransformContainer. Type not supported!!!");
    }
  }
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
void
Dream3DITransformContainerToTransform<TParametersValueType, CompositeNDimensions>
::GenerateData()
{
  auto* outputPtr = this->GetOutput();
  ITKTransformPointer itktransform;
  GenerateData(itktransform, m_ITransformContainer);
  outputPtr->Set(itktransform);
}


template <typename TParametersValueType, unsigned int CompositeNDimensions>
typename Dream3DITransformContainerToTransform<TParametersValueType, CompositeNDimensions>::DecoratorType*
Dream3DITransformContainerToTransform<TParametersValueType, CompositeNDimensions>
::GetOutput()
{
  return itkDynamicCastInDebugMode<DecoratorType*>(this->GetPrimaryOutput());
}

} // end of itk namespace

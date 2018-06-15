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

#ifndef itkDream3DTransforContainerTestHelper_h
#define itkDream3DTransforContainerTestHelper_h

#include "itkAffineTransform.h"
#include "itkCompositeTransform.h"

#include "SIMPLib/Geometry/CompositeTransformContainer.h"
#include "SIMPLib/Geometry/TransformContainer.h"

using AffineType = itk::AffineTransform<float, 3>;
using CompositeTransform = itk::CompositeTransform<float, 3>;

template <typename ParameterType> bool CompareITKParameters(ParameterType trans1, ParameterType trans2)
{
  DREAM3D_REQUIRE_EQUAL(trans1.GetSize(), trans2.GetSize());
  for(size_t p = 0; p < trans1.GetSize(); p++)
  {
    DREAM3D_REQUIRE_EQUAL(trans1[p], trans2[p]);
  }
  return true;
}

template <typename ParameterType> bool CompareDREAM3DParameters(ParameterType trans1, ParameterType trans2)
{
  DREAM3D_REQUIRE_EQUAL(trans1.size(), trans2.size());
  for(size_t p = 0; p < trans1.size(); p++)
  {
    DREAM3D_REQUIRE_EQUAL(trans1[p], trans2[p]);
  }
  return true;
}

typename AffineType::Pointer GetTestAffine()
{
  // Create an ITK affine transform as a reference
  AffineType::Pointer itkAffine = AffineType::New();
  itk::Matrix<float, 3, 3> matrix;
  matrix.SetIdentity();
  itkAffine->SetMatrix(matrix);
  return itkAffine;
}

typename TransformContainer::Pointer GetTransformContainerFromITKAffineTransform(const AffineType::Pointer& itkAffine)
{
  auto parameters = itkAffine->GetParameters();
  auto fixedParameters = itkAffine->GetFixedParameters();
  auto itkAffineName = itkAffine->GetTransformTypeAsString();

  std::vector<::TransformContainer::ParametersValueType> dream3DParameters(parameters.GetSize());
  std::vector<::TransformContainer::ParametersValueType> dream3DFixedParameters(fixedParameters.GetSize());
  for(size_t p = 0; p < dream3DParameters.size(); p++)
  {
    dream3DParameters[p] = parameters[p];
  }
  for(size_t p = 0; p < dream3DFixedParameters.size(); p++)
  {
    dream3DFixedParameters[p] = fixedParameters[p];
  }
  // Create a SIMPL transform container and manually convert the ITK transform to
  // a SIMPL transform container.
  TransformContainer::Pointer transformContainer = TransformContainer::New();
  transformContainer->setTransformTypeAsString(itkAffineName);
  transformContainer->setMovingName("");
  transformContainer->setReferenceName("World");
  transformContainer->setFixedParameters(dream3DFixedParameters);
  transformContainer->setParameters(dream3DParameters);
  return transformContainer;
}

#endif
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SIMPLib/ITK/itkTransformToDream3DTransformContainer.h"

#include "itkDream3DTransformContainerTestHelper.h"

class itkTransformToDream3DTransformContainerTest
{

public:
  itkTransformToDream3DTransformContainerTest() = default;
  virtual ~itkTransformToDream3DTransformContainerTest() = default;

  int TestitkTransformToDream3DTransformContainerTest()
  {
    AffineType::Pointer itkAffine = GetTestAffine();
    TransformContainer::Pointer transformContainer = GetTransformContainerFromITKAffineTransform(itkAffine);
    auto containerParameters = transformContainer->getParameters();
    auto containerFixedParameters = transformContainer->getFixedParameters();

    //
    using FilterType = itk::TransformToDream3DTransformContainer<AffineType>;

    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(itkAffine);
    filter->Update();
    auto convertedTransformContainer = filter->GetOutput()->Get();
    CompareDREAM3DParameters(containerParameters, convertedTransformContainer->getParameters());
    CompareDREAM3DParameters(containerFixedParameters, convertedTransformContainer->getFixedParameters());
    DREAM3D_REQUIRE_EQUAL(QString(itkAffine->GetTransformTypeAsString().c_str()), QString(convertedTransformContainer->getTransformTypeAsString().c_str()));
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;
    std::cout << "#### itkTransformToDream3DTransformContainerTest Starting ####" << std::endl;
    DREAM3D_REGISTER_TEST(TestitkTransformToDream3DTransformContainerTest());
  }

private:
  itkTransformToDream3DTransformContainerTest(const itkTransformToDream3DTransformContainerTest&) = delete; // Copy Constructor Not Implemented
  void operator=(const itkTransformToDream3DTransformContainerTest&) = delete;                              // Move assignment Not Implemented
};

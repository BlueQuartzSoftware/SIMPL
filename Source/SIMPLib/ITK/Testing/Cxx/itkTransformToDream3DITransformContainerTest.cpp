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
#include "SIMPLib/ITK/itkTransformToDream3DITransformContainer.h"

#include "itkDream3DTransformContainerTestHelper.h"

class itkTransformToDream3DITransformContainerTest
{

public:
  itkTransformToDream3DITransformContainerTest() = default;
  virtual ~itkTransformToDream3DITransformContainerTest() = default;

  int TestitkTransformToDream3DITransformContainerTest()
  {
    AffineType::Pointer itkAffine = GetTestAffine();
    TransformContainer::Pointer transformContainer = GetTransformContainerFromITKAffineTransform(itkAffine);
    auto containerParameters = transformContainer->getParameters();
    auto containerFixedParameters = transformContainer->getFixedParameters();

    //
    using FilterType = itk::TransformToDream3DITransformContainer<float, 3>;

    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(itkAffine);
    filter->Update();
    ::ITransformContainer::Pointer convertedITransformContainer = filter->GetOutput()->Get();
    ::TransformContainer::Pointer convertedTransformContainer = std::dynamic_pointer_cast<::TransformContainer>(convertedITransformContainer);

    DREAM3D_REQUIRE_VALID_POINTER(convertedTransformContainer.get());
    CompareDREAM3DParameters(convertedTransformContainer->getParameters(), containerParameters);
    CompareDREAM3DParameters(convertedTransformContainer->getFixedParameters(), containerFixedParameters);
    DREAM3D_REQUIRE_EQUAL(QString(itkAffine->GetTransformTypeAsString().c_str()), QString(convertedTransformContainer->getTransformTypeAsString().c_str()));

    using CompositeTransform = itk::CompositeTransform<float, 3>;
    CompositeTransform::Pointer ct = CompositeTransform::New();
    ct->AddTransform(itkAffine);
    ct->AddTransform(itkAffine);

    filter->SetInput(ct);
    filter->Update();
    ::ITransformContainer::Pointer convertedITransformContainer2 = filter->GetOutput()->Get();
    ::CompositeTransformContainer::Pointer ctransformContainer = std::dynamic_pointer_cast<::CompositeTransformContainer>(convertedITransformContainer2);
    DREAM3D_REQUIRE_VALID_POINTER(ctransformContainer.get());
    ::ITransformContainer::Pointer iTransformContainer = ctransformContainer->getTransformContainers()[0];
    ::TransformContainer::Pointer convertedTransformContainer2 = std::dynamic_pointer_cast<::TransformContainer>(iTransformContainer);
    DREAM3D_REQUIRE_VALID_POINTER(convertedTransformContainer2.get());
    CompareDREAM3DParameters(containerParameters, convertedTransformContainer2->getParameters());
    CompareDREAM3DParameters(containerFixedParameters, convertedTransformContainer2->getFixedParameters());
    DREAM3D_REQUIRE_EQUAL(QString(itkAffine->GetTransformTypeAsString().c_str()), QString(convertedTransformContainer2->getTransformTypeAsString().c_str()));
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;
    std::cout << "#### itkTransformToDream3DITransformContainerTest Starting ####" << std::endl;
    DREAM3D_REGISTER_TEST(TestitkTransformToDream3DITransformContainerTest());
  }

private:
  itkTransformToDream3DITransformContainerTest(const itkTransformToDream3DITransformContainerTest&) = delete; // Copy Constructor Not Implemented
  void operator=(const itkTransformToDream3DITransformContainerTest&) = delete;                               // Move assignment Not Implemented
};

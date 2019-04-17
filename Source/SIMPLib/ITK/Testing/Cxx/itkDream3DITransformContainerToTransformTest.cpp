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

#include "itkDream3DTransformContainerTestHelper.h"

#include "SIMPLib/ITK/itkDream3DITransformContainerToTransform.h"
#include "itkTransformFactory.h"

class itkDream3DITransformContainerToTransformTest
{

public:
  itkDream3DITransformContainerToTransformTest() = default;
  virtual ~itkDream3DITransformContainerToTransformTest() = default;

  int TestitkDream3DITransformContainerToTransformTest()
  {
    AffineType::Pointer itkAffine = GetTestAffine();
    TransformContainer::Pointer transformContainer = GetTransformContainerFromITKAffineTransform(itkAffine);

    // Test simple conversion from ITK transform to transform container.
    using FilterType = itk::Dream3DITransformContainerToTransform<float, 3>;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(transformContainer);
    filter->Update();
    auto convertedITKTransform = filter->GetOutput()->Get();
    CompareITKParameters(itkAffine->GetParameters(), convertedITKTransform->GetParameters());
    CompareITKParameters(itkAffine->GetFixedParameters(), convertedITKTransform->GetFixedParameters());
    DREAM3D_REQUIRE_EQUAL(QString(itkAffine->GetTransformTypeAsString().c_str()), QString(convertedITKTransform->GetTransformTypeAsString().c_str()));

    // Modify manually transform type to an existing transform type that is
    // not automatically registered in the transform factory. The test
    // should fail by throwing an exception (that we catch).
    using MO = itk::MatrixOffsetTransformBase<float, 3, 3>;
    MO::Pointer matrixOffset = MO::New();
    matrixOffset->SetMatrix(itkAffine->GetMatrix());
    transformContainer->setTransformTypeAsString(matrixOffset->GetTransformTypeAsString());
    filter->Modified();
    try
    {
      filter->Update();
      DREAM3D_TEST_THROW_EXCEPTION("An exception was expected and this line should not be executed.");
    } catch(itk::ExceptionObject& ex)
    {
      std::cout << "Caught exception as expected." << ex.GetDescription() << std::endl;
    }
    // If we manually register the transform to the transform factory,
    // the test should now pass.
    itk::TransformFactory<MO>::RegisterTransform();
    filter->Update();

    // We test that we can also convert
    CompositeTransformContainer::Pointer ct = CompositeTransformContainer::New();
    ct->addTransformContainer(transformContainer);
    ct->addTransformContainer(transformContainer);
    ct->addTransformContainer(transformContainer);

    filter->SetInput(ct);
    filter->Update();
    auto convertedCompositeContainer = filter->GetOutput()->Get();

    CompositeTransform::Pointer compositeTransform = dynamic_cast<CompositeTransform*>(convertedCompositeContainer.GetPointer());

    DREAM3D_REQUIRE_VALID_POINTER(compositeTransform.GetPointer());

    DREAM3D_REQUIRE_EQUAL(compositeTransform->GetNumberOfTransforms(), 3)
    DREAM3D_REQUIRE_EQUAL(QString(compositeTransform->GetNthTransform(0)->GetTransformTypeAsString().c_str()), QString(matrixOffset->GetTransformTypeAsString().c_str()));

    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;
    std::cout << "#### itkDream3DITransformContainerToTransformTest Starting ####" << std::endl;

    DREAM3D_REGISTER_TEST(TestitkDream3DITransformContainerToTransformTest());
  }

private:
  itkDream3DITransformContainerToTransformTest(const itkDream3DITransformContainerToTransformTest&) = delete; // Copy Constructor Not Implemented
  void operator=(const itkDream3DITransformContainerToTransformTest&) = delete;                               // Move assignment Not Implemented
};

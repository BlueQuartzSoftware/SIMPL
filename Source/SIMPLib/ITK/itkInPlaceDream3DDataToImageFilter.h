#pragma once

#include "itkDream3DImage.h"
#include <itkImportImageFilter.h>
#include <itkNumericTraits.h>
#include <itkNumericTraitsRGBAPixel.h>
#include <itkNumericTraitsRGBPixel.h>
#include <itkNumericTraitsVectorPixel.h>

namespace itk
{

template <typename PixelType, unsigned int VDimension>
class InPlaceDream3DDataToImageFilter : public ImageSource<itk::Dream3DImage<PixelType, VDimension>>
{
public:
  /** Standard class typedefs. */
  using Self = InPlaceDream3DDataToImageFilter;
  using Pointer = SmartPointer<Self>;

  using ImageType = typename itk::Dream3DImage<PixelType, VDimension>;
  using ImportImageContainerType = typename ImageType::PixelContainerType;
  using ImagePointer = typename ImageType::Pointer;
  using ValueType = typename itk::NumericTraits<PixelType>::ValueType;
  using DataArrayPixelType = typename ::DataArray<ValueType>;
  using Superclass = typename itk::ImageSource<ImageType>;
  using SizeValueType = typename itk::SizeValueType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  itkTypeMacro(InPlaceDream3DDataToImageFilter, ImportImageFilter);

  virtual void SetInput(DataContainer::Pointer& dc);

  itkSetMacro(DataArrayName, std::string);
  itkGetMacro(DataArrayName, std::string);

  itkSetMacro(AttributeMatrixArrayName, std::string);
  itkGetMacro(AttributeMatrixArrayName, std::string);

  itkSetMacro(PixelContainerWillOwnTheBuffer, bool);
  itkGetMacro(PixelContainerWillOwnTheBuffer, bool);

  itkSetMacro(InPlace, bool);
  itkGetConstMacro(InPlace, bool);
  itkBooleanMacro(InPlace);

protected:
  InPlaceDream3DDataToImageFilter();
  ~InPlaceDream3DDataToImageFilter();

  void VerifyPreconditions() ITKv5_CONST override;

  void GenerateOutputInformation() override;
  void GenerateData() override;
  DataContainer::Pointer m_DataContainer = DataContainer::NullPointer();

private:
  using Superclass::SetInput;
  std::string m_DataArrayName;
  std::string m_AttributeMatrixArrayName;
  typename ImportImageContainerType::Pointer m_ImportImageContainer;
  bool m_InPlace = true;                         // enable the possibility of in-place
  bool m_PixelContainerWillOwnTheBuffer = false; // By default, this filter does not take data ownership

public:
  InPlaceDream3DDataToImageFilter(const InPlaceDream3DDataToImageFilter&) = delete;            // Copy Constructor Not Implemented
  InPlaceDream3DDataToImageFilter(InPlaceDream3DDataToImageFilter&&) = delete;                 // Move Constructor Not Implemented
  InPlaceDream3DDataToImageFilter& operator=(const InPlaceDream3DDataToImageFilter&) = delete; // Copy Assignment Not Implemented
  InPlaceDream3DDataToImageFilter& operator=(InPlaceDream3DDataToImageFilter&&) = delete;      // Move Assignment Not Implemented
};
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkInPlaceDream3DDataToImageFilter.hxx"
#endif


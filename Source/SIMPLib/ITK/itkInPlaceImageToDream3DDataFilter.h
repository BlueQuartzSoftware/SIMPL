#pragma once

#include "itkDream3DImage.h"
#include "itkGetComponentsDimensions.h"

#include "SIMPLib/DataContainers/DataContainerArray.h"

#include <itkProcessObject.h>
#include <itkSimpleDataObjectDecorator.h>

#include <itkNumericTraits.h>
#include <itkNumericTraitsRGBAPixel.h>
#include <itkNumericTraitsRGBPixel.h>
#include <itkNumericTraitsVectorPixel.h>

namespace itk
{
template <typename PixelType, unsigned int VDimension> class InPlaceImageToDream3DDataFilter : public ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef InPlaceImageToDream3DDataFilter Self;
  typedef ProcessObject Superclass;
  typedef SmartPointer<Self> Pointer;

  typedef typename itk::Dream3DImage<PixelType, VDimension> ImageType;
  typedef typename ImageType::Pointer ImagePointer;
  typedef typename itk::NumericTraits<PixelType>::ValueType ValueType;
  typedef typename ::DataArray<ValueType> DataArrayPixelType;
  typedef typename itk::SimpleDataObjectDecorator<DataContainer::Pointer> DecoratorType;
  typedef DecoratorType::Pointer DecoratorPointer;
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  itkTypeMacro(InPlaceImageToDream3DDataFilter, ProcessObject);

  virtual void SetInput(const ImageType* image);

  DecoratorType* GetOutput();

  void SetDataContainer(DataContainer::Pointer dc);

  itkSetMacro(DataArrayName, std::string);
  itkGetMacro(DataArrayName, std::string);

  itkSetMacro(AttributeMatrixArrayName, std::string);
  itkGetMacro(AttributeMatrixArrayName, std::string);

  itkSetMacro(InPlace, bool);
  itkGetConstMacro(InPlace, bool);

  itkBooleanMacro(InPlace);

protected:
  InPlaceImageToDream3DDataFilter();
  virtual ~InPlaceImageToDream3DDataFilter();

  virtual void VerifyPreconditions() override;
  ProcessObject::DataObjectPointer MakeOutput(ProcessObject::DataObjectPointerArraySizeType) override;

  virtual void GenerateData() override;
  virtual void GenerateOutputInformation() override;

  void CheckValidArrayPathComponentName(std::string var);

private:
  using Superclass::SetInput;
  InPlaceImageToDream3DDataFilter(const InPlaceImageToDream3DDataFilter&) = delete; // Copy Constructor Not Implemented
  void operator=(const InPlaceImageToDream3DDataFilter&);                           // Move assignment Not Implemented
  std::string m_DataArrayName;
  std::string m_AttributeMatrixArrayName;
  bool m_InPlace; // enable the possibility of in-place
};                // end of class InPlaceImageToDream3DDataFilter
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkInPlaceImageToDream3DDataFilter.hxx"
#endif


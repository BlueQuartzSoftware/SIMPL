#pragma once

#include "itkInPlaceImageToDream3DDataFilter.h"
#include "itkGetComponentsDimensions.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include <QString>

namespace itk
{

template<typename PixelType, unsigned int VDimension>
InPlaceImageToDream3DDataFilter<PixelType,VDimension>
::InPlaceImageToDream3DDataFilter()
{
  m_DataArrayName = (SIMPL::Defaults::CellAttributeMatrixName).toStdString();
  m_AttributeMatrixArrayName = (SIMPL::CellData::ImageData).toStdString();
  // Create the output. We use static_cast<> here because we know the default
  // output must be of type DecoratorType
  typename DecoratorType::Pointer output =
    static_cast< DecoratorType * >(this->MakeOutput(0).GetPointer());
  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, output.GetPointer());
  this->SetNumberOfRequiredInputs(1);
  this->SetDataContainer(DataContainer::NullPointer());
  m_InPlace = true;
}


template< typename PixelType, unsigned int VDimension>
void
InPlaceImageToDream3DDataFilter< PixelType, VDimension >
::SetDataContainer(DataContainer::Pointer dc)
{
  DecoratorType *outputPtr = this->GetOutput();
  outputPtr->Set(dc);
}



template< typename PixelType, unsigned int VDimension>
ProcessObject::DataObjectPointer
InPlaceImageToDream3DDataFilter< PixelType, VDimension >
::MakeOutput(ProcessObject::DataObjectPointerArraySizeType)
{
  return DecoratorType::New().GetPointer();
}


template<typename PixelType, unsigned int VDimension>
InPlaceImageToDream3DDataFilter<PixelType, VDimension>
::~InPlaceImageToDream3DDataFilter()
{
}

template< typename PixelType, unsigned int VDimension>
void
InPlaceImageToDream3DDataFilter< PixelType, VDimension >
::SetInput(const ImageType *input)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0,
    const_cast< ImageType * >(input));
}


template<typename PixelType, unsigned int VDimension>
void
InPlaceImageToDream3DDataFilter<PixelType, VDimension>
::GenerateOutputInformation()
{
  DecoratorType *outputPtr = this->GetOutput();
  DataContainer::Pointer dataContainer = outputPtr->Get();
  //float tol = 0.000001;
  QVector<float> torigin(3, 0);
  QVector<float> tspacing(3, 1);
  QVector<size_t> tDims(3, 1);
  // Get Input image properties
  ImagePointer inputPtr = dynamic_cast<ImageType*>(this->GetInput(0));
  typename ImageType::PointType origin = inputPtr->GetOrigin();
  typename ImageType::SpacingType spacing = inputPtr->GetSpacing();
  typename ImageType::SizeType size = inputPtr->GetLargestPossibleRegion().GetSize();
  //// Create image geometry (data container)
  ImageGeom::Pointer imageGeom;
  IGeometry::Pointer geom = dataContainer->getGeometry();
  if (geom)
  {
    imageGeom = std::dynamic_pointer_cast<ImageGeom>(geom);
    if (!imageGeom)
    {
      itkExceptionMacro("Data container contains a geometry that is not ImageGeometry");
    }
  }
  else
  {
    imageGeom = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    if (!imageGeom)
    {
      itkExceptionMacro("Could not create image geometry");
    }
  }
  for (size_t i = 0; i < VDimension; i++)
  {
	  torigin[i] = origin[i];
	  tspacing[i] = spacing[i];
	  tDims[i] = size[i];
  }
  imageGeom->setOrigin(torigin[0], torigin[1], torigin[2]);
  imageGeom->setResolution(tspacing[0], tspacing[1], tspacing[2]);
  imageGeom->setDimensions(tDims[0], tDims[1], tDims[2]);
  dataContainer->setGeometry(imageGeom);
}

template<typename PixelType, unsigned int VDimension>
void
InPlaceImageToDream3DDataFilter<PixelType, VDimension>
::GenerateData()
{
  DecoratorType *outputPtr = this->GetOutput();
  DataContainer::Pointer dataContainer = outputPtr->Get();
  ImagePointer inputPtr = dynamic_cast<ImageType*>(this->GetInput(0));
  // Create data array
  QVector<size_t> cDims = ITKDream3DHelper::GetComponentsDimensions<PixelType>();
  IGeometry::Pointer geom = dataContainer->getGeometry();
  ImageGeom::Pointer imageGeom = std::dynamic_pointer_cast<ImageGeom>(geom);
  QVector<size_t> tDims(3, 1);
  std::tie(tDims[0], tDims[1], tDims[2]) = imageGeom->getDimensions();
  AttributeMatrix::Pointer attrMat;
  if( dataContainer->doesAttributeMatrixExist(m_AttributeMatrixArrayName.c_str()))
  {
	attrMat = dataContainer->getAttributeMatrix(m_AttributeMatrixArrayName.c_str());
	// Check that attribute matrix type is 'cell'
	if (attrMat->getType() != AttributeMatrix::Type::Cell)
	{
		itkExceptionMacro("Attribute matrix is not of type AttributeMatrix::Type::Cell.");
	}
	// Check that if size does not match, there are no other data array than the one we expect.
	// That makes it possible to modify the attribute matrix without having to worry.
	QVector<size_t> matDims = attrMat->getTupleDimensions();
	if (matDims != tDims)
	{
    if (! ((attrMat->doesAttributeArrayExist(m_DataArrayName.c_str()) && attrMat->getNumAttributeArrays() == 1)
      || ! (attrMat->doesAttributeArrayExist(m_DataArrayName.c_str()) && attrMat->getNumAttributeArrays() == 0)) )
		{
			itkExceptionMacro("Tuples dimension of existing matrix array do not match image size and other attribute arrays are contained in this attribute matrix.");
		}
		dataContainer->removeAttributeMatrix(m_AttributeMatrixArrayName.c_str());
		attrMat = dataContainer->createAndAddAttributeMatrix(tDims, m_AttributeMatrixArrayName.c_str(), AttributeMatrix::Type::Cell);
	}
  }
  else
  {
    attrMat = dataContainer->createAndAddAttributeMatrix(tDims, m_AttributeMatrixArrayName.c_str(), AttributeMatrix::Type::Cell);
  }
  // Checks if doesAttributeArray exists and remove it if it is the case
  if (attrMat->doesAttributeArrayExist(m_DataArrayName.c_str()))
  {
	  IDataArray::Pointer attrArray = attrMat->getAttributeArray(m_DataArrayName.c_str());
	  if (attrArray->getNumberOfTuples() != imageGeom->getNumberOfElements())
	  {
		  attrMat->removeAttributeArray(m_DataArrayName.c_str());
	  }
  }
  typename DataArrayPixelType::Pointer data;
  inputPtr->SetBufferedRegion( inputPtr->GetLargestPossibleRegion() );
  if( m_InPlace )
  {
    inputPtr->GetPixelContainer()->SetContainerManageMemory( false );
    data = DataArrayPixelType::WrapPointer( reinterpret_cast<ValueType*>(inputPtr->GetBufferPointer()),
              imageGeom->getNumberOfElements(), cDims, this->GetDataArrayName().c_str(), true );
  }
  else
  {
    data = DataArrayPixelType::CreateArray(imageGeom->getNumberOfElements(), cDims,
              m_DataArrayName.c_str(), true);
    if (nullptr != data.get())
    {
      ::memcpy(data->getPointer(0), reinterpret_cast<ValueType*>(inputPtr->GetBufferPointer()), imageGeom->getNumberOfElements() * sizeof(ValueType));
    }
  }
  attrMat->addAttributeArray(m_DataArrayName.c_str(), data);
  outputPtr->Set(dataContainer);
}

// Check that names has been initialized correctly
template<typename PixelType, unsigned int VDimension>
void
InPlaceImageToDream3DDataFilter<PixelType, VDimension>
::CheckValidArrayPathComponentName(std::string var)
{
  if (var.find('/') != std::string::npos)
  {
    itkExceptionMacro("Name contains a '/'");
  }
  if (var.empty())
  {
    itkExceptionMacro("Name is empty");
  }
}


// Check that the inputs have been initialized correctly
template<typename PixelType, unsigned int VDimension>
void
InPlaceImageToDream3DDataFilter<PixelType, VDimension>
::VerifyPreconditions()
{
  //Test only works if image if of dimension 2 or 3
  if (VDimension != 2 && VDimension != 3)
  {
    itkExceptionMacro("Dimension must be 2 or 3.");
  }
  CheckValidArrayPathComponentName(m_AttributeMatrixArrayName);
  CheckValidArrayPathComponentName(m_DataArrayName);
  // Verify data container
  DecoratorType *outputPtr = this->GetOutput();
  if (!outputPtr->Get())
  {
    itkExceptionMacro("Data container not set");
  }
  //
  Superclass::VerifyPreconditions();
}

/**
*
*/
template<typename PixelType, unsigned int VDimension>
typename InPlaceImageToDream3DDataFilter<PixelType, VDimension>::DecoratorType*
InPlaceImageToDream3DDataFilter<PixelType, VDimension>
::GetOutput()
{
  return itkDynamicCastInDebugMode< DecoratorType * >(this->GetPrimaryOutput());
}

} // end of itk namespace


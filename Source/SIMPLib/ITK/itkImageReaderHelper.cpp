/* This class is meant to be a stand in until a true utility ITKReader lib is created.
 * the developer should #include this file in their class that needs the functionality
 * and be sure all the methods that are needed are implemented.
 *
 * The developer should #define ITK_IMAGE_READER_CLASS_NAME BlahBlahBlah
 * The developer can use the SIMPLib/ITK/ITKImageReaderHelper.h file in thier header file
 */

#ifndef ITK_IMAGE_READER_CLASS_NAME
#define ITK_IMAGE_READER_CLASS_NAME Foo
#include "SIMPLib/ITK/itkImageReaderHelper.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/ITK/itkGetComponentsDimensions.h"
#include "SIMPLib/ITK/itkInPlaceImageToDream3DDataFilter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include <itkImageFileReader.h>

class ITK_IMAGE_READER_CLASS_NAME : public AbstractFilter
{
public:
  ITK_IMAGE_READER_CLASS_NAME() = default;
  ~ITK_IMAGE_READER_CLASS_NAME() = default;

protected:
  QString getFileName()
  {
    return QString("");
  }

  template <typename TComponent>
  void readImage(const DataArrayPath& dataArrayPath, const itk::ImageIOBase::Pointer& imageIO, const QString& filename, bool dataCheck);
  template <typename TComponent, uint32_t dimensions>
  void readImage(const DataArrayPath& dataArrayPath, const itk::ImageIOBase::Pointer& imageIO, const QString& filename, bool dataCheck);

  template <typename TPixel, uint32_t dimensions>
  void readImage(const DataArrayPath& dataArrayPath, const QString& filename, bool dataCheck);

  void readImage(const DataArrayPath& dataArrayPath, bool dataCheck);

  template <typename TPixel, uint32_t dimensions>
  void readImageOutputInformation(const DataArrayPath& dataArrayPath, typename itk::ImageFileReader<itk::Image<TPixel, dimensions>>::Pointer& reader, DataContainer::Pointer& container);
};

#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename TComponent>
void ITK_IMAGE_READER_CLASS_NAME ::readImage(const DataArrayPath& dataArrayPath, const itk::ImageIOBase::Pointer& imageIO, const QString& filename, bool dataCheck)
{
  const uint32_t dimensions = imageIO->GetNumberOfDimensions();
  switch(dimensions)
  {
  case 1: {
    readImage<TComponent, 1>(dataArrayPath, imageIO, filename, dataCheck);
    break;
  }
  case 2: {
    readImage<TComponent, 2>(dataArrayPath, imageIO, filename, dataCheck);
    break;
  }
  default: {
    readImage<TComponent, 3>(dataArrayPath, imageIO, filename, dataCheck);
    break;
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename TComponent, uint32_t dimensions>
void ITK_IMAGE_READER_CLASS_NAME ::readImage(const DataArrayPath& dataArrayPath, const itk::ImageIOBase::Pointer& imageIO, const QString& filename, bool dataCheck)
{
  using PixelTypeType = itk::ImageIOBase::IOPixelType;
  PixelTypeType pixel = imageIO->GetPixelType();

  const uint32_t nbComponents = imageIO->GetNumberOfComponents();
  switch(pixel)
  {
  case itk::ImageIOBase::SCALAR:
    readImage<TComponent, dimensions>(dataArrayPath, filename, dataCheck);
    break;
  case itk::ImageIOBase::RGBA:
    readImage<itk::RGBAPixel<TComponent>, dimensions>(dataArrayPath, filename, dataCheck);
    break;
  case itk::ImageIOBase::RGB:
    readImage<itk::RGBPixel<TComponent>, dimensions>(dataArrayPath, filename, dataCheck);
    break;
  case itk::ImageIOBase::VECTOR:
    if(nbComponents == 2)
    {
      readImage<itk::Vector<TComponent, 2>, dimensions>(dataArrayPath, filename, dataCheck);
    }
    else if(nbComponents == 3)
    {
      readImage<itk::Vector<TComponent, 3>, dimensions>(dataArrayPath, filename, dataCheck);
    }
    else if(nbComponents == 36)
    {
      readImage<itk::Vector<TComponent, 36>, dimensions>(dataArrayPath, filename, dataCheck);
    }
    else
    {
      QString errorMessage = QString("Unsupported number of components: %1.").arg(nbComponents);
      setErrorCondition(-4, errorMessage);
      break;
    }
    break;
  case itk::ImageIOBase::UNKNOWNPIXELTYPE:
  case itk::ImageIOBase::POINT:
  case itk::ImageIOBase::COVARIANTVECTOR:
  case itk::ImageIOBase::SYMMETRICSECONDRANKTENSOR:
  case itk::ImageIOBase::DIFFUSIONTENSOR3D:
  case itk::ImageIOBase::COMPLEX:
  case itk::ImageIOBase::FIXEDARRAY:
  case itk::ImageIOBase::MATRIX:
  default:
    QString errorMessage = QString("Unsupported pixel type: %1.").arg(itk::ImageIOBase::GetPixelTypeAsString(pixel).c_str());
    setErrorCondition(-4, errorMessage);
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename TPixel, uint32_t dimensions>
void ITK_IMAGE_READER_CLASS_NAME ::readImage(const DataArrayPath& dataArrayPath, const QString& filename, bool dataCheck)
{
  DataContainer::Pointer container = getDataContainerArray()->getDataContainer(dataArrayPath.getDataContainerName());
  if(nullptr == container.get())
  {
    setErrorCondition(-4, "Container not found.");
    return;
  }

  using ImageType = itk::Image<TPixel, dimensions>;
  using ReaderType = itk::ImageFileReader<ImageType>;
  using ToDream3DType = itk::InPlaceImageToDream3DDataFilter<TPixel, dimensions>;

  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename.toStdString().c_str());

  if(dataCheck)
  {
    readImageOutputInformation<TPixel, dimensions>(dataArrayPath, reader, container);
  }
  else
  {
    typename ToDream3DType::Pointer toDream3DFilter = ToDream3DType::New();
    toDream3DFilter->SetInput(reader->GetOutput());
    toDream3DFilter->SetInPlace(true);
    toDream3DFilter->SetAttributeMatrixArrayName(dataArrayPath.getAttributeMatrixName().toStdString());
    toDream3DFilter->SetDataArrayName(dataArrayPath.getDataArrayName().toStdString());
    toDream3DFilter->SetDataContainer(container);
    toDream3DFilter->Update();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITK_IMAGE_READER_CLASS_NAME ::readImage(const DataArrayPath& dataArrayPath, bool dataCheck)
{
  try
  {
    QString filename = getFileName();
    itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(filename.toLatin1(), itk::ImageIOFactory::ReadMode);
    if(nullptr == imageIO)
    {
      QString errorMessage = "ITK could not read the given file \"%1\". Format is likely unsupported.";
      setErrorCondition(-5, errorMessage.arg(filename));
      return;
    }
    imageIO->SetFileName(filename.toLatin1());
    imageIO->ReadImageInformation();

    using ComponentType = itk::ImageIOBase::IOComponentType;
    const ComponentType component = imageIO->GetComponentType();
    switch(component)
    {
    case itk::ImageIOBase::UCHAR:
      readImage<uint8_t>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::CHAR:
      readImage<int8_t>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::USHORT:
      readImage<uint16_t>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::SHORT:
      readImage<int16_t>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::UINT:
      readImage<uint32_t>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::INT:
      readImage<int32_t>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::ULONG:
      readImage<uint64_t>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::LONG:
      readImage<int64_t>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::FLOAT:
      readImage<float>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::DOUBLE:
      readImage<double>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    default:
      QString errorMessage = QString("Unsupported pixel component: %1.").arg(imageIO->GetComponentTypeAsString(component).c_str());
      setErrorCondition(-4, errorMessage);
      break;
    }
  } catch(itk::ExceptionObject& err)
  {
    QString errorMessage = "ITK exception was thrown while processing input file: %1";
    setErrorCondition(-55557, errorMessage.arg(err.what()));
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename TPixel, uint32_t dimensions>
void ITK_IMAGE_READER_CLASS_NAME ::readImageOutputInformation(const DataArrayPath& dataArrayPath, typename itk::ImageFileReader<itk::Image<TPixel, dimensions>>::Pointer& reader,
                                                              DataContainer::Pointer& container)
{
  using ImageType = itk::Image<TPixel, dimensions>;
  using ValueType = typename itk::NumericTraits<TPixel>::ValueType;
  reader->UpdateOutputInformation();
  const typename ImageType::PointType origin = reader->GetOutput()->GetOrigin();
  const typename ImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  const typename ImageType::SpacingType spacing = reader->GetOutput()->GetSpacing();
  // Initialize torigin/tspacing/tDims since arrays are always of size 3 and ITK image may have a different size.
  FloatVec3Type torigin = {0.0f, 0.0f, 0.0f};
  FloatVec3Type tspacing = {1.0f, 1.0f, 1.0f};
  SizeVec3Type tDims = {1, 1, 1};
  for(size_t i = 0; i < dimensions; i++)
  {
    torigin[i] = static_cast<float>(origin[i]);
    tspacing[i] = static_cast<float>(spacing[i]);
    tDims[i] = static_cast<float>(size[i]);
  }
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  image->setDimensions(tDims);
  image->setOrigin(torigin);
  image->setSpacing(tspacing);
  container->setGeometry(image);

  std::vector<size_t> cDims = ITKDream3DHelper::GetComponentsDimensions<TPixel>();
  std::vector<size_t> qTdims = {tDims[0], tDims[1], tDims[2]};
  AttributeMatrix::Pointer cellAttrMat = container->createNonPrereqAttributeMatrix(this, dataArrayPath.getAttributeMatrixName(), qTdims, AttributeMatrix::Type::Cell);
  if(getErrorCode() < 0)
  {
    return;
  }

  getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<ValueType>>(this, dataArrayPath, 0, cDims);
}

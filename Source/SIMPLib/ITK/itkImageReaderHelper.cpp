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
#include <itkImageFileReader.h>

class ITK_IMAGE_READER_CLASS_NAME : public AbstractFilter
{
public:
  ITK_IMAGE_READER_CLASS_NAME()
  {
  }
  ~ITK_IMAGE_READER_CLASS_NAME() = default;

protected:
  QString getFileName()
  {
    return QString("");
  }

  template <typename TComponent>
  void readImage(const DataArrayPath& dataArrayPath, const itk::ImageIOBase::Pointer& imageIO, const QString& filename, bool dataCheck);
  template <typename TComponent, unsigned int dimensions>
  void readImage(const DataArrayPath& dataArrayPath, const itk::ImageIOBase::Pointer& imageIO, const QString& filename, bool dataCheck);

  template <typename TPixel, unsigned int dimensions>
  void readImage(const DataArrayPath& dataArrayPath, const QString& filename, bool dataCheck);

  void readImage(const DataArrayPath& dataArrayPath, bool dataCheck);

  template <typename TPixel, unsigned int dimensions>
  void readImageOutputInformation(const DataArrayPath& dataArrayPath, typename itk::ImageFileReader<itk::Dream3DImage<TPixel, dimensions>>::Pointer& reader, DataContainer::Pointer& container);
};

#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename TComponent>
void
ITK_IMAGE_READER_CLASS_NAME
::readImage(const DataArrayPath& dataArrayPath, const itk::ImageIOBase::Pointer& imageIO, const QString& filename, bool dataCheck)
{
  const unsigned int dimensions = imageIO->GetNumberOfDimensions();
  switch(dimensions)
  {
  case 1:
  {
    readImage<TComponent, 1>(dataArrayPath, imageIO, filename, dataCheck);
    break;
  }
  case 2:
  {
    readImage<TComponent, 2>(dataArrayPath, imageIO, filename, dataCheck);
    break;
  }
  default:
  {
    readImage<TComponent, 3>(dataArrayPath, imageIO, filename, dataCheck);
    break;
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename TComponent, unsigned int dimensions>
void
ITK_IMAGE_READER_CLASS_NAME
::readImage(const DataArrayPath& dataArrayPath, const itk::ImageIOBase::Pointer& imageIO, const QString& filename, bool dataCheck)
{
  using PixelTypeType = itk::ImageIOBase::IOPixelType;
  // using ComponentType = itk::ImageIOBase::IOComponentType;
  PixelTypeType pixel = imageIO->GetPixelType();
  // ComponentType compType = imageIO->GetComponentType();

  const unsigned int nbComponents = imageIO->GetNumberOfComponents();
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
      setErrorCondition(-4);
      QString errorMessage = QString("Unsupported number of components: %1.").arg(nbComponents);
      notifyErrorMessage(getHumanLabel(), errorMessage, getErrorCondition());
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
    setErrorCondition(-4);
    QString errorMessage = QString("Unsupported pixel type: %1.").arg(itk::ImageIOBase::GetPixelTypeAsString(pixel).c_str());
    notifyErrorMessage(getHumanLabel(), errorMessage, getErrorCondition());
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename TPixel, unsigned int dimensions>
void
ITK_IMAGE_READER_CLASS_NAME
::readImage(const DataArrayPath& dataArrayPath, const QString& filename, bool dataCheck)
{
  DataContainer::Pointer container = getDataContainerArray()->getDataContainer(dataArrayPath.getDataContainerName());
  if(nullptr == container.get())
  {
    setErrorCondition(-4);
    notifyErrorMessage(getHumanLabel(), "Container not found.", getErrorCondition());
    return;
  }

  using ImageType = itk::Dream3DImage<TPixel, dimensions>;
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
void
ITK_IMAGE_READER_CLASS_NAME
::readImage(const DataArrayPath& dataArrayPath, bool dataCheck)
{
  try
  {
    QString filename = getFileName();
    itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(filename.toLatin1(), itk::ImageIOFactory::ReadMode);
    if(nullptr == imageIO)
    {
      setErrorCondition(-5);
      QString errorMessage = "ITK could not read the given file \"%1\". Format is likely unsupported.";
      notifyErrorMessage(getHumanLabel(), errorMessage.arg(filename), getErrorCondition());
      return;
    }
    imageIO->SetFileName(filename.toLatin1());
    imageIO->ReadImageInformation();

    using ComponentType = itk::ImageIOBase::IOComponentType;
    const ComponentType component = imageIO->GetComponentType();
    switch(component)
    {
    case itk::ImageIOBase::UCHAR:
      readImage<unsigned char>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::CHAR:
      readImage<char>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::USHORT:
      readImage<unsigned short>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::SHORT:
      readImage<short>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::UINT:
      readImage<unsigned int>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::INT:
      readImage<int>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::ULONG:
      readImage<unsigned long>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::LONG:
      readImage<long>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::FLOAT:
      readImage<float>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    case itk::ImageIOBase::DOUBLE:
      readImage<double>(dataArrayPath, imageIO, filename, dataCheck);
      break;
    default:
      setErrorCondition(-4);
      QString errorMessage = QString("Unsupported pixel component: %1.").arg(imageIO->GetComponentTypeAsString(component).c_str());
      notifyErrorMessage(getHumanLabel(), errorMessage, getErrorCondition());
      break;
    }
  } catch(itk::ExceptionObject& err)
  {
    setErrorCondition(-55557);
    QString errorMessage = "ITK exception was thrown while processing input file: %1";
    notifyErrorMessage(getHumanLabel(), errorMessage.arg(err.what()), getErrorCondition());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename TPixel, unsigned int dimensions>
void
ITK_IMAGE_READER_CLASS_NAME
::readImageOutputInformation(const DataArrayPath& dataArrayPath, typename itk::ImageFileReader<itk::Dream3DImage<TPixel, dimensions>>::Pointer& reader, DataContainer::Pointer& container)
{
  typedef itk::Dream3DImage<TPixel, dimensions> ImageType;
  typedef typename itk::NumericTraits<TPixel>::ValueType ValueType;
  reader->UpdateOutputInformation();
  const typename ImageType::PointType origin = reader->GetOutput()->GetOrigin();
  const typename ImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  const typename ImageType::SpacingType spacing = reader->GetOutput()->GetSpacing();
  QVector<float> torigin(3, 0);
  QVector<float> tspacing(3, 0);
  QVector<size_t> tDims(3, 1);
  // Initialize torigin/tspacing/tDims since arrays are always of size 3 and ITK image may have a different size.
  for(size_t i = 0; i < dimensions; i++)
  {
    torigin[i] = origin[i];
    tspacing[i] = spacing[i];
    tDims[i] = size[i];
  }
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  image->setDimensions(tDims[0], tDims[1], tDims[2]);
  image->setOrigin(torigin[0], torigin[1], torigin[2]);
  image->setResolution(tspacing[0], tspacing[1], tspacing[2]);
  container->setGeometry(image);

  QVector<size_t> cDims = ITKDream3DHelper::GetComponentsDimensions<TPixel>();
  AttributeMatrix::Pointer cellAttrMat = container->createNonPrereqAttributeMatrix(this, dataArrayPath.getAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(getErrorCondition() < 0)
  {
    return;
  }

  getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<ValueType>, AbstractFilter, ValueType>(this, dataArrayPath, 0, cDims);
}

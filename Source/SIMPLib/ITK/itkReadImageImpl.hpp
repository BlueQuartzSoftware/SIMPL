/* ============================================================================
 * Copyright (c) 2014 William Lenthe
 * Copyright (c) 2014 DREAM3D Consortium
 * All rights reserved.
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
 * Neither the name of William Lenthe or any of the DREAM3D Consortium contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *  This code was partially written under United States Air Force Contract number
 *                              FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

// image reading
#include "itkImageFileReader.h"
#include "itkImageIOBase.h"
#include "itkImageIOFactory.h"
#include "itkRGBAPixel.h"
#include "itkRGBPixel.h"
#include "itkVectorImage.h"

// ImageProcessing Plugin
#include "itkBridge.h"

/**
 * @brief This is a private implementation for the filter that handles the actual algorithm implementation details
 * for us like figuring out if we can use this private implementation with the data array that is assigned.
 */
template <typename PixelType, typename AbstractFilter> class ItkReadImagePrivate
{
public:
  typedef DataArray<PixelType> DataArrayType;

  ItkReadImagePrivate()
  {
  }
  virtual ~ItkReadImagePrivate()
  {
  }

  // -----------------------------------------------------------------------------
  // Determine if this is the proper type of an array to downcast from the IDataArray
  // -----------------------------------------------------------------------------
  bool operator()(IDataArray::Pointer p)
  {
    return (std::dynamic_pointer_cast<DataArrayType>(p).get() != nullptr);
  }

  // -----------------------------------------------------------------------------
  // This is the actual templated algorithm
  // -----------------------------------------------------------------------------
  void static Execute(AbstractFilter* filter, QString inputFile, IDataArray::Pointer outputIDataArray)
  {
    typename DataArrayType::Pointer outputDataPtr = std::dynamic_pointer_cast<DataArrayType>(outputIDataArray);

    // convert arrays to correct type
    PixelType* outputData = static_cast<PixelType*>(outputDataPtr->getPointer(0));
    size_t numVoxels = outputDataPtr->getNumberOfTuples();

    // read image meta data and get pixel type
    itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(inputFile.toLocal8Bit().constData(), itk::ImageIOFactory::ReadMode);
    imageIO->SetFileName(inputFile.toLocal8Bit().data());
    imageIO->ReadImageInformation();
    itk::ImageIOBase::IOPixelType pixelType = imageIO->GetPixelType();

    itk::ProcessObject::Pointer readerObject;

    // read based on pixel type
    switch(pixelType)
    {
    case itk::ImageIOBase::SCALAR:
    {
      typedef itk::Image<PixelType, ImageProcessingConstants::ImageDimension> ImageType;
      typedef itk::ImageFileReader<ImageType> ReaderType;
      typename ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(inputFile.toLocal8Bit().constData());
      reader->GetOutput()->GetPixelContainer()->SetImportPointer(outputData, numVoxels, false);
      readerObject = reader;
    }
    break;

    case itk::ImageIOBase::RGB:
    {
      typedef itk::Image<itk::RGBPixel<PixelType>, ImageProcessingConstants::ImageDimension> ImageType;
      typedef itk::ImageFileReader<ImageType> ReaderType;
      typename ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(inputFile.toLocal8Bit().constData());
      reader->GetOutput()->GetPixelContainer()->SetImportPointer(reinterpret_cast<itk::RGBPixel<PixelType>*>(outputData), numVoxels, false);
      readerObject = reader;
    }
    break;

    case itk::ImageIOBase::RGBA:
    {
      typedef itk::Image<itk::RGBAPixel<PixelType>, ImageProcessingConstants::ImageDimension> ImageType;
      typedef itk::ImageFileReader<ImageType> ReaderType;
      typename ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(inputFile.toLocal8Bit().constData());
      reader->GetOutput()->GetPixelContainer()->SetImportPointer(reinterpret_cast<itk::RGBAPixel<PixelType>*>(outputData), numVoxels, false);
      readerObject = reader;
    }
    break;
    /**
    case itk::ImageIOBase::FIXEDARRAY:
      {
        typedef itk::VectorImage<PixelType>, ImageProcessingConstants::ImageDimension> ImageType;
        typedef itk::ImageFileReader<ImageType> ReaderType;
        typename ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName(inputFile.toLocal8Bit().constData());
        reader->GetOutput()->GetPixelContainer()->SetImportPointer(outputData, numVoxels, false);
        readerObject=reader;
      }break;
      */
    case itk::ImageIOBase::UNKNOWNPIXELTYPE:
    case itk::ImageIOBase::OFFSET:
    case itk::ImageIOBase::VECTOR:
    case itk::ImageIOBase::POINT:
    case itk::ImageIOBase::COVARIANTVECTOR:
    case itk::ImageIOBase::SYMMETRICSECONDRANKTENSOR:
    case itk::ImageIOBase::DIFFUSIONTENSOR3D:
    case itk::ImageIOBase::COMPLEX:
    case itk::ImageIOBase::FIXEDARRAY:
    case itk::ImageIOBase::MATRIX:
      break;
    default:
      filter->setErrorCondition(-2);
      QString message = QObject::tr("Unable to read image '%1'").arg(inputFile);
      filter->notifyErrorMessage(filter->getHumanLabel(), message, filter->getErrorCondition());
      outputIDataArray->resize(0);
    }

    try
    {
      readerObject->Update();
    } catch(itk::ExceptionObject& err)
    {
      filter->setErrorCondition(-5);
      QString ss = QObject::tr("Failed to read image '%1': %2").arg(inputFile).arg(err.GetDescription());
      filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
    }
  }

private:
  ItkReadImagePrivate(const ItkReadImagePrivate&); // Copy Constructor Not Implemented
  void operator=(const ItkReadImagePrivate&);      // Move assignment Not Implemented
};


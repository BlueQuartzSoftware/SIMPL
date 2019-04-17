// Based on the same paradigm as VTKTemplateAliasMacro.h, in
// VTK (www.vtk.org), we create macros that allow to easily
// choose which template version of a function to run.

// --------------------
// Content of this file
// --------------------
// Dream3DTemplateAliasMacro is used in a switch statement to
// automatically generate duplicate code for all enabled types.
// The code can be written to create ITK filters in Dream3D.
//
// This file has 3 entry-point (main macros):
//    Dream3DArraySwitchMacro(call, type, path, errorCondition)
//    Dream3DArraySwitchMacroOutputType(call, path, errorCondition, typeOUT, typeOUTTypename)
//    Dream3DArraySwitchOutputComponentMacro(call, type, path, errorCondition)
//
// ---------
// Rationale
// ---------
// Why use a macro instead of implementing functions in a based class that would be derived:
// If the functionalities implemented in the macros defined in this file were implemented
// in methods defined in a base class that would be derived to specialize the processing
// to perform, the templated base class function would have to call the templated derived
// method that implements the processing. Since it is not possible to overload a templated
// method, the solution of using macro was chosen.
//
// ------------
// Macros usage
// ------------
// Dream3DArraySwitchMacro(...) and Dream3DArraySwitchMacroOutputType(...) correspond to the
// same macro, but the latter allows to specify an "output type" that is different from the
// input type.
//
//   Dream3DArraySwitchMacro(call, path, errorCondition);
//
//       call : templated function to be called for each supported type
//           Expected function signature:
//               template<typename InputPixelType, typename OutputPixelType, unsigned int Dimension>
//               void functionName()
//       type : DataArrayPath that specifies the data array to extract the data type from.
//              Typical value: getSelectedCellArrayPath()
//       errorCondition : integer value set with setErrorCondition(int) if an error occurs
//                        (e.g. type is not supported).
//
//   Dream3DArraySwitchMacroOutputType(call, path, errorCondition, typeOUT, typeOUTTypename);
//
//       call/path/errorCondition: See Dream3DArraySwitchMacro(...) above.
//       typeOUT: output image component type (e.g. float)
//       typeOUTTypename: If typeOUT is defined using the keyword "typename", set this to 1,
//                        otherwise 0.
//
//   Dream3DArraySwitchOutputComponentMacro(call, type, path, errorCondition);
//
//       It is used instead of Dream3DArraySwitchMacroOutputType(...) if the output image
//       component type of a filter is independent from the input image component type.
//       This macro calls Dream3DArraySwitchMacroOutputType(...) for each supported component
//       type (See ITKCastImage.cpp)
//
// --------------------------------
// Pixel and component type support
// --------------------------------
//
// Pixel Types can be individually activated or deactivated. By default, scalar pixel types are
// supported while RGB/RGBA and Vector pixel types are not.
// To remove the support of scalar images, copy the following line before including this header
// file:
//   #define DREAM3D_USE_Scalar 0
// To support RGB/RGBA pixels and vector pixels, copy the following lines:
//   #define DREAM3D_USE_Vector 1
//   #define DREAM3D_USE_RGB_RGBA   1
//
// You can also individually remove the support for certain component type. By default, all
// integer and float types are supported.
// To remove the support of uint8, copy before including this header file:
//   #define DREAM3D_USE_uint8_t 0

#pragma once

#include "SIMPLib/Filtering/AbstractFilter.h"
#include <QString>
#include <itkImageIOBase.h>
#include <itkRGBAPixel.h>
#include <itkRGBPixel.h>
#include <itkVector.h>

//--------------------------------------------------------------------------
// Allow individual switching of support for each scalar size/signedness and
// pixel type.
// These could be made advanced user options to be configured by CMake.
#ifndef DREAM3D_USE_int8_t
#define DREAM3D_USE_int8_t 1
#endif

#ifndef DREAM3D_USE_uint8_t
#define DREAM3D_USE_uint8_t 1
#endif

#ifndef DREAM3D_USE_int16_t
#define DREAM3D_USE_int16_t 1
#endif

#ifndef DREAM3D_USE_uint16_t
#define DREAM3D_USE_uint16_t 1
#endif

#ifndef DREAM3D_USE_int32_t
#define DREAM3D_USE_int32_t 1
#endif

#ifndef DREAM3D_USE_uint32_t
#define DREAM3D_USE_uint32_t 1
#endif

#ifndef DREAM3D_USE_int64_t
#define DREAM3D_USE_int64_t 1
#endif

#ifndef DREAM3D_USE_uint64_t
#define DREAM3D_USE_uint64_t 1
#endif

#ifndef DREAM3D_USE_float
#define DREAM3D_USE_float 1
#endif

#ifndef DREAM3D_USE_double
#define DREAM3D_USE_double 1
#endif

#ifndef DREAM3D_USE_Scalar
#define DREAM3D_USE_Scalar 1
#endif

#ifndef DREAM3D_USE_Vector
#define DREAM3D_USE_Vector 0
#endif

#ifndef DREAM3D_USE_RGB_RGBA
#define DREAM3D_USE_RGB_RGBA 0
#endif
//--------------------------------------------------------------------------

// Define helper macros to switch types on and off.
#define Q(x) #x
#define QUOTE(x) Q(x)

//////////////////////////////////////////////////////////////////////////////
//                          Common to scalar, Vector and RGB/RGBA               //
//////////////////////////////////////////////////////////////////////////////

// Expand the value of typeIN to be able to concatenate it with 'DREAM3D_USE' to check if the type is accepted by the filter
#define Dream3DTemplateAliasMacroCase(typeIN, typeOUT, call, var_type, tDims, errorCondition, isTypeOUT, typeOUTTypename)                      \
  Dream3DTemplateAliasMacroCase0(typeIN, typeOUT, call, var_type, tDims, errorCondition, DREAM3D_USE_##typeIN, isTypeOUT, typeOUTTypename)
#define Dream3DTemplateAliasMacroCase0(typeIN, typeOUT, call, var_type, tDims, errorCondition, value, isTypeOUT, typeOUTTypename)              \
  Dream3DTemplateAliasMacroCase1(typeIN, typeOUT, call, var_type, tDims, errorCondition, value, isTypeOUT, typeOUTTypename)
#define Dream3DTemplateAliasMacroCase1(typeIN, typeOUT, call, var_type, tDims, errorCondition, value, isTypeOUT, typeOUTTypename)              \
  Dream3DTemplateAliasMacroCase_##value(typeIN, typeOUT, call, var_type, tDims, errorCondition, QUOTE(typeIN), isTypeOUT, typeOUTTypename)

// Type is not accepted, throw an error message.
#define Dream3DTemplateAliasMacroCase_0(typeIN, typeOUT, call, var_type, tDims, errorCondition, quotedType, isTypeOUT, typeOUTTypename)        \
  if(var_type.compare(quotedType) == 0)           \
  {                   \
    setErrorCondition(errorCondition);            \
    QString errorMessage = QString("Unsupported pixel type: %1.").arg(quotedType);     \
    notifyErrorMessage(getHumanLabel(), errorMessage, getErrorCondition());   \
  }

// Type is accepted, select the dimension of the input and output images
#define Dream3DTemplateAliasMacroCase_1(typeIN, typeOUT, call, var_type, tDims, errorCondition, quotedType, isTypeOUT, typeOUTTypename)        \
  if(var_type.compare(quotedType) == 0)           \
  {                   \
    if(tDims[2] == 1) \
    {                 \
      /* 2D image */  \
      Dream3DTemplateAliasMacroPixelType(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, 2)     \
    }                 \
    else              \
    {                 \
      /* 3D */        \
      Dream3DTemplateAliasMacroPixelType(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, 3)     \
    }                 \
  }

// Select vector, RGB/RGBA, or scalar images
#define Dream3DTemplateAliasMacroPixelType(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension)                       \
  QVector<size_t> cDims = ptr->getComponentDimensions();                      \
  if(cDims.size() > 1)  \
  {                   \
    Dream3DTemplateAliasMacroCaseVectorImage0(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, DREAM3D_USE_Vector); \
  }                   \
  else                \
  {                   \
    if(cDims[0] == 1) \
    {                 \
      Dream3DTemplateAliasMacroCaseScalarImage0(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, DREAM3D_USE_Scalar);                         \
    }                 \
    else if(cDims[0] == 3 || cDims[0] == 4)       \
    {                 \
      Dream3DTemplateAliasMacroCaseRGBRGBAImage0(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, DREAM3D_USE_RGB_RGBA, cDims[0]);            \
    }                 \
    else              \
    {                 \
      setErrorCondition(errorCondition);          \
      notifyErrorMessage(getHumanLabel(), QString("Size of tuple not handled:%1").arg(cDims[0]), getErrorCondition());                         \
    }                 \
  }

// Replaces typeOUT by typeIN if no typeOUT is given
#define Dream3DTemplateAliasMacroCase_1_0(typeIN, typeOUT, call, typeOUTTypename, dimension) Dream3DTemplateAliasMacroCaseIf(typeIN, typeIN, call, typeOUTTypename, dimension)

// Otherwise, just forward the given arguments to the next macro
#define Dream3DTemplateAliasMacroCase_1_1(typeIN, typeOUT, call, typeOUTTypename, dimension)                       \
  DefineInputImageType##typeOUTTypename(typeIN, dimension) Dream3DTemplateAliasMacroCaseIf(typeIN, typeOUT, call, typeOUTTypename, dimension)

// Call the given function, templated with typeIN, typeOUT, and dimension.
#define Dream3DTemplateAliasMacroCaseIf(typeIN, typeOUT, call, typeOUTTypename, dimension) call<typeIN, typeOUT, dimension>();

// 3 possible pixel type: C-type, InputImageType, and TImageType
#define DefineInputImageType0(typeIN, dimension) // Do nothing, no type to defined
#define DefineInputImageType1(typeIN, dimension) typedef itk::Dream3DImage<typeIN, dimension> InputImageType;
#define DefineInputImageType2(typeIN, dimension) typedef itk::Dream3DImage<typeIN, dimension> TImageType;
///////////////////////////////////////////////////////////////////////////////////////////////
//        Handles scalar images      //
///////////////////////////////////////////////////////////////////////////////////////////////
// Expands the value of 'Scalar' that is 0 if the filter does not accept 'Scalar' images, and '1' if it does.
#define Dream3DTemplateAliasMacroCaseScalarImage0(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, Scalar)        \
  Dream3DTemplateAliasMacroCaseScalarImage1(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, Scalar)
#define Dream3DTemplateAliasMacroCaseScalarImage1(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, Scalar)        \
  Dream3DTemplateAliasMacroCaseScalarImage1_##Scalar(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension)
// Scalar images not accepted, throw an error message if a scalar image is given.
#define Dream3DTemplateAliasMacroCaseScalarImage1_0(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension)              \
  setErrorCondition(errorCondition);              \
  notifyErrorMessage(getHumanLabel(), "Scalar images not supported. Try RGB/RGBA or vector images", getErrorCondition());
// Scalar images accepted
#define Dream3DTemplateAliasMacroCaseScalarImage1_1(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension)              \
  Dream3DTemplateAliasMacroCase_1_##isTypeOUT(typeIN, typeOUT, call, typeOUTTypename, dimension)

///////////////////////////////////////////////////////////////////////////////////////////////
//        Handles vector images      //
///////////////////////////////////////////////////////////////////////////////////////////////
// Define the input and output pixel types, depending on 'isTypeOUT' value. This is required for
// vectors because it is not possible to pass a macro argument that contains a comma.
#define DefineVectorPixelTypes_0(typeIN, typeOUT, vDim)                       \
  typedef itk::Vector<typeIN, vDim> InputPixelType; \
  typedef itk::Vector<typeIN, vDim> OutputPixelType;
#define DefineVectorPixelTypes_1(typeIN, typeOUT, vDim)                       \
  typedef itk::Vector<typeIN, vDim> InputPixelType; \
  typedef typeOUT OutputPixelType;
// Expands the value of 'Vector' that is 0 if the filter does not accept 'Vector' images, and '1' if it does.
#define Dream3DTemplateAliasMacroCaseVectorImage0(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, Vector)        \
  Dream3DTemplateAliasMacroCaseVectorImage1(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, Vector)
#define Dream3DTemplateAliasMacroCaseVectorImage1(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, Vector)        \
  Dream3DTemplateAliasMacroCaseVectorImage1_##Vector(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension)
// Vector images not accepted, throw an error message if a vector image is given.
#define Dream3DTemplateAliasMacroCaseVectorImage1_0(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension)              \
  setErrorCondition(errorCondition);              \
  notifyErrorMessage( \
      getHumanLabel(),  \
      "Vector not supported. Try converting the selected input image to an image with scalar components using 'ITK::RGB to Luminance ImageFilter' or 'Convert Rgb To GrayScale' filters",              \
      getErrorCondition());
// Vector images: Call the given function with the correct dimension after defining the input and output vector types.
#define Dream3DTemplateAliasMacroCaseVectorImage1_1(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension)              \
  if(cDims.size() == 2) \
  {                   \
    DefineVectorPixelTypes_##isTypeOUT(typeIN, typeOUT, 2);                   \
    Dream3DTemplateAliasMacroCaseIf(InputPixelType, OutputPixelType, call, typeOUTTypename, dimension);            \
  }                   \
  else if(cDims.size() == 3)                      \
  {                   \
    DefineVectorPixelTypes_##isTypeOUT(typeIN, typeOUT, 3);                   \
    Dream3DTemplateAliasMacroCaseIf(InputPixelType, OutputPixelType, call, typeOUTTypename, dimension);            \
  }                   \
  else if(cDims.size() == 36)                     \
  {                   \
    DefineVectorPixelTypes_##isTypeOUT(typeIN, typeOUT, 36);                  \
    Dream3DTemplateAliasMacroCaseIf(InputPixelType, OutputPixelType, call, typeOUTTypename, dimension);            \
  }                   \
  else                \
  {                   \
    setErrorCondition(errorCondition);            \
    notifyErrorMessage( \
        getHumanLabel(),                          \
        "Vector dimension not supported. Try converting the selected input image to an image with scalar components using 'ITK::RGB to Luminance ImageFilter' or 'Convert Rgb To GrayScale' filters",  \
        getErrorCondition());                     \
  }

//////////////////////////////////////////////////////////////////////////////
//                          Handles RGB/RGBA images                         //
//////////////////////////////////////////////////////////////////////////////
// Expand 'RGB/RGBA' argument that will be used to call the appropriate macro, depending if the filter accepts 'RGB/RGBA' images or not.
#define Dream3DTemplateAliasMacroCaseRGBRGBAImage0(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, RGBRGBA, nbComponents)                    \
  Dream3DTemplateAliasMacroCaseRGBRGBAImage1(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, RGBRGBA, nbComponents)
#define Dream3DTemplateAliasMacroCaseRGBRGBAImage1(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, RGBRGBA, nbComponents)                    \
  Dream3DTemplateAliasMacroCaseRGBRGBAImage1_##RGBRGBA(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, nbComponents)
// If RGB/RGBA not accepted by the current filter, prints an error message
#define Dream3DTemplateAliasMacroCaseRGBRGBAImage1_0(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, nbComponents) \
  setErrorCondition(errorCondition);              \
  notifyErrorMessage( \
      getHumanLabel(),  \
      "RGB/RGBA not supported. Try converting the selected input image to an image with scalar components using 'ITK::RGB to Luminance ImageFilter' or 'Convert Rgb To GrayScale' filters",            \
      getErrorCondition());
// If RGB/RGBA accepted by current filter, call the macro that will call the given function
#define Dream3DTemplateAliasMacroCaseRGBRGBAImage1_1(typeIN, typeOUT, call, errorCondition, isTypeOUT, typeOUTTypename, dimension, nbComponents) \
  if(nbComponents == 3) \
  {                   \
    Dream3DTemplateAliasMacroCase_1_##isTypeOUT(itk::RGBPixel<typeIN>, itk::RGBPixel<typeOUT>, call, typeOUTTypename, dimension)               \
  }                   \
  else /* 4 components = RGBA*/                   \
  {                   \
    Dream3DTemplateAliasMacroCase_1_##isTypeOUT(itk::RGBAPixel<typeIN>, itk::RGBAPixel<typeOUT>, call, typeOUTTypename, dimension)             \
  }

//////////////////////////////////////////////////////////////////////////////
//                "switch case" statements between each type                //
//////////////////////////////////////////////////////////////////////////////
// Define a macro to dispatch calls to a template instantiated over
// the aliased scalar types.
#define Dream3DTemplateAliasMacro(call, var_type, typeOUT, tDims, errorCondition, isTypeOUT, typeOUTTypename) \
           Dream3DTemplateAliasMacroCase(double, typeOUT, call, var_type, tDims, errorCondition, isTypeOUT, typeOUTTypename) \
      else Dream3DTemplateAliasMacroCase(float, typeOUT, call, var_type, tDims, errorCondition, isTypeOUT, typeOUTTypename) \
      else Dream3DTemplateAliasMacroCase(int8_t, typeOUT, call, var_type, tDims, errorCondition, isTypeOUT, typeOUTTypename) \
      else Dream3DTemplateAliasMacroCase(uint8_t, typeOUT, call, var_type, tDims, errorCondition, isTypeOUT, typeOUTTypename) \
      else Dream3DTemplateAliasMacroCase(int16_t, typeOUT, call, var_type, tDims, errorCondition, isTypeOUT, typeOUTTypename) \
      else Dream3DTemplateAliasMacroCase(uint16_t, typeOUT, call, var_type, tDims, errorCondition, isTypeOUT, typeOUTTypename) \
      else Dream3DTemplateAliasMacroCase(int32_t, typeOUT, call, var_type, tDims, errorCondition, isTypeOUT, typeOUTTypename) \
      else Dream3DTemplateAliasMacroCase(uint32_t, typeOUT, call, var_type, tDims, errorCondition, isTypeOUT, typeOUTTypename) \
      else Dream3DTemplateAliasMacroCase(int64_t, typeOUT, call,  var_type, tDims, errorCondition, isTypeOUT, typeOUTTypename) \
      else Dream3DTemplateAliasMacroCase(uint64_t, typeOUT, call, var_type, tDims, errorCondition, isTypeOUT, typeOUTTypename)

// Define a macro that is specific to Dream3D and dispatches calls to a template
// instantiated over the aliased scalar type based on the type of a data array
// which is saved in the filter's data container array.
#define Dream3DArraySwitchMacroLongOutputType(call, path, errorCondition, typeOUT, isTypeOUT, typeOUTTypename)     \
  {                   \
    IDataArray::Pointer ptr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, path);                    \
    if(nullptr != ptr)  \
    {                 \
      ImageGeom::Pointer imageGeometry = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, path.getDataContainerName());        \
      if(nullptr != imageGeometry)                \
      {               \
        QVector<size_t> tDims(3, 0);              \
        std::tie(tDims[0], tDims[1], tDims[2]) = imageGeometry->getDimensions();       \
        if(getErrorCondition() >= 0)              \
        {             \
          QString type = ptr->getTypeAsString();  \
          Dream3DTemplateAliasMacro(call, type, typeOUT, tDims, errorCondition, isTypeOUT, typeOUTTypename);       \
        }             \
      }               \
      else            \
      {               \
        setErrorCondition(errorCondition);        \
        notifyErrorMessage(getHumanLabel(), "Geometry not found", getErrorCondition());  \
      }               \
    }                 \
    else              \
    {                 \
      setErrorCondition(errorCondition);          \
      notifyErrorMessage(getHumanLabel(), "Array not found", getErrorCondition());     \
    }                 \
  }

//////////////////////////////////////////////////////////////////////////////
//              Main entry points (fixed output component type              //
//////////////////////////////////////////////////////////////////////////////

// Define a macro that is specific to Dream3D and dispatches calls to a template
// instantiated over the aliased scalar type based on the type of a data array
// which is saved in the filter's data container array.
#define Dream3DArraySwitchMacroOutputType(call, path, errorCondition, typeOUT, typeOUTTypename) \
  Dream3DArraySwitchMacroLongOutputType(call, path, errorCondition, typeOUT, 1, typeOUTTypename)

// Define a macro that is specific to Dream3D and dispatches calls to a template
// instantiated over the aliased scalar type based on the type of a data array
// which is saved in the filter's data container array. Output type is of the same
// type as the input data array.
#define Dream3DArraySwitchMacro(call, path, errorCondition) \
  Dream3DArraySwitchMacroLongOutputType(call, path, errorCondition, "", 0, 0)

///////////////////////////////////////////////////////////////////////////////////////
//          Entry Point : manually select the output image component type            //
///////////////////////////////////////////////////////////////////////////////////////
//
// Subtract 1 to enum values because 'type' values are expected to start at 0 while
// itk::ImageIOBase::IOComponentType '0' value is UNKNOWNCOMPONENTTYPE and therefore
// should be skipped.
//
#define Dream3DArraySwitchOutputComponentMacro(call, type, path, errorCondition)       \
  switch(type)        \
  {                   \
  case itk::ImageIOBase::IOComponentType::UCHAR - 1:                          \
    Dream3DArraySwitchMacroOutputType(call, path, -4, uint8_t, 0);            \
    break;            \
  case itk::ImageIOBase::IOComponentType::CHAR - 1: \
    Dream3DArraySwitchMacroOutputType(call, path, -4, int8_t, 0);             \
    break;            \
  case itk::ImageIOBase::IOComponentType::USHORT - 1:                         \
    Dream3DArraySwitchMacroOutputType(call, path, -4, uint16_t, 0);           \
    break;            \
  case itk::ImageIOBase::IOComponentType::SHORT - 1:                          \
    Dream3DArraySwitchMacroOutputType(call, path, -4, int16_t, 0);            \
    break;            \
  case itk::ImageIOBase::IOComponentType::UINT - 1: \
    Dream3DArraySwitchMacroOutputType(call, path, -4, uint32_t, 0);           \
    break;            \
  case itk::ImageIOBase::IOComponentType::INT - 1:  \
    Dream3DArraySwitchMacroOutputType(call, path, -4, int32_t, 0);            \
    break;            \
  case itk::ImageIOBase::IOComponentType::ULONG - 1:                          \
    Dream3DArraySwitchMacroOutputType(call, path, -4, uint64_t, 0);           \
    break;            \
  case itk::ImageIOBase::IOComponentType::LONG - 1: \
    Dream3DArraySwitchMacroOutputType(call, path, -4, int64_t, 0);            \
    break;            \
  case itk::ImageIOBase::IOComponentType::FLOAT - 1:                          \
    Dream3DArraySwitchMacroOutputType(call, path, -4, float, 0);              \
    break;            \
  case itk::ImageIOBase::IOComponentType::DOUBLE - 1:                         \
    Dream3DArraySwitchMacroOutputType(call, path, -4, double, 0);             \
    break;            \
  default:            \
    setErrorCondition(-4);                        \
    notifyErrorMessage(getHumanLabel(), "Unsupported pixel component", errorCondition);  \
    break;            \
  }

////////////////////////////////////////////////////////////////////////////////////////
// Entry Point : Switch case to select output template type based on given data array //
////////////////////////////////////////////////////////////////////////////////////////
//
// Given a data array, extract component type as a string, convert it a an integer
// corresponding to an ITK ImageIO component type, and call
// `Dream3DArraySwitchOutputComponentMacro()`
//
#define Dream3DArrayOutputComponentFromDataMacro(call, input2_path, input1_path, errorCondition)                                                                                                       \
  {                                                                                                                                                                                                    \
    IDataArray::Pointer ptr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, input2_path);                                                                     \
    if(ptr.get() != nullptr)                                                                                                                                                                           \
    {                                                                                                                                                                                                  \
      ImageGeom::Pointer imageGeometry = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, input2_path.getDataContainerName());                             \
      if(imageGeometry.get() != nullptr)                                                                                                                                                               \
      {                                                                                                                                                                                                \
        QVector<size_t> tDims(3, 0);                                                                                                                                                                   \
        std::tie(tDims[0], tDims[1], tDims[2]) = imageGeometry->getDimensions();                                                                                                                       \
        if(getErrorCondition() >= 0)                                                                                                                                                                   \
        {                                                                                                                                                                                              \
          QString str_type = ptr->getTypeAsString();                                                                                                                                                   \
          itk::ImageIOBase::IOComponentType type = itk::ImageIOBase::GetComponentTypeFromString(str_type.toStdString());                                                                               \
          Dream3DArraySwitchOutputComponentMacro(call, type, input1_path, errorCondition)                                                                                                              \
        }                                                                                                                                                                                              \
      }                                                                                                                                                                                                \
      else                                                                                                                                                                                             \
      {                                                                                                                                                                                                \
        setErrorCondition(errorCondition);                                                                                                                                                             \
        notifyErrorMessage(getHumanLabel(), "Geometry not found", getErrorCondition());                                                                                                                \
      }                                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
    else                                                                                                                                                                                               \
    {                                                                                                                                                                                                  \
      setErrorCondition(errorCondition);                                                                                                                                                               \
      notifyErrorMessage(getHumanLabel(), "Array not found", getErrorCondition());                                                                                                                     \
    }                                                                                                                                                                                                  \
  }


#pragma once

#include <QVector>
#include <itkRGBAPixel.h>
#include <itkRGBPixel.h>
#include <itkVector.h>

namespace ITKDream3DHelper
{

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(PixelType*)
{
  QVector<size_t> cDims(1, 1);
  return cDims;
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 36>*)
{
  QVector<size_t> cDims(36, 1);
  return cDims;
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 3>*)
{
  QVector<size_t> cDims(3, 1);
  return cDims;
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 2>*)
{
  QVector<size_t> cDims(2, 1);
  return cDims;
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::RGBPixel<PixelType>*)
{
  QVector<size_t> cDims(1, 3);
  return cDims;
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::RGBAPixel<PixelType>*)
{
  QVector<size_t> cDims(1, 4);
  return cDims;
}

template <class PixelType> QVector<size_t> GetComponentsDimensions()
{
  return GetComponentsDimensions_impl(static_cast<PixelType*>(0));
}

} // namespace ITKDream3DHelper


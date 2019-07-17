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
  QVector<size_t> cDims(1, 36);
  return cDims;
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 3>*)
{
  QVector<size_t> cDims(1, 3);
  return cDims;
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 2>*)
{
  QVector<size_t> cDims(1, 2);
  return cDims;
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 4>*)
{
  QVector<size_t> cDims(1, 4);
  return cDims;
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 11>*)
{
  QVector<size_t> cDims(1, 11);
  return cDims;
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 10>*)
{
  QVector<size_t> cDims(1, 10);
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


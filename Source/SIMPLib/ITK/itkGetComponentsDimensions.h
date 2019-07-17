#pragma once

#include <QVector>
#include <itkRGBAPixel.h>
#include <itkRGBPixel.h>
#include <itkVector.h>

namespace ITKDream3DHelper
{

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(PixelType*)
{
  return {1};
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 36>*)
{
  return {36};
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 3>*)
{
  return {3};
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 2>*)
{
  return {2};
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 4>*)
{
  return {4};
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 11>*)
{
  return {11};
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 10>*)
{
  return {10};
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::RGBPixel<PixelType>*)
{
  return {3};
}

template <class PixelType> QVector<size_t> GetComponentsDimensions_impl(itk::RGBAPixel<PixelType>*)
{
  return {4};
}

template <class PixelType> QVector<size_t> GetComponentsDimensions()
{
  return GetComponentsDimensions_impl(static_cast<PixelType*>(0));
}

} // namespace ITKDream3DHelper


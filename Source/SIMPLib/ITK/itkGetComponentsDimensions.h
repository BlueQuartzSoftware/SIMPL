/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <vector>

#include <itkRGBAPixel.h>
#include <itkRGBPixel.h>
#include <itkVector.h>

namespace ITKDream3DHelper
{

template <class PixelType>
std::vector<size_t> GetComponentsDimensions_impl(PixelType*)
{
  return {1};
}

template <class PixelType>
std::vector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 36>*)
{
  return {36};
}

template <class PixelType>
std::vector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 3>*)
{
  return {3};
}

template <class PixelType>
std::vector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 2>*)
{
  return {2};
}

template <class PixelType>
std::vector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 4>*)
{
  return {4};
}

template <class PixelType>
std::vector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 11>*)
{
  return {11};
}

template <class PixelType>
std::vector<size_t> GetComponentsDimensions_impl(itk::Vector<PixelType, 10>*)
{
  return {10};
}

template <class PixelType>
std::vector<size_t> GetComponentsDimensions_impl(itk::RGBPixel<PixelType>*)
{
  return {3};
}

template <class PixelType>
std::vector<size_t> GetComponentsDimensions_impl(itk::RGBAPixel<PixelType>*)
{
  return {4};
}

template <class PixelType>
std::vector<size_t> GetComponentsDimensions()
{
  return GetComponentsDimensions_impl(static_cast<PixelType*>(0));
}

} // namespace ITKDream3DHelper

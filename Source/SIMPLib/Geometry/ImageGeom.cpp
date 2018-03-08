/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ============================================================================
 * ImageGeom re-implements code from the following vtk modules:
 *
 * * vtkLine.cxx
 *   - re-implemented vtkVoxel::InterpolationDerivs to ImageGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - re-implemented vtkGradientFilter template function ComputeGradientsSG to
 *     ImageGeom::findDerivatives
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SIMPLib/Geometry/ImageGeom.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "H5Support/H5Lite.h"
#include "SIMPLib/Geometry/GeometryHelpers.h"
#include "SIMPLib/HDF5/VTKH5Constants.h"

/**
 * @brief The FindImageDerivativesImpl class implements a threaded algorithm that computes the
 * derivative of an arbitrary dimensional field on the underlying image
 */
class FindImageDerivativesImpl
{
public:
  FindImageDerivativesImpl(ImageGeom* image, DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivs)
  : m_Image(image)
  , m_Field(field)
  , m_Derivatives(derivs)
  {
  }
  virtual ~FindImageDerivativesImpl()
  {
  }

  void compute(size_t zStart, size_t zEnd, size_t yStart, size_t yEnd, size_t xStart, size_t xEnd) const
  {
    double xp[3] = {0.0, 0.0, 0.0};
    double xm[3] = {0.0, 0.0, 0.0};
    double factor = 0.0;
    double xxi, yxi, zxi, xeta, yeta, zeta, xzeta, yzeta, zzeta;
    xxi = yxi = zxi = xeta = yeta = zeta = xzeta = yzeta = zzeta = 0;
    double aj, xix, xiy, xiz, etax, etay, etaz, zetax, zetay, zetaz;
    aj = xix = xiy = xiz = etax = etay = etaz = zetax = zetay = zetaz = 0;
    size_t index = 0;
    int32_t numComps = m_Field->getNumberOfComponents();
    double* fieldPtr = m_Field->getPointer(0);
    double* derivsPtr = m_Derivatives->getPointer(0);
    std::vector<double> plusValues(numComps);
    std::vector<double> minusValues(numComps);
    std::vector<double> dValuesdXi(numComps);
    std::vector<double> dValuesdEta(numComps);
    std::vector<double> dValuesdZeta(numComps);

    size_t dims[3] = {0, 0, 0};
    m_Image->getDimensions(dims);

    int64_t counter = 0;
    size_t totalElements = m_Image->getNumberOfElements();
    int64_t progIncrement = static_cast<int64_t>(totalElements / 100);

    for(size_t z = zStart; z < zEnd; z++)
    {
      for(size_t y = yStart; y < yEnd; y++)
      {
        for(size_t x = xStart; x < xEnd; x++)
        {
          //  Xi derivatives (X)
          if(dims[0] == 1)
          {
            findValuesForFiniteDifference(TwoDimensional, XDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(x == 0)
          {
            findValuesForFiniteDifference(LeftSide, XDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(x == (dims[0] - 1))
          {
            findValuesForFiniteDifference(RightSide, XDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else
          {
            findValuesForFiniteDifference(Centered, XDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }

          xxi = factor * (xp[0] - xm[0]);
          yxi = factor * (xp[1] - xm[1]);
          zxi = factor * (xp[2] - xm[2]);
          for(int32_t i = 0; i < numComps; i++)
          {
            dValuesdXi[i] = factor * (plusValues[i] - minusValues[i]);
          }

          //  Eta derivatives (Y)
          if(dims[1] == 1)
          {
            findValuesForFiniteDifference(TwoDimensional, YDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(y == 0)
          {
            findValuesForFiniteDifference(LeftSide, YDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(y == (dims[1] - 1))
          {
            findValuesForFiniteDifference(RightSide, YDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else
          {
            findValuesForFiniteDifference(Centered, YDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }

          xeta = factor * (xp[0] - xm[0]);
          yeta = factor * (xp[1] - xm[1]);
          zeta = factor * (xp[2] - xm[2]);
          for(int32_t i = 0; i < numComps; i++)
          {
            dValuesdEta[i] = factor * (plusValues[i] - minusValues[i]);
          }

          //  Zeta derivatives (Z)
          if(dims[2] == 1)
          {
            findValuesForFiniteDifference(TwoDimensional, ZDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(z == 0)
          {
            findValuesForFiniteDifference(LeftSide, ZDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(z == (dims[2] - 1))
          {
            findValuesForFiniteDifference(RightSide, ZDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else
          {
            findValuesForFiniteDifference(Centered, ZDirection, x, y, z, dims, xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }

          xzeta = factor * (xp[0] - xm[0]);
          yzeta = factor * (xp[1] - xm[1]);
          zzeta = factor * (xp[2] - xm[2]);
          for(int32_t i = 0; i < numComps; i++)
          {
            dValuesdZeta[i] = factor * (plusValues[i] - minusValues[i]);
          }

          // Now calculate the Jacobian.  Grids occasionally have
          // singularities, or points where the Jacobian is infinite (the
          // inverse is zero).  For these cases, we'll set the Jacobian to
          // zero, which will result in a zero derivative.
          aj = xxi * yeta * zzeta + yxi * zeta * xzeta + zxi * xeta * yzeta - zxi * yeta * xzeta - yxi * xeta * zzeta - xxi * zeta * yzeta;
          if(aj != 0.0)
          {
            aj = 1.0 / aj;
          }

          //  Xi metrics
          xix = aj * (yeta * zzeta - zeta * yzeta);
          xiy = -aj * (xeta * zzeta - zeta * xzeta);
          xiz = aj * (xeta * yzeta - yeta * xzeta);

          //  Eta metrics
          etax = -aj * (yxi * zzeta - zxi * yzeta);
          etay = aj * (xxi * zzeta - zxi * xzeta);
          etaz = -aj * (xxi * yzeta - yxi * xzeta);

          //  Zeta metrics
          zetax = aj * (yxi * zeta - zxi * yeta);
          zetay = -aj * (xxi * zeta - zxi * xeta);
          zetaz = aj * (xxi * yeta - yxi * xeta);

          // Compute the actual derivatives
          index = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
          for(int32_t i = 0; i < numComps; i++)
          {
            derivsPtr[index * numComps * 3 + i * 3] = xix * dValuesdXi[i] + etax * dValuesdEta[i] + zetax * dValuesdZeta[i];

            derivsPtr[index * numComps * 3 + i * 3 + 1] = xiy * dValuesdXi[i] + etay * dValuesdEta[i] + zetay * dValuesdZeta[i];

            derivsPtr[index * numComps * 3 + i * 3 + 2] = xiz * dValuesdXi[i] + etaz * dValuesdEta[i] + zetaz * dValuesdZeta[i];
          }

          if(counter > progIncrement)
          {
            m_Image->sendThreadSafeProgressMessage(counter, totalElements);
            counter = 0;
          }
          counter++;
        }
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range3d<size_t, size_t, size_t>& r) const
  {
    compute(r.pages().begin(), r.pages().end(), r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
  }
#endif

  void computeIndices(int32_t differenceType, int32_t directionType, size_t& index1, size_t& index2, size_t dims[3], size_t x, size_t y, size_t z, double xp[3], double xm[3]) const

  {
    size_t tmpIndex1 = 0;
    size_t tmpIndex2 = 0;

    switch(directionType)
    {
    case XDirection:
    {
      if(differenceType == LeftSide)
      {
        index1 = (z * dims[1] * dims[0]) + (y * dims[0]) + (x + 1);
        index2 = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
        tmpIndex1 = x + 1;
        m_Image->getCoords(tmpIndex1, y, z, xp);
        m_Image->getCoords(x, y, z, xm);
      }
      else if(differenceType == RightSide)
      {
        index1 = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
        index2 = (z * dims[1] * dims[0]) + (y * dims[0]) + (x - 1);
        tmpIndex1 = x - 1;
        m_Image->getCoords(x, y, z, xp);
        m_Image->getCoords(tmpIndex1, y, z, xm);
      }
      else if(differenceType == Centered)
      {
        index1 = (z * dims[1] * dims[0]) + (y * dims[0]) + (x + 1);
        index2 = (z * dims[1] * dims[0]) + (y * dims[0]) + (x - 1);
        tmpIndex1 = x + 1;
        tmpIndex2 = x - 1;
        m_Image->getCoords(tmpIndex1, y, z, xp);
        m_Image->getCoords(tmpIndex2, y, z, xm);
      }
      break;
    }
    case YDirection:
    {
      if(differenceType == LeftSide)
      {
        index1 = (z * dims[1] * dims[0]) + ((y + 1) * dims[0]) + x;
        index2 = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
        tmpIndex1 = y + 1;
        m_Image->getCoords(x, tmpIndex1, z, xp);
        m_Image->getCoords(x, y, z, xm);
      }
      else if(differenceType == RightSide)
      {
        index1 = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
        index2 = (z * dims[1] * dims[0]) + ((y - 1) * dims[0]) + x;
        tmpIndex1 = y - 1;
        m_Image->getCoords(x, y, z, xp);
        m_Image->getCoords(x, tmpIndex1, z, xm);
      }
      else if(differenceType == Centered)
      {
        index1 = (z * dims[1] * dims[0]) + ((y + 1) * dims[0]) + x;
        index2 = (z * dims[1] * dims[0]) + ((y - 1) * dims[0]) + x;
        tmpIndex1 = y + 1;
        tmpIndex2 = y - 1;
        m_Image->getCoords(x, tmpIndex1, z, xp);
        m_Image->getCoords(x, tmpIndex2, z, xm);
      }
      break;
    }
    case ZDirection:
    {
      if(differenceType == LeftSide)
      {
        index1 = ((z + 1) * dims[1] * dims[0]) + (y * dims[0]) + x;
        index2 = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
        tmpIndex1 = z + 1;
        m_Image->getCoords(x, y, tmpIndex1, xp);
        m_Image->getCoords(x, y, z, xm);
      }
      else if(differenceType == RightSide)
      {
        index1 = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
        index2 = ((z - 1) * dims[1] * dims[0]) + (y * dims[0]) + x;
        tmpIndex1 = z - 1;
        m_Image->getCoords(x, y, z, xp);
        m_Image->getCoords(x, y, tmpIndex1, xm);
      }
      else if(differenceType == Centered)
      {
        index1 = ((z + 1) * dims[1] * dims[0]) + (y * dims[0]) + x;
        index2 = ((z - 1) * dims[1] * dims[0]) + (y * dims[0]) + x;
        tmpIndex1 = z + 1;
        tmpIndex2 = z - 1;
        m_Image->getCoords(x, y, tmpIndex1, xp);
        m_Image->getCoords(x, y, tmpIndex2, xm);
      }
      break;
    }
    default:
    {
      break;
    }
    }
  }

  void findValuesForFiniteDifference(int32_t differenceType, int32_t directionType, size_t x, size_t y, size_t z, size_t dims[3], double xp[3], double xm[3], double& factor, int32_t numComps,
                                     std::vector<double>& plusValues, std::vector<double>& minusValues, double* field) const
  {
    size_t index1 = 0;
    size_t index2 = 0;

    factor = 1.0;

    if(differenceType == TwoDimensional)
    {
      for(size_t i = 0; i < 3; i++)
      {
        xp[i] = xm[i] = 0.0;
      }
      xp[directionType] = 1.0;
      for(int32_t i = 0; i < numComps; i++)
      {
        plusValues[i] = minusValues[i] = 0.0;
      }
    }
    else
    {
      computeIndices(differenceType, directionType, index1, index2, dims, x, y, z, xp, xm);
      for(int32_t i = 0; i < numComps; i++)
      {
        plusValues[i] = field[index1 * numComps + i];
        minusValues[i] = field[index2 * numComps + i];
      }
    }

    if(differenceType == Centered)
    {
      factor = 0.5;
    }
  }

private:
  ImageGeom* m_Image;
  DoubleArrayType::Pointer m_Field;
  DoubleArrayType::Pointer m_Derivatives;

  enum FiniteDifferenceType_t
  {
    TwoDimensional = 0,
    LeftSide,
    RightSide,
    Centered
  };

  enum DirectionType_t
  {
    XDirection = 0,
    YDirection,
    ZDirection
  };
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageGeom::ImageGeom()
{
  m_GeometryTypeName = SIMPL::Geometry::ImageGeometry;
  m_GeometryType = IGeometry::Type::Image;
  m_XdmfGridType = SIMPL::XdmfGridType::RectilinearGrid;
  m_MessagePrefix = "";
  m_MessageTitle = "";
  m_MessageLabel = "";
  m_UnitDimensionality = 3;
  m_SpatialDimensionality = 3;
  m_Dimensions[0] = 0;
  m_Dimensions[1] = 0;
  m_Dimensions[2] = 0;
  m_Resolution[0] = 1.0f;
  m_Resolution[1] = 1.0f;
  m_Resolution[2] = 1.0f;
  m_Origin[0] = 0.0f;
  m_Origin[1] = 0.0f;
  m_Origin[2] = 0.0f;
  m_VoxelSizes = FloatArrayType::NullPointer();
  m_ProgressCounter = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageGeom::~ImageGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageGeom::Pointer ImageGeom::CreateGeometry(const QString& name)
{
  if(name.isEmpty() == true)
  {
    return NullPointer();
  }
  ImageGeom* d = new ImageGeom();
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getBoundingBox(float &xMin, float &xMax, float &yMin, float &yMax, float &zMin, float &zMax)
{
    xMin = m_Origin[0];
    xMax = m_Origin[0] + (m_Dimensions[0] * m_Resolution[0]);
    yMin = m_Origin[1];
    yMax = m_Origin[1] + (m_Dimensions[1] * m_Resolution[1]);
    zMin = m_Origin[2];
    zMax = m_Origin[2] + (m_Dimensions[2] * m_Resolution[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getBoundingBox(float boundingBox[6])
{
    boundingBox[0] = m_Origin[0];
    boundingBox[1] = m_Origin[0] + (m_Dimensions[0] * m_Resolution[0]);
    boundingBox[2] = m_Origin[1];
    boundingBox[3] = m_Origin[1] + (m_Dimensions[1] * m_Resolution[1]);
    boundingBox[4] = m_Origin[2];
    boundingBox[5] = m_Origin[2] + (m_Dimensions[2] * m_Resolution[2]);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ImageGeom::getXPoints() 
{
  return m_Dimensions[0];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ImageGeom::getYPoints() 
{ 
  return m_Dimensions[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ImageGeom::getZPoints() 
{
  return m_Dimensions[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t idx[3], float coords[3])
{
  coords[0] = idx[0] * m_Resolution[0] + m_Origin[0];
  coords[1] = idx[1] * m_Resolution[1] + m_Origin[1];
  coords[2] = idx[2] * m_Resolution[2] + m_Origin[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t x, size_t y, size_t z, float coords[3])
{
  coords[0] = x * m_Resolution[0] + m_Origin[0];
  coords[1] = y * m_Resolution[1] + m_Origin[1];
  coords[2] = z * m_Resolution[2] + m_Origin[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t idx, float coords[3])
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  coords[0] = column * m_Resolution[0] + m_Origin[0];
  coords[1] = row * m_Resolution[1] + m_Origin[1];
  coords[2] = plane * m_Resolution[2] + m_Origin[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t idx[3], double coords[3])
{
  coords[0] = static_cast<double>(idx[0] * m_Resolution[0] + m_Origin[0]);
  coords[1] = static_cast<double>(idx[1] * m_Resolution[1] + m_Origin[1]);
  coords[2] = static_cast<double>(idx[2] * m_Resolution[2] + m_Origin[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t x, size_t y, size_t z, double coords[3])
{
  coords[0] = static_cast<double>(x * m_Resolution[0] + m_Origin[0]);
  coords[1] = static_cast<double>(y * m_Resolution[1] + m_Origin[1]);
  coords[2] = static_cast<double>(z * m_Resolution[2] + m_Origin[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t idx, double coords[3])
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  coords[0] = static_cast<double>(column * m_Resolution[0] + m_Origin[0]);
  coords[1] = static_cast<double>(row * m_Resolution[1] + m_Origin[1]);
  coords[2] = static_cast<double>(plane * m_Resolution[2] + m_Origin[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t idx[3], float coords[3])
{
  coords[0] = idx[0] * m_Resolution[0] + m_Origin[0] + (0.5f * m_Resolution[0]);
  coords[1] = idx[1] * m_Resolution[1] + m_Origin[1] + (0.5f * m_Resolution[1]);
  coords[2] = idx[2] * m_Resolution[2] + m_Origin[2] + (0.5f * m_Resolution[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t x, size_t y, size_t z, float coords[3])
{
  coords[0] = x * m_Resolution[0] + m_Origin[0] + (0.5f * m_Resolution[0]);
  coords[1] = y * m_Resolution[1] + m_Origin[1] + (0.5f * m_Resolution[1]);
  coords[2] = z * m_Resolution[2] + m_Origin[2] + (0.5f * m_Resolution[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t idx, float coords[3])
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  coords[0] = column * m_Resolution[0] + m_Origin[0] + (0.5f * m_Resolution[0]);
  coords[1] = row * m_Resolution[1] + m_Origin[1] + (0.5f * m_Resolution[1]);
  coords[2] = plane * m_Resolution[2] + m_Origin[2] + (0.5f * m_Resolution[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t idx[3], double coords[3])
{
  coords[0] = static_cast<double>(idx[0] * m_Resolution[0] + m_Origin[0] + (0.5f * m_Resolution[0]));
  coords[1] = static_cast<double>(idx[1] * m_Resolution[1] + m_Origin[1] + (0.5f * m_Resolution[1]));
  coords[2] = static_cast<double>(idx[2] * m_Resolution[2] + m_Origin[2] + (0.5f * m_Resolution[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t x, size_t y, size_t z, double coords[3])
{
  coords[0] = static_cast<double>(x * m_Resolution[0] + m_Origin[0] + (0.5f * m_Resolution[0]));
  coords[1] = static_cast<double>(y * m_Resolution[1] + m_Origin[1] + (0.5f * m_Resolution[1]));
  coords[2] = static_cast<double>(z * m_Resolution[2] + m_Origin[2] + (0.5f * m_Resolution[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t idx, double coords[3])
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  coords[0] = static_cast<double>(column * m_Resolution[0] + m_Origin[0] + (0.5f * m_Resolution[0]));
  coords[1] = static_cast<double>(row * m_Resolution[1] + m_Origin[1] + (0.5f * m_Resolution[1]));
  coords[2] = static_cast<double>(plane * m_Resolution[2] + m_Origin[2] + (0.5f * m_Resolution[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::initializeWithZeros()
{
  for(size_t i = 0; i < 3; i++)
  {
    m_Dimensions[i] = 0;
    m_Resolution[i] = 1.0f;
    m_Origin[i] = 0.0f;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if(data->getType() != AttributeMatrix::Type::Cell)
  {
    // ImageGeom can only accept cell Attribute Matrices
    return;
  }
  if(data->getNumberOfTuples() != getNumberOfElements())
  {
    return;
  }
  if(data->getName().compare(name) != 0)
  {
    data->setName(name);
  }
  m_AttributeMatrices[name] = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ImageGeom::getNumberOfElements()
{
  return (m_Dimensions[0] * m_Dimensions[1] * m_Dimensions[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::findElementsContainingVert()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer ImageGeom::getElementsContainingVert()
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::setElementsContainingVert(ElementDynamicList::Pointer SIMPL_NOT_USED(elementsContainingVert))
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::deleteElementsContainingVert()
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::findElementNeighbors()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer ImageGeom::getElementNeighbors()
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::setElementNeighbors(ElementDynamicList::Pointer SIMPL_NOT_USED(elementNeighbors))
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::deleteElementNeighbors()
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::findElementCentroids()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer ImageGeom::getElementCentroids()
{
  return FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::setElementCentroids(FloatArrayType::Pointer SIMPL_NOT_USED(elementsCentroids))
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::deleteElementCentroids()
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::findElementSizes()
{
  if (getXRes() <= 0.0f || getYRes() <= 0.0f || getZRes() <= 0.0f)
  {
    return -1;
  }
  m_VoxelSizes = FloatArrayType::CreateArray(getNumberOfElements(), SIMPL::StringConstants::VoxelSizes);
  m_VoxelSizes->initializeWithValue(getXRes() * getYRes() * getZRes());
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer ImageGeom::getElementSizes()
{
  return m_VoxelSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::setElementSizes(FloatArrayType::Pointer elementSizes)
{
  m_VoxelSizes = elementSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::deleteElementSizes()
{
  m_VoxelSizes = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getParametricCenter(double pCoords[3])
{
  pCoords[0] = 0.5;
  pCoords[1] = 0.5;
  pCoords[2] = 0.5;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getShapeFunctions(double pCoords[3], double* shape)
{
  double rm = 0.0;
  double sm = 0.0;
  double tm = 0.0;

  rm = 1.0 - pCoords[0];
  sm = 1.0 - pCoords[1];
  tm = 1.0 - pCoords[2];

  // r derivatives
  shape[0] = -sm * tm;
  shape[1] = sm * tm;
  shape[2] = -pCoords[1] * tm;
  shape[3] = pCoords[1] * tm;
  shape[4] = -sm * pCoords[2];
  shape[5] = sm * pCoords[2];
  shape[6] = -pCoords[1] * pCoords[2];
  shape[7] = pCoords[1] * pCoords[2];

  // s derivatives
  shape[8] = -rm * tm;
  shape[9] = -pCoords[0] * tm;
  shape[10] = rm * tm;
  shape[11] = pCoords[0] * tm;
  shape[12] = -rm * pCoords[2];
  shape[13] = -pCoords[0] * pCoords[2];
  shape[14] = rm * pCoords[2];
  shape[15] = pCoords[0] * pCoords[2];

  // t derivatives
  shape[16] = -rm * sm;
  shape[17] = -pCoords[0] * sm;
  shape[18] = -rm * pCoords[1];
  shape[19] = -pCoords[0] * pCoords[1];
  shape[20] = rm * sm;
  shape[21] = pCoords[0] * sm;
  shape[22] = rm * pCoords[1];
  shape[23] = pCoords[0] * pCoords[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable)
{
  m_ProgressCounter = 0;
  size_t dims[3] = {0, 0, 0};
  getDimensions(dims);

  if(observable)
  {
    connect(this, SIGNAL(filterGeneratedMessage(const PipelineMessage&)), observable, SLOT(broadcastPipelineMessage(const PipelineMessage&)));
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS

  size_t grain = dims[2] == 1 ? 1 : dims[2] / init.default_num_threads();

  if(grain == 0) // This can happen if dims[2] > number of processors
  {
    grain = 1;
  }

  if(doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range3d<size_t, size_t, size_t>(0, dims[2], grain, 0, dims[1], dims[1], 0, dims[0], dims[0]),
                      FindImageDerivativesImpl(this, field, derivatives), tbb::auto_partitioner());
  }
  else
#endif
  {
    FindImageDerivativesImpl serial(this, field, derivatives);
    serial.compute(0, dims[2], 0, dims[1], 0, dims[0]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf))
{
  herr_t err = 0;
  int64_t volDims[3] = {static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints())};
  float spacing[3] = {getXRes(), getYRes(), getZRes()};
  float origin[3] = {0.0f, 0.0f, 0.0f};
  std::tie(origin[0], origin[1], origin[2]) = getOrigin();

  int32_t rank = 1;
  hsize_t dims[1] = {3};

  err = H5Lite::writePointerDataset(parentId, H5_DIMENSIONS, rank, dims, volDims);
  if(err < 0)
  {
    return err;
  }
  err = H5Lite::writePointerDataset(parentId, H5_ORIGIN, rank, dims, origin);
  if(err < 0)
  {
    return err;
  }
  err = H5Lite::writePointerDataset(parentId, H5_SPACING, rank, dims, spacing);
  if(err < 0)
  {
    return err;
  }
  if(m_VoxelSizes.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_VoxelSizes);
    if(err < 0)
    {
      return err;
    }
  }

  err = H5Gclose(parentId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName)
{
  herr_t err = 0;

  int64_t volDims[3] = {static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints())};
  float spacing[3] = {getXRes(), getYRes(), getZRes()};
  float origin[3] = {0.0f, 0.0f, 0.0f};
  std::tie(origin[0], origin[1], origin[2]) = getOrigin();

  out << "  <!-- *************** START OF " << dcName << " *************** -->"
      << "\n";
  out << "  <Grid Name=\"" << dcName << "\" GridType=\"Uniform\">"
      << "\n";
  if(getEnableTimeSeries())
  {
    out << "    <Time TimeType=\"Single\" Value=\"" << getTimeValue() << "\"/>\n";
  }
  out << "    <Topology TopologyType=\"3DCoRectMesh\" Dimensions=\"" << volDims[2] + 1 << " " << volDims[1] + 1 << " " << volDims[0] + 1 << " \"></Topology>"
      << "\n";
  out << "    <Geometry Type=\"ORIGIN_DXDYDZ\">"
      << "\n";
  out << "      <!-- Origin  Z, Y, X -->"
      << "\n";
  out << "      <DataItem Format=\"XML\" Dimensions=\"3\">" << origin[2] << " " << origin[1] << " " << origin[0] << "</DataItem>"
      << "\n";
  out << "      <!-- DxDyDz (Spacing/Resolution) Z, Y, X -->"
      << "\n";
  out << "      <DataItem Format=\"XML\" Dimensions=\"3\">" << spacing[2] << " " << spacing[1] << " " << spacing[0] << "</DataItem>"
      << "\n";
  out << "    </Geometry>"
      << "\n";

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImageGeom::getInfoString(SIMPL::InfoStringFormat format)
{
  QString info;
  QTextStream ss(&info);

  int64_t volDims[3] = {static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints())};
  float spacing[3] = {getXRes(), getYRes(), getZRes()};
  float origin[3] = {0.0f, 0.0f, 0.0f};
  std::tie(origin[0], origin[1], origin[2]) = getOrigin();

  if(format == SIMPL::HtmlFormat)
  {
    ss << "<tr bgcolor=\"#FFFCEA\"><th colspan=2>Geometry Info</th></tr>";
    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Type</th><td>" << GeometryHelpers::Translation::TypeToString(getGeometryType()) << "</td></tr>";
    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Dimensions:</th><td>" << volDims[0] << " x " << volDims[1] << " x " << volDims[2] << "</td></tr>";
    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Origin:</th><td>" << origin[0] << ", " << origin[1] << ", " << origin[2] << "</td></tr>";
    ss << "<tr bgcolor=\"#FFFCEA\"><th align=\"right\">Spacing/Resolution:</th><td>" << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << "</td></tr>";
  }
  else
  {
  }
  return info;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  int err = 0;
  size_t volDims[3] = {0, 0, 0};
  float spacing[3] = {1.0f, 1.0f, 1.0f};
  float origin[3] = {0.0f, 0.0f, 0.0f};
  unsigned int spatialDims = 0;
  QString geomName = "";
  err = gatherMetaData(parentId, volDims, spacing, origin, spatialDims, geomName, preflight);
  if(err < 0)
  {
    return err;
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer ImageGeom::deepCopy(bool forceNoAllocate)
{
  ImageGeom::Pointer imageCopy = ImageGeom::CreateGeometry(getName());

  std::tuple<size_t, size_t, size_t> volDims = std::make_tuple(static_cast<size_t>(0), static_cast<size_t>(0), static_cast<size_t>(0));
  std::tuple<float, float, float> spacing = std::make_tuple(1.0f, 1.0f, 1.0f);
  std::tuple<float, float, float> origin = std::make_tuple(0.0f, 0.0f, 0.0f);
  volDims = getDimensions();
  spacing = getResolution();
  origin = getOrigin();
  imageCopy->setDimensions(volDims);
  imageCopy->setResolution(spacing);
  imageCopy->setOrigin(origin);
  FloatArrayType::Pointer elementSizes = std::dynamic_pointer_cast<FloatArrayType>((getElementSizes().get() == nullptr) ? nullptr : getElementSizes()->deepCopy(forceNoAllocate));
  imageCopy->setElementSizes(elementSizes);
  imageCopy->setSpatialDimensionality(getSpatialDimensionality());

  return imageCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::gatherMetaData(hid_t parentId, size_t volDims[3], float spacing[3], float origin[3], unsigned int spatialDims, QString geomName, bool preflight)
{
  int err = QH5Lite::readPointerDataset(parentId, H5_DIMENSIONS, volDims);
  if(err < 0)
  {
    return err;
  }
  err = QH5Lite::readPointerDataset(parentId, H5_SPACING, spacing);
  if(err < 0)
  {
    return err;
  }
  err = QH5Lite::readPointerDataset(parentId, H5_ORIGIN, origin);
  if(err < 0)
  {
    return err;
  }
  FloatArrayType::Pointer voxelSizes = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::VoxelSizes, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  setDimensions(volDims);
  setResolution(spacing);
  setOrigin(origin);
  setElementSizes(voxelSizes);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageGeom::ErrorType ImageGeom::computeCellIndex(float coords[3], size_t index[3])
{
  ImageGeom::ErrorType err = ImageGeom::ErrorType::NoError;
  for(size_t i = 0; i < 3; i++)
  {
    if(coords[i] < m_Origin[i])
    {
      return static_cast<ImageGeom::ErrorType>(i * 2);
    }
    if(coords[i] > (m_Origin[i] + m_Dimensions[i] * m_Resolution[i]))
    {
      return static_cast<ImageGeom::ErrorType>(i * 2 + 1);
    }
    index[i] = static_cast<size_t>((coords[i] - m_Origin[i]) / m_Resolution[i]);
    if(index[i] > m_Dimensions[i])
    {
      return static_cast<ImageGeom::ErrorType>(i * 2 + 1);
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageGeom::ErrorType ImageGeom::computeCellIndex(float coords[3], size_t& index)
{
  ImageGeom::ErrorType err = ImageGeom::ErrorType::NoError;
  size_t cell[3] = {0, 0, 0};
  for(size_t i = 0; i < 3; i++)
  {
    if(coords[i] < m_Origin[i])
    {
      return static_cast<ImageGeom::ErrorType>(i * 2);
    }
    cell[i] = static_cast<size_t>((coords[i] - m_Origin[i]) / m_Resolution[i]);
    if(cell[i] > m_Dimensions[i])
    {
      return static_cast<ImageGeom::ErrorType>(i * 2 + 1);
    }
  }

  index = (m_Dimensions[0] * m_Dimensions[1] * cell[2]) + (m_Dimensions[0] * cell[1]) + cell[0];
  if(index > getNumberOfElements())
  {
    err = ImageGeom::ErrorType::IndexOutOfBounds;
  }
  return err;
}

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

#include <QtCore/QTextStream>

#include <thread>

#include "SIMPLib/Geometry/ImageGeom.h"

#include "H5Support/H5Lite.h"
#include "SIMPLib/Geometry/GeometryHelpers.h"
#include "SIMPLib/HDF5/VTKH5Constants.h"
#include "SIMPLib/Utilities/ParallelData3DAlgorithm.h"

/**
 * @brief The FindImageDerivativesImpl class implements a threaded algorithm that computes the
 * derivative of an arbitrary dimensional field on the underlying image
 */
class FindImageDerivativesImpl
{
public:
  FindImageDerivativesImpl(ImageGeom* image, const DoubleArrayType::Pointer& field, const DoubleArrayType::Pointer& derivs)
  : m_Image(image)
  , m_Field(field)
  , m_Derivatives(derivs)
  {
  }
  virtual ~FindImageDerivativesImpl() = default;

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

    SizeVec3Type dims = m_Image->getDimensions();

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
            findValuesForFiniteDifference(TwoDimensional, XDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(x == 0)
          {
            findValuesForFiniteDifference(LeftSide, XDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(x == (dims[0] - 1))
          {
            findValuesForFiniteDifference(RightSide, XDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else
          {
            findValuesForFiniteDifference(Centered, XDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
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
            findValuesForFiniteDifference(TwoDimensional, YDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(y == 0)
          {
            findValuesForFiniteDifference(LeftSide, YDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(y == (dims[1] - 1))
          {
            findValuesForFiniteDifference(RightSide, YDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else
          {
            findValuesForFiniteDifference(Centered, YDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
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
            findValuesForFiniteDifference(TwoDimensional, ZDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(z == 0)
          {
            findValuesForFiniteDifference(LeftSide, ZDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else if(z == (dims[2] - 1))
          {
            findValuesForFiniteDifference(RightSide, ZDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
          }
          else
          {
            findValuesForFiniteDifference(Centered, ZDirection, x, y, z, dims.data(), xp, xm, factor, numComps, plusValues, minusValues, fieldPtr);
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
          for(size_t i = 0; i < numComps; i++)
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

  void operator()(const SIMPLRange3D& r) const
  {
    compute(r[0], r[1], r[2], r[3], r[4], r[5]);
  }

  void computeIndices(int32_t differenceType, int32_t directionType, size_t& index1, size_t& index2, const size_t dims[3], size_t x, size_t y, size_t z, double xp[3], double xm[3]) const

  {
    size_t tmpIndex1 = 0;
    size_t tmpIndex2 = 0;

    switch(directionType)
    {
    case XDirection: {
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
    case YDirection: {
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
    case ZDirection: {
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
    default: {
      break;
    }
    }
  }

  void findValuesForFiniteDifference(int32_t differenceType, int32_t directionType, size_t x, size_t y, size_t z, size_t dims[3], double xp[3], double xm[3], double& factor, int32_t numComps,
                                     std::vector<double>& plusValues, std::vector<double>& minusValues, const double* field) const
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
: m_Spacing(1.0f, 1.0f, 1.0f)
, m_Origin(0.0f, 0.0f, 0.0f)
{
  m_GeometryTypeName = SIMPL::Geometry::ImageGeometry;
  m_GeometryType = IGeometry::Type::Image;
  m_XdmfGridType = SIMPL::XdmfGridType::RectilinearGrid;
  m_UnitDimensionality = 3;
  m_SpatialDimensionality = 3;
  m_Dimensions[0] = 0;
  m_Dimensions[1] = 0;
  m_Dimensions[2] = 0;
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
  if(name.isEmpty())
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
FloatVec3Type ImageGeom::getSpacing() const
{
  return m_Spacing;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::setSpacing(const FloatVec3Type& spacing)
{
  m_Spacing = spacing;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::setSpacing(float x, float y, float z)
{
  m_Spacing[0] = x;
  m_Spacing[1] = y;
  m_Spacing[2] = z;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Type ImageGeom::getOrigin() const
{
  return m_Origin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::setOrigin(const FloatVec3Type& origin)
{
  m_Origin = origin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::setOrigin(float x, float y, float z)
{
  m_Origin[0] = x;
  m_Origin[1] = y;
  m_Origin[2] = z;
}

SizeVec3Type ImageGeom::getDimensions() const
{
  return m_Dimensions;
}

void ImageGeom::setDimensions(const SizeVec3Type& dims)
{
  m_Dimensions = dims;
}

void ImageGeom::setDimensions(size_t x, size_t y, size_t z)
{
  m_Dimensions[0] = x;
  m_Dimensions[1] = y;
  m_Dimensions[2] = z;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getBoundingBox(float* boundingBox)
{
  boundingBox[0] = m_Origin[0];
  boundingBox[1] = m_Origin[0] + (m_Dimensions[0] * m_Spacing[0]);
  boundingBox[2] = m_Origin[1];
  boundingBox[3] = m_Origin[1] + (m_Dimensions[1] * m_Spacing[1]);
  boundingBox[4] = m_Origin[2];
  boundingBox[5] = m_Origin[2] + (m_Dimensions[2] * m_Spacing[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec6Type ImageGeom::getBoundingBox() const
{
  return FloatVec6Type(m_Origin[0], m_Origin[0] + (m_Dimensions[0] * m_Spacing[0]), m_Origin[1], m_Origin[1] + (m_Dimensions[1] * m_Spacing[1]), m_Origin[2],
                       m_Origin[2] + (m_Dimensions[2] * m_Spacing[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ImageGeom::getXPoints() const
{
  return m_Dimensions[0];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ImageGeom::getYPoints() const
{
  return m_Dimensions[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ImageGeom::getZPoints() const
{
  return m_Dimensions[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t idx[3], float coords[3]) const
{
  coords[0] = idx[0] * m_Spacing[0] + m_Origin[0];
  coords[1] = idx[1] * m_Spacing[1] + m_Origin[1];
  coords[2] = idx[2] * m_Spacing[2] + m_Origin[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t x, size_t y, size_t z, float coords[3]) const
{
  coords[0] = x * m_Spacing[0] + m_Origin[0];
  coords[1] = y * m_Spacing[1] + m_Origin[1];
  coords[2] = z * m_Spacing[2] + m_Origin[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t idx, float coords[3]) const
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  coords[0] = column * m_Spacing[0] + m_Origin[0];
  coords[1] = row * m_Spacing[1] + m_Origin[1];
  coords[2] = plane * m_Spacing[2] + m_Origin[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t idx[3], double coords[3]) const
{
  coords[0] = static_cast<double>(idx[0]) * m_Spacing[0] + m_Origin[0];
  coords[1] = static_cast<double>(idx[1]) * m_Spacing[1] + m_Origin[1];
  coords[2] = static_cast<double>(idx[2]) * m_Spacing[2] + m_Origin[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t x, size_t y, size_t z, double coords[3]) const
{
  coords[0] = static_cast<double>(x) * m_Spacing[0] + m_Origin[0];
  coords[1] = static_cast<double>(y) * m_Spacing[1] + m_Origin[1];
  coords[2] = static_cast<double>(z) * m_Spacing[2] + m_Origin[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getPlaneCoords(size_t idx, double coords[3]) const
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  coords[0] = static_cast<double>(column) * m_Spacing[0] + m_Origin[0];
  coords[1] = static_cast<double>(row) * m_Spacing[1] + m_Origin[1];
  coords[2] = static_cast<double>(plane) * m_Spacing[2] + m_Origin[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t idx[3], float coords[3]) const
{
  coords[0] = idx[0] * m_Spacing[0] + m_Origin[0] + (0.5f * m_Spacing[0]);
  coords[1] = idx[1] * m_Spacing[1] + m_Origin[1] + (0.5f * m_Spacing[1]);
  coords[2] = idx[2] * m_Spacing[2] + m_Origin[2] + (0.5f * m_Spacing[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t x, size_t y, size_t z, float coords[3]) const
{
  coords[0] = x * m_Spacing[0] + m_Origin[0] + (0.5f * m_Spacing[0]);
  coords[1] = y * m_Spacing[1] + m_Origin[1] + (0.5f * m_Spacing[1]);
  coords[2] = z * m_Spacing[2] + m_Origin[2] + (0.5f * m_Spacing[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t idx, float coords[3]) const
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  coords[0] = column * m_Spacing[0] + m_Origin[0] + (0.5f * m_Spacing[0]);
  coords[1] = row * m_Spacing[1] + m_Origin[1] + (0.5f * m_Spacing[1]);
  coords[2] = plane * m_Spacing[2] + m_Origin[2] + (0.5f * m_Spacing[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t idx[3], double coords[3]) const
{
  coords[0] = static_cast<double>(idx[0]) * m_Spacing[0] + m_Origin[0] + (0.5 * m_Spacing[0]);
  coords[1] = static_cast<double>(idx[1]) * m_Spacing[1] + m_Origin[1] + (0.5 * m_Spacing[1]);
  coords[2] = static_cast<double>(idx[2]) * m_Spacing[2] + m_Origin[2] + (0.5 * m_Spacing[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t x, size_t y, size_t z, double coords[3]) const
{
  coords[0] = static_cast<double>(x) * m_Spacing[0] + m_Origin[0] + (0.5 * m_Spacing[0]);
  coords[1] = static_cast<double>(y) * m_Spacing[1] + m_Origin[1] + (0.5 * m_Spacing[1]);
  coords[2] = static_cast<double>(z) * m_Spacing[2] + m_Origin[2] + (0.5 * m_Spacing[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getCoords(size_t idx, double coords[3]) const
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  coords[0] = static_cast<double>(column) * m_Spacing[0] + m_Origin[0] + (0.5 * m_Spacing[0]);
  coords[1] = static_cast<double>(row) * m_Spacing[1] + m_Origin[1] + (0.5 * m_Spacing[1]);
  coords[2] = static_cast<double>(plane) * m_Spacing[2] + m_Origin[2] + (0.5 * m_Spacing[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::initializeWithZeros()
{
  for(size_t i = 0; i < 3; i++)
  {
    m_Dimensions[i] = 0;
    m_Spacing[i] = 1.0f;
    m_Origin[i] = 0.0f;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::addOrReplaceAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
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
  // if(data->getName().compare(name) != 0)
  //{
  //  data->setName(name);
  //}
  m_AttributeMatrices[name] = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ImageGeom::getNumberOfElements() const
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
ElementDynamicList::Pointer ImageGeom::getElementsContainingVert() const
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::setElementsContainingVert(ElementDynamicList::Pointer SIMPL_NOT_USED(elementsContainingVert))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::deleteElementsContainingVert()
{
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
ElementDynamicList::Pointer ImageGeom::getElementNeighbors() const
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::setElementNeighbors(ElementDynamicList::Pointer SIMPL_NOT_USED(elementNeighbors))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::deleteElementNeighbors()
{
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
FloatArrayType::Pointer ImageGeom::getElementCentroids() const
{
  return FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::setElementCentroids(FloatArrayType::Pointer SIMPL_NOT_USED(elementsCentroids))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::deleteElementCentroids()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::findElementSizes()
{
  FloatVec3Type res = getSpacing();

  if(res[0] <= 0.0f || res[1] <= 0.0f || res[2] <= 0.0f)
  {
    return -1;
  }
  m_VoxelSizes = FloatArrayType::CreateArray(getNumberOfElements(), SIMPL::StringConstants::VoxelSizes, true);
  m_VoxelSizes->initializeWithValue(res[0] * res[1] * res[2]);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer ImageGeom::getElementSizes() const
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
void ImageGeom::getParametricCenter(double pCoords[3]) const
{
  pCoords[0] = 0.5;
  pCoords[1] = 0.5;
  pCoords[2] = 0.5;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageGeom::getShapeFunctions(double pCoords[3], double* shape) const
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
  SizeVec3Type dims = getDimensions();

  if(observable != nullptr)
  {
    connect(this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), observable, SLOT(processDerivativesMessage(const AbstractMessage::Pointer&)));
  }

  size_t grain = dims[2] == 1 ? 1 : dims[2] / std::thread::hardware_concurrency();

  if(grain == 0) // This can happen if dims[2] > number of processors
  {
    grain = 1;
  }

  ParallelData3DAlgorithm dataAlg;
  dataAlg.setRange(dims[2], dims[1], dims[0]);
  dataAlg.setGrain(grain);
  dataAlg.execute(FindImageDerivativesImpl(this, field, derivatives));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf)) const
{
  herr_t err = 0;
  int64_t volDims[3] = {static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints())};
  FloatVec3Type spacing = getSpacing();
  FloatVec3Type origin = getOrigin();

  int32_t rank = 1;
  hsize_t dims[1] = {3};

  err = H5Lite::writePointerDataset(parentId, H5_DIMENSIONS, rank, dims, volDims);
  if(err < 0)
  {
    return err;
  }
  err = H5Lite::writePointerDataset(parentId, H5_ORIGIN, rank, dims, origin.data());
  if(err < 0)
  {
    return err;
  }
  err = H5Lite::writePointerDataset(parentId, H5_SPACING, rank, dims, spacing.data());
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
int ImageGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName) const
{
  herr_t err = 0;

  int64_t volDims[3] = {static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints())};
  FloatVec3Type spacing = getSpacing();
  FloatVec3Type origin = getOrigin();

  out << "  <!-- *************** START OF " << dcName << " *************** -->"
      << "\n";
  out << "  <Grid Name=\"" << dcName << R"(" GridType="Uniform">)"
      << "\n";
  if(getEnableTimeSeries())
  {
    out << R"(    <Time TimeType="Single" Value=")" << getTimeValue() << "\"/>\n";
  }
  out << "    <Topology TopologyType=\"3DCoRectMesh\" Dimensions=\"" << volDims[2] + 1 << " " << volDims[1] + 1 << " " << volDims[0] + 1 << " \"></Topology>"
      << "\n";
  out << "    <Geometry Type=\"ORIGIN_DXDYDZ\">"
      << "\n";
  out << "      <!-- Origin  Z, Y, X -->"
      << "\n";
  out << R"(      <DataItem Format="XML" Dimensions="3">)" << origin[2] << " " << origin[1] << " " << origin[0] << "</DataItem>"
      << "\n";
  out << "      <!-- DxDyDz (Spacing/Spacing) Z, Y, X -->"
      << "\n";
  out << R"(      <DataItem Format="XML" Dimensions="3">)" << spacing[2] << " " << spacing[1] << " " << spacing[0] << "</DataItem>"
      << "\n";
  out << "    </Geometry>"
      << "\n";

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImageGeom::getInfoString(SIMPL::InfoStringFormat format) const
{
  if(format == SIMPL::HtmlFormat)
  {
    return getToolTipGenerator().generateHTML();
  }

  QString info;
  QTextStream ss(&info);
  ss << "Requested InfoStringFormat is not supported. " << format;

  return info;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ToolTipGenerator ImageGeom::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;

  QString lengthUnit = IGeometry::LengthUnitToString(static_cast<IGeometry::LengthUnit>(getUnits()));
  int64_t volDims[3] = {static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints())};
  FloatVec3Type spacing = getSpacing();
  FloatVec3Type origin = getOrigin();

  float halfRes[3] = {spacing[0] / 2.0f, spacing[1] / 2.0f, spacing[2] / 2.0f};
  float vol = (volDims[0] * spacing[0]) * (volDims[1] * spacing[1]) * (volDims[2] * spacing[2]);
  QLocale usa(QLocale::English, QLocale::UnitedStates);

  toolTipGen.addTitle("Geometry Info");
  toolTipGen.addValue("Type", TypeToString(getGeometryType()));
  toolTipGen.addValue("Units", LengthUnitToString(getUnits()));
  toolTipGen.addTitle("Extents");
  toolTipGen.addValue("X", "0 to " + QString::number(volDims[0] - 1) + " (dimension: " + QString::number(volDims[0]) + ")");
  toolTipGen.addValue("Y", "0 to " + QString::number(volDims[1] - 1) + " (dimension: " + QString::number(volDims[1]) + ")");
  toolTipGen.addValue("Z", "0 to " + QString::number(volDims[2] - 1) + " (dimension: " + QString::number(volDims[2]) + ")");
  toolTipGen.addValue("Origin", QString::number(origin[0]) + ", " + QString::number(origin[1]) + ", " + QString::number(origin[2]));
  toolTipGen.addValue("Spacing", QString::number(spacing[0]) + ", " + QString::number(spacing[1]) + ", " + QString::number(spacing[2]));
  toolTipGen.addValue("Volume", usa.toString(vol) + " " + lengthUnit + "s ^3");
  toolTipGen.addTitle("Bounds (Cell Centered)");
  toolTipGen.addValue("X Range", QString::number(origin[0] - halfRes[0]) + " to " + QString::number(origin[0] - halfRes[0] + volDims[0] * spacing[0]) +
                                     " (delta: " + QString::number(volDims[0] * spacing[0]) + ")");
  toolTipGen.addValue("Y Range", QString::number(origin[1] - halfRes[1]) + " to " + QString::number(origin[1] - halfRes[1] + volDims[1] * spacing[1]) +
                                     " (delta: " + QString::number(volDims[1] * spacing[1]) + ")");
  toolTipGen.addValue("Z Range", QString::number(origin[2] - halfRes[2]) + " to " + QString::number(origin[2] - halfRes[2] + volDims[2] * spacing[2]) +
                                     " (delta: " + QString::number(volDims[2] * spacing[2]) + ")");

  return toolTipGen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  int err = 0;
  size_t volDims[3] = {0, 0, 0};
  FloatVec3Type spacing = {1.0f, 1.0f, 1.0f};
  FloatVec3Type origin = {0.0f, 0.0f, 0.0f};
  unsigned int spatialDims = 0;
  QString geomName = "";
  err = gatherMetaData(parentId, volDims, spacing.data(), origin.data(), spatialDims, geomName, preflight);
  if(err < 0)
  {
    return err;
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer ImageGeom::deepCopy(bool forceNoAllocate) const
{
  ImageGeom::Pointer imageCopy = ImageGeom::CreateGeometry(getName());

  SizeVec3Type volDims = getDimensions();
  FloatVec3Type spacing = getSpacing();
  FloatVec3Type origin = getOrigin();
  imageCopy->setDimensions(volDims);
  imageCopy->setSpacing(spacing);
  imageCopy->setOrigin(origin);
  FloatArrayType::Pointer elementSizes = std::dynamic_pointer_cast<FloatArrayType>((getElementSizes().get() == nullptr) ? nullptr : getElementSizes()->deepCopy(forceNoAllocate));
  imageCopy->setElementSizes(elementSizes);
  imageCopy->setSpatialDimensionality(getSpatialDimensionality());
  imageCopy->setUnits(getUnits());

  return imageCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::gatherMetaData(hid_t parentId, size_t volDims[3], float spacing[3], float origin[3], unsigned int spatialDims, const QString& geomName, bool preflight)
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
  setSpacing(spacing);
  setOrigin(origin);
  setElementSizes(voxelSizes);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageGeom::ErrorType ImageGeom::computeCellIndex(const float coords[3], size_t index[3])
{
  ImageGeom::ErrorType err = ImageGeom::ErrorType::NoError;
  for(size_t i = 0; i < 3; i++)
  {
    if(coords[i] < m_Origin[i])
    {
      return static_cast<ImageGeom::ErrorType>(i * 2);
    }
    if(coords[i] > (m_Origin[i] + m_Dimensions[i] * m_Spacing[i]))
    {
      return static_cast<ImageGeom::ErrorType>(i * 2 + 1);
    }
    index[i] = static_cast<size_t>((coords[i] - m_Origin[i]) / m_Spacing[i]);
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
ImageGeom::ErrorType ImageGeom::computeCellIndex(const float coords[], size_t& index)
{
  size_t cell[3] = {0, 0, 0};
  ImageGeom::ErrorType err = computeCellIndex(coords, cell);
  if(err == ImageGeom::ErrorType::NoError)
  {
    index = (m_Dimensions[0] * m_Dimensions[1] * cell[2]) + (m_Dimensions[0] * cell[1]) + cell[0];
    if(index > getNumberOfElements())
    {
      err = ImageGeom::ErrorType::IndexOutOfBounds;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
ImageGeom::Pointer ImageGeom::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
ImageGeom::Pointer ImageGeom::New()
{
  Pointer sharedPtr(new(ImageGeom));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString ImageGeom::getNameOfClass() const
{
  return QString("ImageGeom");
}

// -----------------------------------------------------------------------------
QString ImageGeom::ClassName()
{
  return QString("ImageGeom");
}

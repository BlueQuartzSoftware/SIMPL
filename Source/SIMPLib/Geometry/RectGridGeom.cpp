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
 * RectGridGeom re-implements code from the following vtk modules:
 *
 * * vtkLine.cxx
 *   - re-implemented vtkVoxel::InterpolationDerivs to RectGridGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - re-implemented vtkGradientFilter template function ComputeGradientsSG to
 *     RectGridGeom::findDerivatives
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QTextStream>

#include <thread>

#include "SIMPLib/Geometry/RectGridGeom.h"

#include "H5Support/H5Lite.h"
#include "SIMPLib/Geometry/GeometryHelpers.h"
#include "SIMPLib/HDF5/VTKH5Constants.h"
#include "SIMPLib/Utilities/ParallelData3DAlgorithm.h"

/**
 * @brief The FindImageDerivativesImpl class implements a threaded algorithm that computes the
 * derivative of an arbitrary dimensional field on the underlying rectilinear grid
 */
class FindRectGridDerivativesImpl
{
public:
  FindRectGridDerivativesImpl(RectGridGeom* image, DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivs)
  : m_RectGrid(image)
  , m_Field(field)
  , m_Derivatives(derivs)
  {
  }
  virtual ~FindRectGridDerivativesImpl() = default;

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

    SizeVec3Type dims = m_RectGrid->getDimensions();

    int64_t counter = 0;
    size_t totalElements = m_RectGrid->getNumberOfElements();
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
            m_RectGrid->sendThreadSafeProgressMessage(counter, totalElements);
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

  void computeIndices(int32_t differenceType, int32_t directionType, size_t& index1, size_t& index2, size_t dims[3], size_t x, size_t y, size_t z, double xp[3], double xm[3]) const

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
        m_RectGrid->getCoords(tmpIndex1, y, z, xp);
        m_RectGrid->getCoords(x, y, z, xm);
      }
      else if(differenceType == RightSide)
      {
        index1 = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
        index2 = (z * dims[1] * dims[0]) + (y * dims[0]) + (x - 1);
        tmpIndex1 = x - 1;
        m_RectGrid->getCoords(x, y, z, xp);
        m_RectGrid->getCoords(tmpIndex1, y, z, xm);
      }
      else if(differenceType == Centered)
      {
        index1 = (z * dims[1] * dims[0]) + (y * dims[0]) + (x + 1);
        index2 = (z * dims[1] * dims[0]) + (y * dims[0]) + (x - 1);
        tmpIndex1 = x + 1;
        tmpIndex2 = x - 1;
        m_RectGrid->getCoords(tmpIndex1, y, z, xp);
        m_RectGrid->getCoords(tmpIndex2, y, z, xm);
      }
      break;
    }
    case YDirection: {
      if(differenceType == LeftSide)
      {
        index1 = (z * dims[1] * dims[0]) + ((y + 1) * dims[0]) + x;
        index2 = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
        tmpIndex1 = y + 1;
        m_RectGrid->getCoords(x, tmpIndex1, z, xp);
        m_RectGrid->getCoords(x, y, z, xm);
      }
      else if(differenceType == RightSide)
      {
        index1 = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
        index2 = (z * dims[1] * dims[0]) + ((y - 1) * dims[0]) + x;
        tmpIndex1 = y - 1;
        m_RectGrid->getCoords(x, y, z, xp);
        m_RectGrid->getCoords(x, tmpIndex1, z, xm);
      }
      else if(differenceType == Centered)
      {
        index1 = (z * dims[1] * dims[0]) + ((y + 1) * dims[0]) + x;
        index2 = (z * dims[1] * dims[0]) + ((y - 1) * dims[0]) + x;
        tmpIndex1 = y + 1;
        tmpIndex2 = y - 1;
        m_RectGrid->getCoords(x, tmpIndex1, z, xp);
        m_RectGrid->getCoords(x, tmpIndex2, z, xm);
      }
      break;
    }
    case ZDirection: {
      if(differenceType == LeftSide)
      {
        index1 = ((z + 1) * dims[1] * dims[0]) + (y * dims[0]) + x;
        index2 = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
        tmpIndex1 = z + 1;
        m_RectGrid->getCoords(x, y, tmpIndex1, xp);
        m_RectGrid->getCoords(x, y, z, xm);
      }
      else if(differenceType == RightSide)
      {
        index1 = (z * dims[1] * dims[0]) + (y * dims[0]) + x;
        index2 = ((z - 1) * dims[1] * dims[0]) + (y * dims[0]) + x;
        tmpIndex1 = z - 1;
        m_RectGrid->getCoords(x, y, z, xp);
        m_RectGrid->getCoords(x, y, tmpIndex1, xm);
      }
      else if(differenceType == Centered)
      {
        index1 = ((z + 1) * dims[1] * dims[0]) + (y * dims[0]) + x;
        index2 = ((z - 1) * dims[1] * dims[0]) + (y * dims[0]) + x;
        tmpIndex1 = z + 1;
        tmpIndex2 = z - 1;
        m_RectGrid->getCoords(x, y, tmpIndex1, xp);
        m_RectGrid->getCoords(x, y, tmpIndex2, xm);
      }
      break;
    }
    default: {
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
  RectGridGeom* m_RectGrid;
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
RectGridGeom::RectGridGeom()
{
  m_GeometryTypeName = SIMPL::Geometry::RectGridGeometry;
  m_GeometryType = IGeometry::Type::RectGrid;
  m_XdmfGridType = SIMPL::XdmfGridType::RectilinearGrid;
  m_UnitDimensionality = 3;
  m_SpatialDimensionality = 3;
  m_Dimensions[0] = 0;
  m_Dimensions[1] = 0;
  m_Dimensions[2] = 0;
  m_xBounds = FloatArrayType::NullPointer();
  m_yBounds = FloatArrayType::NullPointer();
  m_zBounds = FloatArrayType::NullPointer();
  m_VoxelSizes = FloatArrayType::NullPointer();
  m_ProgressCounter = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RectGridGeom::~RectGridGeom() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RectGridGeom::Pointer RectGridGeom::CreateGeometry(const QString& name)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  RectGridGeom* d = new RectGridGeom();
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SizeVec3Type RectGridGeom::getDimensions() const
{
  return m_Dimensions;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setDimensions(const SizeVec3Type& dims)
{
  m_Dimensions = dims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setXBounds(const FloatArrayType::Pointer& xBnds)
{
  if(xBnds.get() != nullptr)
  {
    if(xBnds->getName().compare(SIMPL::Geometry::xBoundsList) != 0)
    {
      xBnds->setName(SIMPL::Geometry::xBoundsList);
    }
  }
  m_xBounds = xBnds;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setYBounds(const FloatArrayType::Pointer& yBnds)
{
  if(yBnds.get() != nullptr)
  {
    if(yBnds->getName().compare(SIMPL::Geometry::yBoundsList) != 0)
    {
      yBnds->setName(SIMPL::Geometry::yBoundsList);
    }
  }
  m_yBounds = yBnds;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setZBounds(const FloatArrayType::Pointer& zBnds)
{
  if(zBnds.get() != nullptr)
  {
    if(zBnds->getName().compare(SIMPL::Geometry::zBoundsList) != 0)
    {
      zBnds->setName(SIMPL::Geometry::zBoundsList);
    }
  }
  m_zBounds = zBnds;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer RectGridGeom::getXBounds() const
{
  return m_xBounds;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer RectGridGeom::getYBounds() const
{
  return m_yBounds;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer RectGridGeom::getZBounds() const
{
  return m_zBounds;
}

// -----------------------------------------------------------------------------
size_t RectGridGeom::getXPoints() const
{
  return m_Dimensions[0];
}
// -----------------------------------------------------------------------------
size_t RectGridGeom::getYPoints() const
{
  return m_Dimensions[1];
}
// -----------------------------------------------------------------------------
size_t RectGridGeom::getZPoints() const
{
  return m_Dimensions[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getPlaneCoords(size_t idx[3], float coords[3]) const
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = xBnds[idx[0]];
  coords[1] = yBnds[idx[1]];
  coords[2] = zBnds[idx[2]];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getPlaneCoords(size_t x, size_t y, size_t z, float coords[3]) const
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = xBnds[x];
  coords[1] = yBnds[y];
  coords[2] = zBnds[z];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getPlaneCoords(size_t idx, float coords[3]) const
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = xBnds[column];
  coords[1] = yBnds[row];
  coords[2] = zBnds[plane];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getPlaneCoords(size_t idx[3], double coords[3]) const
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = static_cast<double>(xBnds[idx[0]]);
  coords[1] = static_cast<double>(yBnds[idx[1]]);
  coords[2] = static_cast<double>(zBnds[idx[2]]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getPlaneCoords(size_t x, size_t y, size_t z, double coords[3]) const
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = static_cast<double>(xBnds[x]);
  coords[1] = static_cast<double>(yBnds[y]);
  coords[2] = static_cast<double>(zBnds[z]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getPlaneCoords(size_t idx, double coords[3]) const
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = static_cast<double>(xBnds[column]);
  coords[1] = static_cast<double>(yBnds[row]);
  coords[2] = static_cast<double>(zBnds[plane]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getCoords(size_t idx[3], float coords[3]) const
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = 0.5f * (xBnds[idx[0]] + xBnds[idx[0] + 1]);
  coords[1] = 0.5f * (yBnds[idx[1]] + yBnds[idx[1] + 1]);
  coords[2] = 0.5f * (zBnds[idx[2]] + zBnds[idx[2] + 1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getCoords(size_t x, size_t y, size_t z, float coords[3]) const
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = 0.5f * (xBnds[x] + xBnds[x + 1]);
  coords[1] = 0.5f * (yBnds[y] + yBnds[y + 1]);
  coords[2] = 0.5f * (zBnds[z] + zBnds[z + 1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getCoords(size_t idx, float coords[3]) const
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = 0.5f * (xBnds[column] + xBnds[column + 1]);
  coords[1] = 0.5f * (yBnds[row] + yBnds[row + 1]);
  coords[2] = 0.5f * (zBnds[plane] + zBnds[plane + 1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getCoords(size_t idx[3], double coords[3]) const
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = 0.5 * (static_cast<double>(xBnds[idx[0]]) + xBnds[idx[0] + 1]);
  coords[1] = 0.5 * (static_cast<double>(yBnds[idx[1]]) + yBnds[idx[1] + 1]);
  coords[2] = 0.5 * (static_cast<double>(zBnds[idx[2]]) + zBnds[idx[2] + 1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getCoords(size_t x, size_t y, size_t z, double coords[3]) const
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = 0.5 * (static_cast<double>(xBnds[x]) + xBnds[x + 1]);
  coords[1] = 0.5 * (static_cast<double>(yBnds[y]) + yBnds[y + 1]);
  coords[2] = 0.5 * (static_cast<double>(zBnds[z]) + zBnds[z + 1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getCoords(size_t idx, double coords[3]) const
{
  size_t column = idx % m_Dimensions[0];
  size_t row = (idx / m_Dimensions[0]) % m_Dimensions[1];
  size_t plane = idx / (m_Dimensions[0] * m_Dimensions[1]);

  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = 0.5 * (static_cast<double>(xBnds[column]) + xBnds[column + 1]);
  coords[1] = 0.5 * (static_cast<double>(yBnds[row]) + yBnds[row + 1]);
  coords[2] = 0.5 * (static_cast<double>(zBnds[plane]) + zBnds[plane + 1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::initializeWithZeros()
{
  for(size_t i = 0; i < 3; i++)
  {
    m_Dimensions[i] = 0;
  }
  m_xBounds = FloatArrayType::NullPointer();
  m_yBounds = FloatArrayType::NullPointer();
  m_zBounds = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::addOrReplaceAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if(data->getType() != AttributeMatrix::Type::Cell)
  {
    // RectGridGeom can only accept cell Attribute Matrices
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
size_t RectGridGeom::getNumberOfElements() const
{
  return (m_Dimensions[0] * m_Dimensions[1] * m_Dimensions[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::findElementsContainingVert()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer RectGridGeom::getElementsContainingVert() const
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setElementsContainingVert(ElementDynamicList::Pointer SIMPL_NOT_USED(elementsContainingVert))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::deleteElementsContainingVert()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::findElementNeighbors()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer RectGridGeom::getElementNeighbors() const
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setElementNeighbors(ElementDynamicList::Pointer SIMPL_NOT_USED(elementNeighbors))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::deleteElementNeighbors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::findElementCentroids()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer RectGridGeom::getElementCentroids() const
{
  return FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setElementCentroids(FloatArrayType::Pointer SIMPL_NOT_USED(elementsCentroids))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::deleteElementCentroids()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::findElementSizes()
{
  m_VoxelSizes = FloatArrayType::CreateArray(getNumberOfElements(), SIMPL::StringConstants::VoxelSizes, true);

  float* sizes = m_VoxelSizes->getPointer(0);
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);
  float xRes = 0.0f;
  float yRes = 0.0f;
  float zRes = 0.0f;

  for(size_t z = 0; z < m_Dimensions[2]; z++)
  {
    for(size_t y = 0; y < m_Dimensions[1]; y++)
    {
      for(size_t x = 0; x < m_Dimensions[0]; x++)
      {
        xRes = xBnds[x + 1] - xBnds[x];
        yRes = yBnds[y + 1] - yBnds[y];
        zRes = zBnds[z + 1] - zBnds[z];
        if(xRes <= 0.0f || yRes <= 0.0f || zRes <= 0.0f)
        {
          m_VoxelSizes = FloatArrayType::NullPointer();
          return -1;
        }
        sizes[(m_Dimensions[0] * m_Dimensions[1] * z) + (m_Dimensions[0] * y) + x] = zRes * yRes * xRes;
      }
    }
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer RectGridGeom::getElementSizes() const
{
  return m_VoxelSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setElementSizes(FloatArrayType::Pointer elementSizes)
{
  m_VoxelSizes = elementSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::deleteElementSizes()
{
  m_VoxelSizes = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getParametricCenter(double pCoords[3]) const
{
  pCoords[0] = 0.5;
  pCoords[1] = 0.5;
  pCoords[2] = 0.5;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getShapeFunctions(double pCoords[3], double* shape) const
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
void RectGridGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable)
{
  m_ProgressCounter = 0;
  SizeVec3Type dims = getDimensions();

  if(observable != nullptr)
  {
    connect(this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), observable, SLOT(processDerivativesMessage(const AbstractMessage::Pointer&)));
  }

  size_t grain = dims[2] == 1 ? 1 : dims[2] / std::thread::hardware_concurrency();
  if(grain == 0)
  {
    grain = 1;
  }

  ParallelData3DAlgorithm dataAlg;
  dataAlg.setRange(dims[2], dims[1], dims[0]);
  dataAlg.setGrain(grain);
  dataAlg.execute(FindRectGridDerivativesImpl(this, field, derivatives));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf)) const
{
  herr_t err = 0;
  int64_t volDims[3] = {static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints())};

  int32_t rank = 1;
  hsize_t dims[1] = {3};

  err = H5Lite::writePointerDataset(parentId, H5_DIMENSIONS, rank, dims, volDims);
  if(err < 0)
  {
    return err;
  }
  if(m_xBounds.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_xBounds);
    if(err < 0)
    {
      return err;
    }
  }
  if(m_yBounds.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_yBounds);
    if(err < 0)
    {
      return err;
    }
  }
  if(m_zBounds.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_zBounds);
    if(err < 0)
    {
      return err;
    }
  }
  if(m_VoxelSizes.get() != nullptr)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_VoxelSizes);
    if(err < 0)
    {
      return err;
    }
  }

  err |= H5Gclose(parentId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName) const
{
  herr_t err = 0;

  int64_t volDims[3] = {static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints())};

  out << "  <!-- *************** START OF " << dcName << " *************** -->"
      << "\n";
  out << "  <Grid Name=\"" << dcName << R"(" GridType="Uniform">)"
      << "\n";
  if(getEnableTimeSeries())
  {
    out << R"(    <Time TimeType="Single" Value=")" << getTimeValue() << "\"/>\n";
  }
  out << "    <Topology TopologyType=\"3DRectMesh\" Dimensions=\"" << volDims[2] + 1 << " " << volDims[1] + 1 << " " << volDims[0] + 1 << " \"></Topology>"
      << "\n";
  out << "    <Geometry Type=\"VxVyVz\">"
      << "\n";
  out << "    <DataItem Format=\"HDF\" Dimensions=\"" << m_xBounds->getNumberOfTuples() << "\" NumberType=\"Float\" Precision=\"4\">"
      << "\n";
  out << "      " << hdfFileName << ":/DataContainers/" << dcName << "/" << SIMPL::Geometry::Geometry << "/" << SIMPL::Geometry::xBoundsList << "\n";
  out << "    </DataItem>"
      << "\n";
  out << "    <DataItem Format=\"HDF\" Dimensions=\"" << m_yBounds->getNumberOfTuples() << "\" NumberType=\"Float\" Precision=\"4\">"
      << "\n";
  out << "      " << hdfFileName << ":/DataContainers/" << dcName << "/" << SIMPL::Geometry::Geometry << "/" << SIMPL::Geometry::yBoundsList << "\n";
  out << "    </DataItem>"
      << "\n";
  out << "    <DataItem Format=\"HDF\" Dimensions=\"" << m_zBounds->getNumberOfTuples() << "\" NumberType=\"Float\" Precision=\"4\">"
      << "\n";
  out << "      " << hdfFileName << ":/DataContainers/" << dcName << "/" << SIMPL::Geometry::Geometry << "/" << SIMPL::Geometry::zBoundsList << "\n";
  out << "    </DataItem>"
      << "\n";
  out << "    </Geometry>"
      << "\n";

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RectGridGeom::getInfoString(SIMPL::InfoStringFormat format) const
{
  if(format == SIMPL::HtmlFormat)
  {
    return getToolTipGenerator().generateHTML();
  }

  return QString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ToolTipGenerator RectGridGeom::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;

  int64_t volDims[3] = {static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints())};

  toolTipGen.addTitle("Geometry Info");
  toolTipGen.addValue("Type", TypeToString(getGeometryType()));
  toolTipGen.addValue("Units", LengthUnitToString(getUnits()));
  toolTipGen.addValue("Dimmensions", QString::number(volDims[0]) + " x " + QString::number(volDims[1]) + " x " + QString::number(volDims[2]));
  toolTipGen.addValue("Spacing", "Variable");

  return toolTipGen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  int err = 0;
  size_t volDims[3] = {0, 0, 0};
  err = gatherMetaData(parentId, volDims, preflight);
  if(err < 0)
  {
    return err;
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer RectGridGeom::deepCopy(bool forceNoAllocate) const
{
  FloatArrayType::Pointer xBounds = std::dynamic_pointer_cast<FloatArrayType>((getXBounds().get() == nullptr) ? nullptr : getXBounds()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer yBounds = std::dynamic_pointer_cast<FloatArrayType>((getYBounds().get() == nullptr) ? nullptr : getYBounds()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer zBounds = std::dynamic_pointer_cast<FloatArrayType>((getZBounds().get() == nullptr) ? nullptr : getZBounds()->deepCopy(forceNoAllocate));
  FloatArrayType::Pointer elementSizes = std::dynamic_pointer_cast<FloatArrayType>((getElementSizes().get() == nullptr) ? nullptr : getElementSizes()->deepCopy(forceNoAllocate));

  RectGridGeom::Pointer copy = RectGridGeom::CreateGeometry(getName());

  SizeVec3Type volDims = getDimensions();
  copy->setDimensions(volDims);
  copy->setXBounds(xBounds);
  copy->setYBounds(yBounds);
  copy->setZBounds(zBounds);
  copy->setElementSizes(elementSizes);
  copy->setSpatialDimensionality(getSpatialDimensionality());

  return copy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::gatherMetaData(hid_t parentId, size_t volDims[3], bool preflight)
{
  int err = QH5Lite::readPointerDataset(parentId, H5_DIMENSIONS, volDims);
  if(err < 0)
  {
    return err;
  }
  FloatArrayType::Pointer xBnds = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::Geometry::xBoundsList, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer yBnds = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::Geometry::yBoundsList, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer zBnds = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::Geometry::zBoundsList, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer voxelSizes = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(SIMPL::StringConstants::VoxelSizes, parentId, preflight, err);
  if(err < 0 && err != -2)
  {
    return -1;
  }
  setDimensions(volDims);
  setXBounds(xBnds);
  setYBounds(yBnds);
  setZBounds(zBnds);
  setElementSizes(voxelSizes);

  return err;
}

// -----------------------------------------------------------------------------
RectGridGeom::Pointer RectGridGeom::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
RectGridGeom::Pointer RectGridGeom::New()
{
  Pointer sharedPtr(new(RectGridGeom));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString RectGridGeom::getNameOfClass() const
{
  return QString("RectGridGeom");
}

// -----------------------------------------------------------------------------
QString RectGridGeom::ClassName()
{
  return QString("RectGridGeom");
}

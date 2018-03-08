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

#ifndef _igeometrygrid_h_
#define _igeometrygrid_h_

#include <tuple>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Geometry/IGeometry.h"

    
    using Tuple3FV_Type = std::tuple<float, float, float>;
    using Tuple3DV_Type = std::tuple<double, double, double>;
    using Tuple3SV_Type = std::tuple<size_t, size_t, size_t>;
/**
 * @brief The IGeometryGrid class extends IGeometry for grid type geometries
 */
class SIMPLib_EXPORT IGeometryGrid : public IGeometry
{
  public:
    SIMPL_SHARED_POINTERS(IGeometryGrid)
     SIMPL_TYPE_MACRO_SUPER_OVERRIDE(IGeometryGrid, Observable)

    IGeometryGrid();
    virtual ~IGeometryGrid();
    
    virtual void setDimensions(size_t dims[3]) = 0;
    virtual void setDimensions(size_t a, size_t b, size_t c) = 0;
    virtual void setDimensions(const std::tuple<size_t, size_t, size_t> &dims) = 0;
    
    virtual void getDimensions(size_t dims[3]) = 0;
    virtual void getDimensions(size_t &a, size_t &b, size_t &c) = 0;
    virtual std::tuple<size_t, size_t, size_t> getDimensions() = 0;
    
    virtual size_t getXPoints() = 0;
    virtual size_t getYPoints() = 0;
    virtual size_t getZPoints() = 0;
    
    virtual void getPlaneCoords(size_t idx[3], float coords[3]) = 0;
    virtual void getPlaneCoords(size_t x, size_t y, size_t z, float coords[3]) = 0;
    virtual void getPlaneCoords(size_t idx, float coords[3]) = 0;
    
    virtual void getPlaneCoords(size_t idx[3], double coords[3]) = 0;
    virtual void getPlaneCoords(size_t x, size_t y, size_t z, double coords[3]) = 0;
    virtual void getPlaneCoords(size_t idx, double coords[3]) = 0;

    virtual void getCoords(size_t idx[3], float coords[3]) = 0;
    virtual void getCoords(size_t x, size_t y, size_t z, float coords[3]) = 0;
    virtual void getCoords(size_t idx, float coords[3]) = 0;
    
    virtual void getCoords(size_t idx[3], double coords[3]) = 0;
    virtual void getCoords(size_t x, size_t y, size_t z, double coords[3]) = 0;
    virtual void getCoords(size_t idx, double coords[3]) = 0;

  private:
    IGeometryGrid(const IGeometryGrid&) = delete;  // Copy Constructor Not Implemented
    void operator=(const IGeometryGrid&) = delete; // Operator '=' Not Implemented
};

#endif /* _igeometrygrid_h_ */


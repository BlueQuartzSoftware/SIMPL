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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <cmath>

#include <Eigen/Core>
#include <Eigen/Dense>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

class EdgeGeom;
class TriangleGeom;
class QuadGeom;
class TetrahedralGeom;
class HexahedralGeom;

/**
 * @brief This file contains a namespace with classes for computing derivatives on IGeometry objects
 */
namespace DerivativeHelpers
{

/**
 * @brief TriJacobian
 */
using TriangleJacobian = Eigen::Matrix<double, 2, 2, Eigen::RowMajor>;

/**
 * @brief QuadJacobian
 */
using QuadJacobian = Eigen::Matrix<double, 2, 2, Eigen::RowMajor>;

/**
 * @brief TetJacobian
 */
using TetJacobian = Eigen::Matrix<double, 3, 3, Eigen::RowMajor>;

/**
 * @brief HexJacobian
 */
using HexJacobian = Eigen::Matrix<double, 3, 3, Eigen::RowMajor>;

class EdgeDeriv
{
public:
  void operator()(EdgeGeom* edges, size_t edgeId, double values[2], double derivs[3]);
};

/**
 * @brief The TriangleDeriv class
 */
class TriangleDeriv
{
public:
  void operator()(TriangleGeom* triangles, size_t triId, double values[3], double derivs[3]);
};

/**
 * @brief The QuadDeriv class
 */
class QuadDeriv
{
public:
  void operator()(QuadGeom* quads, size_t quadId, double values[4], double derivs[3]);
};

/**
 * @brief The TetDeriv class
 */
class TetDeriv
{
public:
  void operator()(TetrahedralGeom* tets, size_t tetId, double values[4], double derivs[3]);
};

/**
 * @brief The HexDeriv class
 */
class HexDeriv
{
public:
  void operator()(HexahedralGeom* hexas, size_t hexId, double values[8], double derivs[3]);
};

} // namespace DerivativeHelpers

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

#include "ShapeOps.h"

#include "SIMPLib/Geometry/ShapeOps/CubeOctohedronOps.h"
#include "SIMPLib/Geometry/ShapeOps/CylinderAOps.h"
#include "SIMPLib/Geometry/ShapeOps/CylinderBOps.h"
#include "SIMPLib/Geometry/ShapeOps/CylinderCOps.h"
#include "SIMPLib/Geometry/ShapeOps/EllipsoidOps.h"
#include "SIMPLib/Geometry/ShapeOps/SuperEllipsoidOps.h"
#include "SIMPLib/Math/SIMPLibMath.h"

static const float cube_root_of_one = powf(1.0f, 0.333333333f);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeOps::ShapeOps() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeOps::~ShapeOps() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<ShapeOps::Pointer> ShapeOps::getShapeOpsQVector()
{
  QVector<ShapeOps::Pointer> m_ShapeOps;
  m_ShapeOps.push_back(EllipsoidOps::New());
  m_ShapeOps.push_back(SuperEllipsoidOps::New());
  m_ShapeOps.push_back(CubeOctohedronOps::New());
  m_ShapeOps.push_back(CylinderAOps::New());
  m_ShapeOps.push_back(CylinderBOps::New());
  m_ShapeOps.push_back(CylinderCOps::New());

  return m_ShapeOps;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<ShapeOps::Pointer> ShapeOps::getShapeOpsVector()
{
  std::vector<ShapeOps::Pointer> m_ShapeOps;
  m_ShapeOps.push_back(EllipsoidOps::New());
  m_ShapeOps.push_back(SuperEllipsoidOps::New());
  m_ShapeOps.push_back(CubeOctohedronOps::New());
  m_ShapeOps.push_back(CylinderAOps::New());
  m_ShapeOps.push_back(CylinderBOps::New());
  m_ShapeOps.push_back(CylinderCOps::New());

  return m_ShapeOps;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float ShapeOps::radcur1(QMap<ArgName, float> args)
{
  return cube_root_of_one;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float ShapeOps::inside(float axis1comp, float axis2comp, float axis3comp)
{
  return -1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeOps::init()
{
}

// -----------------------------------------------------------------------------
ShapeOps::Pointer ShapeOps::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
QString ShapeOps::getNameOfClass() const
{
  return QString("ShapeOps");
}

// -----------------------------------------------------------------------------
QString ShapeOps::ClassName()
{
  return QString("ShapeOps");
}

// -----------------------------------------------------------------------------
ShapeOps::Pointer ShapeOps::New()
{
  Pointer sharedPtr(new(ShapeOps));
  return sharedPtr;
}

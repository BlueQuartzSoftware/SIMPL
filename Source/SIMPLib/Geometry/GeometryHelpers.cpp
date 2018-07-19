/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
#include "GeometryHelpers.h"

namespace GeometryHelpers
{

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Translation::Translation() = default;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Translation::~Translation() = default;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Type Translation::StringToType(const QString& str)
{
  IGeometry::Type t = IGeometry::Type::Unknown;

  if(str.compare(SIMPL::Geometry::ImageGeometry) == 0)
  {
    return IGeometry::Type::Image;
  }
  if(str.compare(SIMPL::Geometry::RectGridGeometry) == 0)
  {
    return IGeometry::Type::RectGrid;
  }
  if(str.compare(SIMPL::Geometry::VertexGeometry) == 0)
  {
    return IGeometry::Type::Vertex;
  }
  if(str.compare(SIMPL::Geometry::EdgeGeometry) == 0)
  {
    return IGeometry::Type::Edge;
  }
  if(str.compare(SIMPL::Geometry::TriangleGeometry) == 0)
  {
    return IGeometry::Type::Triangle;
  }
  if(str.compare(SIMPL::Geometry::QuadGeometry) == 0)
  {
    return IGeometry::Type::Quad;
  }
  if(str.compare(SIMPL::Geometry::TetrahedralGeometry) == 0)
  {
    return IGeometry::Type::Tetrahedral;
  }
  if(str.compare(SIMPL::Geometry::UnknownGeometry) == 0)
  {
    return IGeometry::Type::Unknown;
  }
  if(str.compare(SIMPL::Geometry::AnyGeometry) == 0)
  {
    return IGeometry::Type::Any;
  }

  return t;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Translation::TypeToString(IGeometry::Type t)
{
  QString geomType;
  // The type values for RectGrid and ImageGeom are the same value (Why?)
  // which causes compile errors with the switch statement so we special
  // case the RectGrid here, and leave it out of the Switch block.
  if (t ==  IGeometry::Type::RectGrid)
  {
    return SIMPL::Geometry::RectGridGeometry;
  }

  switch(t)
  {
  
  case IGeometry::Type::Image:
    geomType = SIMPL::Geometry::ImageGeometry;
    break;
  case IGeometry::Type::Vertex:
    geomType = SIMPL::Geometry::VertexGeometry;
    break;
  case IGeometry::Type::Edge:
    geomType = SIMPL::Geometry::EdgeGeometry;
    break;
  case IGeometry::Type::Triangle:
    geomType = SIMPL::Geometry::TriangleGeometry;
    break;
  case IGeometry::Type::Quad:
    geomType = SIMPL::Geometry::QuadGeometry;
    break;
  case IGeometry::Type::Tetrahedral:
    geomType = SIMPL::Geometry::TetrahedralGeometry;
    break;
  case IGeometry::Type::Hexahedral:
    geomType = SIMPL::Geometry::HexahedralGeometry;
    break;
  case IGeometry::Type::Unknown:
    geomType = SIMPL::Geometry::UnknownGeometry;
    break;
  case IGeometry::Type::RectGrid:
    geomType = SIMPL::Geometry::RectGridGeometry;
    break;
  case IGeometry::Type::Any:
    geomType = SIMPL::Geometry::AnyGeometry;
    break;
  }

  return geomType;
}


}

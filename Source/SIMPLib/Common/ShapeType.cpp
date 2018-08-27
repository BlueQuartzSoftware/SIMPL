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

#include "ShapeType.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeType::ShapeType() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeType::~ShapeType() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<ShapeType::EnumType> ShapeType::ToQVector(Types &types)
{
  QVector<ShapeType::EnumType> vec(types.size());
  for(int i = 0; i < types.size(); i++)
  {
    vec[i] = static_cast<ShapeType::EnumType>(types[i]);
  }
  return vec;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeType::Types ShapeType::FromQVector(QVector<ShapeType::EnumType> &vec)
{
  ShapeType::Types types(vec.size());
  for(int i = 0; i < vec.size(); i++)
  {
    types[i] = static_cast<ShapeType::Type>(vec[i]);
  }
  return types;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ShapeType::EllipsoidStr()
{
  return QString("Ellipsoid");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ShapeType::SuperEllipsoid()
{
  return QString("Super Ellipsoid");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ShapeType::CubeOctahedronStr()
{
  return QString("Cube Octahedron");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ShapeType::CylinderAStr()
{
  return QString("Cylinder (A)");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ShapeType::CylinderBStr()
{
  return QString("Cylinder (B)");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ShapeType::CylinderCStr()
{
  return QString("Cylinder (C)");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ShapeType::ShapeTypeEndStr()
{
  return QString("Shape Type End");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ShapeType::UnknownShapeTypeStr()
{
  return QString("Unknown Shape Type");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ShapeType::getShapeTypeString(Type ShapeType)
{
  switch(ShapeType)
  {
  case Type::Ellipsoid:
    return EllipsoidStr();
  case Type::SuperEllipsoid:
    return SuperEllipsoid();
  case Type::CubeOctahedron:
    return CubeOctahedronStr();
  case Type::CylinderA:
    return CylinderAStr();
  case Type::CylinderB:
    return CylinderBStr();
  case Type::CylinderC:
    return CylinderCStr();
  case Type::ShapeTypeEnd:
      return ShapeTypeEndStr();
  case Type::Unknown:
    return UnknownShapeTypeStr();
  default:
    break;
  }
  return QString("Undefined ShapeType (Error)");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeType::Type ShapeType::getShapeType(const char* str)
{
  if(EllipsoidStr().compare(str) == 0)
  {
    return Type::Ellipsoid;
  }
  else if(SuperEllipsoid().compare(str) == 0)
  {
    return Type::SuperEllipsoid;
  }
  else if(CubeOctahedronStr().compare(str) == 0)
  {
    return Type::CubeOctahedron;
  }
  else if(CylinderAStr().compare(str) == 0)
  {
    return Type::CylinderA;
  }
  else if(CylinderBStr().compare(str) == 0)
  {
    return Type::CylinderB;
  }
  else if(CylinderCStr().compare(str) == 0)
  {
    return Type::CylinderC;
  }
  else if(ShapeTypeEndStr().compare(str) == 0)
  {
    return Type::ShapeTypeEnd;
  }

  return Type::Unknown;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeType::getShapeTypeStrings(QVector<QString>& strings)
{
  strings.clear();
  strings.push_back(EllipsoidStr());
  strings.push_back(SuperEllipsoid());
  strings.push_back(CubeOctahedronStr());
  strings.push_back(CylinderAStr());
  strings.push_back(CylinderBStr());
  strings.push_back(CylinderCStr());
  strings.push_back(ShapeTypeEndStr());
  strings.push_back(UnknownShapeTypeStr());

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeType::getShapeTypeEnums(ShapeType::Types& types)
{
  types.clear();
  types.push_back(Type::Ellipsoid);
  types.push_back(Type::SuperEllipsoid);
  types.push_back(Type::CubeOctahedron);
  types.push_back(Type::CylinderA);
  types.push_back(Type::CylinderB);
  types.push_back(Type::CylinderC);
  types.push_back(Type::ShapeTypeEnd);
  types.push_back(Type::Unknown);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeType::getShapeTypeMap(QMap<ShapeType::Type, QString>& map)
{
  map.clear();
  map[Type::Ellipsoid] = EllipsoidStr();
  map[Type::SuperEllipsoid] = SuperEllipsoid();
  map[Type::CubeOctahedron] = CubeOctahedronStr();
  map[Type::CylinderA] = CylinderAStr();
  map[Type::CylinderB] = CylinderBStr();
  map[Type::CylinderC] = CylinderCStr();
  map[Type::ShapeTypeEnd] = ShapeTypeEndStr();
  map[Type::Unknown] = UnknownShapeTypeStr();
}

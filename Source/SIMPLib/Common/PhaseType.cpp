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

#include "PhaseType.h"

#include "SIMPLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseType::PhaseType() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseType::~PhaseType() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<PhaseType::EnumType> PhaseType::ToQVector(Types &types)
{
  QVector<PhaseType::EnumType> vec(types.size());
  for(int i = 0; i < types.size(); i++)
  {
    vec[i] = static_cast<PhaseType::EnumType>(types[i]);
  }
  return vec;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseType::Types PhaseType::FromQVector(QVector<PhaseType::EnumType> &vec)
{
  PhaseType::Types types(vec.size());
  for(int i = 0; i < vec.size(); i++)
  {
    types[i] = static_cast<PhaseType::Type>(vec[i]);
  }
  return types;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhaseType::PrimaryStr()
{
  return QString("Primary");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhaseType::PrecipitateStr()
{
  return QString("Precipitate");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhaseType::TransformationStr()
{
  return QString("Transformation");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhaseType::MatrixStr()
{
  return QString("Matrix");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhaseType::BoundaryStr()
{
  return QString("Boundary");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhaseType::UnknownStr()
{
  return QString("Unknown Phase Type");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhaseType::getPhaseTypeString(Type phaseType)
{
  switch(phaseType)
  {
  case Type::Primary:
    return PrimaryStr();
  case Type::Precipitate:
    return PrecipitateStr();
  case Type::Transformation:
    return TransformationStr();
  case Type::Matrix:
    return MatrixStr();
  case Type::Boundary:
    return BoundaryStr();
  case Type::Unknown:
    return UnknownStr();
  default:
    break;
  }
  return QString("Undefined Phase Type (Error)");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseType::Type PhaseType::getPhaseType(const char* str)
{
  if(PrimaryStr().compare(str) == 0)
  {
    return Type::Primary;
  }
  else if(PrecipitateStr().compare(str) == 0)
  {
    return Type::Precipitate;
  }
  else if(TransformationStr().compare(str) == 0)
  {
    return Type::Transformation;
  }
  else if(MatrixStr().compare(str) == 0)
  {
    return Type::Matrix;
  }
  else if(BoundaryStr().compare(str) == 0)
  {
    return Type::Boundary;
  }
  return Type::Unknown;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseType::getPhaseTypeStrings(QStringList& strings)
{
  strings.clear();
  strings.push_back(PrimaryStr());
  strings.push_back(PrecipitateStr());
  strings.push_back(TransformationStr());
  strings.push_back(MatrixStr());
  strings.push_back(BoundaryStr());
  strings.push_back(UnknownStr());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseType::getPhaseTypeEnums(QVector<Type>& types)
{
  types.clear();
  types.push_back(Type::Primary);
  types.push_back(Type::Precipitate);
  types.push_back(Type::Transformation);
  types.push_back(Type::Matrix);
  types.push_back(Type::Boundary);
  types.push_back(Type::Unknown);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseType::getPhaseTypeMap(QMap<Type, QString>& map)
{
  map.clear();
  map[Type::Primary] = PrimaryStr();
  map[Type::Precipitate] = PrecipitateStr();
  map[Type::Transformation] = TransformationStr();
  map[Type::Matrix] = MatrixStr();
  map[Type::Boundary] = BoundaryStr();
  map[Type::Unknown] = UnknownStr();
}

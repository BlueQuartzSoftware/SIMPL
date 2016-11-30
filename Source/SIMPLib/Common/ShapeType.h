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

#ifndef _shapetype_h_
#define _shapetype_h_


#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QMap>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"



/**
 * @class ShapeType ShapeType.h PathToHeader/ShapeType.h
 * @brief
 * @author Mike Jackson for BlueQuartz.net
 * @date June 28, 2011
 * @version 1.0
 */
class SIMPLib_EXPORT ShapeType
{
  public:

    virtual ~ShapeType();

    using EnumType = unsigned int;

		enum class Type : EnumType
		{
			EllipsoidShape = 0, //!<
			SuperEllipsoidShape = 1, //!<
			CubeOctahedronShape = 2, //!<
			CylinderAShape = 3, //!<
			CylinderBShape = 4, //!<
			CylinderCShape = 5, //!<
			ShapeTypeEnd = 6,
			UnknownShapeType = 999 //!<
		};

		using Types = QVector<Type>;

    static QString EllipsoidStr() { return QString("Ellipsoid"); }
    static QString SuperEllipsoid() { return QString("Super Ellipsoid"); }
    static QString CubeOctahedronStr() { return QString("Cube Octahedron"); }
    static QString CylinderAStr() { return QString("Cylinder (A)"); }
    static QString CylinderBStr() { return QString("Cylinder (B)"); }
    static QString CylinderCStr() { return QString("Cylinder (C)"); }
    static QString UnknownShapeTypeStr() { return QString("Unknown Shape Type"); }

    static QString getShapeTypeString(Type ShapeType)
    {
      switch(ShapeType)
      {
         case Type::EllipsoidShape:
          return EllipsoidStr();
        case Type::SuperEllipsoidShape:
          return SuperEllipsoid();
        case Type::CubeOctahedronShape:
          return CubeOctahedronStr();
        case Type::CylinderAShape:
          return CylinderAStr();
        case Type::CylinderBShape:
          return CylinderBStr();
        case Type::CylinderCShape:
          return CylinderCStr();
        case Type::UnknownShapeType:
          return UnknownShapeTypeStr();
        default:
          break;
      }
      return QString("Undefined ShapeType (Error)");
    }

		static Type getShapeType(const char* str)
    {
      if (EllipsoidStr().compare(str) == 0)
      {
        return Type::EllipsoidShape;
      }
      else if (SuperEllipsoid().compare(str) == 0)
      {
        return Type::SuperEllipsoidShape;
      }
      else if (CubeOctahedronStr().compare(str) == 0)
      {
        return Type::CubeOctahedronShape;
      }
      else if (CylinderAStr().compare(str) == 0)
      {
        return Type::CylinderAShape;
      }
      else if (CylinderBStr().compare(str) == 0)
      {
        return Type::CylinderBShape;
      }
      else if (CylinderCStr().compare(str) == 0)
      {
        return Type::CylinderCShape;
      }
      return Type::UnknownShapeType;
    }


    static void getShapeTypeStrings(QVector<QString>& strings)
    {
      strings.clear();
      strings.push_back(EllipsoidStr());
      strings.push_back(SuperEllipsoid());
      strings.push_back(CubeOctahedronStr());
      strings.push_back(CylinderAStr());
      strings.push_back(CylinderBStr());
      strings.push_back(CylinderCStr());
      strings.push_back(UnknownShapeTypeStr());
    }

		static void getShapeTypeEnums(QVector<Type>& types)
    {
      types.clear();
      types.push_back(Type::EllipsoidShape);
      types.push_back(Type::SuperEllipsoidShape);
      types.push_back(Type::CubeOctahedronShape);
      types.push_back(Type::CylinderAShape);
      types.push_back(Type::CylinderBShape);
      types.push_back(Type::CylinderCShape);
      types.push_back(Type::UnknownShapeType);
    }

		static void getShapeTypeMap(QMap<Type, QString>& map)
    {
      map.clear();
      map[Type::EllipsoidShape] = EllipsoidStr();
      map[Type::SuperEllipsoidShape] = SuperEllipsoid();
      map[Type::CubeOctahedronShape] = CubeOctahedronStr();
      map[Type::CylinderAShape] = CylinderAStr();
      map[Type::CylinderBShape] = CylinderBStr();
      map[Type::CylinderCShape] = CylinderCStr();
      map[Type::UnknownShapeType] = UnknownShapeTypeStr();
    }
  protected:
    ShapeType();
  private:
    ShapeType(const ShapeType&); //Not Implemented
    void operator=(const ShapeType&); //Not Implemented



};

Q_DECLARE_METATYPE(ShapeType::Type)

#endif /* SHAPETYPE_H_ */


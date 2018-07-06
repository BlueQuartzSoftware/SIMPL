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

#pragma once

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVector>


#include "SIMPLib/SIMPLib.h"

/**
 * @class PhaseType PhaseType.h PathToHeader/PhaseType.h
 * @brief
 * @author Mike Jackson for BlueQuartz.net
 * @version 1.0
 */
class SIMPLib_EXPORT PhaseType
{
public:
  virtual ~PhaseType();

  using EnumType = uint32_t;

  enum class Type : EnumType
  {
    Primary = 0,        //!<
    Precipitate = 1,    //!<
    Transformation = 2, //!<
    Matrix = 3,         //!<
    Boundary = 4,       //!<
    Unknown = 999,   //!<
    Any = 4294967295U

  };

  using Types = QVector<Type>;

  /**
   * @brief Converts to a QVector<EnumType>. Useful for
   * writing to/from HDF5 or other streams that do not understand Type
   * @param types
   * @return
   */
  static QVector<EnumType> ToQVector(Types &types);

  /**
   * @brief Converts <b>From</b> a QVector<EnumType> to QVector<Type>. Useful for
   * writing to/from HDF5 or other streams that do not understand Type
   * @param types
   * @return
   */
  static Types FromQVector(QVector<EnumType> &types);

  /**
   * @brief PrimaryStr
   * @return
   */
  static QString PrimaryStr();

  /**
   * @brief PrecipitateStr
   * @return
   */
  static QString PrecipitateStr();

  /**
   * @brief TransformationStr
   * @return
   */
  static QString TransformationStr();

  /**
   * @brief MatrixStr
   * @return
   */
  static QString MatrixStr();

  /**
   * @brief BoundaryStr
   * @return
   */
  static QString BoundaryStr();

  /**
   * @brief UnknownStr
   * @return
   */
  static QString UnknownStr();

  /**
   * @brief getPhaseTypeString
   * @param phaseType
   * @return
   */
  static QString getPhaseTypeString(Type phaseType);

  /**
   * @brief getPhaseType
   * @param str
   * @return
   */
  static Type getPhaseType(const char* str);

  /**
   * @brief getPhaseTypeStrings
   * @param strings
   */
  static void getPhaseTypeStrings(QStringList& strings);

  /**
   * @brief getPhaseTypeEnums
   * @param types
   */
  static void getPhaseTypeEnums(QVector<Type>& types);

  /**
   * @brief getPhaseTypeMap
   * @param map
   */
  static void getPhaseTypeMap(QMap<Type, QString>& map);

protected:
  PhaseType();

private:
  PhaseType(const PhaseType&);      // Not Implemented
  void operator=(const PhaseType&); // Not Implemented
};

Q_DECLARE_METATYPE(PhaseType::Type)
Q_DECLARE_METATYPE(PhaseType::Types)


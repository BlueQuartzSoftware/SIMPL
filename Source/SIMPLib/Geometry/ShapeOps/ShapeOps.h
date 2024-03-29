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

#include <memory>
#include <vector>

#include <QtCore/QMap>
#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"

/**
 * @brief The ShapeOps class
 */
class SIMPLib_EXPORT ShapeOps
{
public:
  using Self = ShapeOps;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for ShapeOps
   */
  virtual QString getNameOfClass() const;
  /**
   * @brief Returns the name of the class for ShapeOps
   */
  static QString ClassName();

  static Pointer New();

  virtual ~ShapeOps();

  enum ArgName
  {
    Omega3 = 0,
    B_OverA = 1,
    C_OverA = 2,
    VolCur = 3
  };

  float ShapeClass2Omega[41][2];

  /**
   * @brief getShapeOpsVector This method returns a vector of each type of ShapeOps placed such that the
   * index into the vector is the value of the constant at DRAM3D::ShapeType::***
   * @return Vector of ShapeOps subclasses.
   */
  static QVector<ShapeOps::Pointer> getShapeOpsQVector();

  /**
   * @brief getShapeOpsVector This method returns a vector of each type of Shape placed such that the
   * index into the vector is the value of the constant at
   * @return Vector of ShapeOps subclasses.
   */
  static std::vector<ShapeOps::Pointer> getShapeOpsVector();

  virtual float radcur1(QMap<ArgName, float> args);

  virtual float inside(float axis1comp, float axis2comp, float axis3comp);

  virtual void init();

protected:
  ShapeOps();

public:
  ShapeOps(const ShapeOps&) = delete;            // Copy Constructor Not Implemented
  ShapeOps(ShapeOps&&) = delete;                 // Move Constructor Not Implemented
  ShapeOps& operator=(const ShapeOps&) = delete; // Copy Assignment Not Implemented
  ShapeOps& operator=(ShapeOps&&) = delete;      // Move Assignment Not Implemented

private:
};

/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLPythonMacros.h"

/**
 * @brief
 */
class SIMPLib_EXPORT AxisAngleInput
{
  // Start Python bindings declarations
  // clang-format off
  PYB11_BEGIN_BINDINGS(AxisAngleInput)
  PYB11_CREATION(float float float float)
  PYB11_END_BINDINGS()
  // clang-format on
  // End Python bindings declarations

public:
  AxisAngleInput();
  ~AxisAngleInput();

  AxisAngleInput(float h, float k, float l, float angle);

  float h = 0.0f;
  float k = 0.0f;
  float l = 0.0f;
  float angle = 0.0f;

  /**
   * @brief writeJson
   * @param json
   */
  void writeJson(QJsonObject& json) const;

  /**
   * @brief readJson
   * @param json
   * @return
   */
  bool readJson(const QJsonObject& json);

public:
  AxisAngleInput(const AxisAngleInput&) = default;
  AxisAngleInput(AxisAngleInput&&) = default;
  AxisAngleInput& operator=(const AxisAngleInput&) = default;
  AxisAngleInput& operator=(AxisAngleInput&&) = default;
};

Q_DECLARE_METATYPE(AxisAngleInput)

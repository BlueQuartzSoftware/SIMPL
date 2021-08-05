/* ============================================================================
 * Copyright (c) 2021-2021 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once
#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLPythonMacros.h"

/**
 * @brief The Float2ndOrderPolynomial class
 */
class SIMPLib_EXPORT Float2ndOrderPolynomial
{
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(Float2ndOrderPolynomial)
  PYB11_CREATION(float float float float float float)
  PYB11_END_BINDINGS()
  // End Python bindings declarations
public:
  Float2ndOrderPolynomial();
  ~Float2ndOrderPolynomial();

  /**
   * @brief Float2ndOrderPolynomial
   * @param c00
   * @param c01
   * @param c10
   * @param c11
   * @param c02
   * @param c20
   */
  Float2ndOrderPolynomial(float c00, float c01, float c10, float c11, float c02, float c20);

  float c20 = 0.0f;
  float c02 = 0.0f;
  float c11 = 0.0f;
  float c10 = 0.0f;
  float c01 = 0.0f;
  float c00 = 0.0f;

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
};

Q_DECLARE_METATYPE(Float2ndOrderPolynomial)

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
#include "Float2ndOrderPolynomial.h"

// -----------------------------------------------------------------------------
Float2ndOrderPolynomial::Float2ndOrderPolynomial() = default;

// -----------------------------------------------------------------------------
Float2ndOrderPolynomial::~Float2ndOrderPolynomial() = default;

// -----------------------------------------------------------------------------
Float2ndOrderPolynomial::Float2ndOrderPolynomial(float c00, float c01, float c10, float c11, float c02, float c20)
: c20(c20)
, c02(c02)
, c11(c11)
, c10(c10)
, c01(c01)
, c00(c00)
{
}

// -----------------------------------------------------------------------------
void Float2ndOrderPolynomial::writeJson(QJsonObject& json) const
{
  json["c20"] = c20;
  json["c02"] = c02;
  json["c11"] = c11;
  json["c10"] = c10;
  json["c01"] = c01;
  json["c00"] = c00;
}

// -----------------------------------------------------------------------------
bool Float2ndOrderPolynomial::readJson(const QJsonObject& json)
{
  if(json["c20"].isDouble() && json["c02"].isDouble() && json["c11"].isDouble() && json["c10"].isDouble() && json["c01"].isDouble() && json["c00"].isDouble())
  {
    c20 = static_cast<float>(json["c20"].toDouble());
    c02 = static_cast<float>(json["c02"].toDouble());
    c11 = static_cast<float>(json["c11"].toDouble());
    c10 = static_cast<float>(json["c10"].toDouble());
    c01 = static_cast<float>(json["c01"].toDouble());
    c00 = static_cast<float>(json["c00"].toDouble());
    return true;
  }
  return false;
}

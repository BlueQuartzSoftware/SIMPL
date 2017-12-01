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
#ifndef _floatSummation_h_
#define _floatSummation_h_

#include <vector>
#include <initializer_list>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"

/**
* @brief The FloatSummation class contains helper methods for summation of floating point numbers
*/
class SIMPLib_EXPORT FloatSummation
{
public:
  FloatSummation();
  virtual ~FloatSummation();

  /**
  * @brief Performs a Kahan summation over a vector of floating point numbers and returns the result
  * @param values The vector of floats used for the summation
  * @returns Kahan summation of floating point numbers
  */
  static float Kahanf(std::vector<float> values);
  /**
  * @brief Performs a Kahan summation over a vector of floating point numbers and returns the result
  * @param values The vector of doubles used for the summation
  * @returns Kahan summation of floating point numbers
  */
  static double Kahan(std::vector<double> values);

  /**
  * @brief Performs a Kahan summation over a list of floating point numbers and returns the result
  * @param values The initializer_list of floats used for the summation
  * @returns Kahan summation of floating point numbers
  */
  static float Kahanf(std::initializer_list<float> values);
  /**
  * @brief Performs a Kahan summation over a list of floating point numbers and returns the result
  * @param values The initializer_list of doubles used for the summation
  * @returns Kahan summation of floating point numbers
  */
  static double Kahan(std::initializer_list<double> values);

private:
  FloatSummation(const FloatSummation&) = delete; // Copy Constructor Not Implemented
  void operator=(const FloatSummation&) = delete; // Operator '=' Not Implemented
};

#endif

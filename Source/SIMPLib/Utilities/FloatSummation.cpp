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
#include "SIMPLib/Utilities/FloatSummation.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatSummation::FloatSummation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatSummation::~FloatSummation() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float FloatSummation::Kahanf(std::vector<float> values)
{
  float sum = 0.0;
  float compensation = 0.0;

  for(std::vector<float>::size_type i = 0; i < values.size(); i++)
  {
    float adjustedValue = values.at(i) - compensation;
    float newSum = sum + adjustedValue;
    compensation = (newSum - sum) - adjustedValue;

    sum = newSum;
  }

  return sum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double FloatSummation::Kahan(std::vector<double> values)
{
  double sum = 0.0;
  double compensation = 0.0;

  for(std::vector<double>::size_type i = 0; i < values.size(); i++)
  {
    double adjustedValue = values.at(i) - compensation;
    double newSum = sum + adjustedValue;
    compensation = (newSum - sum) - adjustedValue;

    sum = newSum;
  }

  return sum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float FloatSummation::Kahanf(std::initializer_list<float> values)
{
  float sum = 0.0;
  float compensation = 0.0;

  for(auto iter = std::begin(values); iter != std::end(values); iter++)
  {
    float adjustedValue = (*iter) - compensation;
    float newSum = sum + adjustedValue;
    compensation = (newSum - sum) - adjustedValue;

    sum = newSum;
  }

  return sum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double FloatSummation::Kahan(std::initializer_list<double> values)
{
  double sum = 0.0;
  double compensation = 0.0;

  for(auto iter = std::begin(values); iter != std::end(values); iter++)
  {
    double adjustedValue = (*iter) - compensation;
    double newSum = sum + adjustedValue;
    compensation = (newSum - sum) - adjustedValue;

    sum = newSum;
  }

  return sum;
}

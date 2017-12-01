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

#include "ComparisonValue.h"

#include "SIMPLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonValue::ComparisonValue()
: AbstractComparison()
{
  m_attributeArrayName = "";
  m_compOperator = SIMPL::Comparison::Operator_GreaterThan;
  m_compValue = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonValue::~ComparisonValue() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValue::writeJson(QJsonObject& json)
{
  json["Union Operator"] = m_unionOperator;
  json["Attribute Array Name"] = m_attributeArrayName;
  json["Comparison Operator"] = m_compOperator;
  json["Comparison Value"] = m_compValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonValue::readJson(QJsonObject& json)
{
  if(json["Attribute Array Name"].isString() && json["Comparison Operator"].isDouble() && json["Comparison Value"].isDouble())
  {
    // allow reading legacy stored data without union operators
    if(json["Union Operator"].isDouble())
    {
      m_unionOperator = json["Union Operator"].toInt();
    }
    else
    {
      m_unionOperator = SIMPL::Union::Operator_And;
    }

    m_attributeArrayName = json["Attribute Array Name"].toString();
    m_compOperator = json["Comparison Operator"].toInt();
    m_compValue = json["Comparison Value"].toDouble();
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComparisonValue::getAttributeArrayName()
{
  return m_attributeArrayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ComparisonValue::getCompOperator()
{
  return m_compOperator;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double ComparisonValue::getCompValue()
{
  return m_compValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValue::setAttributeArrayName(QString daName)
{
  m_attributeArrayName = daName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValue::setCompOperator(int compOperator)
{
  m_compOperator = compOperator;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValue::setCompValue(double value)
{
  m_compValue = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSet::Pointer ComparisonValue::getParentSet()
{
  return m_parentSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValue::setParentSet(ComparisonSet::Pointer parentSet)
{
  m_parentSet = parentSet;
}

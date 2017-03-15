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

#include "ComparisonInputsAdvanced.h"

#include "SIMPLib/Common/ComparisonSet.h"
#include "SIMPLib/Common/ComparisonValue.h"

#include "moc_ComparisonInputsAdvanced.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInputsAdvanced::ComparisonInputsAdvanced()
{
  m_invert = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInputsAdvanced::ComparisonInputsAdvanced(const ComparisonInputsAdvanced& rhs)
{
  m_dataContainerName = rhs.m_dataContainerName;
  m_attributeMatrixName = rhs.m_attributeMatrixName;
  m_Inputs = rhs.m_Inputs;

  m_invert = rhs.m_invert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// ComparisonInputs::ComparisonInputs(ComparisonInputs& rhs)
//{
//  m_Inputs = rhs.m_Inputs;
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInputsAdvanced::~ComparisonInputsAdvanced()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ComparisonInputsAdvanced::size()
{
  return m_Inputs.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonInputsAdvanced::addInput(int unionOperator, const QString arrayName, int compOperator, double compValue)
{
  ComparisonValue::Pointer v = ComparisonValue::New();
  v->setUnionOperator(unionOperator);
  v->setAttributeArrayName(arrayName);
  v->setCompOperator(compOperator);
  v->setCompValue(compValue);
  m_Inputs.push_back(v);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonInputsAdvanced::addInput(int unionOperator, bool invertComparison, QVector<AbstractComparison::Pointer> comparisons)
{
  ComparisonSet::Pointer s = ComparisonSet::New();
  s->setUnionOperator(unionOperator);
  s->setInvertComparison(invertComparison);
  s->setComparisons(comparisons);
  m_Inputs.push_back(s);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonInputsAdvanced::addInput(const AbstractComparison::Pointer input)
{
  m_Inputs.push_back(input);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonInputsAdvanced::setInputs(QVector<AbstractComparison::Pointer> comparisons)
{
  m_Inputs = comparisons;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractComparison::Pointer ComparisonInputsAdvanced::getInput(int index)
{
  return m_Inputs[index];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractComparison::Pointer ComparisonInputsAdvanced::operator[](int index)
{
  return m_Inputs[index];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AbstractComparison::Pointer>& ComparisonInputsAdvanced::getInputs()
{
  return m_Inputs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComparisonInputsAdvanced::getDataContainerName()
{
  return m_dataContainerName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComparisonInputsAdvanced::getAttributeMatrixName()
{
  return m_attributeMatrixName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonInputsAdvanced::setDataContainerName(QString dcName)
{
  m_dataContainerName = dcName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonInputsAdvanced::setAttributeMatrixName(QString amName)
{
  m_attributeMatrixName = amName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonInputsAdvanced::operator=(const ComparisonInputsAdvanced& rhs)
{
  m_dataContainerName = rhs.m_dataContainerName;
  m_attributeMatrixName = rhs.m_attributeMatrixName;
  m_Inputs = rhs.m_Inputs;

  m_invert = rhs.m_invert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonInputsAdvanced::hasComparisonValue()
{
  if (m_Inputs.size() == 0)
    return false;

  for (int i = 0; i < m_Inputs.size(); i++)
  {
    if (std::dynamic_pointer_cast<ComparisonValue>(m_Inputs[i]))
    {
      return true;
    }
    if (std::dynamic_pointer_cast<ComparisonSet>(m_Inputs[i]))
    {
      ComparisonSet::Pointer comparisonSet = std::dynamic_pointer_cast<ComparisonSet>(m_Inputs[i]);
      if (comparisonSet->hasComparisonValue())
      {
        return true;
      }
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AbstractComparison::Pointer> ComparisonInputsAdvanced::getComparisonValues()
{
  QVector<AbstractComparison::Pointer> comparisonValues;

  for (int i = 0; i < m_Inputs.size(); i++)
  {
    if (std::dynamic_pointer_cast<ComparisonValue>(m_Inputs[i]))
    {
      ComparisonValue::Pointer compValue = std::dynamic_pointer_cast<ComparisonValue>(m_Inputs[i]);
      comparisonValues.push_back(compValue);
    }
    if (std::dynamic_pointer_cast<ComparisonSet>(m_Inputs[i]))
    {
      ComparisonSet::Pointer comparisonSet = std::dynamic_pointer_cast<ComparisonSet>(m_Inputs[i]);
      QVector<AbstractComparison::Pointer> setValues = comparisonSet->getComparisonValues();
      
      comparisonValues.append(setValues);
    }
  }

  return comparisonValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonInputsAdvanced::shouldInvert()
{
  return m_invert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonInputsAdvanced::setInvert(bool invert)
{
  m_invert = invert;
}

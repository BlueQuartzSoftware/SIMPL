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

#include "ComparisonSet.h"

#include <QtCore/QJsonArray>

#include "SIMPLib/Filtering/ComparisonValue.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSet::ComparisonSet()
: m_invertComparison(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSet::~ComparisonSet()
{
  m_comparisons.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSet::writeJson(QJsonObject& json)
{
  json["Union Operator"] = m_unionOperator;
  json["Invert Comparison"] = m_invertComparison;

  QJsonArray jsonArray;
  for(int i = 0; i < m_comparisons.size(); i++)
  {
    QJsonObject valueObj;
    m_comparisons[i]->writeJson(valueObj);

    jsonArray.push_back(valueObj);
  }

  json["Comparison Values"] = jsonArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonSet::readJson(QJsonObject& json)
{
  if(json["Union Operator"].isDouble() && json["Invert Comparison"].isBool())
  {
    m_unionOperator = json["Union Operator"].toInt();
    m_invertComparison = json["Invert Comparison"].toBool();

    m_comparisons.clear();
    QJsonArray jsonArray = json["Comparison Values"].toArray();
    for(int i = 0; i < jsonArray.size(); i++)
    {
      QJsonObject jsonObj = jsonArray.at(i).toObject();

      if(jsonObj["Invert Comparison"].isBool())
      {
        ComparisonSet::Pointer childSet = ComparisonSet::New();
        if(childSet->readJson(jsonObj))
        {
          m_comparisons.push_back(childSet);
        }
        else
        {
          return false;
        }
      }
      else
      {
        ComparisonValue::Pointer childValue = ComparisonValue::New();
        if(childValue->readJson(jsonObj))
        {
          m_comparisons.push_back(childValue);
        }
        else
        {
          return false;
        }
      }
    }

    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonSet::getInvertComparison()
{
  return m_invertComparison;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AbstractComparison::Pointer> ComparisonSet::getComparisons()
{
  return m_comparisons;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSet::setInvertComparison(bool invert)
{
  m_invertComparison = invert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSet::setComparisons(QVector<AbstractComparison::Pointer> comparisonValues)
{
  m_comparisons = comparisonValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSet::addComparison(AbstractComparison::Pointer comparison)
{
  m_comparisons.push_back(comparison);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSet::insertComparison(int index, AbstractComparison::Pointer comparison)
{
  if(index >= 0 && index <= m_comparisons.size())
  {
    m_comparisons.insert(index, comparison);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonSet::hasComparisonValue()
{
  if(m_comparisons.empty())
  {
    return false;
  }

  for(int i = 0; i < m_comparisons.size(); i++)
  {
    if(std::dynamic_pointer_cast<ComparisonValue>(m_comparisons[i]))
    {
      return true;
    }
    if(std::dynamic_pointer_cast<ComparisonSet>(m_comparisons[i]))
    {
      ComparisonSet::Pointer comparisonSet = std::dynamic_pointer_cast<ComparisonSet>(m_comparisons[i]);
      if(comparisonSet->hasComparisonValue())
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
QVector<AbstractComparison::Pointer> ComparisonSet::getComparisonValues()
{
  QVector<AbstractComparison::Pointer> comparisonValues;

  for(int i = 0; i < m_comparisons.size(); i++)
  {
    if(std::dynamic_pointer_cast<ComparisonValue>(m_comparisons[i]))
    {
      ComparisonValue::Pointer compValue = std::dynamic_pointer_cast<ComparisonValue>(m_comparisons[i]);
      comparisonValues.push_back(compValue);
    }
    if(std::dynamic_pointer_cast<ComparisonSet>(m_comparisons[i]))
    {
      ComparisonSet::Pointer comparisonSet = std::dynamic_pointer_cast<ComparisonSet>(m_comparisons[i]);
      QVector<AbstractComparison::Pointer> setValues = comparisonSet->getComparisonValues();

      comparisonValues.append(setValues);
    }
  }

  return comparisonValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonSet::renameDataArrayPath(DataArrayPath::RenameType renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  if(oldPath.getDataArrayName().isEmpty() || oldPath.getDataArrayName() == newPath.getDataArrayName())
  {
    return false;
  }

  bool updated = false;
  QVector<AbstractComparison::Pointer> comparisons = getComparisonValues();
  for(AbstractComparison::Pointer comparison : comparisons)
  {
    updated |= comparison->renameDataArrayPath(renamePath);
  }

  return updated;
}

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

#ifndef _ComparisonSet_h_
#define _ComparisonSet_h_

#include "AbstractComparison.h"

#include <QtCore/QVector>

/**
* @brief The ComparisonSet class is used by ComparisonInputsAdvanced to group ComparisonValues
* as a single entity to apply union operators or invert the resulting comparison of the group.
*/
class SIMPLib_EXPORT ComparisonSet : public AbstractComparison
{
public:
  SIMPL_SHARED_POINTERS(ComparisonSet)
  SIMPL_STATIC_NEW_MACRO(ComparisonSet)
   SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ComparisonSet, AbstractComparison)

  ~ComparisonSet();

  /**
  * @brief Write ComparisonSet to JSon
  * @param json
  */
  void writeJson(QJsonObject& json);
  /**
  * @brief Read ComparisonSet from JSon
  * @param json
  */
  bool readJson(QJsonObject& json);

  /**
  * @brief Returns whether or not the results from the comparison have their values flipped
  * @return
  */
  bool getInvertComparison();
  /**
  * @brief Return the comparisons contained by the ComparisonSet
  * @return
  */
  QVector<AbstractComparison::Pointer> getComparisons();

  /**
  * @brief Specifies whether or not the results from the comparison should have their values flipped
  * @param invert
  */
  void setInvertComparison(bool invert);
  /**
  * @brief Changes the comparisons used by the ComparisonSet to a new set
  * @param comparisonValues
  */
  void setComparisons(QVector<AbstractComparison::Pointer> comparisonValues);

  /**
  * @brief Adds a new comparison to the end of the ComparisonSet
  * @param comparison
  */
  void addComparison(AbstractComparison::Pointer comparison);
  /**
  * @brief Adds a new comparison to the ComparisonSet at the given index
  * @param index
  * @param comparison
  */
  void insertComparison(int index, AbstractComparison::Pointer comparison);

  /**
  * @brief Returns whether or not the ComparisonSet contains any ComparisonValues
  * @return
  */
  bool hasComparisonValue();
  /**
  * @brief Returns any ComparisonValues contained within the ComparisonSet
  * @return
  */
  QVector<AbstractComparison::Pointer> getComparisonValues();

protected:
  bool m_invertComparison;
  QVector<AbstractComparison::Pointer> m_comparisons;

  ComparisonSet();
};

#endif

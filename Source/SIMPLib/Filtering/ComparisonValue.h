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

#ifndef _comparisonValue_h_
#define _comparisonValue_h_

#include <QtCore/QString>

#include "SIMPLib/Filtering/AbstractComparison.h"
#include "SIMPLib/Filtering/ComparisonSet.h"

/**
* @brief The ComparisonValue class is used to store comparison operators, values, and DataArray names
* to be used for ComparisonInputsAdvanced
*/
class SIMPLib_EXPORT ComparisonValue : public AbstractComparison
{
public:
  SIMPL_SHARED_POINTERS(ComparisonValue)
  SIMPL_STATIC_NEW_MACRO(ComparisonValue)
   SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ComparisonValue, AbstractComparison)

  ~ComparisonValue();

  /**
  * @brief Writes the ComparisonValue to JSon
  * @param json
  */
  void writeJson(QJsonObject& json);
  /**
  * @brief Reads the ComparisonValue from JSon
  * @param json
  */
  bool readJson(QJsonObject& json);

  /**
  * @brief Returns the AttributeArray name
  * @return
  */
  QString getAttributeArrayName();
  /**
  * @brief Returns the comparison operator used
  * @return
  */
  int getCompOperator();
  /**
  * @brief Returns the value used by the ComparisonValue
  */
  double getCompValue();

  /**
  * @brief Sets the AttributeArrayName used
  * @param name
  */
  void setAttributeArrayName(QString name);
  /**
  * @brief Sets the comparison operator used
  * @param compOperator
  */
  void setCompOperator(int compOperator);
  /**
  * @brief Sets the value used by the comparison
  * @param value
  */
  void setCompValue(double value);

  /**
  * @brief Returns the parent ComparisonSet
  * @return
  */
  ComparisonSet::Pointer getParentSet();
  /**
  * @brief Changes the parent ComparisonSet
  * @param parentSet
  */
  void setParentSet(ComparisonSet::Pointer parentSet);

protected:
  QString m_attributeArrayName;
  int m_compOperator;
  double m_compValue;

  ComparisonSet::Pointer m_parentSet;

  ComparisonValue();
};

#endif

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

#pragma once

#include <memory>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>
#include <QtCore/QObject>
#include <QtCore/QVector>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Filtering/AbstractComparison.h"

/**
 * @class ComparisonInputsAdvanced ComparisonInputsAdvanced.h DREAM3DLib/Common/ComparisonInputsAdvanced.h
 * @brief  This class
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Mar 3, 2017
 * @version 1.0
 */
class SIMPLib_EXPORT ComparisonInputsAdvanced : public QObject
{
  Q_OBJECT

public:
  ComparisonInputsAdvanced();
  ComparisonInputsAdvanced(const ComparisonInputsAdvanced& rhs);

  ~ComparisonInputsAdvanced() override;

  /**
   * @brief size
   * @return
   */
  int size();

  /**
    * @brief Adds a new input to the list of comparisons
    * @param unionOperator
    * @param arrayName
    * @param compOperator
    * @param compValue
    */
  void addInput(int unionOperator, const QString arrayName, int compOperator, double compValue);

  /**
   * @brief Adds a new input to the list of comparisons
   * @param unionOperator
   * @param invertComparison
   * @param comparisons
   */
  void addInput(int unionOperator, bool invertComparison, QVector<AbstractComparison::Pointer> comparisons);

  /**
   * @brief Adds new inputs to the list of comparisons
   * @param input
   */
  void addInput(const AbstractComparison::Pointer input);

  /**
   * @brief Returns the nth comparison
   * @param index
   * @return
   */
  AbstractComparison::Pointer getInput(int index);

  /**
   * @brief Returns the vector of comparisons
   * @return
   */
  QVector<AbstractComparison::Pointer>& getInputs();

  /**
   * @brief Sets the comparisons to be used
   * @param comparisons
   */
  void setInputs(QVector<AbstractComparison::Pointer> comparisons);

  /**
   * @brief Returns the DataContainer name used by the group
   * @return
   */
  QString getDataContainerName();

  /**
   * @brief Returns the AttributeMatrix name used by the group
   * @return
   */
  QString getAttributeMatrixName();

  /**
  * @brief Returns the DataArrayPath to the selected AttributeMatrix
  */
  DataArrayPath getAttributeMatrixPath();

  /**
   * @brief Sets the DataContainer name used by the group of comparisons
   * @param dcName
   */
  void setDataContainerName(QString dcName);

  /**
   * @brief Sets the AttributeMatrix name used by the group of comparisons
   * @brief amName
   */
  void setAttributeMatrixName(QString amName);

  /**
   * @brief Assignment operator
   * @param
   */
  void operator=(const ComparisonInputsAdvanced&);

  /**
   * @brief operator overload[]
   * @param index
   * @return
   */
  AbstractComparison::Pointer operator[](int index);

  /**
   * @brief Returns true if there exists at least one comparison value
   * @return
   */
  bool hasComparisonValue();

  /**
   * @brief Returns a vector of comparisons for the group
   * @return
   */
  QVector<AbstractComparison::Pointer> getComparisonValues();

  /**
   * @brief Returns true if the resulting comparison should be inverted
   * @return
   */
  bool shouldInvert();

  /**
   * @brief Sets whether or not the resulting comparison should be inverted
   * @param invert
   */
  void setInvert(bool invert);

  /**
   * @brief Loads data from json 
   * @param obj
   */
  void readJson(QJsonObject obj);

  /**
   * @brief Writes data to json
   * @param obj
   */
  void writeJson(QJsonObject& obj);

private:
  QVector<AbstractComparison::Pointer> m_Inputs;
  QString m_dataContainerName;
  QString m_attributeMatrixName;

  bool m_invert;
};

Q_DECLARE_METATYPE(ComparisonInputsAdvanced)


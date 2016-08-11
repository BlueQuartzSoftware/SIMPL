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

#ifndef _comparisonselectionfilterparameter_h_
#define _comparisonselectionfilterparameter_h_

#include <QtCore/QJsonObject>

#include "SIMPLib/Common/ComparisonInputs.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"

#define SIMPL_NEW_COMP_SEL_FP(...) \
  _FP_GET_OVERRIDE(__VA_ARGS__, \
  SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4)\
  (ComparisonSelectionFilterParameter, __VA_ARGS__)

class SIMPLib_EXPORT ComparisonSelectionFilterParameter : public FilterParameter
{
  public:
    SIMPL_SHARED_POINTERS(ComparisonSelectionFilterParameter)
    SIMPL_STATIC_NEW_MACRO(ComparisonSelectionFilterParameter)
    SIMPL_TYPE_MACRO_SUPER(ComparisonSelectionFilterParameter, FilterParameter)

    typedef std::function<void(ComparisonInputs)> SetterCallbackType;
    typedef std::function<ComparisonInputs(void)> GetterCallbackType;

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       ComparisonInputs defaultValue, Category category, SetterCallbackType setterCallback,
                       GetterCallbackType getterCallback, QVector<QString> choices,
                       bool showOperators, int groupIndex = -1);

    virtual ~ComparisonSelectionFilterParameter();

    SIMPL_INSTANCE_PROPERTY(QVector<QString>, Choices)
    SIMPL_INSTANCE_PROPERTY(bool, ShowOperators)

    SIMPL_INSTANCE_PROPERTY(QVector<unsigned int>, DefaultGeometryTypes)
    SIMPL_INSTANCE_PROPERTY(QVector<unsigned int>, DefaultAttributeMatrixTypes)

    /**
   * @brief getWidgetType Returns the type of widget that displays and controls
   * this FilterParameter subclass
   * @return
   */
    QString getWidgetType();

    /**
     * @brief readJson
     * @return
     */
    void readJson(const QJsonObject &json);

    /**
     * @brief writeJson
     * @return
     */
    void writeJson(QJsonObject &json);

    /**
    * @param SetterCallback The method in the AbstractFilter subclass that <i>sets</i> the value of the property
    * that this FilterParameter subclass represents.
    * from the filter parameter.
    */
    SIMPL_INSTANCE_PROPERTY(SetterCallbackType, SetterCallback)

    /**
    * @param GetterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
    * that this FilterParameter subclass represents.
    * @return The GetterCallback
    */
    SIMPL_INSTANCE_PROPERTY(GetterCallbackType, GetterCallback)

  protected:
    ComparisonSelectionFilterParameter();

  private:
    ComparisonSelectionFilterParameter(const ComparisonSelectionFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const ComparisonSelectionFilterParameter&); // Operator '=' Not Implemented
};

#endif /* _ComparisonSelectionFilterParameter_H_ */

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

#ifndef _fourthorderpolynomialfilterparameter_h_
#define _fourthorderpolynomialfilterparameter_h_

#include <QtCore/QJsonObject>

#include "SIMPLib/FilterParameters/FilterParameter.h"

typedef struct
{
  float c40; float c04; 
  float c31; float c13; 
  float c22; 
  float c30; float c03; 
  float c21; float c12; float c20; float c02; 
  float c11; float c10; float c01; float c00;
  
  void Float4thOrderPoly(const float& C40, const float& C04, 
    const float& C31, const float& C13,
    const float& C22,
    const float& C30, const float& C03,
    const float& C21, const float& C12, const float& C20, const float& C02,
    const float& C11, const float& C10, const float& C01, const float& C00
  )
  {
  c40 = C40;
  c04 = C04;
  c31 = C31;
  c13 = C13;
  c22 = C22;
  c30 = C30;
  c03 = C03;
  c21 = C21;
  c12 = C12;
  c20 = C20;
  c02 = C02;
  c11 = C11;
  c10 = C10;
  c01 = C01;
  c00 = C00;
  }
  
  void writeJson(QJsonObject &json)
  {
    json["c40"] = static_cast<double>(c40);
    json["c04"] = static_cast<double>(c04);
    json["c31"] = static_cast<double>(c31);
    json["c13"] = static_cast<double>(c13);
    json["c22"] = static_cast<double>(c22);
    json["c30"] = static_cast<double>(c30);
    json["c03"] = static_cast<double>(c03);
    json["c21"] = static_cast<double>(c21);
    json["c12"] = static_cast<double>(c12);
    json["c20"] = static_cast<double>(c20);
    json["c02"] = static_cast<double>(c02);
    json["c11"] = static_cast<double>(c11);
    json["c10"] = static_cast<double>(c10);
    json["c01"] = static_cast<double>(c01);
    json["c00"] = static_cast<double>(c00);
  }

  bool readJson(QJsonObject &json)
  {
    if (json["c40"].isDouble() && json["c04"].isDouble() && json["c31"].isDouble() && json["c13"].isDouble()
      && json["c22"].isDouble() && json["c30"].isDouble() && json["c03"].isDouble() && json["c21"].isDouble()
      && json["c12"].isDouble() && json["c20"].isDouble() && json["c02"].isDouble() && json["c11"].isDouble()
      && json["c10"].isDouble() && json["c01"].isDouble() && json["c00"].isDouble())
    {
      c40 = static_cast<float>(json["c40"].toDouble());
      c04 = static_cast<float>(json["c04"].toDouble());
      c31 = static_cast<float>(json["c31"].toDouble());
      c13 = static_cast<float>(json["c13"].toDouble());
      c22 = static_cast<float>(json["c22"].toDouble());
      c30 = static_cast<float>(json["c30"].toDouble());
      c03 = static_cast<float>(json["c03"].toDouble());
      c21 = static_cast<float>(json["c21"].toDouble());
      c12 = static_cast<float>(json["c12"].toDouble());
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
} Float4thOrderPoly_t;

Q_DECLARE_METATYPE(Float4thOrderPoly_t)

/**
 * @brief SIMPL_NEW_FourthO_POLY_FP This macro is a short-form way of instantiating an instance of
 * FourthOrderPolynomialFilterParameter. There are 4 required parameters and 1 optional parameter
 * that are always passed to this macro in the following order: HumanLabel, PropertyName, Category,
 * FilterName (class name), GroupIndex (optional).
 *
 * Therefore, the macro should be written like this (this is a concrete example):
 * SIMPL_NEW_FourthO_POLY_FP("HumanLabel", PropertyName, Category, FilterName, GroupIndex)
 *
 * Example 1 (instantiated within a filter called [GenericExample](@ref genericexample), with optional GroupIndex parameter):
 * SIMPL_NEW_FourthO_POLY_FP("Fourth Order A Coefficients", FourthOrderACoeff, FilterParameter::Parameter, GenericExample, 2);
 */
#define SIMPL_NEW_FourthO_POLY_FP(...) \
  SIMPL_EXPAND(_FP_GET_OVERRIDE(__VA_ARGS__, \
  SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4)\
  (FourthOrderPolynomialFilterParameter, __VA_ARGS__))

/**
 * @brief The FourthOrderPolynomialFilterParameter class is used by filters to instantiate an FourthOrderPolynomialWidget.  By instantiating an instance of
 * this class in a filter's setupFilterParameters() method, a FourthOrderPolynomialWidget will appear in the filter's "filter input" section in the DREAM3D GUI.
 */
class SIMPLib_EXPORT FourthOrderPolynomialFilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(FourthOrderPolynomialFilterParameter)
    SIMPL_STATIC_NEW_MACRO(FourthOrderPolynomialFilterParameter)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FourthOrderPolynomialFilterParameter, FilterParameter)

    using SetterCallbackType = std::function<void(Float4thOrderPoly_t)>;
    using GetterCallbackType = std::function<Float4thOrderPoly_t(void)>;

    /**
     * @brief New This function instantiates an instance of the FourthOrderPolynomialFilterParameter. Although this function is available to be used,
     * the preferable way to instantiate an instance of this class is to use the SIMPL_NEW_FourthO_POLY_FP(...) macro at the top of this file.

     * @param humanLabel The name that the users of DREAM.3D see for this filter parameter
     * @param propertyName The internal property name for this filter parameter.
     * @param defaultValue The value that this filter parameter will be initialized to by default.
     * @param category The category for the filter parameter in the DREAM.3D user interface.  There
     * are three categories: Parameter, Required Arrays, and Created Arrays.
     * @param setterCallback The method in the AbstractFilter subclass that <i>sets</i> the value of the property
    * that this FilterParameter subclass represents.
     * @param getterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
    * that this FilterParameter subclass represents.
     * @param groupIndex Integer that specifies the group that this filter parameter will be placed in.
     * @return
     */
    static Pointer New(const QString& humanLabel, const QString& propertyName,
    const Float4thOrderPoly_t& defaultValue, Category category, SetterCallbackType setterCallback,
    GetterCallbackType getterCallback, int groupIndex = -1);

    virtual ~FourthOrderPolynomialFilterParameter();

  /**
   * @brief getWidgetType Returns the type of widget that displays and controls
   * this FilterParameter subclass
   * @return
   */
    QString getWidgetType() const override;

    /**
     * @brief readJson Reads this filter parameter's corresponding property out of a QJsonObject.
     * @param json The QJsonObject that the filter parameter reads from.
     */
    void readJson(const QJsonObject& json) override;

    /**
     * @brief writeJson Writes this filter parameter's corresponding property to a QJsonObject.
     * @param json The QJsonObject that the filter parameter writes to.
     */
    void writeJson(QJsonObject& json) override;

    /**
    * @param SetterCallback The method in the AbstractFilter subclass that <i>sets</i> the value of the property
    * that this FilterParameter subclass represents.
    * @return The SetterCallback
    */
    SIMPL_INSTANCE_PROPERTY(SetterCallbackType, SetterCallback)

    /**
    * @param GetterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
    * that this FilterParameter subclass represents.
    * @return The GetterCallback
    */
    SIMPL_INSTANCE_PROPERTY(GetterCallbackType, GetterCallback)

  protected:
    /**
     * @brief FourthOrderPolynomialFilterParameter The default constructor.  It is protected because this
     * filter parameter should only be instantiated using its New(...) function or short-form macro.
     */
  FourthOrderPolynomialFilterParameter();

private:
  FourthOrderPolynomialFilterParameter(const FourthOrderPolynomialFilterParameter&) = delete; // Copy Constructor Not Implemented
  void operator=(const FourthOrderPolynomialFilterParameter&) = delete;                       // Move assignment Not Implemented
};

#endif /* _FourthOrderPolynomialFilterParameter_H_ */

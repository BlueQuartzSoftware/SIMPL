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

#include <cmath>

#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"

typedef struct {
  float x; float y; float z;
  void FloatVec3(const float& xx, const float& yy, const float& zz)
  {
    x = xx;
    y = yy;
    z = zz;
  }

  void writeJson(QJsonObject &json)
  {
    json["x"] = static_cast<double>(x);
    json["y"] = static_cast<double>(y);
    json["z"] = static_cast<double>(z);
  }

  bool readJson(QJsonObject &json)
  {
    if (json["x"].isDouble() && json["y"].isDouble() && json["z"].isDouble())
    {
      x = static_cast<float>(json["x"].toDouble());
      y = static_cast<float>(json["y"].toDouble());
      z = static_cast<float>(json["z"].toDouble());
      return true;
    }
    return false;
  }
  
  void normalize() 
  {
    float denom = std::sqrt(x*x + y*y + z*z);
    x = x / denom;
    y = y / denom;
    z = z / denom;
  }
} FloatVec3_t;

Q_DECLARE_METATYPE(FloatVec3_t)

/**
 * @brief SIMPL_NEW_FLOAT_VEC3_FP This macro is a short-form way of instantiating an instance of
 * FloatVec3FilterParameter. There are 4 required parameters and 1 optional parameter
 * that are always passed to this macro in the following order: HumanLabel, PropertyName, Category,
 * FilterName (class name), GroupIndex (optional).
 *
 * Therefore, the macro should be written like this (this is a concrete example):
 * SIMPL_NEW_FLOAT_VEC3_FP("HumanLabel", PropertyName, Category, FilterName, GroupIndex)
 *
 * Example 1 (instantiated within a filter called [GenericExample](@ref genericexample), with optional GroupIndex parameter):
 * SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, GenericExample, 1);
 */
#define SIMPL_NEW_FLOAT_VEC3_FP(...) \
  SIMPL_EXPAND(_FP_GET_OVERRIDE(__VA_ARGS__, \
  SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4)\
  (FloatVec3FilterParameter, __VA_ARGS__))

/**
 * @brief The FloatVec3FilterParameter class is used by filters to instantiate an FloatVec3Widget.  By instantiating an instance of
 * this class in a filter's setupFilterParameters() method, a FloatVec3Widget will appear in the filter's "filter input" section in the DREAM3D GUI.
 */
class SIMPLib_EXPORT FloatVec3FilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(FloatVec3FilterParameter)
    SIMPL_STATIC_NEW_MACRO(FloatVec3FilterParameter)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FloatVec3FilterParameter, FilterParameter)

    using SetterCallbackType = std::function<void(FloatVec3_t)>;
    using GetterCallbackType = std::function<FloatVec3_t(void)>;

    /**
     * @brief New This function instantiates an instance of the FloatVec3FilterParameter. Although this function is available to be used,
     * the preferable way to instantiate an instance of this class is to use the SIMPL_NEW_FLOAT_VEC3_FP(...) macro at the top of this file.

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
    const FloatVec3_t& defaultValue, Category category, SetterCallbackType setterCallback,
    GetterCallbackType getterCallback, int groupIndex = -1);

    ~FloatVec3FilterParameter() override;

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
     * @brief FloatVec3FilterParameter The default constructor.  It is protected because this
     * filter parameter should only be instantiated using its New(...) function or short-form macro.
     */
  FloatVec3FilterParameter();

public:
  FloatVec3FilterParameter(const FloatVec3FilterParameter&) = delete; // Copy Constructor Not Implemented
  FloatVec3FilterParameter(FloatVec3FilterParameter&&) = delete;      // Move Constructor Not Implemented
  FloatVec3FilterParameter& operator=(const FloatVec3FilterParameter&) = delete; // Copy Assignment Not Implemented
  FloatVec3FilterParameter& operator=(FloatVec3FilterParameter&&) = delete;      // Move Assignment Not Implemented
};


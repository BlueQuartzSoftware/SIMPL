/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include <QtCore/QJsonObject>

#include "SIMPLib/FilterParameters/FilterParameter.h"

/**
 * @brief SIMPL_NEW_PARAGRAPH_FP This macro is a short-form way of instantiating an instance of
 * ParagraphFilterParameter. There are 4 required parameters and 1 optional parameter
 * that are always passed to this macro in the following order: HumanLabel, PropertyName, Category,
 * FilterName (class name), GroupIndex (optional).
 *
 * Therefore, the macro should be written like this (this is a concrete example):
 * SIMPL_NEW_PARAGRAPH_FP("HumanLabel", PropertyName, Category, FilterName, GroupIndex)
 *
 * Example 1 (instantiated within a filter called [GenericExample](@ref genericexample), without optional GroupIndex parameter):
 * SIMPL_NEW_PARAGRAPH_FP("STL Output Prefix", StlFilePrefix, FilterParameter::Category::Parameter, GenericExample);
 */
#define SIMPL_NEW_PARAGRAPH_FP(...)                                                                                                                                                                    \
  SIMPL_EXPAND(_FP_GET_OVERRIDE(__VA_ARGS__, SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4)(ParagraphFilterParameter, __VA_ARGS__))

/**
 * @brief The ParagraphFilterParameter class is used by filters to instantiate an StringWidget.  By instantiating an instance of
 * this class in a filter's setupFilterParameters() method, a ParagraphWidget will appear in the filter's "filter input" section in the DREAM3D GUI.
 */
class SIMPLib_EXPORT ParagraphFilterParameter : public FilterParameter
{
  // Start Python bindings declarations
  // clang-format off
  PYB11_BEGIN_BINDINGS(ParagraphFilterParameter SUPERCLASS FilterParameter)
  PYB11_SHARED_POINTERS(ParagraphFilterParameter)
  PYB11_STATIC_CREATION(Create)
  PYB11_END_BINDINGS()
  // clang-format on
  // End Python bindings declarations
public:
  using Self = ParagraphFilterParameter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for ParagraphFilterParameter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ParagraphFilterParameter
   */
  static QString ClassName();

  typedef std::function<void(QString)> SetterCallbackType;
  typedef std::function<QString(void)> GetterCallbackType;

  /**
   * @brief New This function instantiates an instance of the ParagraphFilterParameter. Although this function is available to be used,
   * the preferable way to instantiate an instance of this class is to use the SIMPL_NEW_STRING_FP(...) macro at the top of this file.

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
  static Pointer Create(const QString& humanLabel, const QString& propertyName, const QString& defaultValue, Category category, const SetterCallbackType& setterCallback,
                        const GetterCallbackType& getterCallback, bool allowPreflight = true, const std::vector<int>& groupIndices = {});

  ~ParagraphFilterParameter() override;

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
  void writeJson(QJsonObject& json) const override;

  /**
   * @brief SetterCallback The method in the AbstractFilter subclass that <i>sets</i> the value of the property
   * that this FilterParameter subclass represents.
   * from the filter parameter.
   */
  /**
   * @brief Setter property for SetterCallback
   */
  void setSetterCallback(const ParagraphFilterParameter::SetterCallbackType& value);
  /**
   * @brief Getter property for SetterCallback
   * @return Value of SetterCallback
   */
  ParagraphFilterParameter::SetterCallbackType getSetterCallback() const;

  /**
   * @brief GetterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
   * that this FilterParameter subclass represents.
   * @return The GetterCallback
   */
  /**
   * @brief Setter property for GetterCallback
   */
  void setGetterCallback(const ParagraphFilterParameter::GetterCallbackType& value);
  /**
   * @brief Getter property for GetterCallback
   * @return Value of GetterCallback
   */
  ParagraphFilterParameter::GetterCallbackType getGetterCallback() const;

  /**
   * @brief Setter property for AllowPreflight
   */
  void setAllowPreflight(bool value);
  /**
   * @brief Getter property for AllowPreflight
   * @return Value of AllowPreflight
   */
  bool getAllowPreflight() const;

protected:
  /**
   * @brief ParagraphFilterParameter The default constructor.  It is protected because this
   * filter parameter should only be instantiated using its New(...) function or short-form macro.
   */
  ParagraphFilterParameter();

public:
  ParagraphFilterParameter(const ParagraphFilterParameter&) = delete;            // Copy Constructor Not Implemented
  ParagraphFilterParameter(ParagraphFilterParameter&&) = delete;                 // Move Constructor Not Implemented
  ParagraphFilterParameter& operator=(const ParagraphFilterParameter&) = delete; // Copy Assignment Not Implemented
  ParagraphFilterParameter& operator=(ParagraphFilterParameter&&) = delete;      // Move Assignment Not Implemented

private:
  ParagraphFilterParameter::SetterCallbackType m_SetterCallback = {};
  ParagraphFilterParameter::GetterCallbackType m_GetterCallback = {};
  bool m_AllowPreflight = {};
};

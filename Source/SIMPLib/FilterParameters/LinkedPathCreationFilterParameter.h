/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
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

#include <QtCore/QJsonObject>

#include "SIMPLib/FilterParameters/FilterParameter.h"

#define SIMPL_NEW_LINKED_DC_STRING(FILTER, PTR, DC_PROP) new LinkedPathCreationFilterParameter::LinkedStringPath(SIMPL_BIND_GETTER(FILTER, PTR, DC_PROP))
#define SIMPL_NEW_LINKED_AM_STRING(FILTER, PTR, DC_PROP, AM_PROP)                                                                                                                                      \
  new LinkedPathCreationFilterParameter::LinkedStringPath(SIMPL_BIND_GETTER(FILTER, PTR, DC_PROP), SIMPL_BIND_GETTER(FILTER, PTR, AM_PROP))

#define SIMPL_NEW_LINKED_DATA_PATH(FILTER, PTR, PATH_PROP) new LinkedPathCreationFilterParameter::LinkedDataPath(SIMPL_BIND_GETTER(FILTER, PTR, PATH_PROP))

/**
 * @brief SIMPL_NEW_STRING_FP This macro is a short-form way of instantiating an instance of
 * LinkedPathCreationFilterParameter. There are 4 required parameters and 1 optional parameter
 * that are always passed to this macro in the following order: HumanLabel, PropertyName, Category,
 * FilterName (class name), GroupIndex (optional).
 *
 * Therefore, the macro should be written like this (this is a concrete example):
 * SIMPL_NEW_STRING_FP("HumanLabel", PropertyName, Category, FilterName, GroupIndex)
 *
 * Example 1 (instantiated within a filter called [GenericExample](@ref genericexample), without optional GroupIndex parameter):
 * SIMPL_NEW_STRING_FP("STL Output Prefix", StlFilePrefix, FilterParameter::Parameter, GenericExample);
 */

#define SIMPL_NEW_AM_WITH_LINKED_DC_NoIndex(Desc, Prop, LinkedDcProp, Category, Filter)                                                                                                                \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_DC_STRING(Filter, this, LinkedDcProp))
#define SIMPL_NEW_AM_WITH_LINKED_DC_Index(Desc, Prop, LinkedDcProp, Category, Filter, Index)                                                                                                           \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_DC_STRING(Filter, this, LinkedDcProp, Index))

#define SIMPL_NEW_DA_WITH_LINKED_AM_NoIndex(Desc, Prop, LinkedDcProp, LinkedAmProp, Category, Class)                                                                                                   \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_AM_STRING(Class, this, LinkedDcProp, LinkedAmProp))
#define SIMPL_NEW_DA_WITH_LINKED_AM_Index(Desc, Prop, LinkedDcProp, LinkedAmProp, Category, Class, Index)                                                                                              \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_AM_STRING(Class, this, LinkedDcProp, LinkedAmProp, Index))

#define SIMPL_NEW_PATH_WITH_LINKED_PARENT_NoIndex(Desc, Prop, LinkedPathProp, Category, Class)                                                                                                         \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_DATA_PATH(Class, this, LinkedPathProp))
#define SIMPL_NEW_PATH_WITH_LINKED_PARENT_Index(Desc, Prop, LinkedPathProp, Category, Class, Index)                                                                                                    \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_DATA_PATH(Class, this, LinkedPathProp, Index))

#define _FP_GET_OVERRIDE6(A, B, C, D, E, F, NAME, ...) NAME
#define _FP_GET_OVERRIDE7(A, B, C, D, E, F, G, NAME, ...) NAME

#define SIMPL_NEW_AM_WITH_LINKED_DC(...) SIMPL_EXPAND(_FP_GET_OVERRIDE6(__VA_ARGS__, SIMPL_NEW_AM_WITH_LINKED_DC_Index, SIMPL_NEW_AM_WITH_LINKED_DC_NoIndex)(__VA_ARGS__))
#define SIMPL_NEW_DA_WITH_LINKED_AM(...) SIMPL_EXPAND(_FP_GET_OVERRIDE7(__VA_ARGS__, SIMPL_NEW_DA_WITH_LINKED_AM_Index, SIMPL_NEW_DA_WITH_LINKED_AM_NoIndex)(__VA_ARGS__))
#define SIMPL_NEW_PATH_WITH_LINKED_PARENT(...) SIMPL_EXPAND(_FP_GET_OVERRIDE6(__VA_ARGS__, SIMPL_NEW_PATH_WITH_LINKED_PARENT_Index, SIMPL_NEW_PATH_WITH_LINKED_PARENT_NoIndex)(__VA_ARGS__))

/**
 * @brief The LinkedPathCreationFilterParameter class is used by filters to instantiate an StringWidget.  By instantiating an instance of
 * this class in a filter's setupFilterParameters() method, a StringWidget will appear in the filter's "filter input" section in the DREAM3D GUI.
 */
class SIMPLib_EXPORT LinkedPathCreationFilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(LinkedPathCreationFilterParameter)
  SIMPL_STATIC_NEW_MACRO(LinkedPathCreationFilterParameter)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(LinkedPathCreationFilterParameter, FilterParameter)

  using SetterCallbackType = std::function<void(QString)>;
  using GetterCallbackType = std::function<QString(void)>;

  /**
   * @brief The ILinkedPath interface is used for generating the linked path from either multiple string properties or a DataArrayPath property
   */
  class SIMPLib_EXPORT ILinkedPath
  {
  public:
    /**
     * @brief Generates a DataArrayPath to the target container
     * @return
     */
    virtual DataArrayPath generatePath() = 0;
  };

  /**
   * @brief The LinkedStringPath struct stores getters for the DataContainer and AttributeMatrix string properties.
   * These getters are used to generate a DataArrayPath to the target container.
   */
  class SIMPLib_EXPORT LinkedStringPath : public ILinkedPath
  {
  public:
    using GetterCallbackType = std::function<QString(void)>;
    GetterCallbackType dcGetter;
    GetterCallbackType amGetter;

    LinkedStringPath(GetterCallbackType getDc)
    : dcGetter(getDc)
    {
    }
    LinkedStringPath(GetterCallbackType getDc, GetterCallbackType getAm)
    : dcGetter(getDc)
    , amGetter(getAm)
    {
    }

    /**
     * @brief Generates a DataArrayPath to the target container
     * @return
     */
    DataArrayPath generatePath() override;
  };
  /**
   * @brief The LinkedDataPath struct stores a getter for the target DataArrayPath.
   */
  class SIMPLib_EXPORT LinkedDataPath : public ILinkedPath
  {
  public:
    using GetterCallbackType = std::function<DataArrayPath(void)>;
    GetterCallbackType pathGetter;

    LinkedDataPath(GetterCallbackType getPath)
    : pathGetter(getPath)
    {
    }

    /**
     * @brief Generates a DataArrayPath to the target container
     * @return
     */
    DataArrayPath generatePath() override;
  };

  /**
   * @brief New This function instantiates an instance of the LinkedPathCreationFilterParameter. Although this function is available to be used,
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
  static Pointer New(const QString& humanLabel, const QString& propertyName, const QString& defaultValue, Category category, SetterCallbackType setterCallback, GetterCallbackType getterCallback,
                     ILinkedPath* linkedPath, int groupIndex = -1);

  ~LinkedPathCreationFilterParameter() override;

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
   * from the filter parameter.
   */
  SIMPL_INSTANCE_PROPERTY(SetterCallbackType, SetterCallback)

  /**
   * @param GetterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
   * that this FilterParameter subclass represents.
   * @return The GetterCallback
   */
  SIMPL_INSTANCE_PROPERTY(GetterCallbackType, GetterCallback)

  void setLinkedPath(ILinkedPath* linkedPath);

  DataArrayPath getLinkedDataArrayPath() const;

protected:
  /**
   * @brief LinkedPathCreationFilterParameter The default constructor.  It is protected because this
   * filter parameter should only be instantiated using its New(...) function or short-form macro.
   */
  LinkedPathCreationFilterParameter();

private:
  ILinkedPath* m_LinkedPath = nullptr;

public:
  LinkedPathCreationFilterParameter(const LinkedPathCreationFilterParameter&) = delete;            // Copy Constructor Not Implemented
  LinkedPathCreationFilterParameter(LinkedPathCreationFilterParameter&&) = delete;                 // Move Constructor Not Implemented
  LinkedPathCreationFilterParameter& operator=(const LinkedPathCreationFilterParameter&) = delete; // Copy Assignment Not Implemented
  LinkedPathCreationFilterParameter& operator=(LinkedPathCreationFilterParameter&&) = delete;      // Move Assignment Not Implemented
};

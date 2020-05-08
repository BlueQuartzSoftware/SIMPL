/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5280
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include <QtCore/QJsonObject>

#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Filtering/ComparisonInputs.h"
#include "SIMPLib/Filtering/ComparisonInputsAdvanced.h"

// Helper macros: LinkedPath
#define SIMPL_NEW_LINKED_DC_PATH(FILTER, PTR, LINKED_PROP) LinkedPathCreationFilterParameter::CreateLinkedPath(SIMPL_BIND_GETTER(FILTER, PTR, LINKED_PROP))
#define SIMPL_NEW_LINKED_AM_PATH(FILTER, PTR, DC_PROP, AM_PROP) LinkedPathCreationFilterParameter::CreateLinkedPath(SIMPL_BIND_GETTER(FILTER, PTR, DC_PROP), SIMPL_BIND_GETTER(FILTER, PTR, AM_PROP))
#define SIMPL_NEW_LINKED_ADV_COMPARE(FILTER, PTR, COMP_PROP) LinkedPathCreationFilterParameter::CreateLinkedPath(SIMPL_BIND_GETTER(FILTER, PTR, COMP_PROP))

// Helper macros: Index / NoIndex implementations
#define SIMPL_NEW_DA_FROM_ADV_COMPARISON_NoIndex(Desc, Prop, LinkedComp, Category, Filter)                                                                                                             \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_ADV_COMPARE(Filter, this, LinkedComp))
#define SIMPL_NEW_DA_FROM_ADV_COMPARISON_Index(Desc, Prop, LinkedComp, Category, Filter, Index)                                                                                                        \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_ADV_COMPARE(Filter, this, LinkedComp), Index)

#define SIMPL_NEW_LINKED_ATTRIBUTE_MATRIX_Index(Desc, Prop, LinkedDcProp, LinkedAmProp, Category, Filter, Index)                                                                                       \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_AM_PATH(Filter, this, LinkedDcProp, LinkedAmProp), Index)
#define SIMPL_NEW_LINKED_ATTRIBUTE_MATRIX_NoIndex(Desc, Prop, LinkedDcProp, LinkedAmProp, Category, Filter)                                                                                            \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_AM_PATH(Filter, this, LinkedDcProp, LinkedAmProp))
#define SIMPL_NEW_LINKED_DATA_CONTAINER_Index(Desc, Prop, LinkedProp, Category, Filter, Index)                                                                                                         \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_DC_PATH(Filter, this, LinkedProp), Index)
#define SIMPL_NEW_LINKED_DATA_CONTAINER_NoIndex(Desc, Prop, LinkedProp, Category, Filter)                                                                                                              \
  LinkedPathCreationFilterParameter::New(Desc, #Prop, get##Prop(), Category, SIMPL_BIND_SETTER(Filter, this, Prop), SIMPL_BIND_GETTER(Filter, this, Prop),                                             \
                                         SIMPL_NEW_LINKED_DC_PATH(Filter, this, LinkedProp))

// Helper macros: Expansion
#define _FP_GET_OVERRIDE6(A, B, C, D, E, F, NAME, ...) NAME
#define _FP_GET_OVERRIDE7(A, B, C, D, E, F, G, NAME, ...) NAME

// Creation Macros
/**
 * @brief SIMPL_NEW_AM_WITH_LINKED_DC_FP This macro is a short-form way of instantiating an instance of
 * LinkedPathCreationFilterParameter. There are 5 required parameters and 1 optional parameter
 * that are passed to this macro in the following order: HumanLabel, PropertyName, LinkedDcProperty (DataArrayPath or name),
 * Category, FilterName (class name), GroupIndex (optional)
 *
 * Therefore, the macro should be written like this (this is a concrete example):
 * SIMPL_NEW_AM_WITH_LINKED_DC_FP("HumanLabel", PropertyName, DcName, Category, FilterName, GroupIndex)
 *
 * Example 1 (instantiated within a filter called [GenericExample](@ref genericexample), without optional GroupIndex parameter):
 * SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Matrix", CellMatrixName, DcName, FilterParameter::Parameter, GenericExample);
 */
#define SIMPL_NEW_AM_WITH_LINKED_DC_FP(...) SIMPL_EXPAND(_FP_GET_OVERRIDE6(__VA_ARGS__, SIMPL_NEW_LINKED_DATA_CONTAINER_Index, SIMPL_NEW_LINKED_DATA_CONTAINER_NoIndex)(__VA_ARGS__))

/**
 * @brief SIMPL_NEW_DA_WITH_LINKED_AM_FP This macro is a short-form way of instantiating an instance of
 * LinkedPathCreationFilterParameter. There are 6 required parameters and 1 optional parameter
 * that are passed to this macro in the following order: HumanLabel, PropertyName, LinkedDcProperty (DataArrayPath or name),
 * LinkedAmProperty, Category, FilterName (class name), GroupIndex (optional)
 *
 * Therefore, the macro should be written like this (this is a concrete example):
 * SIMPL_NEW_DA_WITH_LINKED_AM_FP("HumanLabel", PropertyName, DcName, AmName, Category, FilterName, GroupIndex)
 *
 * Example 1 (instantiated within a filter called [GenericExample](@ref genericexample), without optional GroupIndex parameter):
 * SIMPL_NEW_DA_WITH_LINKED_AM_FP("Feature Id", CellMatrixName, DcName, AmName, FilterParameter::Parameter, GenericExample);
 */
#define SIMPL_NEW_DA_WITH_LINKED_AM_FP(...) SIMPL_EXPAND(_FP_GET_OVERRIDE7(__VA_ARGS__, SIMPL_NEW_LINKED_ATTRIBUTE_MATRIX_Index, SIMPL_NEW_LINKED_ATTRIBUTE_MATRIX_NoIndex)(__VA_ARGS__))

// Special cases
/**
 * @brief SIMPL_NEW_DA_FROM_ADV_COMPARISON_FP This macro is a short-form way of instantiating an instance of
 * LinkedPathCreationFilterParameter. There are 5 required parameters and 1 optional parameter
 * that are passed to this macro in the following order: HumanLabel, PropertyName, LinkedProperty, Category,
 * FilterName (class name), GroupIndex (optional)
 *
 * Therefore, the macro should be written like this (this is a concrete example):
 * SIMPL_NEW_DA_FROM_ADV_COMPARISON_FP("HumanLabel", PropertyName, LinkedAdvComparisonProperty, Category, FilterName, GroupIndex)
 *
 * Example 1 (instantiated within a filter called [GenericExample](@ref genericexample), without optional GroupIndex parameter):
 * SIMPL_NEW_DA_FROM_ADV_COMPARISON_FP("Mask", MaskName, AdvComparison, FilterParameter::Parameter, GenericExample);
 */
#define SIMPL_NEW_DA_FROM_ADV_COMPARISON_FP(...) SIMPL_EXPAND(_FP_GET_OVERRIDE6(__VA_ARGS__, SIMPL_NEW_DA_FROM_ADV_COMPARISON_Index, SIMPL_NEW_DA_FROM_ADV_COMPARISON_NoIndex)(__VA_ARGS__))

/**
 * @brief The LinkedPathCreationFilterParameter class is used by filters to instantiate an StringWidget.  By instantiating an instance of
 * this class in a filter's setupFilterParameters() method, a StringWidget will appear in the filter's "filter input" section in the DREAM3D GUI.
 */
class SIMPLib_EXPORT LinkedPathCreationFilterParameter : public FilterParameter
{
public:
  using Self = LinkedPathCreationFilterParameter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for LinkedPathCreationFilterParameter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for LinkedPathCreationFilterParameter
   */
  static QString ClassName();

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

    virtual ~ILinkedPath() = default;
  };

  /**
   * @brief The LinkedStringPath struct stores getters for the DataContainer and AttributeMatrix string properties.
   * These getters are used to generate a DataArrayPath to the target container.
   */
  class SIMPLib_EXPORT LinkedStringPath : public ILinkedPath
  {
  public:
    using GetterCallbackType = std::function<QString(void)>;
    LinkedStringPath(GetterCallbackType getDc);
    LinkedStringPath(GetterCallbackType getDc, GetterCallbackType getAm);

    /**
     * @brief Generates a DataArrayPath to the target container
     * @return
     */
    DataArrayPath generatePath() override;

  private:
    GetterCallbackType dcGetter;
    GetterCallbackType amGetter;
  };
  /**
   * @brief The LinkedStringPath struct stores getters for the DataContainer and AttributeMatrix string properties.
   * These getters are used to generate a DataArrayPath to the target container.
   */
  class SIMPLib_EXPORT LinkedMixedPath : public ILinkedPath
  {
  public:
    using StringGetterCallbackType = std::function<QString(void)>;
    using PathGetterCallbackType = std::function<DataArrayPath(void)>;
    LinkedMixedPath(PathGetterCallbackType dc, StringGetterCallbackType am);
    LinkedMixedPath(PathGetterCallbackType dc, PathGetterCallbackType am);

    /**
     * @brief Generates a DataArrayPath to the target container
     * @return
     */
    DataArrayPath generatePath() override;

  private:
    PathGetterCallbackType dcGetter;
    StringGetterCallbackType amGetter;
    PathGetterCallbackType amPathGetter;
  };
  /**
   * @brief The LinkedDataPath struct stores a getter for the target DataArrayPath.
   */
  class SIMPLib_EXPORT LinkedDataPath : public ILinkedPath
  {
  public:
    using GetterCallbackType = std::function<DataArrayPath(void)>;
    LinkedDataPath(GetterCallbackType getPath);
    LinkedDataPath(GetterCallbackType getPath, DataArrayPathHelper::DataType targetLevel);

    /**
     * @brief Generates a DataArrayPath to the target container
     * @return
     */
    DataArrayPath generatePath() override;

  private:
    GetterCallbackType pathGetter;
    DataArrayPathHelper::DataType m_TargetPathType = DataArrayPathHelper::DataType::None;
  };
  /**
   * @brief The LinkedAdvComparisonPath stores a getter for the target ComparisonInputsAdvanced DataArrayPath
   */
  class SIMPLib_EXPORT LinkedAdvComparisonPath : public ILinkedPath
  {
  public:
    using GetterCallbackType = std::function<ComparisonInputsAdvanced(void)>;
    LinkedAdvComparisonPath(GetterCallbackType comparison);

    /**
     * @brief Generates a DataArrayPath to the target container
     * @return
     */
    DataArrayPath generatePath() override;

  private:
    GetterCallbackType compGetter;
  };

  /**
   * @brief Create and return an ILinkedPath based on the given Parameters
   * @param DataArrayPath getter
   * @param DataType
   * @return
   */
  static ILinkedPath* CreateLinkedPath(LinkedDataPath::GetterCallbackType, DataArrayPathHelper::DataType);

  /**
   * @brief Create and return an ILinkedPath based on the given Parameters
   * @param QString getter
   * @return
   */
  static ILinkedPath* CreateLinkedPath(LinkedStringPath::GetterCallbackType);

  /**
   * @brief Create and return an ILinkedPath based on the given Parameters
   * @param QString getter
   * @param QString getter
   * @return
   */
  static ILinkedPath* CreateLinkedPath(LinkedStringPath::GetterCallbackType, LinkedStringPath::GetterCallbackType);

  /**
   * @brief Create and return an ILinkedPath based on the given Parameters
   * @param DataArrayPath getter
   * @param DataArrayPath getter
   * @return
   */
  static ILinkedPath* CreateLinkedPath(LinkedMixedPath::PathGetterCallbackType, LinkedMixedPath::PathGetterCallbackType);

  /**
   * @brief Create and return an ILinkedPath based on the given Parameters
   * @param DataArrayPath getter
   * @param QString getter
   * @return
   */
  static ILinkedPath* CreateLinkedPath(LinkedMixedPath::PathGetterCallbackType, LinkedMixedPath::StringGetterCallbackType);

  /**
   * @brief Create and return an ILinkedPath based on the given Parameters
   * @param DataArrayPath dcGetter
   * @return
   */
  static ILinkedPath* CreateLinkedPath(LinkedMixedPath::PathGetterCallbackType dcGetter);

  /**
   * @brief Create and return an ILinkedPath based on the given Parameters
   * @param AdvancedComparison getter
   * @return
   */
  static ILinkedPath* CreateLinkedPath(LinkedAdvComparisonPath::GetterCallbackType);

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
  static Pointer New(const QString& humanLabel, const QString& propertyName, const QString& defaultValue, Category category, const SetterCallbackType& setterCallback,
                     const GetterCallbackType& getterCallback, ILinkedPath* linkedPath, int groupIndex = -1);

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
  /**
   * @brief Setter property for SetterCallback
   */
  void setSetterCallback(const LinkedPathCreationFilterParameter::SetterCallbackType& value);
  /**
   * @brief Getter property for SetterCallback
   * @return Value of SetterCallback
   */
  LinkedPathCreationFilterParameter::SetterCallbackType getSetterCallback() const;

  /**
   * @param GetterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
   * that this FilterParameter subclass represents.
   * @return The GetterCallback
   */
  /**
   * @brief Setter property for GetterCallback
   */
  void setGetterCallback(const LinkedPathCreationFilterParameter::GetterCallbackType& value);
  /**
   * @brief Getter property for GetterCallback
   * @return Value of GetterCallback
   */
  LinkedPathCreationFilterParameter::GetterCallbackType getGetterCallback() const;

  /**
   * @brief Set the ILinkedPath for determining what the parent container is
   * @param linkedPath
   */
  void setLinkedPath(ILinkedPath* linkedPath);

  /**
   * @brief Returned the linked DataArrayPath
   * @return
   */
  DataArrayPath getLinkedDataArrayPath() const;

protected:
  /**
   * @brief LinkedPathCreationFilterParameter The default constructor.  It is protected because this
   * filter parameter should only be instantiated using its New(...) function or short-form macro.
   */
  LinkedPathCreationFilterParameter();

private:
  LinkedPathCreationFilterParameter::SetterCallbackType m_SetterCallback = {};
  LinkedPathCreationFilterParameter::GetterCallbackType m_GetterCallback = {};

  ILinkedPath* m_LinkedPath = nullptr;

public:
  LinkedPathCreationFilterParameter(const LinkedPathCreationFilterParameter&) = delete;            // Copy Constructor Not Implemented
  LinkedPathCreationFilterParameter(LinkedPathCreationFilterParameter&&) = delete;                 // Move Constructor Not Implemented
  LinkedPathCreationFilterParameter& operator=(const LinkedPathCreationFilterParameter&) = delete; // Copy Assignment Not Implemented
  LinkedPathCreationFilterParameter& operator=(LinkedPathCreationFilterParameter&&) = delete;      // Move Assignment Not Implemented
};

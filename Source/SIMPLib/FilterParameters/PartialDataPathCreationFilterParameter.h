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

#include <memory>

#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"

class AbstractFilter;

/**
 * @brief SIMPL_NEW_DA_CREATION_FP This macro is a short-form way of instantiating an instance of
 * PartialDataPathCreationFilterParameter. There are 5 required parameters and 1 optional parameter
 * that are always passed to this macro in the following order: HumanLabel, PropertyName, Category,
 * FilterName (class name), RequirementType, GroupIndex (optional).
 *
 * Therefore, the macro should be written like this (this is a concrete example):
 * SIMPL_NEW_DA_CREATION_FP("HumanLabel", PropertyName, Category, FilterName, RequirementType, GroupIndex)
 *
 * Example 1 (instantiated within a filter called [GenericExample](@ref genericexample), with optional GroupIndex parameter):
 * PartialDataPathCreationFilterParameter::RequirementType req;
 * SIMPL_NEW_DA_CREATION_FP("Created Data Array", CreatedDataArray, FilterParameter::Parameter, GenericExample, req, 0);
 */
#define SIMPL_NEW_PATH_CREATION_FP(...) \
  SIMPL_EXPAND(_FP_GET_OVERRIDE(__VA_ARGS__, \
  SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4)\
  (PartialDataPathCreationFilterParameter, __VA_ARGS__))

#define SIMPL_NEW_LINKED_DC_STRING_PATH(FILTER, PTR, DC_PROP) \
  new PartialDataPathCreationFilterParameter::LinkedStringPath { SIMPL_BIND_GETTER(FILTER, PTR, DC_PROP), nullptr }
#define SIMPL_NEW_LINKED_AM_STRING_PATH(FILTER, PTR, DC_PROP, AM_PROP) \
  new PartialDataPathCreationFilterParameter::LinkedStringPath { SIMPL_BIND_GETTER(FILTER, PTR, DC_PROP), SIMPL_BIND_GETTER(FILTER, PTR, AM_PROP) }

#define SIMPL_NEW_LINKED_DATA_PATH(FILTER, PTR, PATH_PROP) \
  new PartialDataPathCreationFilterParameter::LinkedDataPath { SIMPL_BIND_GETTER(FILTER, PTR, PATH_PROP) }

 /**
  * @brief The PartialDataPathCreationFilterParameter class is used by filters to instantiate an DataArrayCreationWidget.  By instantiating an instance of
  * this class in a filter's setupFilterParameters() method, a DataArrayCreationWidget will appear in the filter's "filter input" section in the DREAM3D GUI.
  */
class SIMPLib_EXPORT PartialDataPathCreationFilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(PartialDataPathCreationFilterParameter)
  SIMPL_STATIC_NEW_MACRO(PartialDataPathCreationFilterParameter)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(PartialDataPathCreationFilterParameter, FilterParameter)

  using SetterCallbackType = std::function<void(QString)>;
  using GetterCallbackType = std::function<QString(void)>;

  /**
   * @brief The ILinkedPath interface is used for generating the linked path from either multiple string properties or a DataArrayPath property
   */
  typedef struct
  {
    /**
     * @brief Generates a DataArrayPath to the target container
     * @return
     */
    virtual DataArrayPath generatePath() const = 0;
  } ILinkedPath;

  /**
   * @brief The LinkedStringPath struct stores getters for the DataContainer and AttributeMatrix string properties.
   * These getters are used to generate a DataArrayPath to the target container.
   */
  struct LinkedStringPath : public ILinkedPath
  {
    using StringGetterCallbackType = std::function<QString(void)>;
    StringGetterCallbackType dcGetter;
    StringGetterCallbackType amGetter;

    /**
     * @brief Generates a DataArrayPath to the target container
     * @return
     */
    DataArrayPath generatePath() const override;
  };
  /**
   * @brief The LinkedDataPath struct stores a getter for the target DataArrayPath.
   */
  struct LinkedDataPath : public ILinkedPath
  {
    using PathGetterCallbackType = std::function<DataArrayPath(void)>;
    PathGetterCallbackType pathGetter;

    /**
     * @brief Generates a DataArrayPath to the target container
     * @return
     */
    DataArrayPath generatePath() const override;
  };

  /**
   * @brief New This function instantiates an instance of the PartialDataPathCreationFilterParameter.  Specifying a RequirementType will
   * automatically grey out Data Containers/Attribute Matrices in the user interface that do not conform to the specified RequirementType.
   * Although this function is available to be used, the preferable way to instantiate an instance of this class is to use the
   * SIMPL_NEW_DA_CREATION_FP(...) macro at the top of this file.

   * @param humanLabel The name that the users of DREAM.3D see for this filter parameter
   * @param propertyName The internal property name for this filter parameter.
   * @param defaultValue The value that this filter parameter will be initialized to by default.
   * @param category The category for the filter parameter in the DREAM.3D user interface.  There
   * are three categories: Parameter, Required Arrays, and Created Arrays.
   * @param setterCallback The method in the AbstractFilter subclass that <i>sets</i> the value of the property
  * that this FilterParameter subclass represents.
   * @param getterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
  * that this FilterParameter subclass represents.
   * @param req The RequirementType that greys out non-conforming selection options in the DREAM.3D
   * user interface.
   * @param groupIndex Integer that specifies the group that this filter parameter will be placed in.
   * @return
   */
  static Pointer New(const QString& humanLabel, const QString& propertyName,
    const QString& defaultValue, Category category,
    SetterCallbackType setterCallback, GetterCallbackType getterCallback,
    ILinkedPath* linkedPath, int groupIndex = -1);

  ~PartialDataPathCreationFilterParameter() override;

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
   * @brief Sets the linked path struct.  If one is already assigned, the existing one is deleted.
   * @param linkedPath
   */
  void setLinkedPath(ILinkedPath* linkedPath);

  /**
   * @brief Returns the DataArrayPath pointing to linked parent container.
   * @return
   */
  DataArrayPath getParentPath() const;

  /**
   * @brief Returns the DataArrayPath pointing to the created object.
   * @return
   */
  DataArrayPath getDataArrayPath() const;

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
  /**
   * @brief PartialDataPathCreationFilterParameter The default constructor.  It is protected because this
   * filter parameter should only be instantiated using its New(...) function or short-form macro.
   */
  PartialDataPathCreationFilterParameter();

private:
  std::unique_ptr<ILinkedPath> m_LinkedPath;

public:
  PartialDataPathCreationFilterParameter(const PartialDataPathCreationFilterParameter&) = delete; // Copy Constructor Not Implemented
  PartialDataPathCreationFilterParameter(PartialDataPathCreationFilterParameter&&) = delete;      // Move Constructor Not Implemented
  PartialDataPathCreationFilterParameter& operator=(const PartialDataPathCreationFilterParameter&) = delete; // Copy Assignment Not Implemented
  PartialDataPathCreationFilterParameter& operator=(PartialDataPathCreationFilterParameter&&) = delete;      // Move Assignment Not Implemented
};




#pragma once

#include <memory>

#include <functional>
#include <iostream>
#include <string>

#include <QtCore/QJsonObject>

#include "SIMPLib/DataContainers/DataArrayPath.h"

// -----------------------------------------------------------------------------
// This section of Macros allows each FilterParameter subclass to create a macro
// or set of macros that can lessen the amout of code that needs to be written
// in order to create an instantiation of the subclass. The technique used here
// is the 'paired, sliding list' of macro parameters that also makes use of
// __VA__ARGS__
// -----------------------------------------------------------------------------

#define SIMPL_BIND_SETTER(Class, Type, Prop) Class::SetterCallbackType _##Prop##_ParamSetter = [this](Type i) { return this->set##Prop(i); };

#define SIMPL_BIND_GETTER(Class, Type, Prop) Class::GetterCallbackType _##Prop##_ParamGetter = [this] { return this->get##Prop(); };

// Define overrides that can be used by the expansion of our main macro.
// Each subclass can define a macro that takes up to nine (9) arguments
// to the constructor. These macros support a minimum of 4 arguments.

#define SIMPL_NEW_FP_9(Class, Type, Desc, Prop, Category, Index, A, B, C, D, E)                                                                                                                        \
  Class::New(                                                                                                                                                                                          \
      Desc, #Prop, get##Prop(), Category, [this](Type i) { return this->set##Prop(i); }, [this] { return this->get##Prop(); }, Index, A, B, C, D, E)

#define SIMPL_NEW_FP_8(Class, Type, Desc, Prop, Category, Index, A, B, C, D)                                                                                                                           \
  Class::New(                                                                                                                                                                                          \
      Desc, #Prop, get##Prop(), Category, [this](Type i) { return this->set##Prop(i); }, [this] { return this->get##Prop(); }, Index, A, B, C, D)

#define SIMPL_NEW_FP_7(Class, Type, Desc, Prop, Category, Index, A, B, C)                                                                                                                              \
  Class::New(                                                                                                                                                                                          \
      Desc, #Prop, get##Prop(), Category, [this](Type i) { return this->set##Prop(i); }, [this] { return this->get##Prop(); }, Index, A, B, C)

#define SIMPL_NEW_FP_6(Class, Type, Desc, Prop, Category, Index, A, B)                                                                                                                                 \
  Class::New(                                                                                                                                                                                          \
      Desc, #Prop, get##Prop(), Category, [this](Type i) { return this->set##Prop(i); }, [this] { return this->get##Prop(); }, Index, A, B)

#define SIMPL_NEW_FP_5(Class, Type, Desc, Prop, Category, Index, A)                                                                                                                                    \
  Class::New(                                                                                                                                                                                          \
      Desc, #Prop, get##Prop(), Category, [this](Type i) { return this->set##Prop(i); }, [this] { return this->get##Prop(); }, Index, A)

#define SIMPL_NEW_FP_4(Class, Type, Desc, Prop, Category, Index)                                                                                                                                       \
  Class::New(                                                                                                                                                                                          \
      Desc, #Prop, get##Prop(), Category, [this](Type i) { return this->set##Prop(i); }, [this] { return this->get##Prop(); }, Index)

#define SIMPL_NEW_FP_3(Class, Type, Desc, Prop, Category)                                                                                                                                              \
  Class::New(                                                                                                                                                                                          \
      Desc, #Prop, get##Prop(), Category, [this](Type i) { return this->set##Prop(i); }, [this] { return this->get##Prop(); })

/**
 * @brief This macro is needed for Visual Studio due to differences of VAR_ARGS when
 * passed to another macro that results in a new macro that needs expansion.
 */
#define SIMPL_EXPAND(x) x

// -----------------------------------------------------------------------------
// Define a macro that uses the "paired, sliding arg list"
// technique to select the appropriate override.
#define _FP_GET_OVERRIDE(A, B, C, D, E, F, G, H, I, NAME, ...) NAME

#define SIMPL_NEW_INT32_FP(...)                                                                                                                                                                        \
  SIMPL_EXPAND(_FP_GET_OVERRIDE(__VA_ARGS__, SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4, SIMPL_NEW_FP_3)(Int32Parameter, int, __VA_ARGS__))

#define SIMPL_NEW_DOUBLE_FP(...)                                                                                                                                                                       \
  SIMPL_EXPAND(_FP_GET_OVERRIDE(__VA_ARGS__, SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4, SIMPL_NEW_FP_3)(DoubleParameter, double, __VA_ARGS__))

#define SIMPL_NEW_DATA_ARRAY_PATH_FP(...)                                                                                                                                                              \
  SIMPL_EXPAND(_FP_GET_OVERRIDE(__VA_ARGS__, SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4, SIMPL_NEW_FP_3)(DataArrayPathParameter, DataArrayPath,    \
                                                                                                                                                             __VA_ARGS__))

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class IFilterParameter
{
public:
  using Self = IFilterParameter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  using EnumType = unsigned int;

  enum class Category : EnumType
  {
    Parameter = 0,
    RequiredArray = 1,
    CreatedArray = 2,
    Uncategorized = 3
  };

  IFilterParameter(const std::string& humanLabel, const std::string& propertyName, Category category, int groupIndex);

  virtual ~IFilterParameter();

  /**
   * @brief Setter property for HumanLabel
   */
  void setHumanLabel(const std::string& value);
  /**
   * @brief Getter property for HumanLabel
   * @return Value of HumanLabel
   */
  std::string getHumanLabel() const;

  /**
   * @brief Setter property for PropertyName
   */
  void setPropertyName(const std::string& value);
  /**
   * @brief Getter property for PropertyName
   * @return Value of PropertyName
   */
  std::string getPropertyName() const;

  /**
   * @brief Setter property for Category
   */
  void setCategory(const Category& value);
  /**
   * @brief Getter property for Category
   * @return Value of Category
   */
  Category getCategory() const;

  /**
   * @brief Setter property for GroupIndex
   */
  void setGroupIndex(int value);
  /**
   * @brief Getter property for GroupIndex
   * @return Value of GroupIndex
   */
  int getGroupIndex() const;

  virtual void readJson(const QJsonObject& json) = 0;
  virtual void writeJson(QJsonObject& json) const = 0;

private:
  IFilterParameter(const IFilterParameter&) = delete; // Copy Constructor Not Implemented
  void operator=(const IFilterParameter&) = delete;   // Move assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int32Parameter : public IFilterParameter
{

public:
  using Self = Int32Parameter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  using SetterCallbackType = std::function<void(int)>;
  using GetterCallbackType = std::function<int(void)>;

  static Pointer New(const std::string& humanLabel, const std::string& propertyName, int defaultValue, Category category, const SetterCallbackType& setterCallback,
                     const GetterCallbackType& getterCallback, int groupIndex = -1);
  virtual ~Int32Parameter();

  /**
   * @brief Setter property for DefaultValue
   */
  void setDefaultValue(int value);
  /**
   * @brief Getter property for DefaultValue
   * @return Value of DefaultValue
   */
  int getDefaultValue() const;

  SetterCallbackType getSetterCallback();
  GetterCallbackType getGetterCallback();

  void readJson(const QJsonObject& json) override;

  void writeJson(QJsonObject& json) const override;

protected:
  Int32Parameter(const std::string& humanLabel, const std::string& propertyName, int defaultValue, Category category, const SetterCallbackType& setterCallback,
                 const GetterCallbackType& getterCallback, int groupIndex);

private:
  Int32Parameter(const Int32Parameter&) = delete; // Copy Constructor Not Implemented
  void operator=(const Int32Parameter&) = delete; // Move assignment Not Implemented

  SetterCallbackType m_SetterCallback;
  GetterCallbackType m_GetterCallback;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DoubleParameter : public IFilterParameter
{
public:
  using Self = DoubleParameter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  using SetterCallbackType = std::function<void(double)>;
  using GetterCallbackType = std::function<double(void)>;

  static Pointer New(const std::string& humanLabel, const std::string& propertyName, double defaultValue, Category category, const SetterCallbackType& setterCallback,
                     const GetterCallbackType& getterCallback, int groupIndex = -1);
  virtual ~DoubleParameter();

  /**
   * @brief Setter property for DefaultValue
   */
  void setDefaultValue(double value);
  /**
   * @brief Getter property for DefaultValue
   * @return Value of DefaultValue
   */
  double getDefaultValue() const;

  SetterCallbackType getSetterCallback();
  GetterCallbackType getGetterCallback();

  void readJson(const QJsonObject& json) override;

  void writeJson(QJsonObject& json) const override;

protected:
  DoubleParameter(const std::string& humanLabel, const std::string& propertyName, double defaultValue, Category category, const SetterCallbackType& setterCallback,
                  const GetterCallbackType& getterCallback, int groupIndex);

private:
  DoubleParameter(const DoubleParameter&) = delete; // Copy Constructor Not Implemented
  void operator=(const DoubleParameter&) = delete;  // Move assignment Not Implemented

  SetterCallbackType m_SetterCallback;
  GetterCallbackType m_GetterCallback;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DataArrayPathParameter : public IFilterParameter
{
public:
  using Self = DataArrayPathParameter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  using SetterCallbackType = std::function<void(DataArrayPath)>;
  using GetterCallbackType = std::function<DataArrayPath(void)>;

  static Pointer New(const std::string& humanLabel, const std::string& propertyName, const DataArrayPath& defaultValue, Category category, const SetterCallbackType& setterCallback,
                     const GetterCallbackType& getterCallback, int groupIndex = -1);

  virtual ~DataArrayPathParameter();

  /**
   * @brief Setter property for DefaultValue
   */
  void setDefaultValue(const DataArrayPath& value);
  /**
   * @brief Getter property for DefaultValue
   * @return Value of DefaultValue
   */
  DataArrayPath getDefaultValue() const;

  SetterCallbackType getSetterCallback();
  GetterCallbackType getGetterCallback();

  void readJson(const QJsonObject& json) override;

  void writeJson(QJsonObject& json) const override;

protected:
  DataArrayPathParameter(const std::string& humanLabel, const std::string& propertyName, const DataArrayPath& defaultValue, Category category, const SetterCallbackType& setterCallback,
                         const GetterCallbackType& getterCallback, int groupIndex);

private:
  std::string m_HumanLabel = {};
  std::string m_PropertyName = {};
  Category m_Category = {};
  int m_GroupIndex = {};
  int m_DefaultInt = {};
  double m_DefaultDouble = {};
  DataArrayPath m_DefaultDAP = {};

  DataArrayPathParameter(const DataArrayPathParameter&) = delete; // Copy Constructor Not Implemented
  void operator=(const DataArrayPathParameter&) = delete;         // Move assignment Not Implemented

  SetterCallbackType m_SetterCallback;
  GetterCallbackType m_GetterCallback;
};

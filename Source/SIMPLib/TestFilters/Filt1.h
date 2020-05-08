/*
 * Your License or Copyright Information can go here
 */

#pragma once

#include <memory>

#include <QString>

#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class SIMPLib_EXPORT Filt1 : public AbstractFilter
{
  Q_OBJECT
public:
  using Self = Filt1;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;

  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for Filt1
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for Filt1
   */
  static QString ClassName();

  ~Filt1() override;
  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const QString& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  QString getDataContainerName() const;

  /**
   * @brief Setter property for Filt1_Float
   */
  void setFilt1_Float(float value);
  /**
   * @brief Getter property for Filt1_Float
   * @return Value of Filt1_Float
   */
  float getFilt1_Float() const;

  /**
   * @brief Setter property for Filt1_Integer
   */
  void setFilt1_Integer(int32_t value);
  /**
   * @brief Getter property for Filt1_Integer
   * @return Value of Filt1_Integer
   */
  int32_t getFilt1_Integer() const;

  /**
   * @brief This returns the group that the filter belonds to. You can select
   * a different group if you want. The string returned here will be displayed
   * in the GUI for the filter
   */
  QString getGroupName() const override
  {
    return "TestFilters";
  }

  /**
   * @brief This returns a string that is displayed in the GUI. It should be readable
   * and understandable by humans.
   */
  QString getHumanLabel() const override
  {
    return "Filt1";
  }

  /**
   * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
   * a subgroup. It should be readable and understandable by humans.
   */
  QString getSubGroupName() const override
  {
    return "Test";
  }

  /**
   * @brief This method will instantiate all the end user settable options/parameters
   * for this filter
   */
  void setupFilterParameters() override;

  /**
   * @brief This method will read the options from a file
   * @param reader The reader that is used to read the options from a file
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  Filt1();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  QString m_DataContainerName = {};
  float m_Filt1_Float = {};
  int32_t m_Filt1_Integer = {};

  Filt1(const Filt1&) = delete;          // Copy Constructor Not Implemented
  void operator=(const Filt1&) = delete; // Move assignment Not Implemented
};

#pragma once

#include <memory>

#include "SIMPLib/Filtering/AbstractFilter.h"

// undef slots since a Python header uses slots
#undef slots

#include "SIMPLib/Python/FilterPyObject.h"

/**
 * @brief The PythonFilter class. See [Filter documentation](@ref PythonFilter) for details.
 */
class SIMPLib_EXPORT PythonFilter : public AbstractFilter
{
  Q_OBJECT

public:
  using Self = PythonFilter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;

  static Pointer NullPointer();

  static Pointer New(PythonSupport::FilterPyObject filterObject);

  /**
   * @brief Returns the name of the class for CreateDataArray
   */
  QString getNameOfClass() const override;

  ~PythonFilter() override;

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

  /**
   * @brief This method will instantiate all the end user settable options/parameters
   * for this filter
   */
  void setupFilterParameters() override;

  /**
   * @brief Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  PythonFilter() = delete;
  PythonFilter(PythonSupport::FilterPyObject object);

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

private:
  PythonSupport::FilterPyObject m_Filter;
  QString m_CompiledLibraryName;
  QString m_FilterVersion;
  QString m_GroupName;
  QString m_SubGroupName;
  QString m_HumanLabel;
  QString m_ClassName;
  QUuid m_Uuid;

public:
  PythonFilter(const PythonFilter&) = delete;            // Copy Constructor Not Implemented
  PythonFilter(PythonFilter&&) = delete;                 // Move Constructor Not Implemented
  PythonFilter& operator=(const PythonFilter&) = delete; // Copy Assignment Not Implemented
  PythonFilter& operator=(PythonFilter&&) = delete;      // Move Assignment Not Implemented
};

#define slots Q_SLOTS

/*
 * Your License or Copyright can go here
 */

#pragma once

#include <utility>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The GenerateTiltSeries class. See [Filter documentation](@ref GenerateTiltSeries) for details.
 */
class SIMPLib_EXPORT GenerateTiltSeries : public AbstractFilter
{
  Q_OBJECT
  // clang-format off
    PYB11_CREATE_BINDINGS(GenerateTiltSeries SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(int RotationAxis READ getRotationAxis WRITE setRotationAxis)
    PYB11_PROPERTY(float Increment READ getIncrement WRITE setIncrement)
    PYB11_PROPERTY(float Spacing READ getSpacing WRITE setSpacing)
    PYB11_PROPERTY(DataArrayPath InputDataArrayPath READ getInputDataArrayPath WRITE setInputDataArrayPath)
    PYB11_PROPERTY(QString OutputPrefix READ getOutputPrefix WRITE setOutputPrefix)

  // clang-format on

public:
  SIMPL_SHARED_POINTERS(GenerateTiltSeries)
  SIMPL_FILTER_NEW_MACRO(GenerateTiltSeries)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(GenerateTiltSeries, AbstractFilter)

  ~GenerateTiltSeries() override;

  static constexpr int32_t k_XAxis = 0;
  static constexpr int32_t k_YAxis = 1;
  static constexpr int32_t k_ZAxis = 2;

  SIMPL_FILTER_PARAMETER(int, RotationAxis)
  Q_PROPERTY(int RotationAxis READ getRotationAxis WRITE setRotationAxis)

  SIMPL_FILTER_PARAMETER(float, Increment)
  Q_PROPERTY(float Increment READ getIncrement WRITE setIncrement)

  SIMPL_FILTER_PARAMETER(FloatVec3Type, Spacing)
  Q_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)

  SIMPL_FILTER_PARAMETER(DataArrayPath, InputDataArrayPath)
  Q_PROPERTY(DataArrayPath InputDataArrayPath READ getInputDataArrayPath WRITE setInputDataArrayPath)

  SIMPL_FILTER_PARAMETER(QString, OutputPrefix)
  Q_PROPERTY(QString OutputPrefix READ getOutputPrefix WRITE setOutputPrefix)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
   * @brief preflight Reimplemented from @see AbstractFilter class
   */
  void preflight() override;

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  GenerateTiltSeries();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  std::pair<FloatArrayType::Pointer, ImageGeom::Pointer> generateGrid();

private:
  static constexpr unsigned Dimension = 3;

public:
  GenerateTiltSeries(const GenerateTiltSeries&) = delete;            // Copy Constructor Not Implemented
  GenerateTiltSeries& operator=(const GenerateTiltSeries&) = delete; // Copy Assignment Not Implemented
  GenerateTiltSeries(GenerateTiltSeries&&) = delete;                 // Move Constructor Not Implemented
  GenerateTiltSeries& operator=(GenerateTiltSeries&&) = delete;      // Move Assignment Not Implemented
};

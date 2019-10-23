/*
 * Your License or Copyright can go here
 */

#pragma once

#include <QtCore/QJsonArray>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

/**
 * @brief The GenerateColorTable class. See [Filter documentation](@ref generatecolortable) for details.
 */
class SIMPLib_EXPORT GenerateColorTable : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(GenerateColorTable SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(GenerateColorTable)
  PYB11_FILTER_NEW_MACRO(GenerateColorTable)
  PYB11_PROPERTY(QString SelectedPresetName READ getSelectedPresetName WRITE setSelectedPresetName)
  PYB11_PROPERTY(QJsonArray SelectedPresetControlPoints READ getSelectedPresetControlPoints WRITE setSelectedPresetControlPoints)
  PYB11_PROPERTY(DataArrayPath SelectedDataArrayPath READ getSelectedDataArrayPath WRITE setSelectedDataArrayPath)
  PYB11_PROPERTY(QString RgbArrayName READ getRgbArrayName WRITE setRgbArrayName)
#endif

public:
  using Self = GenerateColorTable;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer();

  static std::shared_ptr<GenerateColorTable> New();

  /**
   * @brief Returns the name of the class for GenerateColorTable
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GenerateColorTable
   */
  static QString ClassName();

  ~GenerateColorTable() override;

  /**
   * @brief Setter property for SelectedPresetName
   */
  void setSelectedPresetName(const QString& value);
  /**
   * @brief Getter property for SelectedPresetName
   * @return Value of SelectedPresetName
   */
  QString getSelectedPresetName() const;

  Q_PROPERTY(QString SelectedPresetName READ getSelectedPresetName WRITE setSelectedPresetName)

  /**
   * @brief Setter property for SelectedPresetControlPoints
   */
  void setSelectedPresetControlPoints(const QJsonArray& value);
  /**
   * @brief Getter property for SelectedPresetControlPoints
   * @return Value of SelectedPresetControlPoints
   */
  QJsonArray getSelectedPresetControlPoints() const;

  Q_PROPERTY(QJsonArray SelectedPresetControlPoints READ getSelectedPresetControlPoints WRITE setSelectedPresetControlPoints)

  /**
   * @brief Setter property for SelectedDataArrayPath
   */
  void setSelectedDataArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SelectedDataArrayPath
   * @return Value of SelectedDataArrayPath
   */
  DataArrayPath getSelectedDataArrayPath() const;

  Q_PROPERTY(DataArrayPath SelectedDataArrayPath READ getSelectedDataArrayPath WRITE setSelectedDataArrayPath)

  /**
   * @brief Setter property for RgbArrayName
   */
  void setRgbArrayName(const QString& value);
  /**
   * @brief Getter property for RgbArrayName
   * @return Value of RgbArrayName
   */
  QString getRgbArrayName() const;

  Q_PROPERTY(QString RgbArrayName READ getRgbArrayName WRITE setRgbArrayName)

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
  GenerateColorTable();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

public:
  GenerateColorTable(const GenerateColorTable&) = delete;            // Copy Constructor Not Implemented
  GenerateColorTable(GenerateColorTable&&) = delete;                 // Move Constructor Not Implemented
  GenerateColorTable& operator=(const GenerateColorTable&) = delete; // Copy Assignment Not Implemented
  GenerateColorTable& operator=(GenerateColorTable&&) = delete;      // Move Assignment Not Implemented

private:
  QString m_SelectedPresetName = {};
  QJsonArray m_SelectedPresetControlPoints = {};
  DataArrayPath m_SelectedDataArrayPath = {};
  QString m_RgbArrayName = {};
};


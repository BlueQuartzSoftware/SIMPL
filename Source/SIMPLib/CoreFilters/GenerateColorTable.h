/*
 * Your License or Copyright can go here
 */

#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The GenerateColorTable class. See [Filter documentation](@ref generatecolortable) for details.
 */
class SIMPLib_EXPORT GenerateColorTable : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(GenerateColorTable SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(QString SelectedPresetName READ getSelectedPresetName WRITE setSelectedPresetName)
  PYB11_PROPERTY(QJsonArray SelectedPresetControlPoints READ getSelectedPresetControlPoints WRITE setSelectedPresetControlPoints)
  PYB11_PROPERTY(DataArrayPath SelectedDataArrayPath READ getSelectedDataArrayPath WRITE setSelectedDataArrayPath)
  PYB11_PROPERTY(QString RgbArrayName READ getRgbArrayName WRITE setRgbArrayName)

  public:
    SIMPL_SHARED_POINTERS(GenerateColorTable)
    SIMPL_FILTER_NEW_MACRO(GenerateColorTable)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(GenerateColorTable, AbstractFilter)

    ~GenerateColorTable() override;

    SIMPL_INSTANCE_PROPERTY(QString, SelectedPresetName)
    Q_PROPERTY(QString SelectedPresetName READ getSelectedPresetName WRITE setSelectedPresetName)

    SIMPL_INSTANCE_PROPERTY(QJsonArray, SelectedPresetControlPoints)
    Q_PROPERTY(QJsonArray SelectedPresetControlPoints READ getSelectedPresetControlPoints WRITE setSelectedPresetControlPoints)

    SIMPL_INSTANCE_PROPERTY(DataArrayPath, SelectedDataArrayPath)
    Q_PROPERTY(DataArrayPath SelectedDataArrayPath READ getSelectedDataArrayPath WRITE setSelectedDataArrayPath)

    SIMPL_INSTANCE_PROPERTY(QString, RgbArrayName)
    Q_PROPERTY(QString RgbArrayName READ getRgbArrayName WRITE setRgbArrayName)

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
    GenerateColorTable(const GenerateColorTable&) = delete; // Copy Constructor Not Implemented
    GenerateColorTable(GenerateColorTable&&) = delete;      // Move Constructor Not Implemented
    GenerateColorTable& operator=(const GenerateColorTable&) = delete; // Copy Assignment Not Implemented
    GenerateColorTable& operator=(GenerateColorTable&&) = delete;      // Move Assignment
};


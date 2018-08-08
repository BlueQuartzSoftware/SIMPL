/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/CoreFilters/util/ASCIIWizardData.hpp"

/**
 * @brief The ReadASCIIData class. See [Filter documentation](@ref ReadASCIIData) for details.
 */
class SIMPLib_EXPORT ReadASCIIData : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ReadASCIIData SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(ASCIIWizardData WizardData READ getWizardData WRITE setWizardData)

  public:
    SIMPL_SHARED_POINTERS(ReadASCIIData)
    SIMPL_FILTER_NEW_MACRO(ReadASCIIData)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ReadASCIIData, AbstractFilter)

    ~ReadASCIIData() override;

  SIMPL_FILTER_PARAMETER(ASCIIWizardData, WizardData)
  Q_PROPERTY(ASCIIWizardData WizardData READ getWizardData WRITE setWizardData)

  enum ErrorCodes
  {
    EMPTY_FILE = -100,
    EMPTY_ATTR_MATRIX = -101,
    INCONSISTENT_TUPLES = -102,
    INCONSISTENT_COLS = -103,
    CONVERSION_FAILURE = -104,
    DUPLICATE_NAMES = -105,
    INVALID_ARRAY_TYPE = -106
  };

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
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
  * @brief readFilterParametersFromJson Reads the filter parameters from a file
  * @param reader Reader that is used to read the parameters from a file
  */
  void readFilterParameters(QJsonObject& obj) override;

  /**
  * @brief writeFilterParametersToJson Writes the filter parameters to a file
  * @param root The root json object
  */
  void writeFilterParameters(QJsonObject& obj) const override;

   /**
    * @brief execute Reimplemented from @see AbstractFilter class
    */
    void execute() override;

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    void preflight() override;

    /**
    * @brief Updates any DataArrayPath properties from the old path to a new path
    * For DataArrayPaths longer than the given path, only the specified values are modified
    * @param renamePath
    */
    void renameDataArrayPath(DataArrayPath::RenameType renamePath) override;

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
    ReadASCIIData();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


  private:
    QMap<int, IDataArray::Pointer>        m_ASCIIArrayMap;

  public:
    ReadASCIIData(const ReadASCIIData&) = delete;  // Copy Constructor Not Implemented
    ReadASCIIData(ReadASCIIData&&) = delete;       // Move Constructor Not Implemented
    ReadASCIIData& operator=(const ReadASCIIData&) = delete; // Copy Assignment Not Implemented
    ReadASCIIData& operator=(ReadASCIIData&&) = delete;      // Move Assignment
};


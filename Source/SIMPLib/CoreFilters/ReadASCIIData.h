/*
 * Your License or Copyright can go here
 */

#ifndef _readasciidata_h_
#define _readasciidata_h_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/CoreFilters/util/ASCIIWizardData.hpp"

/**
 * @brief The ReadASCIIData class. See [Filter documentation](@ref ReadASCIIData) for details.
 */
class ReadASCIIData : public AbstractFilter
{
  Q_OBJECT

  public:
    SIMPL_SHARED_POINTERS(ReadASCIIData)
    SIMPL_STATIC_NEW_MACRO(ReadASCIIData)
     SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ReadASCIIData, AbstractFilter)

    virtual ~ReadASCIIData();

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
    virtual const QString getCompiledLibraryName() override;

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString() override;

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion() override;

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) override;

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName() override;

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName() override;

    /**
     * @brief getUuid Return the unique identifier for this filter.
     * @return A QUuid object.
     */
    virtual const QUuid getUuid() override;

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel() override;

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters() override;

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
    * @brief readFilterParametersFromJson Reads the filter parameters from a file
    * @param reader Reader that is used to read the parameters from a file
    */
    virtual void readFilterParameters(QJsonObject &obj);

    /**
    * @brief writeFilterParametersToJson Writes the filter parameters to a file
    * @param root The root json object
    */
    virtual void writeFilterParameters(QJsonObject &obj);

   /**
    * @brief execute Reimplemented from @see AbstractFilter class
    */
    virtual void execute() override;

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight() override;

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

    ReadASCIIData(const ReadASCIIData&) = delete;  // Copy Constructor Not Implemented
    void operator=(const ReadASCIIData&) = delete; // Operator '=' Not Implemented
};

#endif /* _readasciidata_h_ */

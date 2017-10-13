/*
 * Your License or Copyright can go here
 */

#ifndef _readhdf5file_h_
#define _readhdf5file_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

/**
 * @brief The ReadHDF5File class. See [Filter documentation](@ref readhdf5file) for details.
 */
class SIMPLib_EXPORT ReadHDF5File : public AbstractFilter
{
  Q_OBJECT

  public:
    SIMPL_SHARED_POINTERS(ReadHDF5File)
    SIMPL_STATIC_NEW_MACRO(ReadHDF5File)
    SIMPL_TYPE_MACRO_SUPER(ReadHDF5File, AbstractFilter)

    virtual ~ReadHDF5File();

    SIMPL_INSTANCE_PROPERTY(QString, HDF5FilePath)
    Q_PROPERTY(QString HDF5FilePath READ getHDF5FilePath WRITE setHDF5FilePath)

    SIMPL_INSTANCE_PROPERTY(QString, DatasetPath)
    Q_PROPERTY(QString DatasetPath READ getDatasetPath WRITE setDatasetPath)

    QString getHDF5Dimensions();
    Q_PROPERTY(QString HDF5Dimensions READ getHDF5Dimensions)

    SIMPL_INSTANCE_PROPERTY(QString, ComponentDimensions)
    Q_PROPERTY(QString ComponentDimensions READ getComponentDimensions WRITE setComponentDimensions)

    SIMPL_INSTANCE_PROPERTY(DataArrayPath, SelectedAttributeMatrix)
    Q_PROPERTY(DataArrayPath SelectedAttributeMatrix READ getSelectedAttributeMatrix WRITE setSelectedAttributeMatrix)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString();

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

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
    ReadHDF5File();

    /**
    * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
    */
    void dataCheck();

    /**
    * @brief Initializes all the private instance variables.
    */
    void initialize();

  private:
    QString m_HDF5Dimensions = "";

    IDataArray::Pointer readIDataArray(hid_t gid, const QString& name, QVector<size_t> tDims, QVector<size_t> cDims, bool metaDataOnly);

    /**
     * @brief createComponentDimensions
     * @return
     */
    QVector<size_t> createComponentDimensions();

    /**
     * @brief calculateTupleDimensions
     * @param cDims
     * @param dsetDims
     * @param amTupleCount
     * @return
     */
    QVector<size_t> calculateTupleDimensions(QVector<size_t> cDims, QVector<hsize_t> dsetDims, size_t amTupleCount);

    ReadHDF5File(const ReadHDF5File&); // Copy Constructor Not Implemented
    void operator=(const ReadHDF5File&); // Operator '=' Not Implemented
};

#endif /* _ReadHDF5File_H_ */

/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <QtCore/QDateTime>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/SIMPLib.h"

class SIMPLH5DataReader;

/**
 * @brief The DataContainerReader class. See [Filter documentation](@ref datacontainerreader) for details.
 */
class SIMPLib_EXPORT DataContainerReader : public AbstractFilter
{
    Q_OBJECT
    
    // This line MUST be first when exposing a class and properties to Python
    PYB11_CREATE_BINDINGS(DataContainerReader SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
    PYB11_PROPERTY(bool OverwriteExistingDataContainers READ getOverwriteExistingDataContainers WRITE setOverwriteExistingDataContainers)
    PYB11_PROPERTY(QString LastFileRead READ getLastFileRead WRITE setLastFileRead)
    PYB11_PROPERTY(QDateTime LastRead READ getLastRead WRITE setLastRead)
    PYB11_PROPERTY(DataContainerArrayProxy InputFileDataContainerArrayProxy READ getInputFileDataContainerArrayProxy WRITE setInputFileDataContainerArrayProxy)

    PYB11_METHOD(DataContainerArrayProxy readDataContainerArrayStructure ARGS path)
  
  public:
    SIMPL_SHARED_POINTERS(DataContainerReader)
    SIMPL_FILTER_NEW_MACRO(DataContainerReader)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(DataContainerReader, AbstractFilter)

    ~DataContainerReader() override;

    SIMPL_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

    SIMPL_FILTER_PARAMETER(bool, OverwriteExistingDataContainers)
    Q_PROPERTY(bool OverwriteExistingDataContainers READ getOverwriteExistingDataContainers WRITE setOverwriteExistingDataContainers)

    SIMPL_FILTER_PARAMETER(QString, LastFileRead)
    Q_PROPERTY(QString LastFileRead READ getLastFileRead WRITE setLastFileRead)

    SIMPL_FILTER_PARAMETER(QDateTime, LastRead)
    Q_PROPERTY(QDateTime LastRead READ getLastRead WRITE setLastRead)

    SIMPL_FILTER_PARAMETER(DataContainerArrayProxy, InputFileDataContainerArrayProxy)
    Q_PROPERTY(DataContainerArrayProxy InputFileDataContainerArrayProxy READ getInputFileDataContainerArrayProxy WRITE setInputFileDataContainerArrayProxy)

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
     * @brief This method is called just before the writeFilterParameters() completes
     * @param obj The json object to add the filter parameters into
     * @param rootObject The parent object of **obj**
     */
    virtual void preWriteFilterParameters(QJsonObject& obj, QJsonObject& rootObject) override;

    /**
     * @brief writeFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void writeFilterParameters(QJsonObject& obj) const override;

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(QJsonObject& obj) override;

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    void execute() override;

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    void preflight() override;

    /**
     * @brief readExistingPipelineFromFile Reads the existing pipeline that is stored in the file and store it
     * in the class instance for later writing to another SIMPLView data file
     * @param fileId HDF5 group Id for pipeline
     * @return Integer error value
     */
    int readExistingPipelineFromFile(hid_t fileId);

    /**
     * @brief writeExistingPipelineToFile Writes the filter parameters of the existing pipline to a
     * SIMPLView file
     * @param obj json object
     * @param index The index that the inserted pipeline should start at.
     * @return
     */
    int writeExistingPipelineToFile(QJsonObject &obj, int index);

    /**
     * @brief readDataContainerArrayStructure Reads the structure of the DataContainerArray from the HDF5 based .dream3d file. For this method to work
     * the member variable for the file path should have been set prior to calling this method.
     * @param path The file path to the SIMPLView file
     * @return DataContainerArrayProxy instance
     */
    DataContainerArrayProxy readDataContainerArrayStructure(const QString& path);

    /**
    * @brief syncProxies Combines the file and cached proxies if they are out-of-sync
    * @return
    */
    bool syncProxies();

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
    DataContainerReader();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();

    /**
     * @brief readData
     * @param proxy
     * @return
     */
    DataContainerArray::Pointer readData(DataContainerArrayProxy& proxy);

  protected slots:
    /**
    * @brief Cleans up the filter after execution
    */
    void cleanupFilter() override;

  private:
    FilterPipeline::Pointer                     m_PipelineFromFile;

  public:
    DataContainerReader(const DataContainerReader&) = delete; // Copy Constructor Not Implemented
    DataContainerReader(DataContainerReader&&) = delete;      // Move Constructor Not Implemented
    DataContainerReader& operator=(const DataContainerReader&) = delete; // Copy Assignment Not Implemented
    DataContainerReader& operator=(DataContainerReader&&) = delete;      // Move Assignment
};


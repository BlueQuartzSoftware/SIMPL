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
#ifndef _jsonfilterparametersreader_h_
#define _jsonfilterparametersreader_h_



#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The JsonFilterParametersReader class
 */
class SIMPLib_EXPORT JsonFilterParametersReader : public AbstractFilterParametersReader
{
  public:
    SIMPL_SHARED_POINTERS(JsonFilterParametersReader)
    SIMPL_STATIC_NEW_MACRO(JsonFilterParametersReader)
     SIMPL_TYPE_MACRO_SUPER_OVERRIDE(JsonFilterParametersReader, AbstractFilterParametersReader)

    virtual ~JsonFilterParametersReader();

    SIMPL_INSTANCE_PROPERTY(QString, FileName)
    SIMPL_INSTANCE_PROPERTY(int, MaxFilterIndex)

    /**
    * @brief ReadPipelineFromFile Reads the Json formatted file and returns a FilterPipeline object
    * that contains all the filters that could be found. If a filter can not be found then that filter is simply skipped.
    * If the IObserver is NON-Null then an error message will be passed to it with an error message
    * @param filePath The path to the INI formatted file
    * @param format The format of the file which is anything that QSettings understands
    * @param obs An IObserver object to report errors.
    * @return Shared Pointer to a FilterPipeline Instance
    */
    FilterPipeline::Pointer readPipelineFromFile(QString filePath, IObserver* obs = nullptr);

    /**
    * @brief getJsonFromFile Reads the Json formatted file and returns a json string
    * that contains all the filters that could be found. If a filter can not be found then that filter is simply skipped.
    * If the IObserver is NON-Null then an error message will be passed to it with an error message
    * @param filePath The path to the INI formatted file
    * @param format The format of the file which is anything that QSettings understands
    * @param obs An IObserver object to report errors.
    * @return Shared Pointer to a FilterPipeline Instance
    */
    QString getJsonFromFile(QString filePath, IObserver* obs = nullptr);

    /**
    * @brief ReadPipelineFromString Reads the Json formatted file and returns a FilterPipeline object
    * that contains all the filters that could be found. If a filter can not be found then that filter is simply skipped.
    * If the IObserver is NON-Null then an error message will be passed to it with an error message
    * @param filePath The path to the INI formatted file
    * @param format The format of the file which is anything that QSettings understands
    * @param obs An IObserver object to report errors.
    * @return Shared Pointer to a FilterPipeline Instance
    */
    FilterPipeline::Pointer readPipelineFromString(QString contents, IObserver* obs = nullptr);

    /**
     * @brief Gets the name of the pipeline from a pipeline file
     * @param filePath The absolute path to the pipeline file.
     * @param name Sets the name of the pipeline into this variable
     * @param version Sets the DREAM3D Version into this variable.
     * @param obs Any observer that needs to know about errors.
     * @return
     */
    void readNameOfPipelineFromFile(QString filePath, QString& name, QString& version, IObserver* obs = nullptr);

    /**
     * @brief Generate an html formatted string that summarizes the pipeline file by listing
     * each filter by it's human name and C++ class name.
     * @param filePath The path to the pipeline file
     * @param obs Any observer that needs to know about errors.
     * @return html formatted string
     */
    static QString HtmlSummaryFromFile(QString filePath, IObserver* obs);

    /**
     * @brief Opens the pipeline file for operations
     * @param filePath The path to the pipeline file
     * @return
     */
    int openFile(QString filePath);

    /**
    * @brief setPipelineContents
    * @param contents
    * @return
    */
    int setPipelineContents(QString contents);

    /**
     * @brief closeFile
     */
    void closeFile();

    /**
     * @brief openFilterGroup
     * @param unused
     * @param index
     * @return
     */
    virtual int openFilterGroup(AbstractFilter* unused, int index);

    /**
     * @brief closeFilterGroup
     * @return
     */
    virtual int closeFilterGroup();

    /*
     * @brief getCurrentGroupObject
     * @return
     */
    QJsonObject& getCurrentGroupObject();

    bool containsGroup(QString key);

    int openGroup(QString key);
    int closeGroup();

  protected:
    JsonFilterParametersReader();

  private:
    QJsonObject m_Root;
    QJsonObject m_CurrentFilterIndex;

    FilterPipeline::Pointer readPipeline(IObserver* obs);

    JsonFilterParametersReader(const JsonFilterParametersReader&) = delete; // Copy Constructor Not Implemented
    void operator=(const JsonFilterParametersReader&) = delete;             // Operator '=' Not Implemented
};

#endif /* _QFilterParametersReader_H_ */


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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

/**
 * @brief The ConvertDataContainerGeometry class. See [Filter documentation](@ref convertdatacontainergeometry) for details.
 */
class SIMPLib_EXPORT ConvertDataContainerGeometry : public AbstractFilter
{
  Q_OBJECT

  public:
    SIMPL_SHARED_POINTERS(ConvertDataContainerGeometry)
    SIMPL_FILTER_NEW_MACRO(ConvertDataContainerGeometry)
    SIMPL_TYPE_MACRO_SUPER(ConvertDataContainerGeometry, AbstractFilter)

    SIMPL_FILTER_PARAMETER(int, NewDataContainerGeometry)
    Q_PROPERTY(int NewDataContainerGeometry READ getNewDataContainerGeometry WRITE setNewDataContainerGeometry)

    SIMPL_FILTER_PARAMETER(int, ArrayHandling)
    Q_PROPERTY(int ArrayHandling READ getArrayHandling WRITE setArrayHandling)

    SIMPL_FILTER_PARAMETER(QString, SelectedDataContainerName)
    Q_PROPERTY(QString SelectedDataContainerName READ getSelectedDataContainerName WRITE setSelectedDataContainerName)

    SIMPL_FILTER_PARAMETER(QVector<DataArrayPath>, IncludedDataArrayPaths)
    Q_PROPERTY(QVector<DataArrayPath> IncludedDataArrayPaths READ getIncludedDataArrayPaths WRITE setIncludedDataArrayPaths)

    SIMPL_FILTER_PARAMETER(QString, NewDataContainerName)
    Q_PROPERTY(QString NewDataContainerName READ getNewDataContainerName WRITE setNewDataContainerName)

    ~ConvertDataContainerGeometry() override;

    enum class ArrayHandlingType : unsigned int
    {
      MoveArrays,
      CopyArrays
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
    ConvertDataContainerGeometry();

    /**
    * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
    */
    void dataCheck();

    /**
    * @brief Initializes all the private instance variables.
    */
    void initialize();

  private:
    QVector<QString> m_NewDCGeometryChoices;
    QVector<QString> m_ArrayHandlingChoices;

    /**
     * @brief Converts an image geometry into a vertex geometry
     * @param imageGeom
     * @return
     */
    VertexGeom::Pointer Image2Vertex(ImageGeom::Pointer imageGeom);

    /**
     * @brief Converts a rectilinear grid geometry into a vertex geometry
     * @param rectGridGeom
     * @return
     */
    VertexGeom::Pointer RectGrid2Vertex(RectGridGeom::Pointer rectGridGeom);
  
  public:
    /* Rule of 5: All special member functions should be defined if any are defined.
    * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all
    */
    ConvertDataContainerGeometry(const ConvertDataContainerGeometry&) = delete;             // Copy Constructor Not Implemented
    ConvertDataContainerGeometry& operator=(const ConvertDataContainerGeometry&) = delete;  // Copy Assignment Not Implemented
    ConvertDataContainerGeometry(ConvertDataContainerGeometry&&) = delete;                  // Move Constructor Not Implemented
    ConvertDataContainerGeometry& operator=(ConvertDataContainerGeometry&&) = delete;       // Move Assignment Not Implemented

};


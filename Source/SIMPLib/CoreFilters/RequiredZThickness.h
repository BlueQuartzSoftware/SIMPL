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

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SIMPLib/Filtering/AbstractDecisionFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

/**
 * @brief The RequiredZThickness class. See [Filter documentation](@ref requiredzthickness) for details.
 */
class SIMPLib_EXPORT RequiredZThickness : public AbstractDecisionFilter
{
    Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
    PYB11_CREATE_BINDINGS(RequiredZThickness SUPERCLASS AbstractFilter)
    PYB11_SHARED_POINTERS(RequiredZThickness)
    PYB11_STATIC_NEW_MACRO(RequiredZThickness)
    PYB11_FILTER_PARAMETER(DataArrayPath, DataContainerSelection)
    PYB11_FILTER_PARAMETER(int, NumZVoxels)
    PYB11_FILTER_PARAMETER(bool, PreflightCheck)
    PYB11_PROPERTY(DataArrayPath DataContainerSelection READ getDataContainerSelection WRITE setDataContainerSelection)
    PYB11_PROPERTY(int NumZVoxels READ getNumZVoxels WRITE setNumZVoxels)
    PYB11_PROPERTY(bool PreflightCheck READ getPreflightCheck WRITE setPreflightCheck)
#endif

  public:
    using Self = RequiredZThickness;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<Self>;
    static Pointer NullPointer();

    static Pointer New();

    /**
     * @brief Returns the name of the class for RequiredZThickness
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for RequiredZThickness
     */
    static QString ClassName();

    ~RequiredZThickness() override;

    /**
     * @brief Setter property for DataContainerSelection
     */
    void setDataContainerSelection(const DataArrayPath& value);
    /**
     * @brief Getter property for DataContainerSelection
     * @return Value of DataContainerSelection
     */
    DataArrayPath getDataContainerSelection() const;

    Q_PROPERTY(DataArrayPath DataContainerSelection READ getDataContainerSelection WRITE setDataContainerSelection)

    /**
     * @brief Setter property for NumZVoxels
     */
    void setNumZVoxels(int value);
    /**
     * @brief Getter property for NumZVoxels
     * @return Value of NumZVoxels
     */
    int getNumZVoxels() const;

    Q_PROPERTY(int NumZVoxels READ getNumZVoxels WRITE setNumZVoxels)

    /**
     * @brief Setter property for PreflightCheck
     */
    void setPreflightCheck(bool value);
    /**
     * @brief Getter property for PreflightCheck
     * @return Value of PreflightCheck
     */
    bool getPreflightCheck() const;

    Q_PROPERTY(bool PreflightCheck READ getPreflightCheck WRITE setPreflightCheck)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    QString getCompiledLibraryName() const override;

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
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    void execute() override;

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    void preflight() override;

    //  signals:
    //    void decisionMade(bool& dm);

  protected:
    RequiredZThickness();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


  private:
    std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;

    DataArrayPath m_DataContainerSelection = {};
    int m_NumZVoxels = {};
    bool m_PreflightCheck = {};

  public:
    RequiredZThickness(const RequiredZThickness&) = delete; // Copy Constructor Not Implemented
    RequiredZThickness(RequiredZThickness&&) = delete;      // Move Constructor Not Implemented
    RequiredZThickness& operator=(const RequiredZThickness&) = delete; // Copy Assignment Not Implemented
    RequiredZThickness& operator=(RequiredZThickness&&) = delete;      // Move Assignment Not Implemented
};


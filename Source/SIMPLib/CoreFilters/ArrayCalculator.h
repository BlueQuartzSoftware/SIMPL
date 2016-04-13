/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#ifndef _arraycalculator_h_
#define _arraycalculator_h_

#include <QtCore/QStack>
#include <QtCore/QQueue>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/AbstractFilter.h"

#include "util/ICalculatorArray.h"

/**
 * @brief The ArrayCalculator class. See [Filter documentation](@ref createdatacontainer) for details.
 */
class SIMPLib_EXPORT ArrayCalculator : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    enum ErrorAndWarningCodes
    {
      EMPTY_EQUATION = -4010,
      EMPTY_CAL_ARRAY = -4011,
      EMPTY_SEL_MATRIX = -4012,
      LOST_ATTR_MATRIX = -4013,
      INVALID_EQUATION = -4014,
      NUMERIC_VALUE_WARNING = -4015,
      INCORRECT_TUPLE_COUNT = -4016,
      INCONSISTENT_TUPLES = -4017,
      UNRECOGNIZED_ITEM = -4018,
      MISMATCHED_PARENTHESES = -4019,
      UNEXPECTED_OUTPUT = -4020,
      COMPONENT_OUT_OF_RANGE = -4021,
      AMBIGUOUS_NAME_WARNING = -4022,
      INVALID_ARRAY_NAME = -4023,
      INCONSISTENT_INDEXING = -4024,
      INCONSISTENT_COMP_DIMS = -4025
    };

    enum AngleUnits
    {
      Degrees,
      Radians
    };

    Q_ENUMS(AngleUnits)

    SIMPL_SHARED_POINTERS(ArrayCalculator)
    SIMPL_STATIC_NEW_MACRO(ArrayCalculator)
    SIMPL_TYPE_MACRO_SUPER(ArrayCalculator, AbstractFilter)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SelectedAttributeMatrix)
    Q_PROPERTY(DataArrayPath SelectedAttributeMatrix READ getSelectedAttributeMatrix WRITE setSelectedAttributeMatrix)

    SIMPL_FILTER_PARAMETER(QString, InfixEquation)
    Q_PROPERTY(QString InfixEquation READ getInfixEquation WRITE setInfixEquation)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CalculatedArray)
    Q_PROPERTY(DataArrayPath CalculatedArray READ getCalculatedArray WRITE setCalculatedArray)

    SIMPL_FILTER_PARAMETER(AngleUnits, Units)
    Q_PROPERTY(AngleUnits Units READ getUnits WRITE setUnits)

    virtual ~ArrayCalculator();

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
     * @brief writeFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

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
    ArrayCalculator();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    QMap<QString, CalculatorItem::Pointer>                      m_SymbolMap;
    QStack<ICalculatorArray::Pointer>                           m_ExecutionStack;

    QVector<CalculatorItem::Pointer> parseInfixEquation(QString equation);
    QVector<CalculatorItem::Pointer> toRPN(QVector<CalculatorItem::Pointer> infixEquation);

    void checkForAmbiguousArrayName(QString itemStr, QString warningMsg);

    ArrayCalculator(const ArrayCalculator&); // Copy Constructor Not Implemented
    void operator=(const ArrayCalculator&); // Operator '=' Not Implemented
};

#endif /* _ArrayCalculator_H_ */

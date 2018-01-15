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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "util/ICalculatorArray.h"

/**
 * @brief The ArrayCalculator class. See [Filter documentation](@ref createdatacontainer) for details.
 */
class SIMPLib_EXPORT ArrayCalculator : public AbstractFilter
{
    Q_OBJECT

  public:
    enum AngleUnits
    {
      Degrees,
      Radians
    };

    Q_ENUMS(AngleUnits)

    SIMPL_SHARED_POINTERS(ArrayCalculator)
    SIMPL_STATIC_NEW_MACRO(ArrayCalculator)
     SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ArrayCalculator, AbstractFilter)

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
    ArrayCalculator();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


  private:
    QMap<QString, CalculatorItem::Pointer>                      m_SymbolMap;
    QStack<ICalculatorArray::Pointer>                           m_ExecutionStack;

    void createSymbolMap();

    QVector<CalculatorItem::Pointer> parseInfixEquation();
    QVector<CalculatorItem::Pointer> toRPN(QVector<CalculatorItem::Pointer> infixEquation);

    void checkForAmbiguousArrayName(QString itemStr, QString warningMsg);

    /**
     * @brief getRegularExpressionMatches
     * @return
     */
    QVector<QString> getRegularExpressionMatches();

    /**
     * @brief parseNumberArray
     * @param token
     * @param parsedInfix
     */
    void parseNumericValue(QString token, QVector<CalculatorItem::Pointer>& parsedInfix, double number);

    /**
     * @brief parseMinusSign
     * @param strItem
     * @param parsedInfix
     * @param loopIdx
     */
    void parseMinusSign(QString strItem, QVector<CalculatorItem::Pointer>& parsedInfix, int loopIdx);

    /**
     * @brief parseIndexOperator
     * @param token
     * @param parsedInfix
     * @param number
     */
    bool parseIndexOperator(QString token, QVector<CalculatorItem::Pointer>& parsedInfix);

    /**
     * @brief parseCommaOperator
     * @param token
     * @param parsedInfix
     * @return
     */
    bool parseCommaOperator(QString token, QVector<CalculatorItem::Pointer>& parsedInfix);

    /**
     * @brief parseArray
     * @param token
     * @param parsedInfix
     * @param selectedAM
     * @return
     */
    bool parseArray(QString token, QVector<CalculatorItem::Pointer>& parsedInfix, AttributeMatrix::Pointer selectedAM);

    ArrayCalculator(const ArrayCalculator&) = delete; // Copy Constructor Not Implemented
    void operator=(const ArrayCalculator&) = delete;  // Operator '=' Not Implemented
};

#endif /* _ArrayCalculator_H_ */

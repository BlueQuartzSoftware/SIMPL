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

#ifndef _comparisonSelectionAdvancedWidget_h_
#define _comparisonSelectionAdvancedWidget_h_

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSignalMapper>

#include <QtWidgets/QWidget>

#include "SVWidgetsLib/QtSupport/QtSFaderWidget.h"

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/ComparisonSelectionAdvancedFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"

#include "SIMPLib/Filtering/AbstractComparison.h"
#include "SIMPLib/Filtering/ComparisonInputsAdvanced.h"

#include "ui_ComparisonSelectionAdvancedWidget.h"

/**
* @brief The ComparisonSelectionAdvancedWidget class is used to display, edit, and add AbstractComparisons
* for the ComparisonSelectionAdvancedFilterParameter class.
*/
class ComparisonSelectionAdvancedWidget : public FilterParameterWidget, private Ui::ComparisonSelectionAdvancedWidget
{
  Q_OBJECT

  public:
    ComparisonSelectionAdvancedWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);
    virtual ~ComparisonSelectionAdvancedWidget();

    /**
    * @brief setupGui Initializes some of the GUI elements with selections or other GUI related items
    */
    virtual void setupGui();

    /**
    * @brief eventFilter
    * @param obj
    * @param event
    * @return
    */
    bool eventFilter(QObject* obj, QEvent* event);

  public slots:
    /**
    * @brief beforePreflight
    */
    void beforePreflight();

    /**
    * @brief afterPreflight
    */
    void afterPreflight();

    /**
    * @brief filterNeedsInputParameters
    * @param filter
    */
    void filterNeedsInputParameters(AbstractFilter* filter);

    /**
    * @brief attributeMatrixSelected
    * @param path
    */
    void attributeMatrixSelected(QString path);

  protected:
    /**
    * @brief setComparisons
    * @param comparisons
    */
    virtual void setComparisons(QVector<AbstractComparison::Pointer> comparisons);

    /**
    * @brief getComparisonInputs
    * @return
    */
    ComparisonInputsAdvanced getComparisonInputs();

    /**
    * @brief generateAttributeArrayList
    * @param currentDCName
    * @param currentAttrMatName
    * @return
    */
    QStringList generateAttributeArrayList(const QString& currentDCName, const QString &currentAttrMatName);

    /**
    * @brief checkStringValues
    * @param curDcName
    * @param filtDcName
    * @return
    */
    QString checkStringValues(QString curDcName, QString filtDcName);

    /**
    * @brief populateButtonText
    */
    void populateButtonText();

    /**
    * @brief Sets the AttributeMatrixButton text before any attribute matrixes are loaded.
    */
    void presetAttributeMatrix(DataArrayPath path);

  protected slots:
    /**
    * @brief widgetChanged
    * @param text
    */
    void widgetChanged(const QString& text);

    /**
    * @brief setSelectedPath
    * @param path
    */
    void setSelectedPath(QString path);

    /**
    * @brief setSelectedPath
    * @param amPath
    */
    void setSelectedPath(DataArrayPath amPath);

    /**
    * @brief Handles changes to an affected DataArrayPath
    * @param propertyName
    * @param renamePath
    */
    void updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath);

    /**
    * @brief Handles AttributeMatrix path changes through DataArrayPathSelectionWidget
    */
    void attributeMatrixUpdated();

  private:
    DataContainerArrayProxy m_DcaProxy;

    DataArrayPath m_presetPath;

    bool m_DidCausePreflight;

    ComparisonSelectionAdvancedFilterParameter* m_FilterParameter;
};

#endif

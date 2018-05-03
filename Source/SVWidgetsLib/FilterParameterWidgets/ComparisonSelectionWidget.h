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

#ifndef _comparisonselectionwidget_h_
#define _comparisonselectionwidget_h_


#include <QtCore/QObject>
#include <QtCore/QPointer>

#include <QtWidgets/QWidget>


#include "SVWidgetsLib/QtSupport/QtSFaderWidget.h"

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/ComparisonSelectionFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"



#include "ComparisonSelectionTableModel.h"

#include "ui_ComparisonSelectionWidget.h"

#define OLD_GUI 0

class QSignalMapper;

/**
 * @class ComparisonSelectionWidget ComparisonSelectionWidget.h PipelineBuilder/UI/ComparisonSelectionWidget.h
 * @brief This class
 *
 * @date Jan 30, 2011
 * @version 1.0
 */
class ComparisonSelectionWidget : public FilterParameterWidget, private Ui::ComparisonSelectionWidget
{

    Q_OBJECT
  public:
    ComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);
    virtual ~ComparisonSelectionWidget();

    enum ArrayListType
    {
      CellListType,
      FeatureListType,
      EnsembleListType,
      VertexListType,
      EdgeListType,
      FaceListType,
    };

    SIMPL_INSTANCE_PROPERTY(ArrayListType, ArrayListType)
    SIMPL_INSTANCE_PROPERTY(bool, ShowOperators)

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

    void attributeMatrixSelected(QString path);

  signals:
    /**
     * @brief errorSettingFilterParameter
     * @param msg
     */
    void errorSettingFilterParameter(const QString& msg);

    /**
     * @brief parametersChanged
     */
    void parametersChanged();

  protected:
    /**
     * @brief setComparisons
     * @param comparisons
     */
    virtual void setComparisons(QVector<ComparisonInput_t> comparisons);

    /**
     * @brief getComparisonInputs
     * @return
     */
    ComparisonInputs getComparisonInputs();

    /**
     * @brief createSelectionMenu
     */
    void createSelectionMenu();

    /**
     * @brief populateAttributeMatrixList
     */
    void populateAttributeMatrixList();

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


  protected slots:
    /**
     * @brief on_addComparison_clicked
     */
    void on_addComparison_clicked();

    /**
     * @brief on_removeComparison_clicked
     */
    void on_removeComparison_clicked();

    /**
     * @brief tableDataWasChanged
     * @param topLeft
     * @param bottomRight
     */
    void tableDataWasChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

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

    void setSelectedPath(DataArrayPath amPath);

    /**
     * @brief Handles changes to an affected DataArrayPath
     * @param propertyName
     * @param renamePath
     */
    void updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath);

  private:
    DataContainerArrayProxy m_DcaProxy;

    bool m_DidCausePreflight;

    QPointer<QSignalMapper> m_MenuMapper;

    ComparisonSelectionTableModel* m_ComparisonSelectionTableModel;

    ComparisonSelectionFilterParameter* m_FilterParameter;



    /**
     * @brief createComparisonModel
     * @return
     */
    ComparisonSelectionTableModel* createComparisonModel();

    ComparisonSelectionWidget(const ComparisonSelectionWidget&) = delete; // Copy Constructor Not Implemented
    void operator=(const ComparisonSelectionWidget&) = delete;            // Move assignment Not Implemented
};




#endif /* _ComparisonSelectionWidget_H_ */


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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>

#include "SVWidgetsLib/QtSupport/QtSFaderWidget.h"

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

#include "SVWidgetsLib/ui_FilterInputWidget.h"

class SVPipelineFilterWidget;
class PipelineFilterObject;

/**
 * @brief The FilterInputWidget class
 */
class SVWidgetsLib_EXPORT FilterInputWidget : public QWidget
{
    Q_OBJECT

  public:
    FilterInputWidget(AbstractFilter::Pointer filter, QWidget* parent = nullptr);
    ~FilterInputWidget() override;

    virtual void setupGui();

    void setFilterIndex(const QString& index);

    bool eventFilter(QObject* o, QEvent* e) override;

    void toRunningState();
    void toIdleState();

    QWidget* getVariablesTabContentsWidget();

  public slots :
    void clearInputWidgets();
    void showHelp();
    void displayFilterParameters(AbstractFilter::Pointer filter);

  signals:
    void filterParametersChanged(bool preflight = true);
    void errorSettingFilterParameter(const QString &text);

    void viewPathsMatchingReqs(DataContainerSelectionFilterParameter::RequirementType dcReqs);
    void viewPathsMatchingReqs(AttributeMatrixSelectionFilterParameter::RequirementType amReqs);
    void viewPathsMatchingReqs(DataArraySelectionFilterParameter::RequirementType daReqs);
    void endViewPaths();
    void endDataStructureFiltering();
    void filterPath(DataArrayPath path);
    void endPathFiltering();
    void applyPathToFilteringParameter(DataArrayPath path);
    void dataArrayPathSelectionLocked(QToolButton* selection);
    void dataArrayPathSelectionUnlocked(QToolButton* button);

  private slots:
    void fadeInWidget(QWidget* widget);
    void fadeOutWidget(QWidget* widget);
    
    void getEmittedPathReqs(DataContainerSelectionFilterParameter::RequirementType dcReqs);
    void getEmittedPathReqs(AttributeMatrixSelectionFilterParameter::RequirementType amReqs);
    void getEmittedPathReqs(DataArraySelectionFilterParameter::RequirementType daReqs);
    void emitFilterPath(DataArrayPath path);

  private:
    QSharedPointer<Ui::FilterInputWidget> m_Ui;
    QString                           m_FilterClassName;
    QPointer<QtSFaderWidget>          m_FaderWidget;
    bool                              m_AdvFadedOut;
    QString                           m_BrandingLabel;
    static QString                    m_OpenDialogLastFilePath;

    QVBoxLayout*                      m_VariablesVerticalLayout = nullptr;
    QWidget*                          m_VariablesWidget = nullptr;
    QMap<QString, QWidget*>           m_PropertyToWidget;

    /**
     * @brief layoutWidgets
     */
    void layoutWidgets(AbstractFilter* filter);

    /**
     * @brief validateFileSystemFilterParameter
     * @param option
     */
    void validateFileSystemFilterParameter(FilterParameter* parameter, AbstractFilter* filter);

    /**
     * @brief linkConditionalWidgets
     * @param filterParameters
     */
    void linkConditionalWidgets(QVector<FilterParameter::Pointer>& filterParameters);

  public:
    FilterInputWidget(const FilterInputWidget&) = delete; // Copy Constructor Not Implemented
    void operator=(const FilterInputWidget&) = delete;    // Move assignment Not Implemented
};


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

#ifndef _linkeddatacontainerselectionwidget_h_
#define _linkeddatacontainerselectionwidget_h_

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/QtSupport/QtSFaderWidget.h"
#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"

#include "SIMPLib/FilterParameters/LinkedDataContainerSelectionFilterParameter.h"

#include "ui_LinkedDataContainerSelectionWidget.h"

class QSignalMapper;

/**
* @brief
* @author
* @version
*/
class SVWidgetsLib_EXPORT LinkedDataContainerSelectionWidget : public FilterParameterWidget, private Ui::LinkedDataContainerSelectionWidget
{
    Q_OBJECT
  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    LinkedDataContainerSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

    LinkedDataContainerSelectionWidget(QWidget* parent = nullptr);

    virtual ~LinkedDataContainerSelectionWidget();

    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

    /**
     * @brief checkStringValues
     * @param current
     * @param filt
     * @return
     */
    QString checkStringValues(QString current, QString filtDcName);

    /**
     * @brief initializeWidget
     * @param parameter
     * @param filter
     */
    void initializeWidget(FilterParameter* parameter, AbstractFilter* filter);

    /**
     * @brief eventFilter
     * @param obj
     * @param event
     * @return
     */
    bool eventFilter(QObject* obj, QEvent* event);

  public slots:
    void widgetChanged();
    void beforePreflight();
    void afterPreflight();
    void filterNeedsInputParameters(AbstractFilter* filter);
    void dataContainerSelected(QString path);

  signals:
    void conditionalPropertyChanged(int);

  protected:
    /**
     * @brief createSelectionMenu
     */
    void createSelectionMenu();

  private:
    bool m_DidCausePreflight;
    LinkedDataContainerSelectionFilterParameter* m_FilterParameter;
    QPointer<QSignalMapper> m_MenuMapper;
    void setSelectedPath(QString path);
    void setSelectedPath(DataArrayPath dcPath);

    LinkedDataContainerSelectionWidget(const LinkedDataContainerSelectionWidget&) = delete; // Copy Constructor Not Implemented
    void operator=(const LinkedDataContainerSelectionWidget&) = delete;                     // Move assignment Not Implemented
};

#endif /* _linkeddatacontainerselectionwidget_h_ */



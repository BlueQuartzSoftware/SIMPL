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

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/DataContainerArrayProxyFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"


#include "SVWidgetsLib/ui_DataContainerArrayProxyWidget.h"


/**
* @brief
* @author
* @version
*/
class SVWidgetsLib_EXPORT DataContainerArrayProxyWidget : public FilterParameterWidget, private Ui::DataContainerArrayProxyWidget
{
    Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    DataContainerArrayProxyWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

    /**
     * @brief DataContainerArrayProxyWidget
     * @param parent
     */
    DataContainerArrayProxyWidget(QWidget* parent = nullptr);

    ~DataContainerArrayProxyWidget() override;

    void initialize(FilterParameter* parameter, AbstractFilter* filter = nullptr);

    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui() override;

    void setFilterParameter(FilterParameter* value);
    FilterParameter* getFilterParameter() const;

  public slots:
    void beforePreflight();
    void afterPreflight();
    void filterNeedsInputParameters(AbstractFilter* filter);
    void itemChanged(QListWidgetItem* item);
    void itemSelected(QListWidgetItem* item);

  protected:
    QString getDataContainerName();
    QString getAttrMatrixName();
    
    void selectDataContainer(QString name);
    void selectAttributeMatrix(QString name);

    DataContainerProxy& getDataContainerProxy();
    AttributeMatrixProxy& getAttributeMatrixProxy();

    void applyDataContainerArrayProxy(DataContainerArrayProxy proxy);
    void applyDataContainerProxy();
    void applyAttributeMatrixProxy();

    //void updateWidgetFromProxy(DataContainerArrayProxy& proxy);
    void updateProxyFromProxy(DataContainerArrayProxy& current, DataContainerArrayProxy& incoming);

    void updateProxyChecked(QListWidgetItem* item, bool updateSelectAll = true);
    QList<QListWidgetItem*> getChildItems(QListWidgetItem* item, QList<QListWidgetItem*> otherItems);

    bool shouldStrikeOutItem(QListWidgetItem* item);
    Qt::CheckState updateSelectAllState(QListWidget* listWidget);
    QList<QListWidgetItem*> getAllItems(QListWidget* listWidget, bool ignoreSelectAll = true);

    void checkAllItems(QListWidget* listWidget, Qt::CheckState state);

  protected slots:
    void selectAllDataContainersClicked(bool checked);
    void selectAllAttributeMatricesClicked(bool checked);
    void selectAllDataArraysClicked(bool checked);
    void updateDataArrayPath(QString propertyName, DataArrayPath::RenameType renamePath);

  private:

    DataContainerArrayProxyFilterParameter*  m_FilterParameter;
    DataContainerArrayProxy m_DcaProxy;
    QString m_DcName;
    QString m_AmName;
    bool m_DidCausePreflight;
    DataContainerProxy m_EmptyDcProxy;
    AttributeMatrixProxy m_EmptyAmProxy;

    void toggleStrikeOutFont(QListWidgetItem* item, Qt::CheckState state);

    DataContainerArrayProxyWidget(const DataContainerArrayProxyWidget&) = delete; // Copy Constructor Not Implemented
    void operator=(const DataContainerArrayProxyWidget&) = delete;                // Move assignment Not Implemented
};




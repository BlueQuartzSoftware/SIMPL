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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtWidgets/QWidget>

#include "SVWidgetsLib/QtSupport/QtSFaderWidget.h"

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SVWidgetsLib/SVWidgetsLib.h"

#include "SVWidgetsLib/ui_AttributeMatrixCreationWidget.h"

class QSignalMapper;

/**
 * @brief
 * @author
 * @version
 */
class SVWidgetsLib_EXPORT AttributeMatrixCreationWidget : public FilterParameterWidget, private Ui::AttributeMatrixCreationWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  AttributeMatrixCreationWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

  AttributeMatrixCreationWidget(QWidget* parent = nullptr);

  ~AttributeMatrixCreationWidget() override;

  typedef QSharedPointer<QMenu> QMenuPtr;

  /**
   * @brief This method does additional GUI widget connections
   */
  void setupGui() override;

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
  bool eventFilter(QObject* obj, QEvent* event) override;

public Q_SLOTS:
  void beforePreflight();
  void afterPreflight();
  void filterNeedsInputParameters(AbstractFilter* filter);

  void dataContainerSelected(QString path);

protected:
  /**
   * @brief setSelectedPath
   * @param dcName
   * @param attrMatName
   * @param attrArrName
   */
  void setSelectedPath(QString dcName, QString attrMatName, QString attrArrName);

protected Q_SLOTS:
  void updateDataArrayPath(QString propertyName, const DataArrayPath::RenameType& renamePath);

private Q_SLOTS:

private:
  bool m_DidCausePreflight;

  AttributeMatrixCreationFilterParameter* m_FilterParameter;

  void setSelectedPath(QString path);

  void setSelectedPath(DataArrayPath dcPath);

public:
  AttributeMatrixCreationWidget(const AttributeMatrixCreationWidget&) = delete;            // Copy Constructor Not Implemented
  AttributeMatrixCreationWidget(AttributeMatrixCreationWidget&&) = delete;                 // Move Constructor Not Implemented
  AttributeMatrixCreationWidget& operator=(const AttributeMatrixCreationWidget&) = delete; // Copy Assignment Not Implemented
  AttributeMatrixCreationWidget& operator=(AttributeMatrixCreationWidget&&) = delete;      // Move Assignment Not Implemented
};

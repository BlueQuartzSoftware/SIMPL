/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QSharedPointer>
#include <QtWidgets/QWidget>

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SVWidgetsLib/SVWidgetsLib.h"

class AbstractFilter;
class DataContainerGridModel;
class DataContainerGridSelectionFilterParameter;

namespace Ui
{
class DataContainerGridSelectionWidget;
}

/**
 * @brief
 * @author
 * @version
 */
class SVWidgetsLib_EXPORT DataContainerGridSelectionWidget : public FilterParameterWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  DataContainerGridSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

  DataContainerGridSelectionWidget(QWidget* parent = nullptr);

  ~DataContainerGridSelectionWidget() override;

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
  QString checkStringValues(const QString& current, const QString& filtDcName);

  /**
   * @brief initializeWidget
   * @param parameter
   * @param filter
   */
  void initializeWidget(FilterParameter* parameter, AbstractFilter* filter);

public Q_SLOTS:
  void beforePreflight();
  void afterPreflight();
  void filterNeedsInputParameters(AbstractFilter* filter);

protected:
  void updateDataArrayPath(const QString& propertyName, const DataArrayPath::RenameType& renamePath);
  void updateDimensions();
  void checkDepth();
  void increaseDepth();
  void decreaseDepth();

private:
  QSharedPointer<Ui::DataContainerGridSelectionWidget> m_Ui;
  bool m_DidCausePreflight;

  DataContainerGridSelectionFilterParameter* m_FilterParameter;
  DataContainerGridModel* m_GridModel = nullptr;

public:
  DataContainerGridSelectionWidget(const DataContainerGridSelectionWidget&) = delete;            // Copy Constructor Not Implemented
  DataContainerGridSelectionWidget(DataContainerGridSelectionWidget&&) = delete;                 // Move Constructor Not Implemented
  DataContainerGridSelectionWidget& operator=(const DataContainerGridSelectionWidget&) = delete; // Copy Assignment Not Implemented
  DataContainerGridSelectionWidget& operator=(DataContainerGridSelectionWidget&&) = delete;      // Move Assignment Not Implemented
};

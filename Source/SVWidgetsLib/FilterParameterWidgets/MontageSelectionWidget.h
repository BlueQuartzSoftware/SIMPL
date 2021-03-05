/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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

#include <QtCore/QString>
#include <QtWidgets/QWidget>

#include "SIMPLib/Utilities/MontageSelection.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SVWidgetsLib/SVWidgetsLib.h"

class MontageSelectionFilterParameter;
namespace Ui
{
class MontageSelectionWidget;
}

/**
 * @brief
 * @author
 * @version
 */
class SVWidgetsLib_EXPORT MontageSelectionWidget : public FilterParameterWidget
{
  Q_OBJECT

public:
  MontageSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);
  MontageSelectionWidget(QWidget* parent = nullptr);

  /**
   * @brief Creates and returns a MontageSelection based on the the current values.
   * @return
   */
  MontageSelection getMontageSelection() const;

public Q_SLOTS:
  void beforePreflight();
  void afterPreflight();
  void filterNeedsInputParameters(AbstractFilter* filter);

protected:
  /**
   * @brief Helper method for triggering the preflight signal and toggling
   * the values consistently based on signals.
   */
  void causePreflight();

  /**
   * @brief Returns the corresponding QIcon for the given DataContainer name.
   * The icon returned is determined from the presence of the given DataContainer
   * in the given filter's DataContainerArray.
   * @param dcName
   * @return
   */
  QIcon getDcIcon(const QString& dcName) const;

private:
  /**
   * @brief Performs initial setup for signals.
   */
  void setupGui();

  /**
   * @brief Initializes the widget for the given FilterParameter and AbstractFilter.
   * @param parameter
   * @param filter
   */
  void initializeWidget(FilterParameter* parameter, AbstractFilter* filter);

  /**
   * @brief Populate the DataContainer table with names based on the current values.
   */
  void populateDataContainerTable();

  bool m_DidCausePreflight;
  QSharedPointer<Ui::MontageSelectionWidget> m_Ui;

  MontageSelectionFilterParameter* m_FilterParameter;

public:
  MontageSelectionWidget(const MontageSelectionWidget&) = delete;            // Copy Constructor Not Implemented
  MontageSelectionWidget(MontageSelectionWidget&&) = delete;                 // Move Constructor Not Implemented
  MontageSelectionWidget& operator=(const MontageSelectionWidget&) = delete; // Copy Assignment Not Implemented
  MontageSelectionWidget& operator=(MontageSelectionWidget&&) = delete;      // Move Assignment Not Implemented
};

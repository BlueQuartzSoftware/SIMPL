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

#include <QtCore/QModelIndex>
#include <QtCore/QScopedPointer>

#include <QtWidgets/QDialog>

#include "SVWidgetsLib/Dialogs/ColorPresetsDialogTableModel.h"
#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/SVControlWidgets.h"


class QModelIndex;
namespace Json
{
  class Value;
}

/// ColorPresetsDialog is the dialog used by to show the user with a choice of color
/// maps/opacity maps/presets to choose from. The Dialog can be customized to
/// show only indexed (or non-indexed) color maps using ColorPresetsDialog::Modes.
/// Application code should observe the ColorPresetsDialog::applyPreset() signal to
/// perform the applying of the preset to a specific transfer function proxy.
/// This class works with vtkSMTransferFunctionPresets, which acts as the preset
/// manager for the application with support to inspect existing presets as well
/// as updating them.
class SVWidgetsLib_EXPORT ColorPresetsDialog : public SVDialog
{
  Q_OBJECT

public:
  ColorPresetsDialog(QWidget* parent=0);
  ~ColorPresetsDialog() override;

  /**
   * @brief setCurrentPreset
   * @param presetName
   */
  void setCurrentPreset(const char* presetName, bool applyPreset = true);

  /**
   * @brief currentPreset
   * @return
   */
  const QJsonObject currentPreset();

  /**
   * @brief currentPresetIndex
   * @return
   */
  int currentPresetIndex();

signals:
  void applyPreset(const QJsonObject& preset, const QPixmap& pixmap);

protected slots:
  void updateEnabledStateForSelection();
  void updateForSelectedIndex(const QModelIndex& idx);
  void triggerApply();
  void cancel();

private:
  class pqInternals;
  const QScopedPointer<pqInternals> Internals;

  ColorPresetsDialog(const ColorPresetsDialog&) = delete; // Copy Constructor Not Implemented
  void operator=(const ColorPresetsDialog&) = delete;     // Move assignment Not Implemented
};


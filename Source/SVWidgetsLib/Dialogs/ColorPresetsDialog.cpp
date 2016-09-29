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
#include "ColorPresetsDialog.h"

#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QSize>
#include <QtCore/QSortFilterProxyModel>

#include <QtGui/QPixmap>

#include "SVWidgetsLib/Dialogs/ColorPresetsDialogTableModel.h"

//#include "pqFileDialog.h"
//#include "pqPropertiesPanel.h"
//#include "vtkNew.h"
//#include "vtkSMTransferFunctionPresets.h"
//#include "vtkSMTransferFunctionProxy.h"

#include "ui_ColorPresetsDialog.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorPresetsDialog::ColorPresetsDialog(QWidget* parentObject, ColorPresetsDialog::Modes mode) :
  QDialog(parentObject),
  Internals(new ColorPresetsDialog::pqInternals(mode, this))
{
  const Ui::ColorPresetsDialog &ui = this->Internals->Ui;
  this->connect(ui.gradients->selectionModel(),
    SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
    SLOT(updateEnabledStateForSelection()));
  this->updateEnabledStateForSelection();

  this->connect(ui.remove, SIGNAL(clicked()), SLOT(removePreset()));
  this->connect(ui.gradients, SIGNAL(doubleClicked(const QModelIndex&)),
    SLOT(triggerApply(const QModelIndex&)));
  this->connect(ui.apply, SIGNAL(clicked()), SLOT(triggerApply()));
  this->connect(ui.importPresets, SIGNAL(clicked()), SLOT(importPresets()));
  this->connect(ui.exportPresets, SIGNAL(clicked()), SLOT(exportPresets()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorPresetsDialog::~ColorPresetsDialog()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ColorPresetsDialog::setCurrentPreset(const char* presetName)
{
  pqInternals& internals = (*this->Internals);
  QModelIndex idx = internals.Model->indexFromName(presetName);
  idx = internals.ProxyModel->mapFromSource(idx);
  if (idx.isValid())
    {
    internals.Ui.gradients->selectionModel()->setCurrentIndex(
      idx, QItemSelectionModel::ClearAndSelect);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ColorPresetsDialog::updateEnabledStateForSelection()
{
  const Ui::ColorPresetsDialog &ui = this->Internals->Ui;
  QModelIndexList selectedRows = ui.gradients->selectionModel()->selectedRows();
  if (selectedRows.size() == 1)
    {
    this->updateForSelectedIndex(selectedRows[0]);
    }
  else
    {
    ui.colors->setEnabled(false);
    ui.usePresetRange->setEnabled(false);
    ui.opacities->setEnabled(false);
    ui.annotations->setEnabled(false);
    ui.apply->setEnabled(false);
    ui.exportPresets->setEnabled(selectedRows.size() > 0);

    bool isEditable = true;
    foreach (const QModelIndex &idx, selectedRows)
      {
      isEditable &= this->Internals->ProxyModel->flags(idx).testFlag(Qt::ItemIsEditable);
      }
    ui.remove->setEnabled((selectedRows.size() > 0) && isEditable);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ColorPresetsDialog::updateForSelectedIndex(const QModelIndex& proxyIndex)
{
  // update "options" based on what's available.
  const pqInternals& internals = *this->Internals;
  QModelIndex idx = internals.ProxyModel->mapToSource(proxyIndex);
  const Json::Value& preset = internals.Model->Presets->GetPreset(idx.row());
  Q_ASSERT(preset.empty() == false);

  const Ui::ColorPresetsDialog &ui = internals.Ui;

  ui.colors->setEnabled(true);
  ui.usePresetRange->setEnabled(!internals.Model->Presets->GetPresetHasIndexedColors(preset));
  ui.opacities->setEnabled(internals.Model->Presets->GetPresetHasOpacities(preset));
  ui.annotations->setEnabled(internals.Model->Presets->GetPresetHasAnnotations(preset));
  ui.apply->setEnabled(true);
  ui.exportPresets->setEnabled(true);
  ui.remove->setEnabled(internals.Model->flags(idx).testFlag(Qt::ItemIsEditable));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ColorPresetsDialog::triggerApply(const QModelIndex& _proxyIndex)
{
  const pqInternals& internals = *this->Internals;
  const Ui::ColorPresetsDialog &ui = this->Internals->Ui;

  const QModelIndex proxyIndex =  _proxyIndex.isValid()?
    _proxyIndex : ui.gradients->selectionModel()->currentIndex();
  QModelIndex idx = internals.ProxyModel->mapToSource(proxyIndex);
  const Json::Value& preset = internals.Model->Presets->GetPreset(idx.row());
  Q_ASSERT(preset.empty() == false);
  emit this->applyPreset(preset);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const Json::Value& ColorPresetsDialog::currentPreset()
{
  const pqInternals& internals = *this->Internals;
  const Ui::ColorPresetsDialog &ui = this->Internals->Ui;
  QModelIndex proxyIndex = ui.gradients->selectionModel()->currentIndex();
  if (proxyIndex.isValid())
    {
    QModelIndex idx = internals.ProxyModel->mapToSource(proxyIndex);
    const Json::Value& preset = internals.Model->Presets->GetPreset(idx.row());
    Q_ASSERT(preset.empty() == false);
    return preset;
    }

  static Json::Value nullValue;
  return nullValue;
}

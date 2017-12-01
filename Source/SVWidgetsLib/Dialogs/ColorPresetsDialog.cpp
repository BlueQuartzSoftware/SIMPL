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

#include "ui_ColorPresetsDialog.h"

class ColorPresetsDialog::pqInternals
{
public:
  Ui::ColorPresetsDialog Ui;
  QSharedPointer<ColorPresetsDialogTableModel> Model;

  pqInternals(ColorPresetsDialog* self)
    : Model(new ColorPresetsDialogTableModel(self))
  {
    this->Ui.setupUi(self);
    this->Ui.gradients->setModel(this->Model.data());

  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorPresetsDialog::ColorPresetsDialog(QWidget* parentObject) :
  QDialog(parentObject),
  Internals(new ColorPresetsDialog::pqInternals(this))
{
  const Ui::ColorPresetsDialog &ui = this->Internals->Ui;
  this->connect(ui.gradients->selectionModel(),
                SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                SLOT(updateEnabledStateForSelection()));
  this->updateEnabledStateForSelection();

  this->connect(ui.okBtn, SIGNAL(clicked()), SLOT(triggerApply()));
  this->connect(ui.cancelBtn, SIGNAL(clicked()), SLOT(cancel()));

  if (ui.gradients->selectionModel()->selectedRows().size() <= 0)
  {
    ui.okBtn->setDisabled(true);
  }
  else
  {
    ui.okBtn->setEnabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorPresetsDialog::~ColorPresetsDialog() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ColorPresetsDialog::setCurrentPreset(const char* presetName, bool applyPreset)
{
  pqInternals& internals = (*this->Internals);
  QModelIndex idx = internals.Model->indexFromName(presetName);
  if (idx.isValid())
  {
    internals.Ui.gradients->selectionModel()->setCurrentIndex(
          idx, QItemSelectionModel::ClearAndSelect);

    if (applyPreset)
    {
      triggerApply();
    }
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
    ui.okBtn->setEnabled(false);

    bool isEditable = true;
    foreach (const QModelIndex &idx, selectedRows)
    {
      isEditable &= this->Internals->Model->flags(idx).testFlag(Qt::ItemIsEditable);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ColorPresetsDialog::updateForSelectedIndex(const QModelIndex& idx)
{
  // update "options" based on what's available.
  const pqInternals& internals = *this->Internals;
  const QJsonObject& preset = internals.Model->getPresets()->GetPreset(idx.row());
  Q_ASSERT(preset.isEmpty() == false);

  const Ui::ColorPresetsDialog &ui = internals.Ui;

  ui.okBtn->setEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ColorPresetsDialog::triggerApply()
{
  const pqInternals& internals = *this->Internals;
  const Ui::ColorPresetsDialog &ui = this->Internals->Ui;

  const QModelIndex index =  ui.gradients->selectionModel()->currentIndex();
  const QJsonObject& preset = internals.Model->getPresets()->GetPreset(index.row());
  QPixmap presetPixmap = internals.Model->data(index, Qt::DecorationRole).value<QPixmap>();
  Q_ASSERT(preset.isEmpty() == false);
  emit applyPreset(preset, presetPixmap);
  done(1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ColorPresetsDialog::cancel()
{
  close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QJsonObject ColorPresetsDialog::currentPreset()
{
  const pqInternals& internals = *this->Internals;
  const Ui::ColorPresetsDialog &ui = this->Internals->Ui;
  QModelIndex idx = ui.gradients->selectionModel()->currentIndex();
  if (idx.isValid())
  {
    const QJsonObject& preset = internals.Model->getPresets()->GetPreset(idx.row());
    Q_ASSERT(preset.empty() == false);
    return preset;
  }

  return QJsonObject();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ColorPresetsDialog::currentPresetIndex()
{
  const Ui::ColorPresetsDialog &ui = this->Internals->Ui;
  QModelIndex idx = ui.gradients->selectionModel()->currentIndex();
  if (idx.isValid())
  {
    return idx.row();
  }

  return -1;
}

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

#include "ColorPresetsDialogTableModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorPresetsDialogTableModel::ColorPresetsDialogTableModel(QObject* parentObject) :
  QAbstractTableModel(parentObject)
{
  Presets = ColorPresets::New();
  this->Pixmaps.reserve(this->Presets->GetNumberOfPresets());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorPresetsDialogTableModel::~ColorPresetsDialogTableModel()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex ColorPresetsDialogTableModel::indexFromName(const char* presetName) const
{
  if (!presetName)
  {
    return QModelIndex();
  }

  for (int cc=0, max=this->rowCount(QModelIndex()); cc < max; ++cc)
  {
    if (this->Presets->GetPresetName(cc) == presetName)
    {
      return this->index(cc, 0, QModelIndex());
    }
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ColorPresetsDialogTableModel::rowCount(const QModelIndex& idx) const
{
  return idx.isValid()? 0 : static_cast<int>(this->Presets->GetNumberOfPresets());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ColorPresetsDialogTableModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant ColorPresetsDialogTableModel::data(const QModelIndex &idx, int role) const
{
  if (!idx.isValid() || idx.model() != this)
  {
    return QVariant();
  }

  switch (role)
  {
  case Qt::DisplayRole:
  case Qt::ToolTipRole:
  case Qt::StatusTipRole:
  case Qt::EditRole:
    return this->Presets->GetPresetName(idx.row());

  case Qt::DecorationRole:
    return this->pixmap(idx.row());

  case Qt::UserRole:
    return this->Presets->GetPresetHasIndexedColors(idx.row());

  }
  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags ColorPresetsDialogTableModel::flags(const QModelIndex &idx) const
{
  Qt::ItemFlags flgs = QAbstractTableModel::flags(idx);
  return flgs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant ColorPresetsDialogTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_UNUSED(section)

  if (orientation == Qt::Vertical)
  {
    return QVariant();
  }

  if (role == Qt::DisplayRole)
  {
    return "Presets";
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QPixmap& ColorPresetsDialogTableModel::pixmap(int row) const
{
  Pixmaps.reserve(row+1);

  // grow Pixmaps if needed.
  for (int cc=this->Pixmaps.size(); cc <= row; cc++)
  {
    Pixmaps.push_back(QPixmap());
  }

  if (Pixmaps[row].isNull())
  {
    Pixmaps[row] = Presets->getPixmapFromPreset(row);
  }

  return Pixmaps[row];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorPresets::Pointer ColorPresetsDialogTableModel::getPresets()
{
  return Presets;
}

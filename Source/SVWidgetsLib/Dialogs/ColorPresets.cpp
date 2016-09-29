/*=========================================================================

  Program:   ParaView
  Module:    ColorPresets.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "ColorPresets.h"

#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QJsonParseError>

//#include "vtkSMTransferFunctionPresetsBuiltin.h"

class ColorPresets::vtkInternals
{
public:
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  vtkInternals()
  {

  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  ~vtkInternals()
  {

  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  QJsonArray GetPresets()
  {
    if (this->Presets.empty())
    {
      this->LoadPresets();
    }
    return this->Presets;
  }

private:
  QJsonArray Presets;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void LoadPresets()
  {
    if (this->Presets.empty() == false) { return; }

    QFile inputFile(":/CoreResources/ColorTablePresets/ColorTablePresets.json");
    QJsonObject rootObj;
    if(inputFile.open(QIODevice::ReadOnly))
    {
      QJsonParseError parseError;
      QByteArray byteArray = inputFile.readAll();
      QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
      if(parseError.error != QJsonParseError::NoError)
      {
        // "Failed to parse presets file" error
        return;
      }
      rootObj = doc.object();
    }
    else
    {
      // "Failed to open presets file" error
      return;
    }

    //this->Presets.insert(this->BuiltinPresets.end(), value.begin(), value.end());
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorPresets::ColorPresets()
  : Internals(new ColorPresets::vtkInternals())
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorPresets::~ColorPresets()
{
  delete this->Internals;
  this->Internals = NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ColorPresets::GetPresetAsString(unsigned int index)
{
  const QJsonArray &presets = this->Internals->GetPresets();
  return index < static_cast<unsigned int>(presets.size())?
        presets[index].toString() : QString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QJsonObject& ColorPresets::GetPreset(unsigned int index)
{
  const QJsonArray &presets = this->Internals->GetPresets();
  return index < static_cast<unsigned int>(presets.size())?
        presets[index].toObject() : QJsonObject();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QJsonObject& ColorPresets::GetFirstPresetWithName(const char* name)
{
  if (name == NULL)
  {
    return QJsonObject();
  }

  QJsonArray presets = this->Internals->GetPresets();
  for (int i=0; i<presets.size(); i++)
  {
    if (presets[i].isObject() && presets[i].toObject().value("Name").toString() == name)
    {
      return presets[i].toObject();
    }
  }

  return QJsonObject();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ColorPresets::GetPresetName(unsigned int index)
{
  const QJsonArray &presets = this->Internals->GetPresets();
  return index < static_cast<unsigned int>(presets.size())?
        presets[index].toObject().value("Name").toString() : QString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int ColorPresets::GetNumberOfPresets()
{
  return static_cast<unsigned int>(this->Internals->GetPresets().size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ColorPresets::GetPresetHasOpacities(const QJsonObject& preset)
{
  return (!preset.isEmpty() && preset.keys().contains("Points"));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ColorPresets::GetPresetHasIndexedColors(const QJsonObject& preset)
{
  return (!preset.isEmpty() && preset.keys().contains("IndexedColors"));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ColorPresets::GetPresetHasAnnotations(const QJsonObject& preset)
{
  return (!preset.isEmpty() && preset.keys().contains("Annotations"));
}



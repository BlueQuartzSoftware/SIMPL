/*=========================================================================

  Program:   ParaView
  Module:    ColorPresets.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME ColorPresets - manages presets for color, opacity,
// and annotation presets.
// .SECTION Description
// ColorPresets is a manager to manage all color, opacity, and
// annotation presets. It also uses vtkSMSettings to support persistent
// customizations besides hard-coded/builtin presets.
//
// ColorPresets designed to be instantiated, used and then
// destroyed. While there is no explicit synchronization between multiple
// instances of ColorPresets, there should not be any need to
// have multiple instances alive at the same time.
#ifndef vtkSMTransferFunctionPresets_h
#define vtkSMTransferFunctionPresets_h

#include <QtCore/QString>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

class vtkPVXMLElement;
class ColorPresets
{
public:
  static ColorPresets* New();

  // Description:
  // Returns the number of presets current available (including builtin and
  // custom).
  unsigned int GetNumberOfPresets();

  // Description:
  // Gets the raw text for a preset given its index. The preset is provided as a JSON string.
  // Returns an empty string when not available.
  QString GetPresetAsString(unsigned int index);

  // Description:
  // Returns a preset JSON given the name. Since multiple presets can have the
  // same name, this returns the 'first' preset with the specified name.
  const QJsonObject &GetFirstPresetWithName(const char* name);

  // Description:
  // Returns a preset at a given index.
  const QJsonObject &GetPreset(unsigned int index);

  // Description:
  // Returns the name for a preset at the given index.
  QString GetPresetName(unsigned int index);

  // Description:
  // Returns true if the preset has opacities i.e. values for a piecewise function.
  bool GetPresetHasOpacities(const QJsonObject &preset);
  bool GetPresetHasOpacities(unsigned int index)
    { return this->GetPresetHasOpacities(this->GetPreset(index)); }

  // Description:
  // Returns true is the preset has indexed colors.
  bool GetPresetHasIndexedColors(const QJsonObject &preset);
  bool GetPresetHasIndexedColors(unsigned int index)
    { return this->GetPresetHasIndexedColors(this->GetPreset(index)); }

  // Description:
  // Returns true is the preset has annotations.
  bool GetPresetHasAnnotations(const QJsonObject &preset);
  bool GetPresetHasAnnotations(unsigned int index)
    { return this->GetPresetHasAnnotations(this->GetPreset(index)); }

protected:
  ColorPresets();
  ~ColorPresets();

private:
  class vtkInternals;
  vtkInternals* Internals;

  ColorPresets(const ColorPresets&); // Copy Constructor Not Implemented
  void operator=(const ColorPresets&); // Operator '=' Not Implemented
};

#endif

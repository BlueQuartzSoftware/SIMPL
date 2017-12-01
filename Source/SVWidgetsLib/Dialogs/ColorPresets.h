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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

class QPixmap;
class vtkPVXMLElement;

class ColorPresets
{
public:
  SIMPL_SHARED_POINTERS(ColorPresets)
  SIMPL_STATIC_NEW_MACRO(ColorPresets)

  ~ColorPresets();

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
  QJsonObject GetFirstPresetWithName(const char* name);

  // Description:
  // Returns a preset at a given index.
  QJsonObject GetPreset(unsigned int index);

  // Returns an image of the preset
  QPixmap getPixmapFromPreset(unsigned int index);

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

private:
  class vtkInternals;
  vtkInternals* Internals;

  ColorPresets(const ColorPresets&) = delete;   // Copy Constructor Not Implemented
  void operator=(const ColorPresets&) = delete; // Operator '=' Not Implemented
};

#endif

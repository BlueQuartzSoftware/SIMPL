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

#include "ColorPresets.h"

#include <iostream>

#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QJsonParseError>

#include <QtGui/QPixmap>

#include "SIMPLib/Utilities/ColorTable.h"

class ColorPresets::vtkInternals
{
public:
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  vtkInternals() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  ~vtkInternals() = default;

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
    if (Presets.empty() == false) { return; }

    QFile inputFile(":/CoreResources/ColorTablePresets/ColorTablePresets.json");
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
      Presets = doc.array();

      for (int i=0; i<Presets.size(); i++)
      {
        QJsonObject preset = Presets[i].toObject();
        if (preset["ColorSpace"].toString() != "RGB")
        {
          Presets.removeAt(i);
          i--;
        }
      }
    }
    else
    {
      // "Failed to open presets file" error
      return;
    }
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
QJsonObject ColorPresets::GetPreset(unsigned int index)
{
  const QJsonArray &presets = this->Internals->GetPresets();
  return index < static_cast<unsigned int>(presets.size())?
        presets[index].toObject() : QJsonObject();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject ColorPresets::GetFirstPresetWithName(const char* name)
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
QPixmap ColorPresets::getPixmapFromPreset(unsigned int index)
{
  QJsonObject presetObj = GetPreset(index);
  QSize resolution(180, 20);

  if (!presetObj.contains("ColorSpace") || presetObj["ColorSpace"].toString() != "RGB")
  {
    return QPixmap();
  }

  if (presetObj.contains("RGBPoints") && presetObj["RGBPoints"].isArray())
  {
    QJsonArray presetArray = presetObj["RGBPoints"].toArray();
    int numSamples = resolution.width();
    std::vector<unsigned char> colorValues = SIMPLColorTable::GetColorTable(numSamples, presetArray);

    QImage image(numSamples, 1, QImage::Format_RGB888);

    for (int i=0; i < numSamples; ++i)
    {
      image.setPixel(i, 0, qRgb(colorValues[3*i + 0], colorValues[3*i + 1], colorValues[3*i + 2]));
    }

    image = image.scaled(resolution);
    return QPixmap::fromImage(image);
  }

  return QPixmap();
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



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

#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>
#include <QtCore/QObject>
#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Common/PhaseType.h"

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/StringDataArray.h"

/**
 * @class EnsembleInfo EnsembleInfo.h DREAM3DLib/Common/EnsembleInfo.h
 * @brief  This class
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Feb 18, 2014
 * @version 1.0
 */
class SIMPLib_EXPORT EnsembleInfo : public QObject
{
  Q_OBJECT

public:
  using EnumType = uint32_t;

  enum class CrystalStructure : EnumType
  {
    Hexagonal_High = 0,
    Cubic_High,
    Hexagonal_Low,
    Cubic_Low,
    Triclinic,
    Monoclinic,
    OrthoRhombic,
    Tetragonal_Low,
    Tetragonal_High,
    Trigonal_Low,
    Trigonal_High,
    UnknownCrystalStructure = 999
  };

  static QString CrystalStructureToStr(CrystalStructure structure);
  static CrystalStructure CrystalStructureFromStr(QString structure);

  static QStringList CrystalStructureStrings();

  EnsembleInfo();
  EnsembleInfo(const EnsembleInfo& rhs);

  ~EnsembleInfo() override;

  size_t size() const;

  void addValues(const CrystalStructure crystalStructure, const PhaseType::Type phaseType, const QString phaseName);

  void getValues(size_t index, CrystalStructure& structure, PhaseType::Type& phaseType, QString& phaseName);

  CrystalStructure getCrystalStructure(size_t index) const;
  PhaseType::Type getPhaseType(size_t index) const;
  QString getPhaseName(size_t index) const;

  void setCrystalStructure(size_t index, CrystalStructure structure);
  void setPhaseType(size_t index, PhaseType::Type phaseType);
  void setPhaseName(size_t index, QString phaseName);

  std::vector<CrystalStructure> getCrystalStructureArray();
  std::vector<PhaseType::Type> getPhaseTypeArray();
  std::vector<QString> getPhaseNameArray();

  void operator=(const EnsembleInfo&);

  void remove(size_t index);

  void clear();

private:
  std::vector<CrystalStructure> m_CrystalStructures;
  std::vector<PhaseType::Type> m_PhaseTypes;
  std::vector<QString> m_PhaseNames;
};

Q_DECLARE_METATYPE(EnsembleInfo)


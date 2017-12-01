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

#include "EnsembleInfo.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList EnsembleInfo::CrystalStructureStrings()
{
  QStringList choices;
  // The choices here are IN ORDER of the enumerations from the EBSDLib. DO NOT CHANGE THE ORDER.
  choices.push_back("Hexagonal-High 6/mmm");
  choices.push_back("Cubic-High m-3m");
  choices.push_back("Hexagonal-Low 6/m");
  choices.push_back("Cubic-Low m-3 (Tetrahedral)");
  choices.push_back("Triclinic -1");
  choices.push_back("Monoclinic 2/m");
  choices.push_back("Orthorhombic mmm");
  choices.push_back("Tetragonal-Low 4/m");
  choices.push_back("Tetragonal-High 4/mmm");
  choices.push_back("Trigonal-Low -3");
  choices.push_back("Trigonal-High -3m");

  return choices;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnsembleInfo::CrystalStructureToStr(CrystalStructure structure)
{
  int structInt = static_cast<int>(structure);
  QStringList strList = CrystalStructureStrings();

  if(structInt < strList.size())
  {
    return strList[structInt];
  }

  return "Unknown Crystal Structure";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfo::CrystalStructure EnsembleInfo::CrystalStructureFromStr(QString structure)
{
  QStringList strList = CrystalStructureStrings();
  int index = strList.indexOf(structure);

  if(index < 0)
  {
    return CrystalStructure::UnknownCrystalStructure;
  }

  return static_cast<CrystalStructure>(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfo::EnsembleInfo()
: QObject(nullptr)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfo::EnsembleInfo(const EnsembleInfo& rhs)
: QObject(nullptr)
{
  m_CrystalStructures = rhs.m_CrystalStructures;
  m_PhaseTypes = rhs.m_PhaseTypes;
  m_PhaseNames = rhs.m_PhaseNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfo::~EnsembleInfo() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t EnsembleInfo::size() const
{
  return m_PhaseNames.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfo::addValues(const CrystalStructure crystalStructure, const PhaseType::Type phaseType, const QString phaseName)
{
  size_t currentSize = size();

  m_CrystalStructures.resize(currentSize + 1);
  m_PhaseTypes.resize(currentSize + 1);
  m_PhaseNames.resize(currentSize + 1);

  m_CrystalStructures[currentSize] = crystalStructure;
  m_PhaseTypes[currentSize] = phaseType;
  m_PhaseNames[currentSize] = phaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfo::getValues(size_t index, CrystalStructure& structure, PhaseType::Type& phaseType, QString& phaseName)
{
  structure = m_CrystalStructures[index];
  phaseType = m_PhaseTypes[index];
  phaseName = m_PhaseNames[index];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfo::CrystalStructure EnsembleInfo::getCrystalStructure(size_t index) const
{
  return m_CrystalStructures[index];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseType::Type EnsembleInfo::getPhaseType(size_t index) const
{
  return m_PhaseTypes[index];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnsembleInfo::getPhaseName(size_t index) const
{
  return m_PhaseNames[index];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfo::setCrystalStructure(size_t index, EnsembleInfo::CrystalStructure structure)
{
  m_CrystalStructures[index] = structure;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfo::setPhaseType(size_t index, PhaseType::Type phaseType)
{
  m_PhaseTypes[index] = phaseType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfo::setPhaseName(size_t index, QString phaseName)
{
  m_PhaseNames[index] = phaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfo::remove(size_t index)
{
  QVector<size_t> indexVec;
  indexVec.push_back(index);
  m_CrystalStructures.erase(m_CrystalStructures.begin() + index);
  m_PhaseTypes.erase(m_PhaseTypes.begin() + index);
  m_PhaseNames.erase(m_PhaseNames.begin() + index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfo::clear()
{
  m_CrystalStructures.clear();
  m_PhaseTypes.clear();
  m_PhaseNames.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<EnsembleInfo::CrystalStructure> EnsembleInfo::getCrystalStructureArray()
{
  return m_CrystalStructures;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<PhaseType::Type> EnsembleInfo::getPhaseTypeArray()
{
  return m_PhaseTypes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<QString> EnsembleInfo::getPhaseNameArray()
{
  return m_PhaseNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfo::operator=(const EnsembleInfo& rhs)
{
  m_CrystalStructures = rhs.m_CrystalStructures;
  m_PhaseTypes = rhs.m_PhaseTypes;
  m_PhaseNames = rhs.m_PhaseNames;
}

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

#include "BoundaryStatsData.h"

#include <QtCore/QString>
#include <vector>

#include "H5Support/H5Utilities.h"

#include "SIMPLib/HDF5/H5BoundaryStatsDataDelegate.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BoundaryStatsData::BoundaryStatsData()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BoundaryStatsData::~BoundaryStatsData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::Pointer BoundaryStatsData::deepCopy()
{
  BoundaryStatsData::Pointer ptr = BoundaryStatsData::New();
  ptr->setBoundaryArea(getBoundaryArea());
  ptr->setPhaseFraction(getPhaseFraction());
  ptr->setParentPhase(getParentPhase());
  ptr->setName(getName());
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BoundaryStatsData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BoundaryStatsData::getStatsType()
{
  return SIMPL::StringConstants::BoundaryStatsData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseType::Type BoundaryStatsData::getPhaseType()
{
  return PhaseType::Type::Boundary;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BoundaryStatsData::writeHDF5Data(hid_t groupId)
{
  int err = 0;
  H5BoundaryStatsDataDelegate::Pointer writer = H5BoundaryStatsDataDelegate::New();
  err = writer->writeBoundaryStatsData(this, groupId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BoundaryStatsData::readHDF5Data(hid_t groupId)
{
  int err = 0;
  H5BoundaryStatsDataDelegate::Pointer reader = H5BoundaryStatsDataDelegate::New();
  err = reader->readBoundaryStatsData(this, groupId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BoundaryStatsData::writeJson(QJsonObject& json)
{
  json.insert(SIMPL::StringConstants::PhaseType, PhaseType::BoundaryStr());
  // Write the precipitate boundary fraction
  json.insert(SIMPL::StringConstants::BoundaryArea, getBoundaryArea());
  // Write the Phase Fraction
  json.insert(SIMPL::StringConstants::PhaseFraction, getPhaseFraction());
  // Write the Parent Phase
  json.insert(SIMPL::StringConstants::ParentPhase, getParentPhase());
  // Write the name of the phase
  json.insert(SIMPL::StringConstants::Name, getName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BoundaryStatsData::readJson(const QJsonObject& json)
{

  QJsonValue fValue = json[SIMPL::StringConstants::BoundaryArea];
  if(!fValue.isUndefined() && fValue.isDouble())
  {
    setBoundaryArea(fValue.toDouble(0.0));
  }

  fValue = json[SIMPL::StringConstants::PhaseFraction];
  if(!fValue.isUndefined() && fValue.isDouble())
  {
    setPhaseFraction(fValue.toDouble(0.0));
  }

  fValue = json[SIMPL::StringConstants::ParentPhase];
  if(!fValue.isUndefined() && fValue.isDouble())
  {
    setParentPhase(fValue.toDouble(0.0));
  }

  fValue = json[SIMPL::StringConstants::Name];
  if(!fValue.isUndefined() && fValue.isString())
  {
    setName(fValue.toString("Boudary"));
  }
  if(getName().compare(SIMPL::StringConstants::StatsData) == 0)
  {
    setName("Boundary");
  }

}

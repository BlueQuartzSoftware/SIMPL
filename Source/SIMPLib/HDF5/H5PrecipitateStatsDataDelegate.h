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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include <QtCore/QString>

#include <hdf5.h>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/HDF5/H5StatsDataDelegate.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"

/**
 * @class H5PrecipitateStatsDataDelegate H5PrecipitateStatsDataDelegate.h DREAM3DLib/HDF5/H5PrecipitateStatsDataDelegate.h
 * @brief This class handles reading to and writing from a Statistics file that is
 * HDF5 based.
 *
 * @date Jan 22, 2012
 * @version 1.0
 */
class SIMPLib_EXPORT H5PrecipitateStatsDataDelegate : public H5StatsDataDelegate
{
public:
  using Self = H5PrecipitateStatsDataDelegate;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for H5PrecipitateStatsDataDelegate
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for H5PrecipitateStatsDataDelegate
   */
  static QString ClassName();

  ~H5PrecipitateStatsDataDelegate() override;

  int writePrecipitateStatsData(PrecipitateStatsData* data, hid_t groupId);
  int readPrecipitateStatsData(PrecipitateStatsData* data, hid_t groupId);

protected:
  H5PrecipitateStatsDataDelegate();

  int writeBoundaryArea(PrecipitateStatsData* data, hid_t groupId);
  int readBoundaryArea(PrecipitateStatsData* data, hid_t groupId);

  int writePhaseFraction(PrecipitateStatsData* data, hid_t groupId);
  int readPhaseFraction(PrecipitateStatsData* data, hid_t groupId);

  int writePrecipBoundaryFraction(PrecipitateStatsData* data, hid_t groupId);
  int readPrecipBoundaryFraction(PrecipitateStatsData* data, hid_t groupId);

  int writeFeatureDiameterInfo(PrecipitateStatsData* data, hid_t groupId);
  int readFeatureDiameterInfo(PrecipitateStatsData* data, hid_t groupId);

  int writeBinNumbers(PrecipitateStatsData* data, hid_t groupId);
  int readBinNumbers(PrecipitateStatsData* data, hid_t groupId);

  int readMDFWeights(hid_t pid, PrecipitateStatsData* data);
  int readODFWeights(hid_t pid, PrecipitateStatsData* data);
  int readAxisODFWeights(hid_t pid, PrecipitateStatsData* data);

  int writeWeightsData(hid_t pid, const QString& hdf5GroupName, VectorOfFloatArray colData);

  int writeDistributionData(hid_t pid, uint32_t disType, const QString& hdf5GroupName, VectorOfFloatArray colData);

  int writeRDFDistributionData(hid_t pid, RdfData::Pointer rdfData, const QString& hdf5GroupName);

  uint32_t readDistributionType(hid_t pid, const QString& hdf5GroupName);

  int readDistributionData(hid_t pid, const QString& hdf5GroupName, VectorOfFloatArray colData);

  int readRDFDistributionData(hid_t pid, const QString& hdf5GroupName, RdfData::Pointer rdfData);

  int writeVectorOfArrays(hid_t pid, VectorOfFloatArray colData);
  int readVectorOfArrays(hid_t pid, VectorOfFloatArray colData);

  VectorOfFloatArray createDistributionVector(unsigned int distType);

  VectorOfFloatArray createBetaDistributionArrays();

  // VectorOfFloatArray createPowerDistributionArrays();

  VectorOfFloatArray createLogNormalDistributionArrays();

  VectorOfFloatArray createRDFMaxMinDistributionArrays();

public:
  H5PrecipitateStatsDataDelegate(const H5PrecipitateStatsDataDelegate&) = delete;            // Copy Constructor Not Implemented
  H5PrecipitateStatsDataDelegate(H5PrecipitateStatsDataDelegate&&) = delete;                 // Move Constructor Not Implemented
  H5PrecipitateStatsDataDelegate& operator=(const H5PrecipitateStatsDataDelegate&) = delete; // Copy Assignment Not Implemented
  H5PrecipitateStatsDataDelegate& operator=(H5PrecipitateStatsDataDelegate&&) = delete;      // Move Assignment Not Implemented

private:
};

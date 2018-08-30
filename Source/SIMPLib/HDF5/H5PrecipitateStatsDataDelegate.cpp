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

#include "H5PrecipitateStatsDataDelegate.h"

#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5PrecipitateStatsDataDelegate::H5PrecipitateStatsDataDelegate() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5PrecipitateStatsDataDelegate::~H5PrecipitateStatsDataDelegate() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorOfFloatArray H5PrecipitateStatsDataDelegate::createBetaDistributionArrays()
{
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Alpha);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Beta);
  QVector<FloatArrayType::Pointer> vect;
  vect.push_back(alphas);
  vect.push_back(betas);
  return vect;
}

// VectorOfFloatArray H5PrecipitateStatsDataDelegate::createPowerDistributionArrays()
//{
//  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Alpha);
//  FloatArrayType::Pointer ks = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Exp_k);
//  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Beta);
//  QVector<FloatArrayType::Pointer> vect;
//  vect.push_back(alphas);
//  vect.push_back(ks);
//  vect.push_back(betas);
//  return vect;
//}

VectorOfFloatArray H5PrecipitateStatsDataDelegate::createLogNormalDistributionArrays()
{
  FloatArrayType::Pointer avgs = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Average);
  FloatArrayType::Pointer sd = FloatArrayType::CreateArray(0, SIMPL::StringConstants::StandardDeviation);
  QVector<FloatArrayType::Pointer> vect;
  vect.push_back(avgs);
  vect.push_back(sd);
  return vect;
}

VectorOfFloatArray H5PrecipitateStatsDataDelegate::createRDFMaxMinDistributionArrays()
{
  FloatArrayType::Pointer maxmin = FloatArrayType::CreateArray(2, SIMPL::StringConstants::MinMaxNoOfBins);
  QVector<FloatArrayType::Pointer> vect;
  vect.push_back(maxmin);
  return vect;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorOfFloatArray H5PrecipitateStatsDataDelegate::createDistributionVector(unsigned int distType)
{
  QVector<FloatArrayType::Pointer> empty;
  if(distType == SIMPL::DistributionType::Beta)
  {
    return createBetaDistributionArrays();
  }
  //  else if (distType == SIMPL::DistributionType::Power)
  //  {
  //    return createPowerDistributionArrays();
  //  }
  else if(distType == SIMPL::DistributionType::LogNormal)
  {
    return createLogNormalDistributionArrays();
  }
  //  else if (distType == SIMPL::DistributionType::RDFFrequency)
  //  {
  //    return empty();
  //  }
  else if(distType == SIMPL::DistributionType::RDFMaxMin)
  {
    return createRDFMaxMinDistributionArrays();
  }

  return empty;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readPrecipitateStatsData(PrecipitateStatsData* data, hid_t groupId)
{
  int err = 0;
  // Read the NumFeatures
  err = readBoundaryArea(data, groupId);

  err = readStatsDataName(data, groupId);

  // Read the PhaseFraction
  err = readPhaseFraction(data, groupId);

  // Read the Precip Boundary Fraction
  err = readPrecipBoundaryFraction(data, groupId);

  // Read the Feature Diameter Info
  err = readFeatureDiameterInfo(data, groupId);

  // Read the Feature Size Distribution
  uint32_t dType = readDistributionType(groupId, SIMPL::StringConstants::Feature_Size_Distribution);
  data->setFeatureSize_DistType(dType);
  data->setFeatureSizeDistribution(createDistributionVector(data->getFeatureSize_DistType()));
  data->getRadialDistFunction();
  err = readDistributionData(groupId, SIMPL::StringConstants::Feature_Size_Distribution, data->getFeatureSizeDistribution());

  //  // Read the Radial Distribution Function
  RdfData::Pointer rdfData = RdfData::New();
  data->setRadialDistFunction(rdfData);
  err = readRDFDistributionData(groupId, SIMPL::StringConstants::RadialDistFunc, data->getRadialDistFunction());
  if(err < 0)
  {
    data->setRadialDistFunction(RdfData::NullPointer());
  }

  // Read the Bin Numbers
  err = readBinNumbers(data, groupId);

  // Read the B Over A
  dType = readDistributionType(groupId, SIMPL::StringConstants::Feature_SizeVBoverA_Distributions);
  data->setBOverA_DistType(dType);
  data->setFeatureSize_BOverA(createDistributionVector(data->getBOverA_DistType()));
  err = readDistributionData(groupId, SIMPL::StringConstants::Feature_SizeVBoverA_Distributions, data->getFeatureSize_BOverA());

  // Read the C Over A
  dType = readDistributionType(groupId, SIMPL::StringConstants::Feature_SizeVCoverA_Distributions);
  data->setCOverA_DistType(dType);
  data->setFeatureSize_COverA(createDistributionVector(data->getCOverA_DistType()));
  err = readDistributionData(groupId, SIMPL::StringConstants::Feature_SizeVCoverA_Distributions, data->getFeatureSize_COverA());

  // Read the Clustering
  // FIXME: NO CLUSTERING DATA IS CURRENTLY GENERATED BY STATSGENERATOR, DISABLING THIS FOR NOW
  //  dType = readDistributionType(groupId, SIMPL::StringConstants::Feature_SizeVClustering_Distributions);
  //  data->setClustering_DistType(dType);
  //  data->setFeatureSize_Clustering( createDistributionVector(data->getClustering_DistType()));
  //  err = readDistributionData(groupId,
  //                             SIMPL::StringConstants::Feature_SizeVClustering_Distributions,
  //                             data->getFeatureSize_Clustering());

  // Read the Omegas
  dType = readDistributionType(groupId, SIMPL::StringConstants::Feature_SizeVOmega3_Distributions);
  data->setOmegas_DistType(dType);
  data->setFeatureSize_Omegas(createDistributionVector(data->getOmegas_DistType()));
  err = readDistributionData(groupId, SIMPL::StringConstants::Feature_SizeVOmega3_Distributions, data->getFeatureSize_Omegas());

  // Read the Misorientation Bins
  FloatArrayType::Pointer misoBins = FloatArrayType::CreateArray(0, SIMPL::StringConstants::MisorientationBins);
  err = misoBins->readH5Data(groupId);
  if(err < 0)
  {
    misoBins = FloatArrayType::NullPointer();
  }
  data->setMisorientationBins(misoBins);
  err = readMDFWeights(groupId, data);

  // Read the ODF Data
  FloatArrayType::Pointer odfBins = FloatArrayType::CreateArray(0, SIMPL::StringConstants::ODF);
  err = odfBins->readH5Data(groupId);
  if(err < 0)
  {
    odfBins = FloatArrayType::NullPointer();
  }
  data->setODF(odfBins);
  err = readODFWeights(groupId, data);

  // Read the Axis ODF Data
  FloatArrayType::Pointer axisOdfBins = FloatArrayType::CreateArray(0, SIMPL::StringConstants::AxisOrientation);
  err = axisOdfBins->readH5Data(groupId);
  if(err < 0)
  {
    axisOdfBins = FloatArrayType::NullPointer();
  }
  data->setAxisOrientation(axisOdfBins);
  err = readAxisODFWeights(groupId, data);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writePrecipitateStatsData(PrecipitateStatsData* data, hid_t groupId)
{
  if(nullptr == data)
  {
    return -1;
  }
  int err = 0;

  err = QH5Lite::writeStringDataset(groupId, SIMPL::StringConstants::Name, data->getName());
  if(err < 0)
  {
    return err;
  }
  // Write the NumFeatures
  err = writeBoundaryArea(data, groupId);
  if(err < 0)
  {
    return err;
  }

  // Write the PhaseFraction
  err = writePhaseFraction(data, groupId);
  if(err < 0)
  {
    return err;
  }

  // Write the Precip Boundary Fraction
  err = writePrecipBoundaryFraction(data, groupId);

  // Write the Feature Diameter Info
  err = writeFeatureDiameterInfo(data, groupId);

  // Write the Feature Size Distribution
  err = writeDistributionData(groupId, data->getFeatureSize_DistType(), SIMPL::StringConstants::Feature_Size_Distribution, data->getFeatureSizeDistribution());

  // Write the Radial Distribution Function
  err = writeRDFDistributionData(groupId, data->getRadialDistFunction(), SIMPL::StringConstants::RadialDistFunc);

  // Write the Bin Numbers
  err = writeBinNumbers(data, groupId);

  // Write the B Over A
  err = writeDistributionData(groupId, data->getBOverA_DistType(), SIMPL::StringConstants::Feature_SizeVBoverA_Distributions, data->getFeatureSize_BOverA());

  // Write the C Over A
  err = writeDistributionData(groupId, data->getCOverA_DistType(), SIMPL::StringConstants::Feature_SizeVCoverA_Distributions, data->getFeatureSize_COverA());

  // Write the Clustering
  // FIXME: NO CLUSTERING DATA IS CURRENTLY GENERATED BY STATSGENERATOR, DISABLING THIS FOR NOW
  //  err = writeDistributionData(groupId,
  //                              data->getClustering_DistType(),
  //                              SIMPL::StringConstants::Feature_SizeVClustering_Distributions,
  //                              data->getFeatureSize_Clustering());

  // Write the Omegas
  err = writeDistributionData(groupId, data->getOmegas_DistType(), SIMPL::StringConstants::Feature_SizeVOmega3_Distributions, data->getFeatureSize_Omegas());

  // Write the Misorientation Bins
  if(nullptr != data->getMisorientationBins().get())
  {
    QVector<size_t> tDims(1, data->getMisorientationBins()->getNumberOfTuples());
    err = data->getMisorientationBins()->writeH5Data(groupId, tDims);
  }

  err = writeWeightsData(groupId, SIMPL::StringConstants::MDFWeights, data->getMDF_Weights());

  // Write the ODF
  if(nullptr != data->getODF().get())
  {
    QVector<size_t> tDims(1, data->getODF()->getNumberOfTuples());
    err = data->getODF()->writeH5Data(groupId, tDims);
  }
  err = writeWeightsData(groupId, SIMPL::StringConstants::ODFWeights, data->getODF_Weights());

  // Write the Axis ODF
  if(nullptr != data->getAxisOrientation().get())
  {
    QVector<size_t> tDims(1, data->getAxisOrientation()->getNumberOfTuples());
    err = data->getAxisOrientation()->writeH5Data(groupId, tDims);
  }
  err = writeWeightsData(groupId, SIMPL::StringConstants::AxisODFWeights, data->getAxisODF_Weights());

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeVectorOfArrays(hid_t pid, VectorOfFloatArray colData)
{
  herr_t err = 0;
  herr_t retErr = 0;

  // Loop through all the column data and write each one to the HDF5 file
  size_t numColumns = colData.size();
  for(size_t c = 0; c < numColumns; ++c)
  {
    // qDebug() << "Writing Dataset:" << hdf5GroupName << "/" << columnHeaders[c] ;
    err = -1;
    if(nullptr != colData[c].get() && colData[c]->getSize() > 0)
    {
      QVector<size_t> tDims(1, colData[c]->getNumberOfTuples());
      err = colData[c]->writeH5Data(pid, tDims);
      if(err < 0)
      {
        retErr = err;
        break;
      }
    }
    else
    {
      qDebug() << ":Null Data Column had no data. Did you create the data?";
      qDebug() << "  File: " << __FILE__;
      qDebug() << "  Line: " << __LINE__;
      break;
    }
  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readVectorOfArrays(hid_t pid, VectorOfFloatArray colData)
{
  int err = 0;

  for(VectorOfFloatArray::iterator iter = colData.begin(); iter != colData.end(); ++iter)
  {
    FloatArrayType::Pointer d = *iter;
    err = d->readH5Data(pid);
    if(err < 0)
    {
      return err;
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readMDFWeights(hid_t pid, PrecipitateStatsData* data)
{
  int err = 0;
  QVector<size_t> dims(1, 1);
  FloatArrayType::Pointer angles = FloatArrayType::CreateArray(1, dims, SIMPL::StringConstants::Angle);
  FloatArrayType::Pointer weight = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Weight);
  dims[0] = 3;
  FloatArrayType::Pointer axis = FloatArrayType::CreateArray(3, dims, SIMPL::StringConstants::Axis);

  VectorOfFloatArray mdfWeights;
  mdfWeights.push_back(angles);
  mdfWeights.push_back(axis);
  mdfWeights.push_back(weight);

  hid_t groupId = QH5Utilities::openHDF5Object(pid, SIMPL::StringConstants::MDFWeights);

  err = readVectorOfArrays(groupId, mdfWeights);
  if(err >= 0)
  {
    data->setMDF_Weights(mdfWeights);
  }

  err = QH5Utilities::closeHDF5Object(groupId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readODFWeights(hid_t pid, PrecipitateStatsData* data)
{

  int err = 0;

  FloatArrayType::Pointer euler1 = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Euler1);
  FloatArrayType::Pointer euler2 = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Euler2);
  FloatArrayType::Pointer euler3 = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Euler3);
  FloatArrayType::Pointer weight = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Weight);
  FloatArrayType::Pointer sigma = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Sigma);

  VectorOfFloatArray odfWeights;
  odfWeights.push_back(euler1);
  odfWeights.push_back(euler2);
  odfWeights.push_back(euler3);
  odfWeights.push_back(weight);
  odfWeights.push_back(sigma);

  hid_t groupId = QH5Utilities::openHDF5Object(pid, SIMPL::StringConstants::ODFWeights);

  err = readVectorOfArrays(groupId, odfWeights);
  if(err >= 0)
  {
    data->setODF_Weights(odfWeights);
  }

  err |= QH5Utilities::closeHDF5Object(groupId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readAxisODFWeights(hid_t pid, PrecipitateStatsData* data)
{

  int err = 0;

  FloatArrayType::Pointer euler1 = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Euler1);
  FloatArrayType::Pointer euler2 = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Euler2);
  FloatArrayType::Pointer euler3 = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Euler3);
  FloatArrayType::Pointer sigma = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Sigma);
  FloatArrayType::Pointer weight = FloatArrayType::CreateArray(0, SIMPL::StringConstants::Weight);

  VectorOfFloatArray odfWeights;
  odfWeights.push_back(euler1);
  odfWeights.push_back(euler2);
  odfWeights.push_back(euler3);
  odfWeights.push_back(sigma);
  odfWeights.push_back(weight);

  hid_t groupId = QH5Utilities::openHDF5Object(pid, SIMPL::StringConstants::AxisODFWeights);

  err = readVectorOfArrays(groupId, odfWeights);
  if(err >= 0)
  {
    data->setAxisODF_Weights(odfWeights);
  }

  err |= QH5Utilities::closeHDF5Object(groupId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeWeightsData(hid_t pid, const QString& hdf5GroupName, VectorOfFloatArray colData)
{
  herr_t err = 0;
  if(colData.size() == 0)
  {
    return err;
  }
  // Create the Group Folder
  hid_t disId = QH5Utilities::createGroup(pid, hdf5GroupName);
  if(disId > 0)
  {
    err = writeVectorOfArrays(disId, colData);
  }
  // Close the HDF5 Group
  err |= H5Gclose(disId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeDistributionData(hid_t pid, uint32_t disType, const QString& hdf5GroupName, VectorOfFloatArray colData)
{
  herr_t err = 0;
  herr_t retErr = 0;

  QString disTypeStr;
  switch(disType)
  {
  case SIMPL::DistributionType::Beta:
    disTypeStr = SIMPL::StringConstants::BetaDistribution;
    break;
  case SIMPL::DistributionType::LogNormal:
    disTypeStr = SIMPL::StringConstants::LogNormalDistribution;
    break;
  //    case SIMPL::DistributionType::Power:
  //      disTypeStr = SIMPL::StringConstants::PowerLawDistribution;
  //      break;
  case SIMPL::DistributionType::UnknownDistributionType:
    disTypeStr = SIMPL::StringConstants::UnknownDistribution;
    break;
  default:
    disTypeStr = SIMPL::StringConstants::UnknownDistribution;
  }

  // Create the Group Folder
  hid_t disId = QH5Utilities::createGroup(pid, hdf5GroupName);
  if(disId > 0)
  {
    err = QH5Lite::writeStringAttribute(pid, hdf5GroupName, SIMPL::StringConstants::DistributionType, disTypeStr);
    if(err >= 0)
    {
      err |= writeVectorOfArrays(disId, colData);
    }
    else
    {
      retErr = err;
    }
    // Close the HDF5 Group
    err = H5Gclose(disId);
  }
  else
  {
    retErr = disId;
  }

  return retErr;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeRDFDistributionData(hid_t pid, RdfData::Pointer rdfData, const QString& hdf5GroupName)
{
  herr_t err = 0;
  herr_t retErr = 0;

  if(nullptr != rdfData)
  {
    QString disTypeStr = rdfData->getDistributionType();

    // Create the Group Folder
    hid_t disId = QH5Utilities::createGroup(pid, hdf5GroupName);
    if(disId > 0)
    {
      H5ScopedGroupSentinel sentinel(&disId, false);
      std::vector<hsize_t> dims(1);
      // Write the Frequencies
      std::vector<float> freqs = rdfData->getFrequencies();
      dims[0] = freqs.size();
      if(dims[0] != 0)
      {
        err = H5Lite::writeVectorDataset(disId, SIMPL::StringConstants::Frequencies.toStdString(), dims, freqs);
        if(err < 0)
        {
          return err;
        }
        float val = rdfData->getMaxDistance();
        err = QH5Lite::writeScalarAttribute(disId, SIMPL::StringConstants::Frequencies, SIMPL::StringConstants::RdfMaxDistance, val);
        if(err < 0)
        {
          return err;
        }
        val = rdfData->getMinDistance();
        err = QH5Lite::writeScalarAttribute(disId, SIMPL::StringConstants::Frequencies, SIMPL::StringConstants::RdfMinDistance, val);
        if(err < 0)
        {
          return err;
        }
      }
      int32_t rank = 1;
      dims[0] = 3;
      float boxRes[3] = {0.0f, 0.0f, 0.0f};
      std::tie(boxRes[0], boxRes[1], boxRes[2]) = rdfData->getBoxResolution();
      err = H5Lite::writePointerDataset(disId, SIMPL::StringConstants::RdfBoxRes.toStdString(), rank, dims.data(), boxRes);
      if(err < 0)
      {
        return err;
      }

      float boxDims[3] = {0.0f, 0.0f, 0.0f};
      std::tie(boxDims[0], boxDims[1], boxDims[2]) = rdfData->getBoxSize();
      err = H5Lite::writePointerDataset(disId, SIMPL::StringConstants::RdfBoxDims.toStdString(), rank, dims.data(), boxDims);
      if(err < 0)
      {
        return err;
      }

      // Close the HDF5 Group
      err = H5Gclose(disId);
      disId = -1;
    }
    else
    {
      retErr = disId;
    }
  }

  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t H5PrecipitateStatsDataDelegate::readDistributionType(hid_t pid, const QString& hdf5GroupName)
{
  // int err = 0;
  uint32_t dType = SIMPL::DistributionType::UnknownDistributionType;

  QString disTypeStr;
  int err = QH5Lite::readStringAttribute(pid, hdf5GroupName, SIMPL::StringConstants::DistributionType, disTypeStr);
  if(err < 0)
  {
    return dType;
  }
  if(disTypeStr.compare(SIMPL::StringConstants::BetaDistribution) == 0)
  {
    dType = SIMPL::DistributionType::Beta;
  }
  else if(disTypeStr.compare(SIMPL::StringConstants::LogNormalDistribution) == 0)
  {
    dType = SIMPL::DistributionType::LogNormal;
  }
  //  else   if (disTypeStr.compare(SIMPL::StringConstants::PowerLawDistribution) == 0)
  //  {
  //    dType = SIMPL::DistributionType::Power;
  //  }
  return dType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readDistributionData(hid_t pid, const QString& hdf5GroupName, VectorOfFloatArray colData)
{
  int err = 0;
  hid_t disId = QH5Utilities::openHDF5Object(pid, hdf5GroupName);
  if(disId < 0)
  {
    return -1;
  }

  for(VectorOfFloatArray::iterator iter = colData.begin(); iter != colData.end(); ++iter)
  {
    FloatArrayType::Pointer d = *iter;
    err |= d->readH5Data(disId);
  }

  err |= QH5Utilities::closeHDF5Object(disId);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readRDFDistributionData(hid_t pid, const QString& hdf5GroupName, RdfData::Pointer rdfData)
{
  int err = 0;
  hid_t disId = QH5Utilities::openHDF5Object(pid, hdf5GroupName);
  if(disId < 0)
  {
    return -1;
  }
  H5ScopedGroupSentinel sentinel(&disId, false);

  std::vector<float> freqs;
  err = H5Lite::readVectorDataset(disId, SIMPL::StringConstants::Frequencies.toStdString(), freqs);
  if(err < 0)
  {
    return err;
  }
  rdfData->setFrequencies(freqs);

  float val = 0.0f;
  err = QH5Lite::readScalarAttribute(disId, SIMPL::StringConstants::Frequencies, SIMPL::StringConstants::RdfMaxDistance, val);
  if(err < 0)
  {
    return err;
  }
  rdfData->setMaxDistance(val);

  err = QH5Lite::readScalarAttribute(disId, SIMPL::StringConstants::Frequencies, SIMPL::StringConstants::RdfMinDistance, val);
  if(err < 0)
  {
    return err;
  }
  rdfData->setMinDistance(val);

  float boxDims[3] = {0.0f, 0.0f, 0.0f};
  err = QH5Lite::readPointerDataset(disId, SIMPL::StringConstants::RdfBoxDims, boxDims);
  if(err < 0)
  {
    return err;
  }
  rdfData->setBoxSize(std::make_tuple(boxDims[0], boxDims[1], boxDims[2]));

  float boxRes[3] = {0.0f, 0.0f, 0.0f};
  err = QH5Lite::readPointerDataset(disId, SIMPL::StringConstants::RdfBoxRes, boxRes);
  if(err < 0)
  {
    return err;
  }
  rdfData->setBoxResolution(std::make_tuple(boxRes[0], boxRes[1], boxRes[2]));

  err = QH5Utilities::closeHDF5Object(disId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeBoundaryArea(PrecipitateStatsData* data, hid_t pid)
{
  float BoundaryArea = data->getBoundaryArea();
  return QH5Lite::writeScalarDataset(pid, SIMPL::StringConstants::BoundaryArea, BoundaryArea);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readBoundaryArea(PrecipitateStatsData* data, hid_t pid)
{
  float BoundaryArea = 0.0f;
  int err = QH5Lite::readScalarDataset(pid, SIMPL::StringConstants::BoundaryArea, BoundaryArea);
  data->setBoundaryArea(BoundaryArea);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writePhaseFraction(PrecipitateStatsData* data, hid_t pid)
{
  float phaseFraction = data->getPhaseFraction();
  return QH5Lite::writeScalarDataset(pid, SIMPL::StringConstants::PhaseFraction, phaseFraction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readPhaseFraction(PrecipitateStatsData* data, hid_t pid)
{
  float phaseFraction = 0.0f;
  int err = QH5Lite::readScalarDataset(pid, SIMPL::StringConstants::PhaseFraction, phaseFraction);
  data->setPhaseFraction(phaseFraction);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writePrecipBoundaryFraction(PrecipitateStatsData* data, hid_t pid)
{
  float var = data->getPrecipBoundaryFraction();
  return QH5Lite::writeScalarDataset(pid, SIMPL::StringConstants::PrecipitateBoundaryFraction, var);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readPrecipBoundaryFraction(PrecipitateStatsData* data, hid_t pid)
{
  float precipBoundaryFraction = 0.0f;
  int err = QH5Lite::readScalarDataset(pid, SIMPL::StringConstants::PrecipitateBoundaryFraction, precipBoundaryFraction);
  data->setPrecipBoundaryFraction(precipBoundaryFraction);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeFeatureDiameterInfo(PrecipitateStatsData* data, hid_t pid)
{
  hsize_t dims[1];
  dims[0] = 3;
  int32_t rank = 1;

  /*
   * Feature Diameter Info is encode as 3 floats: BinStepSize, MaxDiameter, MinDiameter
   */
  float featureDiameterInfo[3];
  std::tie(featureDiameterInfo[0], featureDiameterInfo[1], featureDiameterInfo[2]) = data->getFeatureDiameterInfo();

  return QH5Lite::writePointerDataset(pid, SIMPL::StringConstants::Feature_Diameter_Info, rank, dims, featureDiameterInfo);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readFeatureDiameterInfo(PrecipitateStatsData* data, hid_t groupId)
{
  int err = 0;
  /*
   * Feature Diameter Info is encode as 3 floats: BinStepSize, MaxDiameter, MinDiameter
   */
  float featureDiameterInfo[3] = {0.0f, 0.0f, 0.0f};

  err = QH5Lite::readPointerDataset(groupId, SIMPL::StringConstants::Feature_Diameter_Info, featureDiameterInfo);
  data->setFeatureDiameterInfo(std::make_tuple(featureDiameterInfo[0], featureDiameterInfo[1], featureDiameterInfo[2]));
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeBinNumbers(PrecipitateStatsData* data, hid_t groupId)
{
  // Ensure we have valid bin numbers
  if(nullptr == data->getBinNumbers().get())
  {
    data->generateBinNumbers();
  }
  QVector<size_t> tDims(1, data->getBinNumbers()->getNumberOfTuples());
  return data->getBinNumbers()->writeH5Data(groupId, tDims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readBinNumbers(PrecipitateStatsData* data, hid_t groupId)
{
  int err = 0;
  FloatArrayType::Pointer p = FloatArrayType::CreateArray(1, SIMPL::StringConstants::BinNumber);
  err = p->readH5Data(groupId);
  data->setBinNumbers(p);
  return err;
}

/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MontageSupport.h"

#include <QtCore/QString>

#include "H5Support/H5Utilities.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/Montages/GridMontage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GridMontage::Pointer readGridMontageFromHDF5(hid_t parentId, const DataContainerArrayShPtr& dca, int& err)
{
  QString name;
  err = QH5Lite::readStringDataset(parentId, SIMPL::StringConstants::Name, name);
  if(err < 0)
  {
    return nullptr;
  }

  SizeVec3Type dims;
  err = QH5Lite::readScalarDataset(parentId, "X Dim", dims[0]);
  if(err < 0)
  {
    return nullptr;
  }
  err = QH5Lite::readScalarDataset(parentId, "Y Dim", dims[1]);
  if(err < 0)
  {
    return nullptr;
  }
  err = QH5Lite::readScalarDataset(parentId, "Z Dim", dims[2]);
  if(err < 0)
  {
    return nullptr;
  }

  GridMontage::Pointer montage = GridMontage::New(name, dims[0], dims[1], dims[2]);
  if(nullptr == dca)
  {
    return montage;
  }

  // Set Montage DataContainers
  char sep = 0x1E; // Use the ASCII 'record separator' value (Decimal value 30) to separate the names
  QString dcNameCollection;
  err = QH5Lite::readStringDataset(parentId, SIMPL::StringConstants::DataContainerNames, dcNameCollection);
  QStringList dcNames = dcNameCollection.split(sep, QString::KeepEmptyParts);

  std::vector<DataContainerShPtr> dataContainers;
  for(const QString& dcName : dcNames)
  {
    dataContainers.push_back(dca->getDataContainer(dcName));
  }
  montage->setDataContainers(dataContainers);

  return montage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractMontageShPtr MontageSupport::IO::ReadMontageFromHDF5(hid_t parentId, const DataContainerArrayShPtr& dca, int& err)
{
  QString typeName;
  err = QH5Lite::readStringDataset(parentId, "Montage Type", typeName);
  if(err < 0)
  {
    return nullptr;
  }

  // Create Montage of the correct type
  if(typeName == "GridMontage")
  {
    return readGridMontageFromHDF5(parentId, dca, err);
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::MontageCollection MontageSupport::IO::ReadMontagesFromHDF5(hid_t parentId, const DataContainerArrayShPtr& dca, int& err)
{
  DataContainerArray::MontageCollection montageCollection;

  // Get list of montages
  QStringList names;
  QH5Utilities::getGroupObjects(parentId, H5Utilities::CustomHDFDataTypes::Any, names);

  for(const QString& name : names)
  {
    hid_t montageId = QH5Utilities::openHDF5Object(parentId, name);
    // This object will make sure the HDF5 Group id is closed when it goes out of scope.
    H5GroupAutoCloser bundleIdClose(&montageId);

    if(!montageCollection.insert(ReadMontageFromHDF5(montageId, dca, err)))
    {
      return montageCollection;
    }
  }

  return montageCollection;
}

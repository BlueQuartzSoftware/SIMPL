/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
#include "GeometryHelpers.h"

namespace GeometryHelpers
{

// -----------------------------------------------------------------------------
MeshIndexArrayType::Pointer GeomIO::ReadMeshIndexListFromHDF5(const QString& listName, hid_t parentId, bool preflight, herr_t& err)
{
  MeshIndexArrayType::Pointer meshIndex = MeshIndexArrayType::NullPointer();

  // In versions of DREAM3D before 6.6, the index list was stored as a Int64_t type, versions starting with 6.6 now store
  // the list as a size_t (UInt64_t). So we need to try each type:
  Int64ArrayType::Pointer tempInt64 = GeometryHelpers::GeomIO::ReadListFromHDF5<Int64ArrayType>(listName, parentId, preflight, err);
  if(tempInt64.get() != nullptr)
  {
    meshIndex = SharedEdgeList::WrapPointer(reinterpret_cast<MeshIndexType*>(tempInt64->data()), tempInt64->getNumberOfTuples(), tempInt64->getComponentDimensions(), tempInt64->getName(), true);
    // Release the ownership of the memory from TempTris and essentially pass it to tris.
    tempInt64->releaseOwnership();
  }
  else // Reading as a Int64 didn't work which means the data _should_ be a UInt64_t (size_t)
  {
#ifdef Q_OS_MACOS
    // Mac OS will fail the dynamic cast from unsigned long ling (Uint64_t) to unsigned long (size_t) so we use the proper
    // type from the hdf5 file for macOS.
    UInt64ArrayType::Pointer tempUInt64 = GeometryHelpers::GeomIO::ReadListFromHDF5<UInt64ArrayType>(listName, parentId, preflight, err);
#else
    MeshIndexArrayType::Pointer tempUInt64 = GeometryHelpers::GeomIO::ReadListFromHDF5<MeshIndexArrayType>(listName, parentId, preflight, err);
#endif
    if(tempUInt64.get() != nullptr)
    {
      meshIndex = SharedEdgeList::WrapPointer(reinterpret_cast<MeshIndexType*>(tempUInt64->data()), tempUInt64->getNumberOfTuples(), tempUInt64->getComponentDimensions(), tempUInt64->getName(), true);
      // Release the ownership of the memory from TempTris and essentially pass it to tris.
      tempUInt64->releaseOwnership();
    }
  }

  return meshIndex;
}

// -----------------------------------------------------------------------------
int GeomIO::ReadMetaDataFromHDF5(hid_t parentId, const IGeometry::Pointer& geometry)
{
  herr_t err = 0;
  unsigned int spatialDims = 0;
  QString geomName = "";
  err = QH5Lite::readScalarAttribute(parentId, SIMPL::Geometry::Geometry, SIMPL::Geometry::SpatialDimensionality, spatialDims);
  if(err < 0)
  {
    return err;
  }
  err = QH5Lite::readStringAttribute(parentId, SIMPL::Geometry::Geometry, SIMPL::Geometry::GeometryName, geomName);
  if(err < 0)
  {
    return err;
  }
  geometry->setSpatialDimensionality(spatialDims);
  geometry->setName(geomName);

  return 1;
}

// -----------------------------------------------------------------------------
int GeomIO::WriteListToHDF5(hid_t parentId, const IDataArray::Pointer& list)
{
  herr_t err = 0;
  if(list->getNumberOfTuples() == 0)
  {
    return err;
  }
  QVector<size_t> tDims(1, list->getNumberOfTuples());
  err = list->writeH5Data(parentId, tDims);
  return err;
}

} // namespace GeometryHelpers

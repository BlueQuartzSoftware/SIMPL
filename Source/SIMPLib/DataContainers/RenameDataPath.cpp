/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "RenameDataPath.h"

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataPath::AlertFilterCreatedPath(AbstractFilter* filter, DataID_t id, const DataArrayPath& path)
{
  if(filter != nullptr)
  {
    filter->checkIfPathRenamed(id, path);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool RenameDataPath::CanReplaceRenamePath(const DataArrayPath::RenameType& originalPair, const DataArrayPath::RenameType& newPair)
{
  return originalPair.first == newPair.first && originalPair.second != newPair.second;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool RenameDataPath::CheckRenamePath(const DataContainerArrayShPtr& oldDca, const DataContainerArrayShPtr& newDca, const DataArrayPath& oldPath, const DataArrayPath& newPath)
{
  // If the paths are not possible renames, return false
  if(!oldPath.possibleRename(newPath))
  {
    return false;
  }

  // Check that the DataArrayPath targets are compatibles
  DataContainer::Pointer oldDc = oldDca->getDataContainer(oldPath.getDataContainerName());
  DataContainer::Pointer newDc = newDca->getDataContainer(newPath.getDataContainerName());

  // Data Container required
  if(oldDc && newDc)
  {
    IGeometry::Pointer oldGeom = oldDc->getGeometry();
    IGeometry::Pointer newGeom = newDc->getGeometry();

    bool hasGeom = oldGeom && newGeom;
    if((hasGeom && oldGeom->getGeometryType() == newGeom->getGeometryType()) || !hasGeom)
    {
      // No Attribute Matrix path
      if(oldPath.getAttributeMatrixName().isEmpty() && newPath.getAttributeMatrixName().isEmpty())
      {
        return true;
      }

      AttributeMatrix::Pointer oldAm = oldDc->getAttributeMatrix(oldPath.getAttributeMatrixName());
      AttributeMatrix::Pointer newAm = newDc->getAttributeMatrix(newPath.getAttributeMatrixName());

      bool hasAttributeMatrix = oldAm && newAm;
      if(hasAttributeMatrix && oldAm->getType() == newAm->getType() && oldAm->getNumberOfTuples() == newAm->getNumberOfTuples())
      {
        // No Data Array path
        if(oldPath.getDataArrayName().isEmpty() && newPath.getDataArrayName().isEmpty())
        {
          return true;
        }

        IDataArray::Pointer oldDa = oldAm->getAttributeArray(oldPath.getDataArrayName());
        IDataArray::Pointer newDa = newAm->getAttributeArray(newPath.getDataArrayName());

        bool hasDataArray = oldDa && newDa;
        bool hasSameType = oldDa->getTypeAsString() == newDa->getTypeAsString();
        bool hasSameCompDims = oldDa->getComponentDimensions() == newDa->getComponentDimensions();
        bool isNeighborList = hasSameType && newDa->getTypeAsString().startsWith("NeighborList");
        if(hasDataArray && hasSameType && (hasSameCompDims || isNeighborList))
        {
          return true;
        }
      }
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::RenameContainer RenameDataPath::CheckForRenamedPaths(const DataContainerArrayShPtr& oldDca, const DataContainerArrayShPtr& newDca, const std::list<DataArrayPath>& oldPaths,
                                                                    const std::list<DataArrayPath>& newPaths)
{
  DataArrayPath::RenameContainer container;
  std::list<DataArrayPath> duplicatedPaths;
  std::list<DataArrayPath> usedNewPaths;

  // For each older path, check for any matching new paths.  If only one new path matches, add it as a possibility
  for(const DataArrayPath& oldPath : oldPaths)
  {
    // If the same path exists in both oldPaths and newPaths, it was not renamed
    if(std::find(newPaths.begin(), newPaths.end(), oldPath) != newPaths.end())
    {
      continue;
    }

    // Find any potential renames in newPaths for the given oldPath
    std::list<DataArrayPath> matches;
    for(const DataArrayPath& newPath : newPaths)
    {
      // If the same path exists in both oldPaths and newPaths, it was not renamed
      if(std::find(oldPaths.begin(), oldPaths.end(), newPath) != oldPaths.end())
      {
        continue;
      }

      // Check that all geometries, AttributeMatrices, and DataArrays are compatible
      if(CheckRenamePath(oldDca, newDca, oldPath, newPath))
      {
        matches.push_back(newPath);
      }
    }
    // If this path was already used, mark it as duplicate and move on
    if(matches.size() == 1)
    {
      const DataArrayPath& newPath = matches.front();
      if(usedNewPaths.end() != std::find(usedNewPaths.begin(), usedNewPaths.end(), newPath))
      {
        duplicatedPaths.push_back(newPath);
      }
      else
      {
        usedNewPaths.push_back(newPath);
        container.push_back(std::make_pair(oldPath, newPath));
      }
    }
  }

  // Remove items with duplicated paths
  for(auto iter = container.begin(); iter != container.end();)
  {
    const DataArrayPath& checkPath = std::get<1>(*iter);
    if(duplicatedPaths.end() != std::find(duplicatedPaths.begin(), duplicatedPaths.end(), checkPath))
    {
      iter = container.erase(iter);
    }
    else
    {
      iter++;
    }
  }

  return container;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataPath::CalculateRenamedPaths(const AbstractFilterShPtr& filter, DataArrayPath::RenameContainer& renamedPaths)
{
  const DataArrayPath::RenameContainer origRenamedPaths = filter->getRenamedPaths();
  filter->preflight();
  // Check if an existing renamed path was created by this filter
  std::list<DataArrayPath> createdPaths = filter->getCreatedPaths();
  for(const auto& origRenamedPath : origRenamedPaths)
  {
    createdPaths.push_back(origRenamedPath.second);
  }
  for(const DataArrayPath& createdPath : createdPaths)
  {
    // Using iterator over range-based for to utilize std::list.erase(iterator)
    for(auto iter = renamedPaths.begin(); iter != renamedPaths.end(); iter++)
    {
      const DataArrayPath& originalPath = (*iter).first;
      if(originalPath == createdPath)
      {
        renamedPaths.erase(iter);
        break;
      }
    }
  }

  // Check for updated rename paths
  MergeRenamedPaths(renamedPaths, origRenamedPaths);
  const DataArrayPath::RenameContainer updatedRenamedPaths = filter->getRenamedPaths();
  MergeRenamedPaths(renamedPaths, updatedRenamedPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataPath::MergeRenamedPaths(RenameContainer& list, const RenameContainer& newRenames)
{
  for(const auto& newRename : newRenames)
  {
    bool added = false;
    // Check if this updates an existing rename
    for(auto iter = list.begin(); iter != list.end(); iter++)
    {
      const DataArrayPath::RenameType& renamedPath = *iter;
      const auto& possibleRename = DataArrayPath::CreateLinkingRename(renamedPath, newRename);
      if(possibleRename.first)
      {
        list.insert(iter, possibleRename.second);
        added = true;
        break;
      }
    }
    // Path wasn't updated
    if(!added)
    {
      list.push_back(newRename);
    }
  }
}

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

#pragma once

#include <cstddef>
#include <memory>
#include <utility>

#include "SIMPLib/SIMPLib.h"

class AbstractFilter;
class DataArrayPath;
class DataContainerArray;

using AbstractFilterShPtr = std::shared_ptr<AbstractFilter>;
using DataContainerArrayShPtr = std::shared_ptr<DataContainerArray>;

/**
 * @brief RenameDataPath is used by DataContainerArray, DataContainer, and AttributeMatrix
 * to alert the specified AbstractFilter that a DataArrayPath has been created
 * with the given DataID_t.  This also serves as the declaration of the type alias,
 * DataID_t so that all classes that require it have access to it without causing
 * dependency issues.
 */
namespace RenameDataPath
{
using RenamePathType = std::pair<DataArrayPath, DataArrayPath>;
using RenameContainer = std::list<RenamePathType>;
using PossibleRename = std::pair<bool, RenamePathType>;

/**
 * @brief DataID_t is used as the ID type when creating DataArrayPaths through
 * createNonPrereq* methods inside AbstractFilter::dataCheck().  This ID is
 * used for mapping created paths and performing quick lookups to detect rename
 * operations far more quickly than previous implementations for rename
 * operations. This forces filters to opt-in to rename functionality rather than
 * provide it across all filters automatically.
 */
using DataID_t = size_t;
const DataID_t k_Invalid_ID = 0;

/**
 * @brief This function serves as the main hub for alerting the specified
 * AbstractFilter that a DataArrayPath has been created with the given ID.
 * The plus side of this is that AbstractFilter has a single friend function
 * instead of having to declare DataContainerArray, DataContainer, and AttributeMatrix
 * all as friend classes in order to achieve the same result.
 * @param filter The filter creating the path
 * @param id The local ID for the filter to perform rename checks on
 * @param path The new DataArrayPath
 */
void SIMPLib_EXPORT AlertFilterCreatedPath(AbstractFilter* filter, DataID_t id, const DataArrayPath& path);

/**
 * @brief Checks if two rename values are compatible to be merged together.
 * @param originalPair
 * @param newPair
 * @return
 */
bool SIMPLib_EXPORT CanReplaceRenamePath(const RenamePathType& originalPair, const RenamePathType& newPair);

/**
 * @brief checks if the targets of the old DataArrayPath and new DataArrayPath are compatible in their given DataContainerArrays
 * @param oldDca
 * @param newDca
 * @param oldPath
 * @param newPath
 */
bool SIMPLib_EXPORT CheckRenamePath(const DataContainerArrayShPtr& oldDca, const DataContainerArrayShPtr& newDca, const DataArrayPath& oldPath, const DataArrayPath& newPath);

/**
 * @brief checks for and returns any updated DataArrayPaths between two sets
 * @param oldPaths
 * @param newPaths
 * @return
 */
RenameContainer SIMPLib_EXPORT CheckForRenamedPaths(const DataContainerArrayShPtr& oldDca, const DataContainerArrayShPtr& newDca, const std::list<DataArrayPath>& oldPaths,
                                                    const std::list<DataArrayPath>& newPaths);

/**
 * @brief Calculate the rename paths for the given filter and removes rename paths as required by created paths.
 * The updated rename paths are added to the list parameter.
 * @param filter
 * @param renamePaths
 */
void SIMPLib_EXPORT CalculateRenamedPaths(const AbstractFilterShPtr& filter, RenameContainer& renamedPaths);

/**
 * @brief Merges a list of DataArrayPath renames into an existing list.
 * @param list
 * @param newRenames
 */
void SIMPLib_EXPORT MergeRenamedPaths(RenameContainer& list, const RenameContainer& newRenames);
} // namespace RenameDataPath

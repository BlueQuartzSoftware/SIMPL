/* ============================================================================
 * Copyright (c) 2009-2018 BlueQuartz Software, LLC
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

#include "TransformContainer.h"
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"
#include "SIMPLib/Common/Constants.h"

#include "H5Support/H5ScopedSentinel.h"

TransformContainer::TransformContainer() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TransformContainer::~TransformContainer() = default;

int TransformContainer::writeTransformContainerToHDF5(hid_t parentId, const std::string& transformName)
{
  herr_t err = 0;
  err = H5Utilities::createGroupsFromPath(transformName, parentId);
  if(err < 0)
  {
    return err;
  }
  hid_t transformId = H5Gopen(parentId, transformName.c_str(), H5P_DEFAULT);
  if(transformId < 0)
  {
    return -1;
  }
  H5ScopedGroupSentinel gSentinel(&transformId, false);
  std::vector<hsize_t> dims(1);
  TransformParametersType parameters = this->getParameters();
  dims[0] = parameters.size();
  err = H5Lite::writeVectorDataset(transformId, SIMPL::Geometry::TransformContainerParameters.toLatin1().data(), dims, parameters);
  if(err < 0)
  {
    return err;
  }
  err = H5Lite::writeStringAttribute(parentId, transformName, SIMPL::Geometry::TransformContainerTypeName.toLatin1().data(), SIMPL::Geometry::TransformContainer.toLatin1().data());
  if(err < 0)
  {
    return err;
  }
  err = H5Lite::writeStringAttribute(parentId, transformName, SIMPL::Geometry::TransformContainerMovingName.toLatin1().data(), this->getMovingName().c_str());
  if(err < 0)
  {
    return err;
  }
  err = H5Lite::writeStringAttribute(parentId, transformName, SIMPL::Geometry::TransformContainerReferenceName.toLatin1().data(), this->getReferenceName().c_str());
  if(err < 0)
  {
    return err;
  }
  dims[0] = this->getTransformTypeAsString().size();
  err = H5Lite::writeStringDataset(transformId, SIMPL::Geometry::TransformContainerTypeAsString.toLatin1().data(), this->getTransformTypeAsString().c_str());
  if(err < 0)
  {
    return err;
  }
  TransformFixedParametersType fixedParameters = this->getFixedParameters();
  dims[0] = fixedParameters.size();
  if(dims[0] > 0)
  {
    err = H5Lite::writeVectorDataset(transformId, SIMPL::Geometry::TransformContainerFixedParameters.toLatin1().data(), dims, fixedParameters);
  }
  return err;
}

int TransformContainer::readTransformContainerFromHDF5(hid_t parentId, bool SIMPL_NOT_USED(metaDataOnly), const std::string& transformName)
{
  herr_t err = 0;
  err = H5Utilities::createGroupsFromPath(transformName, parentId);
  if(err < 0)
  {
    return err;
  }
  hid_t transformId = H5Gopen(parentId, transformName.c_str(), H5P_DEFAULT);
  if(transformId < 0)
  {
    return -1;
  }
  H5ScopedGroupSentinel gSentinel(&transformId, false);
  TransformParametersType parameters;
  err = H5Lite::readVectorDataset(transformId, SIMPL::Geometry::TransformContainerParameters.toLatin1().data(), parameters);
  if(err < 0)
  {
    return err;
  }
  this->setParameters(parameters);
  std::string movingName;
  err = H5Lite::readStringAttribute(parentId, transformName, SIMPL::Geometry::TransformContainerMovingName.toLatin1().data(), movingName);
  // An error can occur if the attribute is missing or empty. If it is the case, we use an empty string.
  if(err < 0)
  {
    movingName = "";
  }
  this->setMovingName(movingName);
  std::string referenceName;
  err = H5Lite::readStringAttribute(parentId, transformName, SIMPL::Geometry::TransformContainerReferenceName.toLatin1().data(), referenceName);
  // An error can occur if the attribute is missing or empty. If it is the case, we use an empty string.
  if(err < 0)
  {
    referenceName = "";
  }
  this->setReferenceName(referenceName);
  std::string transformTypeAsString;
  err = H5Lite::readStringDataset(transformId, SIMPL::Geometry::TransformContainerTypeAsString.toLatin1().data(), transformTypeAsString);
  if(err < 0)
  {
    return err;
  }
  this->setTransformTypeAsString(transformTypeAsString);
  TransformFixedParametersType fixedParameters;
  err = H5Lite::readVectorDataset(transformId, SIMPL::Geometry::TransformContainerFixedParameters.toLatin1().data(), fixedParameters);
  if(err >= 0)
  {
    this->setFixedParameters(fixedParameters);
  }
  return 0;
}

TransformContainer& TransformContainer::operator=(const TransformContainer& old)
{
  if(this != &old)
  {
    this->m_Parameters = old.m_Parameters;
    this->m_FixedParameters = old.m_FixedParameters;
    this->m_TransformTypeAsString = old.m_TransformTypeAsString;
    this->m_MovingName = old.m_MovingName;
    this->m_ReferenceName = old.m_ReferenceName;
  }
  return *this;
}
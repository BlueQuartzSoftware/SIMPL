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

#include "CompositeTransformContainer.h"

#include "H5Support/H5Lite.h"
#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Geometry/TransformContainer.h"

CompositeTransformContainer::CompositeTransformContainer() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CompositeTransformContainer::~CompositeTransformContainer() = default;

int CompositeTransformContainer::writeTransformContainerToHDF5(hid_t parentId, const std::string& transformContainerName)
{
  herr_t err = 0;

  hid_t transformContainerId;
  err = QH5Utilities::createGroupsFromPath(transformContainerName.c_str(), parentId);
  if(err < 0)
  {
    return err;
  }
  transformContainerId = H5Gopen(parentId, transformContainerName.c_str(), H5P_DEFAULT);
  if(transformContainerId < 0)
  {
    return -1;
  }
  H5ScopedGroupSentinel gSentinel(&transformContainerId, false);

  err = QH5Lite::writeStringAttribute(parentId, transformContainerName.c_str(), SIMPL::Geometry::TransformContainerTypeName, SIMPL::Geometry::CompositeTransformContainer);
  if(err < 0)
  {
    return err;
  }

  size_t position = 0;
  for(auto transformContainer : m_TransformContainers)
  {
    QString childTransformContainerName = QString::number(position);
    err = transformContainer->writeTransformContainerToHDF5(transformContainerId, childTransformContainerName.toLatin1().data());
    if(err < 0)
    {
      return err;
    }
    position += 1;
  }
  return err;
}

int CompositeTransformContainer::readTransformContainerFromHDF5(hid_t parentId, bool metaDataOnly, const std::string& transformContainerName)
{
  herr_t err = 0;

  hid_t transformContainerGrpId = H5Gopen(parentId, transformContainerName.c_str(), H5P_DEFAULT);
  if(transformContainerGrpId < 0)
  {
    return -1;
  }
  H5ScopedGroupSentinel gSentinel(&transformContainerGrpId, false);

  hsize_t idx = 0;
  H5Gget_num_objs(transformContainerGrpId, &idx);
  for(hsize_t i = 0; i < idx; i++)
  {
    QString childTransformContainerName = QString::number(i);
    err = QH5Utilities::createGroupsFromPath(childTransformContainerName, transformContainerGrpId);
    if(err < 0)
    {
      return err;
    }
    std::string transformContainerTypeName = SIMPL::Geometry::UnknownTransformContainer.toStdString();
    err =
        H5Lite::readStringAttribute(transformContainerGrpId, childTransformContainerName.toLatin1().data(), SIMPL::Geometry::TransformContainerTypeName.toLatin1().data(), transformContainerTypeName);
    if(err < 0)
    {
      return err;
    }

    if(transformContainerTypeName.compare(SIMPL::Geometry::TransformContainer.toStdString()) == 0)
    {
      TransformContainer::Pointer transformContainer = TransformContainer::New();
      err = transformContainer->readTransformContainerFromHDF5(transformContainerGrpId, metaDataOnly, childTransformContainerName.toLatin1().data());
      if(err < 0)
      {
        return err;
      }
      this->addTransformContainer(transformContainer);
    }
    else if(transformContainerTypeName.compare(SIMPL::Geometry::CompositeTransformContainer.toStdString()) == 0)
    {
      CompositeTransformContainer::Pointer CompositeTransformContainer = CompositeTransformContainer::New();
      err = CompositeTransformContainer->readTransformContainerFromHDF5(transformContainerGrpId, metaDataOnly, childTransformContainerName.toLatin1().data());
      if(err < 0)
      {
        return err;
      }
      this->addTransformContainer(CompositeTransformContainer);
    }
    else // SIMPL::Geometry::UnknownTransformContainer
    {
      return -1;
    }
    return 0;
  }
  return err;
}

CompositeTransformContainer& CompositeTransformContainer::operator=(const CompositeTransformContainer& old)
{
  if(this != &old)
  {
    this->m_TransformContainers = old.m_TransformContainers;
  }
  return *this;
}

void CompositeTransformContainer::addTransformContainer(ITransformContainer::Pointer transformContainer)
{
  this->m_TransformContainers.push_back(transformContainer);
}
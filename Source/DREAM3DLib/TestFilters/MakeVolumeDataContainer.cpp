/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
* Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
* BlueQuartz Software nor the names of its contributors may be used to endorse
* or promote products derived from this software without specific prior written
* permission.
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
*  This code was written under United States Air Force Contract number
*                           FA8650-10-D-5210
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MakeVolumeDataContainer.h"

#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MakeVolumeDataContainer::MakeVolumeDataContainer() :
AbstractFilter(),
m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MakeVolumeDataContainer::~MakeVolumeDataContainer()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeVolumeDataContainer::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MakeVolumeDataContainer::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeVolumeDataContainer::dataCheck()
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if (getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if (getErrorCondition() < 0) { return; }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeVolumeDataContainer::preflight()
{
  dataCheck();

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeVolumeDataContainer::execute()
{
  int err = 0;
  setErrorCondition(err);

  // Run the data check to get references to all of our data arrays initialized to the values stored in memory
  dataCheck();
  if (getErrorCondition() < 0) { return; }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");

}
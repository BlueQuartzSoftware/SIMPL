/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* RedisTetbution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* RedisTetbutions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* RedisTetbutions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the disTetbution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* conTetbutors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTetBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTetBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STetCT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedTetList::Pointer GEOM_CLASS_NAME::CreateSharedTetList(int64_t numTets, bool allocate)
{
  QVector<size_t> tetDims(1, 4);
  SharedTetList::Pointer tets = SharedTetList::CreateArray(numTets, tetDims, SIMPL::Geometry::SharedTetList, allocate);
  tets->initializeWithZeros();
  return tets;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::resizeTetList(int64_t newNumTets)
{
  m_TetList->resize(newNumTets);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setTetrahedra(SharedTetList::Pointer tets)
{
  if(tets.get() != nullptr)
  {
    if(tets->getName().compare(SIMPL::Geometry::SharedTetList) != 0)
    {
      tets->setName(SIMPL::Geometry::SharedTetList);
    }
  }
  m_TetList = tets;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedTetList::Pointer GEOM_CLASS_NAME::getTetrahedra()
{
  return m_TetList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setVertsAtTet(int64_t tetId, int64_t verts[4])
{
  int64_t* tet = m_TetList->getTuplePointer(tetId);
  tet[0] = verts[0];
  tet[1] = verts[1];
  tet[2] = verts[2];
  tet[3] = verts[3];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertsAtTet(int64_t tetId, int64_t verts[4])
{
  int64_t* tet = m_TetList->getTuplePointer(tetId);
  verts[0] = tet[0];
  verts[1] = tet[1];
  verts[2] = tet[2];
  verts[3] = tet[3];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertCoordsAtTet(int64_t tetId, float vert1[3], float vert2[3], float vert3[3], float vert4[3])
{
  int64_t* tet = m_TetList->getTuplePointer(tetId);
  float* tmp1 = m_VertexList->getTuplePointer(tet[0]);
  float* tmp2 = m_VertexList->getTuplePointer(tet[1]);
  float* tmp3 = m_VertexList->getTuplePointer(tet[2]);
  float* tmp4 = m_VertexList->getTuplePointer(tet[3]);
  vert1[0] = tmp1[0];
  vert1[1] = tmp1[1];
  vert1[2] = tmp1[2];
  vert2[0] = tmp2[0];
  vert2[1] = tmp2[1];
  vert2[2] = tmp2[2];
  vert3[0] = tmp3[0];
  vert3[1] = tmp3[1];
  vert3[2] = tmp3[2];
  vert4[0] = tmp4[0];
  vert4[1] = tmp4[1];
  vert4[2] = tmp4[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t* GEOM_CLASS_NAME::getTetPointer(int64_t i)
{
  return m_TetList->getTuplePointer(i);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t GEOM_CLASS_NAME::getNumberOfTets()
{
  return m_TetList->getNumberOfTuples();
}

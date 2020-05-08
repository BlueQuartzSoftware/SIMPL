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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedVertexList::Pointer GEOM_CLASS_NAME::CreateSharedVertexList(size_t numVertices, bool allocate)
{
  std::vector<size_t> vertDims = {3};
  SharedVertexList::Pointer vertices = SharedVertexList::CreateArray(numVertices, vertDims, SIMPL::Geometry::SharedVertexList, allocate);
  vertices->initializeWithZeros();
  return vertices;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::resizeVertexList(size_t newNumVertices)
{
  m_VertexList->resizeTuples(newNumVertices);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setVertices(SharedVertexList::Pointer vertices)
{
  if(vertices.get() != nullptr)
  {
    if(vertices->getName() != SIMPL::Geometry::SharedVertexList)
    {
      vertices->setName(SIMPL::Geometry::SharedVertexList);
    }
  }
  m_VertexList = vertices;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedVertexList::Pointer GEOM_CLASS_NAME::getVertices() const
{
  return m_VertexList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setCoords(size_t vertId, float coords[3])
{
  float* Vert = m_VertexList->getTuplePointer(vertId);
  Vert[0] = coords[0];
  Vert[1] = coords[1];
  Vert[2] = coords[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getCoords(size_t vertId, float coords[3]) const
{
  float* Vert = m_VertexList->getTuplePointer(vertId);
  coords[0] = Vert[0];
  coords[1] = Vert[1];
  coords[2] = Vert[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float* GEOM_CLASS_NAME::getVertexPointer(size_t i) const
{
  return m_VertexList->getTuplePointer(i);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t GEOM_CLASS_NAME::getNumberOfVertices() const
{
  return m_VertexList->getNumberOfTuples();
}

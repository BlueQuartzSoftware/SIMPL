// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedHexList::Pointer GEOM_CLASS_NAME::CreateSharedHexList(int64_t numHexas, bool allocate)
{
  QVector<size_t> hexDims(1, 8);
  SharedHexList::Pointer hexas = SharedHexList::CreateArray(numHexas, hexDims, SIMPL::Geometry::SharedHexList, allocate);
  hexas->initializeWithZeros();
  return hexas;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::resizeHexList(int64_t newNumHexas)
{
  m_HexList->resize(newNumHexas);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setHexahedra(SharedHexList::Pointer hexas)
{
  if(hexas.get() != nullptr)
  {
    if(hexas->getName().compare(SIMPL::Geometry::SharedHexList) != 0)
    {
      hexas->setName(SIMPL::Geometry::SharedHexList);
    }
  }
  m_HexList = hexas;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedHexList::Pointer GEOM_CLASS_NAME::getHexahedra()
{
  return m_HexList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setVertsAtHex(int64_t hexId, int64_t verts[8])
{
  int64_t* hex = m_HexList->getTuplePointer(hexId);
  hex[0] = verts[0];
  hex[1] = verts[1];
  hex[2] = verts[2];
  hex[3] = verts[3];
  hex[4] = verts[4];
  hex[5] = verts[5];
  hex[6] = verts[6];
  hex[7] = verts[7];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertsAtHex(int64_t hexId, int64_t verts[8])
{
  int64_t* hex = m_HexList->getTuplePointer(hexId);
  verts[0] = hex[0];
  verts[1] = hex[1];
  verts[2] = hex[2];
  verts[3] = hex[3];
  verts[4] = hex[4];
  verts[5] = hex[5];
  verts[6] = hex[6];
  verts[7] = hex[7];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertCoordsAtHex(int64_t hexId, float vert1[3], float vert2[3], float vert3[3], float vert4[3], float vert5[3], float vert6[3], float vert7[3], float vert8[3])
{
  int64_t* hex = m_HexList->getTuplePointer(hexId);
  float* tmp1 = m_VertexList->getTuplePointer(hex[0]);
  float* tmp2 = m_VertexList->getTuplePointer(hex[1]);
  float* tmp3 = m_VertexList->getTuplePointer(hex[2]);
  float* tmp4 = m_VertexList->getTuplePointer(hex[3]);
  float* tmp5 = m_VertexList->getTuplePointer(hex[4]);
  float* tmp6 = m_VertexList->getTuplePointer(hex[5]);
  float* tmp7 = m_VertexList->getTuplePointer(hex[6]);
  float* tmp8 = m_VertexList->getTuplePointer(hex[7]);
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
  vert5[0] = tmp5[0];
  vert5[1] = tmp5[1];
  vert5[2] = tmp5[2];
  vert6[0] = tmp6[0];
  vert6[1] = tmp6[1];
  vert6[2] = tmp6[2];
  vert7[0] = tmp7[0];
  vert7[1] = tmp7[1];
  vert7[2] = tmp7[2];
  vert8[0] = tmp8[0];
  vert8[1] = tmp8[1];
  vert8[2] = tmp8[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t* GEOM_CLASS_NAME::getHexPointer(int64_t i)
{
  return m_HexList->getTuplePointer(i);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t GEOM_CLASS_NAME::getNumberOfHexas()
{
  return m_HexList->getNumberOfTuples();
}

/* ============================================================================
 * Copyright (c) 2022-2022 BlueQuartz Software, LLC
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PadImageGeometryImpl.h"

#include "SIMPLib/CoreFilters/PadImageGeometry.h"

namespace Detail
{
template <typename T>
void fillDataArray(const AttributeMatrix& srcAttrMatrix, AttributeMatrix& destAttrMatrix, const QString& dataArrayName, int defaultFillValue, int xMinPadding, int yMinPadding, int zMinPadding)
{
  typename DataArray<T>::Pointer srcArray = srcAttrMatrix.getAttributeArrayAs<DataArray<T>>(dataArrayName);
  typename DataArray<T>::Pointer destArray = destAttrMatrix.getAttributeArrayAs<DataArray<T>>(dataArrayName);

  // Initialize the array with the default value to start
  if constexpr(std::is_same<T, bool>::value)
  {
    destArray->initializeWithValue(defaultFillValue != 0);
  }
  else
  {
    destArray->initializeWithValue(static_cast<T>(defaultFillValue));
  }

  // Copy contents from srcArray to destArray
  SizeVec3Type srcDims = srcAttrMatrix.getTupleDimensions();
  SizeVec3Type destDims = destAttrMatrix.getTupleDimensions();
  size_t compCount = srcArray->getNumberOfComponents();
  T* srcArrayPtr = srcArray->getPointer(0);
  T* destArrayPtr = destArray->getPointer(0);
  for(size_t z = 0; z < srcDims[2]; z++)
  {
    for(size_t y = 0; y < srcDims[1]; y++)
    {
      for(size_t x = 0; x < srcDims[0]; x++)
      {
        size_t srcIndex = (z * srcDims[1] * srcDims[0]) + (y * srcDims[0]) + x;
        size_t destIndex = ((z + zMinPadding) * destDims[1] * destDims[0]) + ((y + yMinPadding) * destDims[0]) + (x + xMinPadding);
        for(size_t c = 0; c < compCount; c++)
        {
          destArrayPtr[destIndex * compCount + c] = srcArrayPtr[srcIndex * compCount + c];
        }
      }
    }
  }
}
} // namespace Detail

// -----------------------------------------------------------------------------
PadImageGeometryImpl::PadImageGeometryImpl(PadImageGeometry* filter, AttributeMatrix& srcAttrMatrix, AttributeMatrix& destAttrMatrix, const QString& dataArrayName, int defaultFillValue,
                                           const IntVec3Type& minPadding)
: m_Filter(filter)
, m_SrcAttrMatrix(srcAttrMatrix)
, m_DestAttrMatrix(destAttrMatrix)
, m_ArrayName(dataArrayName)
, m_DefaultFillValue(defaultFillValue)
, m_MinPadding(minPadding)
{
}

// -----------------------------------------------------------------------------
PadImageGeometryImpl::~PadImageGeometryImpl() = default;

// -----------------------------------------------------------------------------
void PadImageGeometryImpl::operator()() const
{
  IDataArray::Pointer srcArray = m_SrcAttrMatrix.getAttributeArray(m_ArrayName);

  QString arrayType = srcArray->getTypeAsString();
  if(arrayType == "int8_t")
  {
    Detail::fillDataArray<int8_t>(m_SrcAttrMatrix, m_DestAttrMatrix, m_ArrayName, m_DefaultFillValue, m_MinPadding[0], m_MinPadding[1], m_MinPadding[2]);
  }
  else if(arrayType == "int16_t")
  {
    Detail::fillDataArray<int16_t>(m_SrcAttrMatrix, m_DestAttrMatrix, m_ArrayName, m_DefaultFillValue, m_MinPadding[0], m_MinPadding[1], m_MinPadding[2]);
  }
  else if(arrayType == "int32_t")
  {
    Detail::fillDataArray<int32_t>(m_SrcAttrMatrix, m_DestAttrMatrix, m_ArrayName, m_DefaultFillValue, m_MinPadding[0], m_MinPadding[1], m_MinPadding[2]);
  }
  else if(arrayType == "int64_t")
  {
    Detail::fillDataArray<int64_t>(m_SrcAttrMatrix, m_DestAttrMatrix, m_ArrayName, m_DefaultFillValue, m_MinPadding[0], m_MinPadding[1], m_MinPadding[2]);
  }
  else if(arrayType == "uint8_t")
  {
    Detail::fillDataArray<uint8_t>(m_SrcAttrMatrix, m_DestAttrMatrix, m_ArrayName, m_DefaultFillValue, m_MinPadding[0], m_MinPadding[1], m_MinPadding[2]);
  }
  else if(arrayType == "uint16_t")
  {
    Detail::fillDataArray<uint16_t>(m_SrcAttrMatrix, m_DestAttrMatrix, m_ArrayName, m_DefaultFillValue, m_MinPadding[0], m_MinPadding[1], m_MinPadding[2]);
  }
  else if(arrayType == "uint32_t")
  {
    Detail::fillDataArray<uint32_t>(m_SrcAttrMatrix, m_DestAttrMatrix, m_ArrayName, m_DefaultFillValue, m_MinPadding[0], m_MinPadding[1], m_MinPadding[2]);
  }
  else if(arrayType == "uint64_t")
  {
    Detail::fillDataArray<uint64_t>(m_SrcAttrMatrix, m_DestAttrMatrix, m_ArrayName, m_DefaultFillValue, m_MinPadding[0], m_MinPadding[1], m_MinPadding[2]);
  }
  else if(arrayType == "float")
  {
    Detail::fillDataArray<float>(m_SrcAttrMatrix, m_DestAttrMatrix, m_ArrayName, m_DefaultFillValue, m_MinPadding[0], m_MinPadding[1], m_MinPadding[2]);
  }
  else if(arrayType == "double")
  {
    Detail::fillDataArray<double>(m_SrcAttrMatrix, m_DestAttrMatrix, m_ArrayName, m_DefaultFillValue, m_MinPadding[0], m_MinPadding[1], m_MinPadding[2]);
  }
  else if(arrayType == "bool")
  {
    Detail::fillDataArray<bool>(m_SrcAttrMatrix, m_DestAttrMatrix, m_ArrayName, m_DefaultFillValue, m_MinPadding[0], m_MinPadding[1], m_MinPadding[2]);
  }
  else
  {
    QString ss = QObject::tr("Array '%1', contained in attribute matrix '%2', has an unidentified array type.").arg(srcArray->getName(), m_SrcAttrMatrix.getName());
    m_Filter->setErrorCondition(-3010, ss);
    return;
  }

  // Clean up the old array
  m_SrcAttrMatrix.removeAttributeArray(srcArray->getName());
}

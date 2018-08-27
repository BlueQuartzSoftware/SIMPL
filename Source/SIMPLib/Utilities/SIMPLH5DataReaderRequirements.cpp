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

#include "SIMPLH5DataReaderRequirements.h"

#include "SIMPLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLH5DataReaderRequirements::SIMPLH5DataReaderRequirements() = default;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLH5DataReaderRequirements::SIMPLH5DataReaderRequirements(const QString &primitiveType, size_t allowedCompDim, AttributeMatrix::Type attributeMatrixType, IGeometry::Type geometryType)
{
  if(primitiveType.compare(SIMPL::Defaults::AnyPrimitive) != 0)
  {
    m_DATypes = QVector<QString>(1, primitiveType);
  }
  if(SIMPL::Defaults::AnyComponentSize != allowedCompDim)
  {
    m_ComponentDimensions = QVector<QVector<size_t> >(1, QVector<size_t>(1, allowedCompDim));
  }
  if(AttributeMatrix::Type::Any != attributeMatrixType)
  {
    AttributeMatrix::Types amTypes(1, attributeMatrixType);
    m_AMTypes = amTypes;
  }
  if(IGeometry::Type::Any != geometryType)
  {
    IGeometry::Types geomTypes(1, geometryType);
    m_DCGeometryTypes = geomTypes;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLH5DataReaderRequirements::~SIMPLH5DataReaderRequirements() = default;

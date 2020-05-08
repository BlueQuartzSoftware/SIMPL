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

#pragma once

#include <vector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/Geometry/IGeometry.h"

/**
 * @brief The SIMPLH5DataReaderRequirements class
 */
class SIMPLib_EXPORT SIMPLH5DataReaderRequirements
{

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(SIMPLH5DataReaderRequirements)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  SIMPLH5DataReaderRequirements();
  SIMPLH5DataReaderRequirements(const QString& primitiveType, size_t allowedCompDim, AttributeMatrix::Type attributeMatrixType, IGeometry::Type geometryType);

  virtual ~SIMPLH5DataReaderRequirements();

  using QVectorSizeT = std::vector<std::vector<size_t>>;

  /**
   * @brief Setter property for DCGeometryTypes
   */
  void setDCGeometryTypes(const IGeometry::Types& value);
  /**
   * @brief Getter property for DCGeometryTypes
   * @return Value of DCGeometryTypes
   */
  IGeometry::Types getDCGeometryTypes() const;

  /**
   * @brief Setter property for AMTypes
   */
  void setAMTypes(const AttributeMatrix::Types& value);
  /**
   * @brief Getter property for AMTypes
   * @return Value of AMTypes
   */
  AttributeMatrix::Types getAMTypes() const;

  /**
   * @brief Setter property for DATypes
   */
  void setDATypes(const QVector<QString>& value);
  /**
   * @brief Getter property for DATypes
   * @return Value of DATypes
   */
  QVector<QString> getDATypes() const;

  /**
   * @brief Setter property for ComponentDimensions
   */
  void setComponentDimensions(const SIMPLH5DataReaderRequirements::QVectorSizeT& value);
  /**
   * @brief Getter property for ComponentDimensions
   * @return Value of ComponentDimensions
   */
  SIMPLH5DataReaderRequirements::QVectorSizeT getComponentDimensions() const;

private:
  IGeometry::Types m_DCGeometryTypes = {};
  AttributeMatrix::Types m_AMTypes = {};
  QVector<QString> m_DATypes = {};
  SIMPLH5DataReaderRequirements::QVectorSizeT m_ComponentDimensions = {};
};

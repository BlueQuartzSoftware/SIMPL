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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <set>

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/AxisAngleFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/FourthOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/SecondOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/ThirdOrderPolynomialFilterParameter.h"
#include "SIMPLib/Filtering/ComparisonInputs.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

class AbstractFilter;

#define WRITE_ARRAY_SELECTION_PARAMETER(Name) writer->writeArraySelections(#Name, getSelected##Name##Arrays());

/**
 * @class AbstractFilterParametersWriter AbstractFilterParametersWriter.h DREAM3DLib/Common/AbstractFilterParametersWriter.h
 * @brief This class writes the various parameters of a filter to an output file
 *
 * @date Jan 17, 2012
 * @version 1.0
 */
class SIMPLib_EXPORT AbstractFilterParametersWriter
{
public:
  /**
   * @brief Returns the name of the class for AbstractFilterParametersWriter
   */
  virtual QString getNameOfClass() const;
  /**
   * @brief Returns the name of the class for AbstractFilterParametersWriter
   */
  static QString ClassName();

  virtual ~AbstractFilterParametersWriter();

  virtual int openFilterGroup(AbstractFilter* filter, int index) = 0;
  virtual int closeFilterGroup() = 0;

  virtual int writePipelineToFile(FilterPipeline::Pointer pipeline, QString filePath, QString pipelineName, bool expandPipeline, QList<IObserver*> obs = QList<IObserver*>()) = 0;

  virtual int writeValue(const QString& name, const QString& value);
  virtual int writeValue(const QString& name, const QVector<QString>& value);
  virtual int writeValue(const QString& name, const QStringList& value);

  virtual int writeValue(const QString& name, int8_t value);
  virtual int writeValue(const QString& name, int16_t value);
  virtual int writeValue(const QString& name, int32_t value);
  virtual int writeValue(const QString& name, int64_t value);
  virtual int writeValue(const QString& name, uint8_t value);
  virtual int writeValue(const QString& name, uint16_t value);
  virtual int writeValue(const QString& name, uint32_t value);
  virtual int writeValue(const QString& name, uint64_t value);
  virtual int writeValue(const QString& name, float value);
  virtual int writeValue(const QString& name, double value);

  virtual int writeValue(const QString& name, QVector<int8_t> value);
  virtual int writeValue(const QString& name, QVector<int16_t> value);
  virtual int writeValue(const QString& name, QVector<int32_t> value);
  virtual int writeValue(const QString& name, QVector<int64_t> value);
  virtual int writeValue(const QString& name, QVector<uint8_t> value);
  virtual int writeValue(const QString& name, QVector<uint16_t> value);
  virtual int writeValue(const QString& name, QVector<uint32_t> value);
  virtual int writeValue(const QString& name, QVector<uint64_t> value);
  virtual int writeValue(const QString& name, QVector<float> value);
  virtual int writeValue(const QString& name, QVector<double> value);

  virtual int writeValue(const QString& name, IntVec3Type v);
  virtual int writeValue(const QString& name, FloatVec3Type v);
  virtual int writeValue(const QString& name, Float2ndOrderPolynomial v);
  virtual int writeValue(const QString& name, Float3rdOrderPoly_t v);
  virtual int writeValue(const QString& name, Float4thOrderPolynomial v);
  virtual int writeValue(const QString& name, StackFileListInfo v);

  virtual int writeValue(const QString& name, ComparisonInput_t v);
  virtual int writeValue(const QString& name, ComparisonInputs v);

  virtual int writeValue(const QString& name, QVector<AxisAngleInput> v);

  virtual int writeArraySelections(const QString& name, QSet<QString> v);

  virtual int writeValue(const QString& name, DataContainerArrayProxy& dcaProxy);

  virtual int writeValue(const QString& name, AxisAngleInput v);

  virtual int writeValue(const QString& name, const DataArrayPath& path);

  virtual int writeValue(const QString& name, const QVector<DataArrayPath>& path);

  virtual int writeValue(const QString& name, const DynamicTableData& v);

  virtual int writeValue(const QString& name, const QPair<double, double>& v);

protected:
  AbstractFilterParametersWriter();

  virtual int writeValue(const QString& name, AxisAngleInput v, int vectorPos);

public:
  AbstractFilterParametersWriter(const AbstractFilterParametersWriter&) = delete;            // Copy Constructor Not Implemented
  AbstractFilterParametersWriter(AbstractFilterParametersWriter&&) = delete;                 // Move Constructor Not Implemented
  AbstractFilterParametersWriter& operator=(const AbstractFilterParametersWriter&) = delete; // Copy Assignment Not Implemented
  AbstractFilterParametersWriter& operator=(AbstractFilterParametersWriter&&) = delete;      // Move Assignment Not Implemented

private:
};

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
#include <memory>

#include "DynamicTableExample.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableExample::DynamicTableExample() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableExample::~DynamicTableExample() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableExample::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  /* Place all your option initialization code here */

  // Table 1 - Fixed rows and columns, no default data passed in
  {
    // We are using the expanded way to create the filter parameter, because the getter and setter variables are different
    parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Dynamic Table 1", DynamicData1, FilterParameter::Parameter, DynamicTableExample));
  }

  // Table 2 - Fixed rows and columns, default data passed in
  {
    QStringList rHeaders, cHeaders;
    rHeaders << "Passed RowName 1"
             << "Passed RowName 2"
             << "Passed RowName 3"
             << "Passed RowName 4";
    cHeaders << "Passed ColName 1";
    std::vector<std::vector<double>> defaultTable(4, std::vector<double>(1, 3.87));
    m_DynamicData2.setColHeaders(cHeaders);
    m_DynamicData2.setRowHeaders(rHeaders);
    m_DynamicData2.setTableData(defaultTable);
    parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Dynamic Table 2", DynamicData2, FilterParameter::Parameter, DynamicTableExample));
  }

  // Table 3 - Dynamic rows and fixed columns, default data passed in
  {
    QStringList rHeaders, cHeaders;
    rHeaders << "Passed RowName 1"
             << "Passed RowName 2"
             << "Passed RowName 3";
    cHeaders << "Passed ColName 1"
             << "Passed ColName 2"
             << "Passed ColName 3"
             << "Passed ColName 4"
             << "Passed ColName 5";
    std::vector<std::vector<double>> defaultTable(3, std::vector<double>(5, 1.34));
    m_DynamicData3.setColHeaders(cHeaders);
    m_DynamicData3.setRowHeaders(rHeaders);
    m_DynamicData3.setTableData(defaultTable);
    m_DynamicData3.setDynamicRows(true);
    parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Dynamic Table 3", DynamicData3, FilterParameter::Parameter, DynamicTableExample));
  }

  // Table 4 - Fixed rows and dynamic columns, default data passed in
  {
    QStringList rHeaders, cHeaders;
    rHeaders << "Passed RowName 1"
             << "Passed RowName 2";
    cHeaders << "Passed ColName 1"
             << "Passed ColName 2";
    std::vector<std::vector<double>> defaultTable(2, std::vector<double>(2, 3));
    m_DynamicData4.setColHeaders(cHeaders);
    m_DynamicData4.setRowHeaders(rHeaders);
    m_DynamicData4.setTableData(defaultTable);
    m_DynamicData4.setDynamicCols(true);
    parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Dynamic Table 4", DynamicData4, FilterParameter::Parameter, DynamicTableExample));
  }

  // Table 5 - Dynamic rows and dynamic columns, default data passed in
  {
    QStringList rHeaders, cHeaders;
    rHeaders << "Passed RowName 1"
             << "Passed RowName 2";
    cHeaders << "Passed ColName 1"
             << "Passed ColName 2";
    std::vector<std::vector<double>> defaultTable(2, std::vector<double>(2, 3));
    m_DynamicData5.setColHeaders(cHeaders);
    m_DynamicData5.setRowHeaders(rHeaders);
    m_DynamicData5.setTableData(defaultTable);
    m_DynamicData5.setDynamicRows(true);
    m_DynamicData5.setDynamicCols(true);
    m_DynamicData5.setMinRows(6);
    m_DynamicData5.setDefaultRowCount(3);
    m_DynamicData5.setMinCols(3);
    m_DynamicData5.setDefaultColCount(5);
    parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Dynamic Table 5", DynamicData5, FilterParameter::Parameter, DynamicTableExample));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableExample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{

  reader->openFilterGroup(this, index);
  setDynamicData1(reader->readDynamicTableData("DynamicData", getDynamicData1()));
  setDynamicData2(reader->readDynamicTableData("DynamicData", getDynamicData2()));
  setDynamicData3(reader->readDynamicTableData("DynamicData", getDynamicData3()));
  setDynamicData4(reader->readDynamicTableData("DynamicData", getDynamicData4()));
  setDynamicData5(reader->readDynamicTableData("DynamicData", getDynamicData5()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableExample::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableExample::dataCheck()
{
  // This filter does nothing during the dataCheck!
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableExample::execute()
{
  // This filter does nothing during execute!
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer DynamicTableExample::newFilterInstance(bool copyFilterParameters) const
{
  DynamicTableExample::Pointer filter = DynamicTableExample::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableExample::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableExample::getGroupName() const
{
  return SIMPL::FilterGroups::Generic;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid DynamicTableExample::getUuid() const
{
  return QUuid("{7baae579-69bf-5559-a876-d0ef1cab77eb}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableExample::getSubGroupName() const
{
  return "Test";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableExample::getHumanLabel() const
{
  return "Dynamic Table Example";
}

// -----------------------------------------------------------------------------
DynamicTableExample::Pointer DynamicTableExample::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<DynamicTableExample> DynamicTableExample::New()
{
  struct make_shared_enabler : public DynamicTableExample
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString DynamicTableExample::getNameOfClass() const
{
  return QString("DynamicTableExample");
}

// -----------------------------------------------------------------------------
QString DynamicTableExample::ClassName()
{
  return QString("DynamicTableExample");
}

// -----------------------------------------------------------------------------
void DynamicTableExample::setDynamicData1(const DynamicTableData& value)
{
  m_DynamicData1 = value;
}

// -----------------------------------------------------------------------------
DynamicTableData DynamicTableExample::getDynamicData1() const
{
  return m_DynamicData1;
}

// -----------------------------------------------------------------------------
void DynamicTableExample::setDynamicData2(const DynamicTableData& value)
{
  m_DynamicData2 = value;
}

// -----------------------------------------------------------------------------
DynamicTableData DynamicTableExample::getDynamicData2() const
{
  return m_DynamicData2;
}

// -----------------------------------------------------------------------------
void DynamicTableExample::setDynamicData3(const DynamicTableData& value)
{
  m_DynamicData3 = value;
}

// -----------------------------------------------------------------------------
DynamicTableData DynamicTableExample::getDynamicData3() const
{
  return m_DynamicData3;
}

// -----------------------------------------------------------------------------
void DynamicTableExample::setDynamicData4(const DynamicTableData& value)
{
  m_DynamicData4 = value;
}

// -----------------------------------------------------------------------------
DynamicTableData DynamicTableExample::getDynamicData4() const
{
  return m_DynamicData4;
}

// -----------------------------------------------------------------------------
void DynamicTableExample::setDynamicData5(const DynamicTableData& value)
{
  m_DynamicData5 = value;
}

// -----------------------------------------------------------------------------
DynamicTableData DynamicTableExample::getDynamicData5() const
{
  return m_DynamicData5;
}

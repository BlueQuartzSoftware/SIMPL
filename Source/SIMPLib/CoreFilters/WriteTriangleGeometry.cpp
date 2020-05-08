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
#include "WriteTriangleGeometry.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#define WRITE_EDGES_FILE 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteTriangleGeometry::WriteTriangleGeometry()
: m_DataContainerSelection("")
, m_OutputNodesFile("")
, m_OutputTrianglesFile("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteTriangleGeometry::~WriteTriangleGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteTriangleGeometry::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output Nodes File", OutputNodesFile, FilterParameter::Parameter, WriteTriangleGeometry));
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output Triangles File", OutputTrianglesFile, FilterParameter::Parameter, WriteTriangleGeometry));

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("DataContainer", DataContainerSelection, FilterParameter::RequiredArray, WriteTriangleGeometry, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteTriangleGeometry::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerSelection(reader->readDataArrayPath("DataContainerSelection", getDataContainerSelection()));
  setOutputNodesFile(reader->readString("OutputNodesFile", getOutputNodesFile()));
  setOutputTrianglesFile(reader->readString("OutputTrianglesFile", getOutputTrianglesFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteTriangleGeometry::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteTriangleGeometry::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  QFileInfo fi(getOutputNodesFile());
  if(fi.suffix().compare("") == 0)
  {
    m_OutputNodesFile.append(".bin");
  }
  FileSystemPathHelper::CheckOutputFile(this, "Output Nodes File", getOutputNodesFile(), true);

  fi = QFileInfo(getOutputTrianglesFile());
  if(fi.suffix().compare("") == 0)
  {
    m_OutputTrianglesFile.append(".bin");
  }
  FileSystemPathHelper::CheckOutputFile(this, "Output Triangles File", getOutputTrianglesFile(), true);

  DataContainer::Pointer dataContainer = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerSelection());
  if(getErrorCode() < 0)
  {
    return;
  }

  TriangleGeom::Pointer triangles = dataContainer->getPrereqGeometry<TriangleGeom>(this);
  if(getErrorCode() < 0)
  {
    return;
  }

  // We MUST have Nodes
  if(nullptr == triangles->getVertices().get())
  {
    setErrorCondition(-386, "DataContainer Geometry missing Vertices");
  }
  // We MUST have Triangles defined also.
  if(nullptr == triangles->getTriangles().get())
  {
    setErrorCondition(-387, "DataContainer Geometry missing Triangles");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteTriangleGeometry::execute()
{
  clearErrorCode();
  clearWarningCode();

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }
  DataContainer::Pointer dataContainer = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerSelection());

  TriangleGeom::Pointer triangleGeom = dataContainer->getGeometryAs<TriangleGeom>();
  QString geometryType = triangleGeom->getGeometryTypeAsString();
  float* nodes = triangleGeom->getVertexPointer(0);
  size_t* triangles = triangleGeom->getTriPointer(0);

  size_t numNodes = triangleGeom->getNumberOfVertices();
  size_t maxNodeId = numNodes - 1;
  size_t numTriangles = triangleGeom->getNumberOfTris();

  // ++++++++++++++ Write the Nodes File +++++++++++++++++++++++++++++++++++++++++++
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path

  notifyStatusMessage("Writing Nodes Text File");
  QFileInfo fi(getOutputNodesFile());
  QDir parentPath = fi.path();

  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-1, ss);
    return;
  }

  QFile fileNodes(getOutputNodesFile());

  if(!fileNodes.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Output file could not be opened: %1").arg(getOutputNodesFile());
    setErrorCondition(-100, ss);
    return;
  }

  QTextStream outFileNodes(&fileNodes);

  outFileNodes << "# All lines starting with '#' are comments\n";
  outFileNodes << "# DREAM.3D Nodes file\n";
  outFileNodes << "# DREAM.3D Version " << SIMPLib::Version::Complete().toLatin1().constData() << "\n";
  outFileNodes << "# Node Data is X Y Z space delimited.\n";
  outFileNodes << "Node Count: " << numNodes << "\n";

  outFileNodes.setFieldWidth(8);
  outFileNodes.setRealNumberPrecision(5);
  outFileNodes.setRealNumberNotation(QTextStream::FixedNotation);

  for(size_t i = 0; i < numNodes; i++)
  {
    outFileNodes.setFieldWidth(8);
    outFileNodes << nodes[i * 3] << qSetFieldWidth(0);
    outFileNodes << " " << qSetFieldWidth(8);
    outFileNodes << nodes[i * 3 + 1] << qSetFieldWidth(0);
    outFileNodes << " " << qSetFieldWidth(8);
    outFileNodes << nodes[i * 3 + 2] << qSetFieldWidth(0);
    outFileNodes << "\n";
  }

  fileNodes.close();

  // ++++++++++++++ Write the Triangles File +++++++++++++++++++++++++++++++++++++++++++

  notifyStatusMessage("Writing Triangles Text File");
  QFileInfo triFI(getOutputTrianglesFile());
  parentPath.setPath(triFI.path());

  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-1, ss);
    return;
  }

  QFile fileTri(getOutputTrianglesFile());

  if(!fileTri.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Output file could not be opened: %1").arg(getOutputTrianglesFile());
    setErrorCondition(-100, ss);
    return;
  }

  QTextStream outFileTri(&fileTri);

  outFileTri << "# All lines starting with '#' are comments\n";
  outFileTri << "# DREAM.3D Triangle file\n";
  outFileTri << "# DREAM.3D Version " << SIMPLib::Version::Complete().toLatin1().constData() << "\n";
  outFileTri << "# Each Triangle consists of 3 Node Ids.\n";
  outFileTri << "# NODE IDs START AT 0.\n";
  outFileTri << "Geometry Type: " << geometryType.toLatin1().constData() << "\n";
  outFileTri << "Node Count: " << numNodes << "\n";
  outFileTri << "Max Node Id: " << maxNodeId << "\n";
  outFileTri << "Triangle Count: " << numTriangles << "\n";

  int n1, n2, n3;
  for(size_t j = 0; j < numTriangles; ++j)
  {
    n1 = triangles[j * 3];
    n2 = triangles[j * 3 + 1];
    n3 = triangles[j * 3 + 2];

    outFileTri << n1 << " " << n2 << " " << n3 << "\n";
  }

  fileTri.close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WriteTriangleGeometry::newFilterInstance(bool copyFilterParameters) const
{
  WriteTriangleGeometry::Pointer filter = WriteTriangleGeometry::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteTriangleGeometry::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteTriangleGeometry::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteTriangleGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteTriangleGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid WriteTriangleGeometry::getUuid() const
{
  return QUuid("{5e523ec1-49ac-541e-a4ba-6fa725798b91}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteTriangleGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteTriangleGeometry::getHumanLabel() const
{
  return "Export Triangle Geometry";
}

// -----------------------------------------------------------------------------
WriteTriangleGeometry::Pointer WriteTriangleGeometry::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<WriteTriangleGeometry> WriteTriangleGeometry::New()
{
  struct make_shared_enabler : public WriteTriangleGeometry
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString WriteTriangleGeometry::getNameOfClass() const
{
  return QString("WriteTriangleGeometry");
}

// -----------------------------------------------------------------------------
QString WriteTriangleGeometry::ClassName()
{
  return QString("WriteTriangleGeometry");
}

// -----------------------------------------------------------------------------
void WriteTriangleGeometry::setDataContainerSelection(const DataArrayPath& value)
{
  m_DataContainerSelection = value;
}

// -----------------------------------------------------------------------------
DataArrayPath WriteTriangleGeometry::getDataContainerSelection() const
{
  return m_DataContainerSelection;
}

// -----------------------------------------------------------------------------
void WriteTriangleGeometry::setOutputNodesFile(const QString& value)
{
  m_OutputNodesFile = value;
}

// -----------------------------------------------------------------------------
QString WriteTriangleGeometry::getOutputNodesFile() const
{
  return m_OutputNodesFile;
}

// -----------------------------------------------------------------------------
void WriteTriangleGeometry::setOutputTrianglesFile(const QString& value)
{
  m_OutputTrianglesFile = value;
}

// -----------------------------------------------------------------------------
QString WriteTriangleGeometry::getOutputTrianglesFile() const
{
  return m_OutputTrianglesFile;
}

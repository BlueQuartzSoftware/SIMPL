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
#include "WriteTriangleGeometry.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/IGeometry.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"

#define WRITE_EDGES_FILE 0

namespace
{

// -----------------------------------------------------------------------------
class ElementWriter
{
public:
  ElementWriter(QTextStream& out, SharedQuadList::Pointer connectivityPtr)
  : m_Out(out)
  , m_IndexPtr(connectivityPtr)
  {
  }
  void printIndex(size_t index, char delimiter)
  {
    m_Out << index + 1 << delimiter;
    size_t numComp = m_IndexPtr->getNumberOfComponents();
    for(size_t c = 0; c < numComp; c++)
    {
      size_t value = m_IndexPtr->getComponent(index, c);
      m_Out << value + 1 << delimiter;
    }
  }

private:
  QTextStream& m_Out;
  SharedQuadList::Pointer m_IndexPtr;
};

// -----------------------------------------------------------------------------
class NodeWriter
{
public:
  NodeWriter(QTextStream& out, SharedVertexList::Pointer vertPtr)
  : m_Out(out)
  , m_VertPtr(vertPtr)
  {
  }
  void printIndex(size_t index, char delimiter)
  {
    m_Out << index + 1 << delimiter;
    m_VertPtr->printTuple(m_Out, index, delimiter);
  }

private:
  QTextStream& m_Out;
  SharedVertexList::Pointer m_VertPtr;
};

// -----------------------------------------------------------------------------
char lookupDelimiter(int m_Delimiter)
{
  char del = ' ';
  switch(m_Delimiter)
  {
  case WriteTriangleGeometry::DelimiterType::Comma:
    del = ',';
    break;
  case WriteTriangleGeometry::DelimiterType::Semicolon:
    del = ';';
    break;
  case WriteTriangleGeometry::DelimiterType::Space:
    del = ' ';
    break;
  case WriteTriangleGeometry::DelimiterType::Colon:
    del = ':';
    break;
  case WriteTriangleGeometry::DelimiterType::Tab:
    del = '\t';
    break;
  default:
    del = ' ';
  }
  return del;
}

// -----------------------------------------------------------------------------
template <typename T, class WriterType>
void WriteSingleFileOutput(WriteTriangleGeometry* filter, const QString& outputFilePath, const QString& header, const QString& comment, typename DataArray<T>::Pointer geomDataPtr,
                           std::vector<DataArrayPath>& dataPaths)
{
  DataContainerArray::Pointer dca = filter->getDataContainerArray();

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(outputFilePath);
  QDir parentPath(fi.path());
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    filter->setErrorCondition(-12020, ss);
    return;
  }

  QFile file(outputFilePath);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Output file could not be opened: %1").arg(outputFilePath);
    filter->setErrorCondition(-12021, ss);
    return;
  }

  QTextStream outFile(&file);

  std::vector<IDataArray::Pointer> dataArrays;

  // *********** Print the header Line *******************
  char delimiter = lookupDelimiter(filter->getDelimiter());
  QString commentMarker = filter->getCommentMarker();

  outFile << comment;
  outFile << header;

  for(const auto& dataPath : dataPaths)
  {
    IDataArray::Pointer selectedArrayPtr = dca->getPrereqIDataArrayFromPath(filter, dataPath);

    if(selectedArrayPtr->getNumberOfComponents() == 1)
    {
      outFile << selectedArrayPtr->getName();
    }
    else // There are more than a single component so we need to add multiple header values
    {
      for(int32_t k = 0; k < selectedArrayPtr->getNumberOfComponents(); ++k)
      {
        outFile << selectedArrayPtr->getName() << "_" << k;
        if(k < selectedArrayPtr->getNumberOfComponents() - 1)
        {
          outFile << delimiter;
        }
      }
    }
    // if(i < dataPaths.size() - 1)
    //{
    outFile << delimiter;
    // }
    // Get the IDataArray from the DataContainer
    dataArrays.push_back(selectedArrayPtr);
  }
  outFile << "\n";

  // Get the number of tuples in the arrays
  size_t numTuples = 0;
  if(!dataArrays.empty())
  {
    numTuples = dataArrays[0]->getNumberOfTuples();
  }

  QString s;
  QTextStream out(&s);
  WriterType writerType(out, geomDataPtr);

  float threshold = 0.0f;

  // size_t numArrays = data.size();
  for(size_t i = 0; i < numTuples; ++i)
  {
    float percentIncrement = static_cast<float>(i) / static_cast<float>(numTuples) * 100.0f;
    if(percentIncrement > threshold)
    {
      QString ss = QObject::tr("Writing Output: %1%").arg(static_cast<int32_t>(percentIncrement));
      filter->notifyStatusMessage(ss);
      threshold = threshold + 5.0f;
      if(threshold < percentIncrement)
      {
        threshold = percentIncrement;
      }
    }

    writerType.printIndex(i, delimiter);
    // Print a row of data
    for(const auto& data : dataArrays)
    {
      data->printTuple(out, i, delimiter);

      out << delimiter;

      outFile << s;
      s.clear();
    }
    outFile << "\n";
  }
}

} // namespace

enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1,
  AttributeMatrixID
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteTriangleGeometry::WriteTriangleGeometry() = default;

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

  parameters.push_back(SeparatorFilterParameter::Create("Input Options", FilterParameter::Category::Parameter));

  parameters.push_back(SIMPL_NEW_STRING_FP("Comment Marker", CommentMarker, FilterParameter::Category::Parameter, WriteTriangleGeometry, {0}));

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New(); // Delimiter choice
    parameter->setHumanLabel("Delimiter");
    parameter->setPropertyName("Delimiter");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(WriteTriangleGeometry, this, Delimiter));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(WriteTriangleGeometry, this, Delimiter));

    std::vector<QString> choices;
    choices.push_back(", (comma)");
    choices.push_back("; (semicolon)");
    choices.push_back("  (space)");
    choices.push_back(": (colon)");
    choices.push_back("\\t (Tab)");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Category::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SeparatorFilterParameter::Create("Output File Selection", FilterParameter::Category::Parameter));

  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output Nodes File", OutputNodesFile, FilterParameter::Category::Parameter, WriteTriangleGeometry));
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output Triangles File", OutputTrianglesFile, FilterParameter::Category::Parameter, WriteTriangleGeometry));

  parameters.push_back(SeparatorFilterParameter::Create("Input Geometry Selection", FilterParameter::Category::Parameter));

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    IGeometry::Types geomTypes = {IGeometry::Type::Triangle, IGeometry::Type::Quad};
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("DataContainer", DataContainerSelection, FilterParameter::Category::RequiredArray, WriteTriangleGeometry, req));
  }

  parameters.push_back(SeparatorFilterParameter::Create("Output Data Arrays Selection", FilterParameter::Category::Parameter));

  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    req.amTypes = AttributeMatrix::Types(1, AttributeMatrix::Type::Vertex);
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Vertex Data To Write", VertexDataArrayPaths, FilterParameter::Category::RequiredArray, WriteTriangleGeometry, req));
  }

  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    req.amTypes = AttributeMatrix::Types(1, AttributeMatrix::Type::Face);
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Face Data To Write", FaceDataArrayPaths, FilterParameter::Category::RequiredArray, WriteTriangleGeometry, req));
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
    m_OutputNodesFile.append(".csv");
  }
  FileSystemPathHelper::CheckOutputFile(this, "Output Nodes File", getOutputNodesFile(), true);

  fi = QFileInfo(getOutputTrianglesFile());
  if(fi.suffix().compare("") == 0)
  {
    m_OutputTrianglesFile.append(".csv");
  }
  FileSystemPathHelper::CheckOutputFile(this, "Output Triangles File", getOutputTrianglesFile(), true);

  DataContainer::Pointer dataContainer = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerSelection());
  if(getErrorCode() < 0)
  {
    return;
  }

  IGeometry2D::Pointer nodeGeom2D = dataContainer->getGeometryAs<IGeometry2D>();
  if(nodeGeom2D->getGeometryType() != IGeometry::Type::Quad && nodeGeom2D->getGeometryType() != IGeometry::Type::Triangle)
  {
    setErrorCondition(-384, "Only Triangle And Quad Geometry are supported.");
    return;
  }

  std::vector<DataArrayPath> paths = getVertexDataArrayPaths();

  if(!DataArrayPath::ValidateVector(paths))
  {
    QString ss = QObject::tr("There are Attribute Arrays selected that are not contained in the same Attribute Matrix. All selected Attribute Arrays must belong to the same Attribute Matrix");
    setErrorCondition(-11008, ss);
    return;
  }

  for(auto const& path : paths)
  {
    IDataArray::WeakPointer ptr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, path);

    if(getErrorCode() < 0)
    {
      return;
    }

    if(ptr.lock()->getNameOfClass() == "NeighborList<T>")
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedDataType, "NeighborList is unsupported when writing ASCII Data.");
    }
    else if(ptr.lock()->getTypeAsString() == "struct")
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedDataType, "StructArray is unsupported when writing ASCII Data.");
    }
    else if(ptr.lock()->getTypeAsString() == "StatsDataArray")
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedDataType, "StatsDataArray is unsupported when writing ASCII Data.");
    }
  }

  paths = getFaceDataArrayPaths();

  if(!DataArrayPath::ValidateVector(paths))
  {
    QString ss = QObject::tr("There are Attribute Arrays selected that are not contained in the same Attribute Matrix. All selected Attribute Arrays must belong to the same Attribute Matrix");
    setErrorCondition(-11008, ss);
    return;
  }

  for(auto const& path : paths)
  {
    IDataArray::WeakPointer ptr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, path);

    if(getErrorCode() < 0)
    {
      return;
    }

    if(ptr.lock()->getNameOfClass() == "NeighborList<T>")
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedDataType, "NeighborList is unsupported when writing ASCII Data.");
    }
    else if(ptr.lock()->getTypeAsString() == "struct")
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedDataType, "StructArray is unsupported when writing ASCII Data.");
    }
    else if(ptr.lock()->getTypeAsString() == "StatsDataArray")
    {
      setErrorCondition(TemplateHelpers::Errors::UnsupportedDataType, "StatsDataArray is unsupported when writing ASCII Data.");
    }
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

  IGeometry2D::Pointer nodeGeom2D = dataContainer->getGeometryAs<IGeometry2D>();
  SharedVertexList::Pointer vertices = nodeGeom2D->getVertices();

  QString header;
  QTextStream headerStrm(&header);

  QString comment;
  QTextStream commentStrm(&comment);

  char delimiter = lookupDelimiter(m_Delimiter);

  auto geometryType = nodeGeom2D->getGeometryType();
  switch(geometryType)
  {
  case IGeometry::Type::Quad: {

    QuadGeom::Pointer quadGeomPtr = dataContainer->getGeometryAs<QuadGeom>();
    headerStrm << "Index" << delimiter << "X" << delimiter << "Y" << delimiter << "Z" << delimiter;

    commentStrm << getCommentMarker() << " All lines starting with '" << getCommentMarker() << "' are comments\n";
    commentStrm << getCommentMarker() << " DREAM.3D Nodes file\n";
    commentStrm << getCommentMarker() << " DREAM.3D Version " << SIMPLib::Version::Complete().toLatin1().constData() << "\n";
    commentStrm << getCommentMarker() << " Node Data is X Y Z.\n";
    commentStrm << getCommentMarker() << " Indexing starts at 1\n";
    commentStrm << getCommentMarker() << " Total Vertices: " << vertices->getNumberOfTuples() << "\n";

    notifyStatusMessage(QString("Writing Nodes file...."));
    WriteSingleFileOutput<float, NodeWriter>(this, getOutputNodesFile(), header, comment, vertices, m_VertexDataArrayPaths);

    SharedQuadList::Pointer quadList = quadGeomPtr->getQuads();

    header.clear();
    headerStrm << "Index" << delimiter << "V1" << delimiter << "V2" << delimiter << "V3" << delimiter << "V4" << delimiter;
    comment.clear();
    commentStrm << getCommentMarker() << " All lines starting with '" << getCommentMarker() << "' are comments\n";
    commentStrm << getCommentMarker() << " DREAM.3D Elements file\n";
    commentStrm << getCommentMarker() << " DREAM.3D Version " << SIMPLib::Version::Complete().toLatin1().constData() << "\n";
    commentStrm << getCommentMarker() << " Element Data: Each line has the vertex index and any associated element data.\n";
    commentStrm << getCommentMarker() << " Element Data is considered located at the centroid of the element.\n";
    commentStrm << getCommentMarker() << " Indexing starts at 1\n";
    commentStrm << getCommentMarker() << " Total Elements: " << quadList->getNumberOfTuples() << "\n";

    notifyStatusMessage(QString("Writing Elements file...."));
    WriteSingleFileOutput<size_t, ElementWriter>(this, getOutputTrianglesFile(), header, comment, quadList, m_FaceDataArrayPaths);
    break;
  }
  case IGeometry::Type::Triangle: {
    TriangleGeom::Pointer quadGeomPtr = dataContainer->getGeometryAs<TriangleGeom>();
    headerStrm << "Index" << delimiter << "X" << delimiter << "Y" << delimiter << "Z" << delimiter;

    commentStrm << getCommentMarker() << " All lines starting with '" << getCommentMarker() << "' are comments\n";
    commentStrm << getCommentMarker() << " DREAM.3D Nodes file\n";
    commentStrm << getCommentMarker() << " DREAM.3D Version " << SIMPLib::Version::Complete().toLatin1().constData() << "\n";
    commentStrm << getCommentMarker() << " Node Data is X Y Z.\n";
    commentStrm << getCommentMarker() << " Indexing starts at 1\n";
    commentStrm << getCommentMarker() << " Total Vertices: " << vertices->getNumberOfTuples() << "\n";

    notifyStatusMessage(QString("Writing Nodes file...."));
    WriteSingleFileOutput<float, NodeWriter>(this, getOutputNodesFile(), header, comment, vertices, m_VertexDataArrayPaths);

    SharedQuadList::Pointer quadList = quadGeomPtr->getTriangles();

    header.clear();
    headerStrm << "Index" << delimiter << "V1" << delimiter << "V2" << delimiter << "V3" << delimiter;
    comment.clear();
    commentStrm << getCommentMarker() << " All lines starting with '" << getCommentMarker() << "' are comments\n";
    commentStrm << getCommentMarker() << " DREAM.3D Elements file\n";
    commentStrm << getCommentMarker() << " DREAM.3D Version " << SIMPLib::Version::Complete().toLatin1().constData() << "\n";
    commentStrm << getCommentMarker() << " Element Data: Each line has the vertex index and any associated element data.\n";
    commentStrm << getCommentMarker() << " Element Data is considered located at the centroid of the element.\n";
    commentStrm << getCommentMarker() << " Indexing starts at 1\n";
    commentStrm << getCommentMarker() << " Total Elements: " << quadList->getNumberOfTuples() << "\n";

    notifyStatusMessage(QString("Writing Elements file...."));
    WriteSingleFileOutput<size_t, ElementWriter>(this, getOutputTrianglesFile(), header, comment, quadList, m_FaceDataArrayPaths);
  }
  break;
  default:
    break;
  }
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
  return "Export Triangle/Quad Geometry";
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

// -----------------------------------------------------------------------------
void WriteTriangleGeometry::setVertexDataArrayPaths(const std::vector<DataArrayPath>& value)
{
  m_VertexDataArrayPaths = value;
}

// -----------------------------------------------------------------------------
std::vector<DataArrayPath> WriteTriangleGeometry::getVertexDataArrayPaths() const
{
  return m_VertexDataArrayPaths;
}

// -----------------------------------------------------------------------------
void WriteTriangleGeometry::setFaceDataArrayPaths(const std::vector<DataArrayPath>& value)
{
  m_FaceDataArrayPaths = value;
}

// -----------------------------------------------------------------------------
std::vector<DataArrayPath> WriteTriangleGeometry::getFaceDataArrayPaths() const
{
  return m_FaceDataArrayPaths;
}

// -----------------------------------------------------------------------------
void WriteTriangleGeometry::setCommentMarker(const QString& value)
{
  m_CommentMarker = value;
}

// -----------------------------------------------------------------------------
QString WriteTriangleGeometry::getCommentMarker() const
{
  return m_CommentMarker;
}

// -----------------------------------------------------------------------------
void WriteTriangleGeometry::setDelimiter(int value)
{
  m_Delimiter = value;
}

// -----------------------------------------------------------------------------
int WriteTriangleGeometry::getDelimiter() const
{
  return m_Delimiter;
}

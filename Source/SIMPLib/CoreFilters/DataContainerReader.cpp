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
#include "DataContainerReader.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerBundle.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerReaderFilterParameter.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Montages/MontageSupport.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReader::DataContainerReader()
: m_InputFile("")
, m_OverwriteExistingDataContainers(false)
, m_LastFileRead("")
, m_LastRead(QDateTime::currentDateTime())
{
  m_PipelineFromFile = FilterPipeline::New();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReader::~DataContainerReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_BOOL_FP("Overwrite Existing Data Containers", OverwriteExistingDataContainers, FilterParameter::Parameter, DataContainerReader));
  {
    DataContainerReaderFilterParameter::Pointer parameter = DataContainerReaderFilterParameter::New();
    parameter->setHumanLabel("Select Arrays from Input File");
    parameter->setPropertyName("InputFileDataContainerArrayProxy");
    parameter->setDefaultFlagValue(Qt::Checked);
    parameter->setInputFileProperty("InputFile");
    parameter->setCategory(FilterParameter::Parameter);
    parameter->setFilter(this);
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputFile(reader->readString("InputFile", getInputFile()));
  setInputFileDataContainerArrayProxy(reader->readDataContainerArrayProxy("InputFileDataContainerArrayProxy", getInputFileDataContainerArrayProxy()));
  syncProxies(); // Sync the file proxy and currently cached proxy together into one proxy
  setOverwriteExistingDataContainers(reader->readValue("OverwriteExistingDataContainers", getOverwriteExistingDataContainers()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::readFilterParameters(QJsonObject& obj)
{
  AbstractFilter::readFilterParameters(obj);
  syncProxies();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::preWriteFilterParameters(QJsonObject& obj, QJsonObject& rootObject)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::writeFilterParameters(QJsonObject& obj) const
{
  // writeExistingPipelineToFile(obj);
  AbstractFilter::writeFilterParameters(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::dataCheck()
{

  // Sync the file proxy and cached proxy if the time stamps are different
  QFileInfo fi(getInputFile());
  if(getInputFile() == getLastFileRead() && getLastRead() < fi.lastModified())
  {
    if(!syncProxies())
    {
      return;
    }
  }

  QString ss;
  if(getInputFile().isEmpty())
  {
    ss = QObject::tr("The input file must be set");
    setErrorCondition(-387, ss);
  }
  else if(!fi.exists())
  {
    ss = QObject::tr("The input file %1 does not exist").arg(getInputFile());
    setErrorCondition(-388, ss);
  }

  if(getErrorCode() != 0)
  {
    // something has gone wrong and errors were logged already so just return
    return;
  }

  DataContainerArray::Pointer dca = getDataContainerArray();

  if(m_InputFileDataContainerArrayProxy.getDataContainers().empty())
  {
    QString msg = "The DataContainerArrayProxy object is empty. Nothing will be read from the input file. If you are programmatically calling " + getNameOfClass() +
                  " you may need to insert a line such as \n\
    DataContainerArrayProxy proxy = reader->readDataContainerArrayStructure(inputFile);\n\
    reader->setInputFileDataContainerArrayProxy(proxy);\n"
                  " into your code before calling preflight or execute on the filter or pipeline";
    setWarningCondition(1000, msg);
  }

  // Read either the structure or all the data depending on the preflight status
  DataContainerArray::Pointer tempDCA = readData(m_InputFileDataContainerArrayProxy);
  if(tempDCA.get() == nullptr)
  {
    return;
  }

  DataContainerArray::Container tempContainers = tempDCA->getDataContainers();
  for(DataContainer::Pointer container : tempContainers)
  {
    if(getOverwriteExistingDataContainers())
    {
      if(dca->doesDataContainerExist(container->getName()))
      {
        dca->removeDataContainer(container->getName());
      }
      dca->addOrReplaceDataContainer(container);
    }
    else
    {
      if(dca->doesDataContainerExist(container->getName()))
      {
        ss = QObject::tr("The input file has a DataContainer with a name (%1) that already exists in the current DataContainerArray structure").arg(container->getName());
        setErrorCondition(-390, ss);
      }
      else
      {
        dca->addOrReplaceDataContainer(container);
      }
    }
  }
  QMap<QString, IDataContainerBundle::Pointer> bundles = tempDCA->getDataContainerBundles();
  dca->setDataContainerBundles(bundles);

  DataContainerArray::MontageCollection montageGroup = readMontageGroup(dca);
  for(const auto& montage : montageGroup)
  {
    dca->addMontage(montage);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::execute()
{
  /* In this VERY Special circumstance, the data check will actually read the data from the
   * file and move those DataContainer objects into the existing DataContainerArray. Error messages
   * will be passed up the chain if something goes wrong.
   */
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer DataContainerReader::readData(DataContainerArrayProxy& proxy)
{
  clearErrorCode();
  clearWarningCode();

  SIMPLH5DataReader::Pointer simplReader = SIMPLH5DataReader::New();
  connect(simplReader.get(), &SIMPLH5DataReader::errorGenerated, [=](const QString& title, const QString& msg, int code) { setErrorCondition(code, msg); });

  if(!simplReader->openFile(getInputFile()))
  {
    return DataContainerArray::New();
  }

  DataContainerArray::Pointer dca = simplReader->readSIMPLDataUsingProxy(proxy, getInPreflight());
  if(dca == DataContainerArray::NullPointer())
  {
    return DataContainerArray::New();
  }

  hid_t fileId = QH5Utilities::openFile(getInputFile(), true); // Open the file Read Only
  if(fileId < 0)
  {
    QString ss = QObject::tr("Error opening input file '%1'").arg(getInputFile());
    setErrorCondition(-150, ss);
    return DataContainerArray::NullPointer();
  }
  H5ScopedFileSentinel sentinel(&fileId, true);

  if(!getInPreflight())
  {
    int32_t err = readExistingPipelineFromFile(fileId);
    if(err < 0)
    {
      QString ss = QObject::tr("Error trying to read the existing pipeline from the file '%1'").arg(getInputFile());
      setErrorCondition(err, ss);
      return DataContainerArray::New();
    }
  }

  return dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::MontageCollection DataContainerReader::readMontageGroup(const DataContainerArray::Pointer& dca)
{
  hid_t fileId = QH5Utilities::openFile(getInputFile(), true); // Open the file Read Only
  if(fileId < 0)
  {
    QString ss = QObject::tr("Error opening input file '%1'").arg(getInputFile());
    setErrorCondition(-155, ss);
    return DataContainerArray::MontageCollection();
  }
  H5ScopedFileSentinel sentinel(&fileId, true);

  hid_t groupId = QH5Utilities::openHDF5Object(fileId, SIMPL::StringConstants::MontageGroupName);
  sentinel.addGroupId(&groupId);
  int err = 0;
  DataContainerArray::MontageCollection montages = MontageSupport::IO::ReadMontagesFromHDF5(groupId, dca, err);
  if(err < 0)
  {
    QString ss = QObject::tr("Error reading montage collection");
    setErrorCondition(-160, ss);
  }
  return montages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy DataContainerReader::readDataContainerArrayStructure(const QString& path)
{
  SIMPLH5DataReader::Pointer h5Reader = SIMPLH5DataReader::New();
  if(!h5Reader->openFile(path))
  {
    return DataContainerArrayProxy();
  }

  int err = 0;
  SIMPLH5DataReaderRequirements req(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Any, IGeometry::Type::Any);
  DataContainerArrayProxy proxy = h5Reader->readDataContainerArrayStructure(&req, err);
  if(err < 0)
  {
    return DataContainerArrayProxy();
  }

  return proxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerReader::readExistingPipelineFromFile(hid_t fileId)
{
  herr_t err = 0;
  m_PipelineFromFile->clear();

  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();
  // First try to see if we can read the newer JSON style of Pipeline from the HDF5 based file
  m_PipelineFromFile = reader->readPipelineFromFile(fileId, nullptr);
  if(nullptr != m_PipelineFromFile.get())
  {
    return err;
  }

  // If we are on the old style then the shared pointer would have come back null so create a new one.
  m_PipelineFromFile = FilterPipeline::New();

  // HDF5: Open the "Pipeline" Group
  hid_t pipelineGroupId = H5Gopen(fileId, SIMPL::StringConstants::PipelineGroupName.toLatin1().data(), H5P_DEFAULT);
  reader->setPipelineGroupId(pipelineGroupId);

  // Use QH5Lite to ask how many "groups" are in the "Pipeline Group"
  QList<QString> groupList;
  err = QH5Utilities::getGroupObjects(pipelineGroupId, H5Utilities::CustomHDFDataTypes::Group, groupList);

  // Loop over the items getting the "ClassName" attribute from each group
  QString classNameStr = "";
  for(int32_t i = 0; i < groupList.size(); i++)
  {
    QString ss = QString::number(i, 10);

    err = QH5Lite::readStringAttribute(pipelineGroupId, ss, "ClassName", classNameStr);
    if(err < 0)
    {
      qDebug() << "Filter Index = " << i;
    }
    // Instantiate a new filter using the FilterFactory based on the value of the className attribute
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer ff = fm->getFactoryFromClassName(classNameStr);
    if(nullptr != ff.get())
    {
      AbstractFilter::Pointer filter = ff->create();
      if(nullptr != filter.get())
      {
        // Read the parameters
        filter->readFilterParameters(reader.get(), i);

        // Add filter to m_PipelineFromFile
        m_PipelineFromFile->pushBack(filter);
      }
    }
  }
  err = H5Gclose(pipelineGroupId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerReader::writeExistingPipelineToFile(QJsonObject& rootJson, int index)
{
  FilterPipeline::FilterContainerType container = m_PipelineFromFile->getFilterContainer();
  int offset = 0;

  for(FilterPipeline::FilterContainerType::iterator iter = container.begin(); iter != container.end(); ++iter)
  {
    AbstractFilter::Pointer filter = *iter;
    QJsonObject json;

    QString numStr = QString::number(index++);

    if(nullptr != filter.get())
    {
      json[SIMPL::Settings::FilterName] = filter->getNameOfClass();
      json[SIMPL::Settings::HumanLabel] = filter->getHumanLabel();
      json[SIMPL::Settings::FilterEnabled] = filter->getEnabled();

      filter->writeFilterParameters(json);
    }
    else
    {
      json["Unknown Filter"] = "ERROR: Filter instance was nullptr within the SVPipelineFilterWidget instance. Report this error to the DREAM3D Developers";
    }

    rootJson[numStr] = json;
    offset++;
  }
  return offset;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerReader::syncProxies()
{
  SIMPLH5DataReaderRequirements req(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Any, IGeometry::Type::Any);

  SIMPLH5DataReader::Pointer simplReader = SIMPLH5DataReader::New();
  connect(simplReader.get(), &SIMPLH5DataReader::errorGenerated, [=](const QString& title, const QString& msg, int code) { setErrorCondition(code, msg); });

  if(!simplReader->openFile(getInputFile()))
  {
    return false;
  }

  // If there is something in the cached proxy...
  if(!m_InputFileDataContainerArrayProxy.getDataContainers().empty())
  {
    int err = 0;
    DataContainerArrayProxy fileProxy = simplReader->readDataContainerArrayStructure(&req, err);
    if(err < 0)
    {
      return false;
    }

    DataContainerArrayProxy cacheProxy = getInputFileDataContainerArrayProxy();

    // Mesh proxies together into one proxy
    DataContainerArrayProxy mergedProxy = DataContainerArrayProxy::MergeProxies(fileProxy, cacheProxy);
    setInputFileDataContainerArrayProxy(mergedProxy);
  }
  else
  {
    int err = 0;
    DataContainerArrayProxy fileProxy = simplReader->readDataContainerArrayStructure(&req, err);
    if(err < 0)
    {
      return false;
    }

    setInputFileDataContainerArrayProxy(fileProxy);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer DataContainerReader::newFilterInstance(bool copyFilterParameters) const
{
  DataContainerReader::Pointer filter = DataContainerReader::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());

    filter->setInputFile(getInputFile());

    filter->setInputFile(getInputFile());
#if 0
    filter->setOverwriteExistingDataContainers(getOverwriteExistingDataContainers());
    filter->setDataContainerArrayProxy(getDataContainerArrayProxy());
#endif
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerReader::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerReader::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerReader::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerReader::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid DataContainerReader::getUuid() const
{
  return QUuid("{043cbde5-3878-5718-958f-ae75714df0df}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerReader::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerReader::getHumanLabel() const
{
  return "Read DREAM.3D Data File";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::cleanupFilter()
{
  m_PipelineFromFile->clear();
}

// -----------------------------------------------------------------------------
DataContainerReader::Pointer DataContainerReader::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<DataContainerReader> DataContainerReader::New()
{
  struct make_shared_enabler : public DataContainerReader
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString DataContainerReader::getNameOfClass() const
{
  return QString("DataContainerReader");
}

// -----------------------------------------------------------------------------
QString DataContainerReader::ClassName()
{
  return QString("DataContainerReader");
}

// -----------------------------------------------------------------------------
void DataContainerReader::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString DataContainerReader::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void DataContainerReader::setOverwriteExistingDataContainers(bool value)
{
  m_OverwriteExistingDataContainers = value;
}

// -----------------------------------------------------------------------------
bool DataContainerReader::getOverwriteExistingDataContainers() const
{
  return m_OverwriteExistingDataContainers;
}

// -----------------------------------------------------------------------------
void DataContainerReader::setLastFileRead(const QString& value)
{
  m_LastFileRead = value;
}

// -----------------------------------------------------------------------------
QString DataContainerReader::getLastFileRead() const
{
  return m_LastFileRead;
}

// -----------------------------------------------------------------------------
void DataContainerReader::setLastRead(const QDateTime& value)
{
  m_LastRead = value;
}

// -----------------------------------------------------------------------------
QDateTime DataContainerReader::getLastRead() const
{
  return m_LastRead;
}

// -----------------------------------------------------------------------------
void DataContainerReader::setInputFileDataContainerArrayProxy(const DataContainerArrayProxy& value)
{
  m_InputFileDataContainerArrayProxy = value;
}

// -----------------------------------------------------------------------------
DataContainerArrayProxy DataContainerReader::getInputFileDataContainerArrayProxy() const
{
  return m_InputFileDataContainerArrayProxy;
}

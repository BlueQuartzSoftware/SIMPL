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

#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/QH5Utilities.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerBundle.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerReaderFilterParameter.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReader::DataContainerReader()
: m_InputFile("")
, m_OverwriteExistingDataContainers(false)
, m_LastFileRead("")
, m_LastRead(QDateTime::currentDateTime())
, m_InputFileDataContainerArrayProxy()
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
  FilterParameterVector parameters;

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
    if(syncProxies() == false)
    {
      return;
    }
  }

  QString ss;
  if(getInputFile().isEmpty() == true)
  {
    ss = QObject::tr("The input file must be set");
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(fi.exists() == false)
  {
    ss = QObject::tr("The input file %1 does not exist").arg(getInputFile());
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getErrorCondition())
  {
    // something has gone wrong and errors were logged already so just return
    return;
  }

  DataContainerArray::Pointer dca = getDataContainerArray();

  // Read either the structure or all the data depending on the preflight status
  DataContainerArray::Pointer tempDCA = readData(m_InputFileDataContainerArrayProxy);
  if(!tempDCA.get())
  {
    return;
  }

  QList<DataContainer::Pointer>& tempContainers = tempDCA->getDataContainers();

  QListIterator<DataContainer::Pointer> iter(tempContainers);
  while(iter.hasNext())
  {
    DataContainer::Pointer container = iter.next();

    if(getOverwriteExistingDataContainers() == true)
    {
      if(dca->doesDataContainerExist(container->getName()) == true)
      {
        dca->removeDataContainer(container->getName());
      }
      dca->addDataContainer(container);
    }
    else
    {
      if(dca->doesDataContainerExist(container->getName()) == true)
      {
        ss = QObject::tr("The input file has a DataContainer with a name (%1) that already exists in the current DataContainerArray structure").arg(container->getName());
        setErrorCondition(-390);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
      else
      {
        dca->addDataContainer(container);
      }
    }
  }
  QMap<QString, IDataContainerBundle::Pointer> bundles = tempDCA->getDataContainerBundles();
  dca->setDataContainerBundles(bundles);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::preflight()
{
  setInPreflight(true);
  // Annouce we are about to preflight
  // The GUI will pick up the structure
  emit preflightAboutToExecute();
  // The Gui sends down any changes to the Proxy (which for preflight we don't care about)
  emit updateFilterParameters(this);
  // to the read here because this will populate the DataContainerArray with our DataContainer
  dataCheck();
  // The GUI needs to send down the selections that were made by the user and we need to update
  // DataContainerArray->DataContainer object so the rest of the pipeline has the proper information
  emit preflightExecuted(); // Done executing
  setInPreflight(false);
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

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer DataContainerReader::readData(DataContainerArrayProxy& proxy)
{
  setErrorCondition(0);
  setWarningCondition(0);

  SIMPLH5DataReader::Pointer simplReader = SIMPLH5DataReader::New();
  connect(simplReader.get(), &SIMPLH5DataReader::errorGenerated, [=](const QString& title, const QString& msg, const int& code) {
    setErrorCondition(code);
    notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
  });

  if (!simplReader->openFile(getInputFile()))
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
    setErrorCondition(-150);
    QString ss = QObject::tr("Error opening input file '%1'").arg(getInputFile());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return DataContainerArray::NullPointer();
  }
  H5ScopedFileSentinel sentinel(&fileId, true);

  if(!getInPreflight())
  {
    int32_t err = readExistingPipelineFromFile(fileId);
    if(err < 0)
    {
      setErrorCondition(err);
      QString ss = QObject::tr("Error trying to read the existing pipeline from the file '%1'").arg(getInputFile());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return DataContainerArray::New();
    }
  }

  return dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy DataContainerReader::readDataContainerArrayStructure(const QString& path)
{
  SIMPLH5DataReader::Pointer h5Reader = SIMPLH5DataReader::New();
  if(h5Reader->openFile(path) == false)
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
  err = QH5Utilities::getGroupObjects(pipelineGroupId, H5Utilities::H5Support_GROUP, groupList);

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
  connect(simplReader.get(), &SIMPLH5DataReader::errorGenerated, [=](const QString& title, const QString& msg, const int& code) {
    setErrorCondition(code);
    notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
  });

  if(simplReader->openFile(getInputFile()) == false)
  {
    return false;
  }

  // If there is something in the cached proxy...
  if(m_InputFileDataContainerArrayProxy.dataContainers.size() > 0)
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
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());

    SIMPL_COPY_INSTANCEVAR(InputFile)

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
const QString DataContainerReader::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataContainerReader::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataContainerReader::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataContainerReader::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid DataContainerReader::getUuid()
{
  return QUuid("{043cbde5-3878-5718-958f-ae75714df0df}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataContainerReader::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataContainerReader::getHumanLabel() const
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

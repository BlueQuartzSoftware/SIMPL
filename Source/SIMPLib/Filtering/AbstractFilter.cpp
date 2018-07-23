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

#include "AbstractFilter.h"

#include <QtCore/QMetaProperty>

#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/IFilterFactory.hpp"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Plugin/PluginManager.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::AbstractFilter()
: Observable()
, m_ErrorCondition(0)
, m_WarningCondition(0)
, m_InPreflight(false)
, m_Enabled(true)
, m_Removing(false)
, m_PipelineIndex(0)
, m_Cancel(false)

{
  m_DataContainerArray = DataContainerArray::New();
  m_PreviousFilter = NullPointer();
  m_NextFilter = NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::~AbstractFilter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AbstractFilter::CreateFilterFromClassName(const QString& className)
{
  FilterManager* fm = FilterManager::Instance();
  if(nullptr == fm)
  {
    return AbstractFilter::NullPointer();
  }
  IFilterFactory::Pointer wf = fm->getFactoryFromClassName(className);
  if(nullptr == wf)
  {
    return AbstractFilter::NullPointer();
  }

  // Create an instance of the filter. Since we are dealing with the AbstractFilter interface we can not
  // actually use the concrete filter class. We are going to have to rely on QProperties or Signals/Slots
  // to communicate changes back to the filter.
  AbstractFilter::Pointer filter = wf->create();
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::setCancel(bool value)
{
  m_Cancel = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::renameDataArrayPath(DataArrayPath::RenameType renamePath)
{
  // Updates any DataArrayPath or proxy property assigned to this filter.
  // A signal is emitted if a change was required stating the property name, old path, and new path.
  // No additional methods are required aside from a connecting slot in filter parameter widgets to update their UI.
  // If all filters stored their data paths as DataArrayPaths instead of QString without delimiters,
  // this would handle all required updates.  This was moved to FilterParameter::dataArrayRenamed.
  // However, moving this to FilterParameter does not update any private properties that do not 
  // have a FilterParameter assigned.
#if 0
  const QMetaObject* metaobject = metaObject();
  int count = metaobject->propertyCount();
  for(int i = 0; i < count; i++)
  {
    QMetaProperty metaproperty = metaobject->property(i);
    const char* name = metaproperty.name();
    QVariant var = property(name);
    if(var.isValid() && var.canConvert<DataArrayPath>())
    {
      DataArrayPath path = var.value<DataArrayPath>();
      if(path.updatePath(oldPath, newPath))
      {
        //QString ss = QString("Updated property '%1' in %2").arg(name).arg(getHumanLabel());
        //notifyStandardOutputMessage(getHumanLabel(), getPipelineIndex(), ss);
        //notifyStatusMessage(getHumanLabel(), ss);
        var.setValue(path);
        this->setProperty(name, var);
        emit dataArrayPathUpdated(name, oldPath, newPath);
      }
    }
    else if(var.isValid() && var.canConvert<DataContainerArrayProxy>())
    {
      DataContainerArrayProxy proxy = var.value<DataContainerArrayProxy>();
      proxy.updatePath(oldPath, newPath);
      var.setValue(proxy);
      this->setProperty(name, var);
      emit dataArrayPathUpdated(name, oldPath, newPath);
    }
    else if(var.isValid() && var.canConvert<DataContainerProxy>())
    {
      DataContainerProxy proxy = var.value<DataContainerProxy>();
      proxy.updatePath(oldPath, newPath);
      var.setValue(proxy);
      this->setProperty(name, var);
      emit dataArrayPathUpdated(name, oldPath, newPath);
    }
    else if(var.isValid() && var.canConvert<AttributeMatrixProxy>())
    {
      AttributeMatrixProxy proxy = var.value<AttributeMatrixProxy>();
      proxy.updatePath(oldPath, newPath);
      var.setValue(proxy);
      this->setProperty(name, var);
      emit dataArrayPathUpdated(name, oldPath, newPath);
    }
    else if(var.isValid() && var.canConvert<DataArrayProxy>())
    {
      DataArrayProxy proxy = var.value<DataArrayProxy>();
      proxy.updatePath(oldPath, newPath);
      var.setValue(proxy);
      this->setProperty(name, var);
      emit dataArrayPathUpdated(name, oldPath, newPath);
    }
  }
#endif

  // Some filter parameters handle paths as nothing but a QString (i.e. DataContainerSelectionFilterParameter)
  // This does not store data in a way that represents what is stored or in a consistent manner with anything else.
  // Because this format cannot be quieried nicely like the above code, filter parameters have to be able to update
  // their own paths in these cases.  In cases where private properties need to be updated but do not have
  // assigned filter parameters, although this should not be the case, this code, unlike the above snippet, will not update them.
  FilterParameterVector filterParams = getFilterParameters();
  for(FilterParameter::Pointer filterParam : filterParams)
  {
    filterParam->dataArrayPathRenamed(this, renamePath);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::renameDataArrayPaths(DataArrayPath::RenameContainer renamedPaths)
{
  for(DataArrayPath::RenameType rename : renamedPaths)
  {
    renameDataArrayPath(rename);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::setupFilterParameters()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::execute()
{
  setErrorCondition(-3015);
  notifyErrorMessage(getNameOfClass(), "QAbstractFilter does not implement an execute method. Please use a subclass instead.", getErrorCondition());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::preflight()
{
  setInPreflight(true);
  setErrorCondition(-3016);
  notifyErrorMessage(getNameOfClass(), "AbstractFilter does not implement a preflight method. Please use a subclass instead.", getErrorCondition());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::doesPipelineContainFilterBeforeThis(const QString& name)
{
  bool contains = false;
  // Check the previous filter
  AbstractFilter::Pointer prev = getPreviousFilter().lock();
  while(prev.get() != nullptr)
  {
    if(prev->getNameOfClass().compare(name) == 0)
    {
      contains = true;
      break;
    }
    prev = prev->getPreviousFilter().lock();
  }
  return contains;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::doesPipelineContainFilterAfterThis(const QString& name)
{
  bool contains = false;
  // Check the previous filter
  AbstractFilter::Pointer next = getNextFilter().lock();
  while(next.get() != nullptr)
  {
    if(next->getNameOfClass().compare(name) == 0)
    {
      contains = true;
      break;
    }
    next = next->getNextFilter().lock();
  }
  return contains;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void insertAttributeMatrixPaths(DataContainer::Pointer dc, AttributeMatrix::Pointer am, std::list<DataArrayPath>& paths)
{
  paths.push_back(DataArrayPath(dc->getName(), am->getName(), ""));
  // Insert all DataArrayPaths
  for(QString daName : am->getAttributeArrayNames())
  {
    DataArrayPath daPath(dc->getName(), am->getName(), daName);
    paths.push_back(daPath);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void insertDataContainerPaths(DataContainer::Pointer dc, std::list<DataArrayPath>& paths)
{
  paths.push_back(DataArrayPath(dc->getName(), "", ""));
  // Insert all AttributeMatrix paths
  for(AttributeMatrix::Pointer am : dc->getAttributeMatrices())
  {
    insertAttributeMatrixPaths(dc, am, paths);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<DataArrayPath> AbstractFilter::getCreatedPaths()
{
  std::list<DataArrayPath> createdPaths;
  // Check if the DataContainerArray is valid
  if(nullptr == getDataContainerArray())
  {
    return createdPaths;
  }

  AbstractFilter::Pointer previousFilter = getPreviousFilter().lock();
  if(previousFilter && nullptr != previousFilter->getDataContainerArray())
  {
    DataContainerArray::Pointer prevDca = previousFilter->getDataContainerArray();
    DataContainerArray::Pointer dca = getDataContainerArray();

    // Check DataContainers
    for(DataContainer::Pointer dc : dca->getDataContainers())
    {
      if(false == prevDca->doesDataContainerExist(dc->getName()))
      {
        insertDataContainerPaths(dc, createdPaths);
      }
      else
      {
        // Check AttributeMatrices
        for(AttributeMatrix::Pointer am : dc->getAttributeMatrices())
        {
          DataArrayPath amPath(dc->getName(), am->getName(), "");
          if(false == prevDca->doesAttributeMatrixExist(amPath))
          {
            insertAttributeMatrixPaths(dc, am, createdPaths);
          }
          else
          {
            // Check DataArrays
            for(QString daName : am->getAttributeArrayNames())
            {
              DataArrayPath daPath(dc->getName(), am->getName(), daName);
              if(false == prevDca->doesAttributeArrayExist(daPath))
              {
                createdPaths.push_back(daPath);
              }
            }
          }
        }
      }
    }
  }
  else
  {
    DataContainerArray::Pointer dca = getDataContainerArray();

    // Check if the DataContainerArray is valid
    if(dca)
    {
      // Add all paths if there is no previous filter to compare to
      for(DataContainer::Pointer dc : dca->getDataContainers())
      {
        insertDataContainerPaths(dc, createdPaths);
      }
    }
  }

  return createdPaths;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::RenameContainer AbstractFilter::getRenamedPaths()
{
  // Implemented in filters that rename existing paths
  DataArrayPath::RenameContainer container;
  return container;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  Q_ASSERT(reader != nullptr);
  qDebug() << "AbstractFilter::readFilterParameters() -> Writing Filter Options"
           << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::readFilterParameters(QJsonObject& obj)
{
  QVector<FilterParameter::Pointer> filterParameters = getFilterParameters();
  for(int i = 0; i < filterParameters.size(); i++)
  {
    FilterParameter::Pointer fp = filterParameters[i];
    fp->readJson(obj);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::preWriteFilterParameters(QJsonObject& obj, QJsonObject& rootObject)
{
  Q_UNUSED(obj);
  Q_UNUSED(rootObject);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::writeFilterParameters(QJsonObject& obj) const
{
  QVector<FilterParameter::Pointer> filterParameters = getFilterParameters();
  for(auto const fp : filterParameters)
  {
    fp->writeJson(obj);
  }
  obj[SIMPL::Settings::FilterVersion] = getFilterVersion();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::postWriteFilterParameters(QJsonObject& obj, QJsonObject& rootObject)
{
  Q_UNUSED(obj);
  Q_UNUSED(rootObject);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject AbstractFilter::toJson()
{
  QJsonObject json;

  json[SIMPL::Settings::FilterName] = getNameOfClass();
  json[SIMPL::Settings::HumanLabel] = getHumanLabel();
  json[SIMPL::Settings::GroupName] = getGroupName();
  json[SIMPL::Settings::SubGroupName] = getSubGroupName();
  json[SIMPL::Settings::BrandingString] = getBrandingString();
  json[SIMPL::Settings::CompiledLibraryName] = getCompiledLibraryName();
  json[SIMPL::Settings::FilterEnabled] = getEnabled();
  json[SIMPL::Settings::FilterUuid] = getUuid().toString();

  writeFilterParameters(json);

  return json;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::getCancel()
{
  return m_Cancel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ISIMPLibPlugin* AbstractFilter::getPluginInstance()
{
  PluginManager* manager = PluginManager::Instance();
  QString pluginName = manager->getPluginName(getNameOfClass());
  ISIMPLibPlugin* plugin = manager->findPlugin(pluginName);
  return plugin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AbstractFilter::newFilterInstance(bool copyFilterParameters) const
{
  return NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::copyFilterParameterInstanceVariables(AbstractFilter* filter) const
{
  QJsonObject filterJson;
  writeFilterParameters(filterJson);
  filter->readFilterParameters(filterJson);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getGroupName() const
{
  return "YOUR CLASS SHOULD IMPLEMENT THIS";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getSubGroupName() const
{
  return "YOUR CLASS SHOULD IMPLEMENT THIS";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getHumanLabel() const
{
  return "YOUR CLASS SHOULD IMPLEMENT THIS";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getBrandingString() const
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getCompiledLibraryName() const
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::getFilterVersion() const
{
  return QString("0.0.0");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid AbstractFilter::getUuid()
{
  if(m_Uuid.isNull())
  {
    uint l = 100;
    ushort w1 = 200;
    ushort w2 = 300;
    
    QString libName = getCompiledLibraryName();
    uchar b[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int32_t i = 0;
    while(i < 8 && i < libName.size())
    {
      b[i] = static_cast<uint8_t>(libName.at(i).toLatin1());
      i++;
    }
    QUuid uuid = QUuid(l, w1, w2, b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
    QString nameSpace = QString("%1 %2").arg(getNameOfClass()).arg(getHumanLabel());
    QUuid p1 = QUuid::createUuidV5(uuid, nameSpace);
    m_Uuid = p1;
  }
  return m_Uuid;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbstractFilter::generateHtmlSummary() const
{
  QString html;
  QTextStream ss(&html);
  ss << "<html><head></head>\n";
  ss << "<body>\n";

  // A table for the summary items
  ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\">\n";
  ss << "<tbody>\n";
  ss << "	<tr><th align=\"right\">Filter Name:</th><td>" << getHumanLabel() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Class Name:</th><td>" << getNameOfClass() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Group Name:</th><td>" << getGroupName() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Subroup Name:</th><td>" << getSubGroupName() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Version:</th><td>" << getFilterVersion() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Branding String:</th><td>" << getBrandingString() << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Compiled Plugin Name:</th><td>" << getCompiledLibraryName() << "</td></tr>\n";
  ss << "</tbody></table>\n";
  ss << "</body></html>";
  return html;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::printValues(std::ostream& out) const
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::cleanupFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::notifyErrorMessage(const QString& humanLabel, const QString& str, int code)
{
  PipelineMessage pm = PipelineMessage::CreateErrorMessage(getNameOfClass(), humanLabel, str, code);
  pm.setPipelineIndex(getPipelineIndex());
  emit filterGeneratedMessage(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::notifyStatusMessage(const QString& humanLabel, const QString& str)
{
  PipelineMessage pm = PipelineMessage::CreateStatusMessage(getNameOfClass(), humanLabel, str);
  pm.setPipelineIndex(getPipelineIndex());
  emit filterGeneratedMessage(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::notifyStatusMessage(const QString& prefix, const QString& humanLabel, const QString& str)
{
  PipelineMessage pm = PipelineMessage::CreateStatusMessage(getNameOfClass(), humanLabel, str);
  pm.setPrefix(prefix);
  pm.setPipelineIndex(getPipelineIndex());
  emit filterGeneratedMessage(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::notifyStandardOutputMessage(const QString& humanLabel, int pipelineIndex, const QString& str)
{
  PipelineMessage pm = PipelineMessage::CreateStandardOutputMessage(humanLabel, pipelineIndex, str);
  pm.setPipelineIndex(getPipelineIndex());
  emit filterGeneratedMessage(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::notifyWarningMessage(const QString& humanLabel, const QString& str, int code)
{
  PipelineMessage pm = PipelineMessage::CreateWarningMessage(getNameOfClass(), humanLabel, str, code);
  pm.setPipelineIndex(getPipelineIndex());
  emit filterGeneratedMessage(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::notifyProgressMessage(const QString& prefix, const QString& humanLabel, const QString& str, int progress)
{
  PipelineMessage pm = PipelineMessage::CreateStatusMessage(getNameOfClass(), humanLabel, str);
  pm.setPrefix(prefix);
  pm.setProgressValue(progress);
  pm.setType(PipelineMessage::MessageType::StatusMessageAndProgressValue);
  pm.setPipelineIndex(getPipelineIndex());
  emit filterGeneratedMessage(pm);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::notifyMissingProperty(FilterParameter* filterParameter)
{
  QString ss =
      QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
              " Please report this issue to the developers of this filter.")
          .arg(filterParameter->getPropertyName())
          .arg(getHumanLabel());

  setWarningCondition(-1);
  notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
}

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

#include "ImportHDF5Dataset.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/ImportHDF5DatasetFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

#include "H5Support/H5Utilities.h"
#include "H5Support/QH5Utilities.h"
#include "H5Support/H5ScopedSentinel.h"

namespace Detail
{
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> IDataArray::Pointer readH5Dataset(hid_t locId, const QString& datasetPath, const size_t& numOfTuples, const QVector<size_t>& cDims)
{
  herr_t err = -1;
  IDataArray::Pointer ptr;

  ptr = DataArray<T>::CreateArray(numOfTuples, cDims, datasetPath);

  T* data = (T*)(ptr->getVoidPointer(0));
  err = QH5Lite::readPointerDataset(locId, datasetPath, data);
  if(err < 0)
  {
    qDebug() << "readH5Data read error: " << __FILE__ << "(" << __LINE__ << ")";
    ptr = IDataArray::NullPointer();
  }
  return ptr;
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5Dataset::ImportHDF5Dataset()
: AbstractFilter()
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5Dataset::~ImportHDF5Dataset() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5Dataset::initialize()
{
  setErrorCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5Dataset::setupFilterParameters()
{
  FilterParameterVector parameters;

  ImportHDF5DatasetFilterParameter::Pointer parameter =
      ImportHDF5DatasetFilterParameter::New(QString("Select HDF5 File"), // Human Label
                                            QString("ImportHDF5File"),   // Property Name
                                            QString(""),                 // File Path Default Value
                                            QString(""),                 // Dataset Default value
                                            FilterParameter::Parameter,  // Category
                                            SIMPL_BIND_SETTER(ImportHDF5Dataset, this, HDF5FilePath), SIMPL_BIND_GETTER(ImportHDF5Dataset, this, HDF5FilePath),
                                            SIMPL_BIND_SETTER(ImportHDF5Dataset, this, DatasetPath), SIMPL_BIND_GETTER(ImportHDF5Dataset, this, DatasetPath), -1);
  parameters.push_back(parameter);

  parameters.push_back(SIMPL_NEW_STRING_FP("Component Dimensions", ComponentDimensions, FilterParameter::Parameter, ImportHDF5Dataset));

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Attribute Matrix", SelectedAttributeMatrix, FilterParameter::RequiredArray, ImportHDF5Dataset, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5Dataset::dataCheck()
{
  setErrorCondition(0);

  if(m_HDF5FilePath.isEmpty())
  {
    QString ss = "The HDF5 file path is empty.  Please select an HDF5 file.";
    setErrorCondition(-20001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QFileInfo hdf5FileInfo(m_HDF5FilePath);
  QString ext = hdf5FileInfo.suffix();
  if(ext != "h5" && ext != "hdf5" && ext != "dream3d")
  {
    QString ss = tr("The selected file '%1' is not an HDF5 file.").arg(hdf5FileInfo.fileName());
    setErrorCondition(-20002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(hdf5FileInfo.exists() == false)
  {
    QString ss = tr("The selected file '%1' does not exist.").arg(hdf5FileInfo.fileName());
    setErrorCondition(-20003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_DatasetPath.isEmpty())
  {
    QString ss = tr("The dataset path is empty.  Please enter a dataset path.");
    setErrorCondition(-20004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  int err = 0;
  AttributeMatrix::Pointer am = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedAttributeMatrix, err);
  if(getErrorCondition() < 0)
  {
    return;
  }

  hid_t fileId = H5Utilities::openFile(m_HDF5FilePath.toStdString(), true);
  if(fileId < 0)
  {
    std::cout << "Error Reading HDF5 file: " << m_HDF5FilePath.toStdString() << std::endl;
    return;
  }
  H5ScopedFileSentinel sentinel(&fileId, true);

  QString parentPath = QH5Utilities::getParentPath(m_DatasetPath);
  hid_t parentId;
  if(parentPath.isEmpty())
  {
    parentId = fileId;
  }
  else
  {
    parentId = QH5Utilities::openHDF5Object(fileId, parentPath);
    sentinel.addGroupId(&parentId);
  }

  // Read dataset into DREAM.3D structure
  QString objectName = QH5Utilities::getObjectNameFromPath(m_DatasetPath);

  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  err = QH5Lite::getDatasetInfo(parentId, objectName, dims, type_class, type_size);
  if(err < 0)
  {
    QString ss = tr("Error reading type info from dataset with path '%1'").arg(m_DatasetPath);
    setErrorCondition(-20005);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_ComponentDimensions.isEmpty())
  {
    QString ss = tr("The component dimensions are empty.  Please enter the component dimensions, using comma-separated values (ex: 4x2 would be '4, 2').");
    setErrorCondition(-20006);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> cDims = createComponentDimensions();
  if(cDims.isEmpty())
  {
    QString ss = tr("Component Dimensions are not in the right format. Use comma-separated values (ex: 4x2 would be '4, 2').");
    setErrorCondition(-20007);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Calculate the product of the dataset dimensions and the product of the component dimensions.
  // Since we're already looping over both of these sets of dimensions, let's also create our error message
  // in case the equation does not work and we have to bail.
  QString ss = "";
  QTextStream stream(&ss);

  stream << tr("HDF5 File Path: %1\n").arg(m_HDF5FilePath);
  stream << tr("HDF5 Dataset Path: %1\n").arg(m_DatasetPath);

  size_t cDimsProduct = 1;
  stream << tr("Component Dimensions: (");
  for(int i = 0; i < cDims.size(); i++)
  {
    stream << cDims[i];
    cDimsProduct = cDimsProduct * cDims[i];
    if(i != cDims.size() - 1)
    {
      stream << ", ";
    }
  }
  stream << ")\n";

  size_t dsetDimsProduct = 1;
  stream << tr("HDF5 Dataset Dimensions: (");
  for(int i = 0; i < dims.size(); i++)
  {
    stream << dims[i];
    dsetDimsProduct = dsetDimsProduct * dims[i];
    if(i != dims.size() - 1)
    {
      stream << ", ";
    }
  }
  stream << ")\n";

  stream << tr("Attribute Matrix Path: %1/%2\n").arg(m_SelectedAttributeMatrix.getDataContainerName()).arg(m_SelectedAttributeMatrix.getAttributeMatrixName());
  stream << tr("Attribute Matrix Tuple Count: %1\n\n").arg(am->getNumberOfTuples());

  if(dsetDimsProduct % cDimsProduct != 0 || dsetDimsProduct / cDimsProduct != am->getNumberOfTuples())
  {
    stream << tr("This dataset cannot be read because this equation is not satisfied:\n"
                 "(Product of dataset dimensions) / (Product of component dimensions) = (Attribute Matrix Tuple Count)\n"
                 "%1 / %2 = %3\n%4 = %5")
                  .arg(QString::number(dsetDimsProduct))
                  .arg(QString::number(cDimsProduct))
                  .arg(QString::number(am->getNumberOfTuples()))
                  .arg(QString::number(dsetDimsProduct / cDimsProduct))
                  .arg(QString::number(am->getNumberOfTuples()));

    setErrorCondition(-20008);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  IDataArray::Pointer dPtr = readIDataArray(parentId, objectName, am->getNumberOfTuples(), cDims, getInPreflight());
  am->addAttributeArray(dPtr->getName(), dPtr);

  // The sentinel will close the HDF5 File and any groups that were open.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5Dataset::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5Dataset::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> ImportHDF5Dataset::createComponentDimensions()
{
  QVector<size_t> cDims;
  QStringList dimsStrVec = m_ComponentDimensions.split(',', QString::SkipEmptyParts);
  for(int i = 0; i < dimsStrVec.size(); i++)
  {
    QString dimsStr = dimsStrVec[i];
    dimsStr = dimsStr.remove(" ");

    bool ok = false;
    int val = dimsStr.toInt(&ok);
    if(ok == false)
    {
      return QVector<size_t>();
    }

    cDims.push_back(val);
  }

  return cDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer ImportHDF5Dataset::readIDataArray(hid_t gid, const QString& name, size_t numOfTuples, QVector<size_t> cDims, bool metaDataOnly)
{
  herr_t err = -1;
  // herr_t retErr = 1;
  hid_t typeId = -1;
  H5T_class_t attr_type;
  size_t attr_size;
  QString res;

  QVector<hsize_t> dims; // Reusable for the loop
  IDataArray::Pointer ptr = IDataArray::NullPointer();
  // qDebug() << "Reading Attribute " << *iter ;
  typeId = QH5Lite::getDatasetType(gid, name);
  if(typeId < 0)
  {
    return ptr;
  }
  // Get the HDF5 DataSet information. the dimensions will be the combined Tuple Dims and the Data Array Componenet dimes
  err = QH5Lite::getDatasetInfo(gid, name, dims, attr_type, attr_size);
  if(err < 0)
  {
    qDebug() << "Error in getAttributeInfo method in readUserMetaData.";
  }

  switch(attr_type)
  {
  case H5T_STRING:
    res.clear(); // Clear the string out first
    err = QH5Lite::readStringDataset(gid, name, res);
    //        if(err >= 0)
    //        {
    //          IDataArray::Pointer attr = MXAAsciiStringData::Create(res);
    //          attr->setName(name);
    //          attributes[*iter] = attr;
    //        }
    break;
  case H5T_INTEGER:
    // qDebug() << "User Meta Data Type is Integer" ;
    if(H5Tequal(typeId, H5T_STD_U8BE) || H5Tequal(typeId, H5T_STD_U8LE))
    {
      if(metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<uint8_t>(gid, name, numOfTuples, cDims);
      }
      else
      {
        ptr = DataArray<uint8_t>::CreateArray(numOfTuples, cDims, name, false);
      }
    }
    else if(H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId, H5T_STD_U16LE))
    {
      if(metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<uint16_t>(gid, name, numOfTuples, cDims);
      }
      else
      {
        ptr = DataArray<uint16_t>::CreateArray(numOfTuples, cDims, name, false);
      }
    }
    else if(H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId, H5T_STD_U32LE))
    {
      if(metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<uint32_t>(gid, name, numOfTuples, cDims);
      }
      else
      {
        ptr = DataArray<uint32_t>::CreateArray(numOfTuples, cDims, name, false);
      }
    }
    else if(H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId, H5T_STD_U64LE))
    {
      if(metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<uint64_t>(gid, name, numOfTuples, cDims);
      }
      else
      {
        ptr = DataArray<uint64_t>::CreateArray(numOfTuples, cDims, name, false);
      }
    }
    else if(H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId, H5T_STD_I8LE))
    {
      if(metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<int8_t>(gid, name, numOfTuples, cDims);
      }
      else
      {
        ptr = DataArray<int8_t>::CreateArray(numOfTuples, cDims, name, false);
      }
    }
    else if(H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId, H5T_STD_I16LE))
    {
      if(metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<int16_t>(gid, name, numOfTuples, cDims);
      }
      else
      {
        ptr = DataArray<int16_t>::CreateArray(numOfTuples, cDims, name, false);
      }
    }
    else if(H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId, H5T_STD_I32LE))
    {
      if(metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<int32_t>(gid, name, numOfTuples, cDims);
      }
      else
      {
        ptr = DataArray<int32_t>::CreateArray(numOfTuples, cDims, name, false);
      }
    }
    else if(H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId, H5T_STD_I64LE))
    {
      if(metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<int64_t>(gid, name, numOfTuples, cDims);
      }
      else
      {
        ptr = DataArray<int64_t>::CreateArray(numOfTuples, cDims, name, false);
      }
    }
    else
    {
      qDebug() << "Unknown Type: " << typeId << " at " << name;
      err = -1;
    }
    break;
  case H5T_FLOAT:
    if(attr_size == 4)
    {
      if(metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<float>(gid, name, numOfTuples, cDims);
      }
      else
      {
        ptr = DataArray<float>::CreateArray(numOfTuples, cDims, name, false);
      }
    }
    else if(attr_size == 8)
    {
      if(metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<double>(gid, name, numOfTuples, cDims);
      }
      else
      {
        ptr = DataArray<double>::CreateArray(numOfTuples, cDims, name, false);
      }
    }
    else
    {
      qDebug() << "Unknown Floating point type";
      err = -1;
    }
    break;
  default:
    qDebug() << "Error: readUserMetaData() Unknown attribute type: " << attr_type;
    QH5Utilities::printHDFClassType(attr_type);
  }

  err = H5Tclose(typeId);
  // Close the H5A type Id that was retrieved during the loop

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportHDF5Dataset::getHDF5Dimensions()
{
  return m_HDF5Dimensions;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportHDF5Dataset::newFilterInstance(bool copyFilterParameters) const
{
  ImportHDF5Dataset::Pointer filter = ImportHDF5Dataset::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    filter->setHDF5FilePath(getHDF5FilePath());
    filter->setDatasetPath(getDatasetPath());
    filter->setComponentDimensions(getComponentDimensions());
    filter->setSelectedAttributeMatrix(getSelectedAttributeMatrix());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5Dataset::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5Dataset::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5Dataset::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5Dataset::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ImportHDF5Dataset::getUuid()
{
  return QUuid("{9e98c3b0-5707-5a3b-b8b5-23ef83b02896}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5Dataset::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5Dataset::getHumanLabel() const
{
  return "Import HDF5 Dataset";
}

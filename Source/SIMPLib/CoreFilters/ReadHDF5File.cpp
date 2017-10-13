/*
 * Your License or Copyright can go here
 */

#include "ReadHDF5File.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/ReadHDF5FileFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "SIMPLib/SIMPLibVersion.h"

// Include the MOC generated file for this class
#include "moc_ReadHDF5File.cpp"

namespace Detail
{
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> IDataArray::Pointer readH5Dataset(hid_t locId, const QString& datasetPath, const QVector<size_t>& tDims, const QVector<size_t>& cDims)
  {
    herr_t err = -1;
    IDataArray::Pointer ptr;

    ptr = DataArray<T>::CreateArray(tDims, cDims, datasetPath);

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
ReadHDF5File::ReadHDF5File() :
  AbstractFilter()
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadHDF5File::~ReadHDF5File()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadHDF5File::initialize()
{
  setErrorCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadHDF5File::setupFilterParameters()
{
  FilterParameterVector parameters;

  ReadHDF5FileFilterParameter::Pointer parameter = ReadHDF5FileFilterParameter::New();
  parameter->setHumanLabel("Select HDF5 File");
  parameter->setCategory(FilterParameter::Parameter);
  parameter->setFilter(this);
  parameters.push_back(parameter);


//  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("HDF5 File", HDF5FilePath, FilterParameter::Parameter, ReadHDF5File));
//  parameters.push_back(SIMPL_NEW_STRING_FP("Dataset Path", DatasetPath, FilterParameter::Parameter, ReadHDF5File));

//  PreflightUpdatedValueFilterParameter::Pointer param =
//      SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("HDF5 Dimensions", HDF5Dimensions, FilterParameter::Parameter, ReadHDF5File);
//  param->setReadOnly(true);
//  parameters.push_back(param);

  parameters.push_back(SIMPL_NEW_STRING_FP("Component Dimensions", ComponentDimensions, FilterParameter::Parameter, ReadHDF5File));

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Attribute Matrix", SelectedAttributeMatrix, FilterParameter::RequiredArray, ReadHDF5File, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadHDF5File::dataCheck()
{
  setErrorCondition(0);
  
  if (m_HDF5FilePath.isEmpty())
  {
    QString ss = "The HDF5 file path is empty.  Please select an HDF5 file.";
    setErrorCondition(-20001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QFileInfo hdf5FileInfo(m_HDF5FilePath);
  QString ext = hdf5FileInfo.suffix();
  if (hdf5FileInfo.exists() && hdf5FileInfo.isFile() && ext != "h5" && ext != "hdf5" && ext != "dream3d")
  {
    QString ss = tr("The selected file '%1' is not an HDF5 file.").arg(hdf5FileInfo.fileName());
    setErrorCondition(-20002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_DatasetPath.isEmpty())
  {
    QString ss = tr("The dataset path is empty.  Please enter a dataset path.");
    setErrorCondition(-20003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_ComponentDimensions.isEmpty())
  {
    QString ss = tr("The component dimensions are empty.  Please enter the component dimensions, using comma-separated values (ex: 4x2 would be '4, 2').");
    setErrorCondition(-20003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  int err = 0;
  AttributeMatrix::Pointer am = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedAttributeMatrix, err);
  if (getErrorCondition() < 0) { return; }

  hid_t fileId = H5Utilities::openFile(m_HDF5FilePath.toStdString(), true);
  if (fileId < 0)
  {
    std::cout << "Error Reading HDF5 file: " << m_HDF5FilePath.toStdString() << std::endl;
    return;
  }

  QString parentPath = QH5Utilities::getParentPath(m_DatasetPath);

  hid_t parentId = QH5Utilities::openHDF5Object(fileId, parentPath);

  // Read dataset into DREAM.3D structure
  QString objectName = QH5Utilities::getObjectNameFromPath(m_DatasetPath);

  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  err = QH5Lite::getDatasetInfo(parentId, objectName, dims, type_class, type_size);
  if (err < 0)
  {
    QString ss = tr("Error reading type info from dataset with path '%1'").arg(m_DatasetPath);
    setErrorCondition(-20004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> cDims = createComponentDimensions();
  if (getErrorCondition() < 0)
  {
    return;
  }

  QVector<size_t> tDims = calculateTupleDimensions(cDims, dims, am->getNumberOfTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  IDataArray::Pointer dPtr = readIDataArray(parentId, objectName, tDims, cDims, getInPreflight());

  if (dPtr->getNumberOfTuples() != am->getNumberOfTuples())
  {
    QString ss = tr("The number of tuples of dataset with path '%1' does not equal the number of tuples of the selected attribute matrix '%2'").arg(m_DatasetPath).arg(m_SelectedAttributeMatrix.getAttributeMatrixName());
    setErrorCondition(-20004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  am->addAttributeArray(dPtr->getName(), dPtr);

  QH5Utilities::closeHDF5Object(parentId);
  QH5Utilities::closeFile(fileId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadHDF5File::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadHDF5File::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> ReadHDF5File::calculateTupleDimensions(QVector<size_t> cDims, QVector<hsize_t> dsetDims, size_t amTupleCount)
{
  QVector<size_t> tDims;
  QVector<size_t> tempCDims = cDims;

  // Check if the component dimension values exist in the dataset dimensions, and that the leftover
  // dataset dimension values still multiply together to match the attribute matrix count
  {
    for (int i = 0; i < dsetDims.size(); i++)
    {
      if (tempCDims.contains(dsetDims[i]))
      {
        tempCDims.removeOne(dsetDims[i]);
      }
      else
      {
        tDims.push_back(dsetDims[i]);
      }
    }

    size_t product = 1;
    for (int i = 0; i < tDims.size(); i++)
    {
      product = product * tDims[i];
    }

    if (product == amTupleCount)
    {
      return tDims;
    }
  }

  // Check if the product of the component dimensions exists in the dataset dimensions, and that the
  // leftover dataset dimension values still multiply together to match the attribute matrix count
  tDims.clear();
  tempCDims = cDims;
  {
    size_t product = 1;
    for (int i = 0; i < tempCDims.size(); i++)
    {
      product = product * tempCDims[i];
    }
    tempCDims.resize(1);
    tempCDims[0] = product;

    for (int i = 0; i < dsetDims.size(); i++)
    {
      if (tempCDims.contains(dsetDims[i]))
      {
        tempCDims.removeOne(dsetDims[i]);
      }
      else
      {
        tDims.push_back(dsetDims[i]);
      }
    }

    product = 1;
    for (int i = 0; i < tDims.size(); i++)
    {
      product = product * tDims[i];
    }

    if (product == amTupleCount)
    {
      return tDims;
    }
  }

  // Check if the product of the component dimensions added with the attribute matrix tuple count equals
  // the product of the dataset dimension values.
  tDims.clear();
  tempCDims = cDims;
  {
    size_t cDimsProduct = 1;
    for (int i = 0; i < tempCDims.size(); i++)
    {
      cDimsProduct = cDimsProduct * tempCDims[i];
    }

    size_t totalValues = cDimsProduct + amTupleCount;
    size_t dsetDimsProduct = 1;
    for (int i = 0; i < dsetDims.size(); i++)
    {
      dsetDimsProduct = dsetDimsProduct * dsetDims[i];
    }

    if (totalValues == dsetDimsProduct)
    {
      tDims.push_back(amTupleCount);
      return tDims;
    }
  }

  // None of the component dimensions match the dataset dimensions in any of these ways, so throw an error and bail
  QString ss = tr("The specified component dimensions do not exist in the dataset dimensions.  Please specify valid component dimensions.");
  setErrorCondition(-20006);
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  return QVector<size_t>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> ReadHDF5File::createComponentDimensions()
{
  QVector<size_t> cDims;
  QStringList dimsStrVec = m_ComponentDimensions.split(',', QString::SkipEmptyParts);
  for (int i = 0; i < dimsStrVec.size(); i++)
  {
    QString dimsStr = dimsStrVec[i];
    dimsStr = dimsStr.remove(" ");

    bool ok = false;
    int val = dimsStr.toInt(&ok);
    if (ok == false)
    {
      QString ss = tr("Component Dimensions are not in the right format. Use comma-separated values (ex: 4x2 would be '4, 2').").arg(m_DatasetPath);
      setErrorCondition(-20005);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return QVector<size_t>();
    }

    cDims.push_back(val);
  }

  return cDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer ReadHDF5File::readIDataArray(hid_t gid, const QString& name, QVector<size_t> tDims, QVector<size_t> cDims, bool metaDataOnly)
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
          ptr = Detail::readH5Dataset<uint8_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<uint8_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId, H5T_STD_U16LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<uint16_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<uint16_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId, H5T_STD_U32LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<uint32_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<uint32_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId, H5T_STD_U64LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<uint64_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<uint64_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId, H5T_STD_I8LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<int8_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<int8_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId, H5T_STD_I16LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<int16_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<int16_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId, H5T_STD_I32LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<int32_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<int32_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId, H5T_STD_I64LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<int64_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<int64_t>::CreateArray(tDims, cDims, name, false);
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
          ptr = Detail::readH5Dataset<float>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<float>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(attr_size == 8)
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<double>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<double>::CreateArray(tDims, cDims, name, false);
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
QString ReadHDF5File::getHDF5Dimensions()
{
  return m_HDF5Dimensions;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReadHDF5File::newFilterInstance(bool copyFilterParameters)
{
  ReadHDF5File::Pointer filter = ReadHDF5File::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadHDF5File::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadHDF5File::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadHDF5File::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadHDF5File::getGroupName()
{ return SIMPL::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadHDF5File::getSubGroupName()
{ return SIMPL::FilterSubGroups::InputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadHDF5File::getHumanLabel()
{ return "Read HDF5 File"; }


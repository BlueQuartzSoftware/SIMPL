/*
 * Your License or Copyright can go here
 */

#include "ImportHDF5File.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/ImportHDF5FileFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "SIMPLib/SIMPLibVersion.h"

// Include the MOC generated file for this class
#include "moc_ImportHDF5File.cpp"

namespace Detail
{
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> IDataArray::Pointer readH5Dataset(hid_t locId, const QString& datasetPath, const size_t &numOfTuples, const QVector<size_t>& cDims)
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
ImportHDF5File::ImportHDF5File() :
  AbstractFilter()
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5File::~ImportHDF5File()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5File::initialize()
{
  setErrorCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5File::setupFilterParameters()
{
  FilterParameterVector parameters;

  ImportHDF5FileFilterParameter::Pointer parameter = ImportHDF5FileFilterParameter::New();
  parameter->setHumanLabel("Select HDF5 File");
  parameter->setCategory(FilterParameter::Parameter);
  parameter->setFilter(this);
  parameters.push_back(parameter);


//  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("HDF5 File", HDF5FilePath, FilterParameter::Parameter, ImportHDF5File));
//  parameters.push_back(SIMPL_NEW_STRING_FP("Dataset Path", DatasetPath, FilterParameter::Parameter, ImportHDF5File));

//  PreflightUpdatedValueFilterParameter::Pointer param =
//      SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("HDF5 Dimensions", HDF5Dimensions, FilterParameter::Parameter, ImportHDF5File);
//  param->setReadOnly(true);
//  parameters.push_back(param);

  parameters.push_back(SIMPL_NEW_STRING_FP("Component Dimensions", ComponentDimensions, FilterParameter::Parameter, ImportHDF5File));

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Attribute Matrix", SelectedAttributeMatrix, FilterParameter::RequiredArray, ImportHDF5File, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5File::dataCheck()
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
  if (cDims.isEmpty())
  {
    QString ss = tr("Component Dimensions are not in the right format. Use comma-separated values (ex: 4x2 would be '4, 2').").arg(m_DatasetPath);
    setErrorCondition(-20005);
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
  for (int i = 0; i < cDims.size(); i++)
  {
    stream << cDims[i];
    cDimsProduct = cDimsProduct * cDims[i];
    if (i != cDims.size() - 1)
    {
      stream << ", ";
    }
  }
  stream << ")\n";

  size_t dsetDimsProduct = 1;
  stream << tr("HDF5 Dataset Dimensions: (");
  for (int i = 0; i < dims.size(); i++)
  {
    stream << dims[i];
    dsetDimsProduct = dsetDimsProduct * dims[i];
    if (i != dims.size() - 1)
    {
      stream << ", ";
    }
  }
  stream << ")\n";

  stream << tr("Attribute Matrix Path: %1/%2\n").arg(m_SelectedAttributeMatrix.getDataContainerName()).arg(m_SelectedAttributeMatrix.getAttributeMatrixName());
  stream << tr("Attribute Matrix Tuple Count: %1\n\n").arg(am->getNumberOfTuples());

  if (dsetDimsProduct / cDimsProduct != am->getNumberOfTuples())
  {
    stream << tr("This dataset cannot be read because this equation is not satisfied:\n"
                 "(Product of dataset dimensions) / (Product of component dimensions) = (Attribute Matrix Tuple Count)\n"
                 "%1 / %2 = %3\n%4 = %5").arg(QString::number(dsetDimsProduct)).arg(QString::number(cDimsProduct))
              .arg(QString::number(am->getNumberOfTuples())).arg(QString::number(dsetDimsProduct / cDimsProduct)).arg(QString::number(am->getNumberOfTuples()));

    setErrorCondition(-20006);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  IDataArray::Pointer dPtr = readIDataArray(parentId, objectName, am->getNumberOfTuples(), cDims, getInPreflight());
  am->addAttributeArray(dPtr->getName(), dPtr);

  QH5Utilities::closeHDF5Object(parentId);
  QH5Utilities::closeFile(fileId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5File::preflight()
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
void ImportHDF5File::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> ImportHDF5File::createComponentDimensions()
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
      return QVector<size_t>();
    }

    cDims.push_back(val);
  }

  return cDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer ImportHDF5File::readIDataArray(hid_t gid, const QString& name, size_t numOfTuples, QVector<size_t> cDims, bool metaDataOnly)
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
QString ImportHDF5File::getHDF5Dimensions()
{
  return m_HDF5Dimensions;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportHDF5File::newFilterInstance(bool copyFilterParameters)
{
  ImportHDF5File::Pointer filter = ImportHDF5File::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5File::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5File::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5File::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5File::getGroupName()
{ return SIMPL::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5File::getSubGroupName()
{ return SIMPL::FilterSubGroups::InputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportHDF5File::getHumanLabel()
{ return "Import HDF5 File"; }


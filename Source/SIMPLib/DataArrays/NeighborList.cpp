#include "NeighborList.hpp"

#include <QtCore/QMap>
#include <QtCore/QTextStream>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/DataArrays/DataArray.hpp"

// -----------------------------------------------------------------------------
template <typename T>
NeighborList<T>::NeighborList(size_t numTuples, const QString name)
: IDataArray(name)
, m_NumTuples(numTuples)
, m_IsAllocated(false)
{
}

// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::Pointer NeighborList<T>::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
// -----------------------------------------------------------------------------
template <typename T>
QString NeighborList<T>::getNameOfClass() const
{
  return QString("NeighborList<T>");
}
// -----------------------------------------------------------------------------
template <typename T>
QString NeighborList<T>::ClassName()
{
  return QString("NeighborList<T>");
}
// -----------------------------------------------------------------------------
template <typename T>
int32_t NeighborList<T>::getClassVersion() const
{
  return 2;
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::setNumNeighborsArrayName(const QString& name)
{
  m_NumNeighborsArrayName = name;
}

// -----------------------------------------------------------------------------
template <typename T>
QString NeighborList<T>::getNumNeighborsArrayName()
{
  return m_NumNeighborsArrayName;
}

// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::Pointer NeighborList<T>::New()
{
  return CreateArray(0, std::string("NeighborList"), false);
}

// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::Pointer NeighborList<T>::CreateArray(size_t numTuples, const QString& name, bool allocate)
{
  // std::cout << "NeighborList::CreateArray  name= " << name.toStdString() << "   numTuples= " << numTuples << std::endl;
  if(name.isEmpty())
  {
    return NullPointer();
  }
  NeighborList<T>::Pointer ptr = Pointer(new NeighborList<T>(numTuples, name));
  if(allocate)
  {
    ptr->resizeTuples(numTuples);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::Pointer NeighborList<T>::CreateArray(size_t numTuples, const std::string& name, bool allocate)
{
  // std::cout << "NeighborList::CreateArray  name= " << name.toStdString() << "   numTuples= " << numTuples << std::endl;
  if(name.empty())
  {
    return NullPointer();
  }
  NeighborList<T>::Pointer ptr = Pointer(new NeighborList<T>(numTuples, QString::fromStdString(name)));
  if(allocate)
  {
    ptr->resizeTuples(numTuples);
  }
  return ptr;
}
// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::Pointer NeighborList<T>::CreateArray(size_t numTuples, int rank, const size_t* dims, const QString& name, bool allocate)
{
  // std::cout << "NeighborList::CreateArray  name= " << name.toStdString() << "   numTuples= " << numTuples << std::endl;
  if(name.isEmpty())
  {
    return NullPointer();
  }

  size_t numElements = numTuples;
  for(int iter = 0; iter < rank; iter++)
  {
    numElements *= dims[iter];
  }
  NeighborList<T>::Pointer ptr = Pointer(new NeighborList<T>(numElements, name));
  if(allocate)
  {
    ptr->resizeTuples(numElements);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::Pointer NeighborList<T>::CreateArray(size_t numTuples, const std::vector<size_t>& cDims, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  size_t numElements = std::accumulate(cDims.begin(), cDims.end(), numTuples, std::multiplies<>());

  NeighborList<T>::Pointer ptr = Pointer(new NeighborList<T>(numElements, name));
  if(allocate)
  {
    ptr->resizeTuples(numElements);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::Pointer NeighborList<T>::CreateArray(const std::vector<size_t>& tDims, const std::vector<size_t>& cDims, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  size_t numElements = tDims[0];
  for(int iter = 1; iter < tDims.size(); iter++)
  {
    numElements *= tDims[iter];
  }
  for(int iter = 0; iter < cDims.size(); iter++)
  {
    numElements *= cDims[iter];
  }
  NeighborList<T>::Pointer ptr = Pointer(new NeighborList<T>(numElements, name));
  if(allocate)
  {
    ptr->resizeTuples(numElements);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
template <typename T>
IDataArray::Pointer NeighborList<T>::createNewArray(size_t numElements, int rank, const size_t* dims, const QString& name, bool allocate) const
{
  return NeighborList<T>::CreateArray(numElements, rank, dims, name, allocate);
}

// -----------------------------------------------------------------------------
template <typename T>
IDataArray::Pointer NeighborList<T>::createNewArray(size_t numElements, const std::vector<size_t>& dims, const QString& name, bool allocate) const
{
  return NeighborList<T>::CreateArray(numElements, dims, name, allocate);
}

/**
 * @brief isAllocated
 * @return
 */
// -----------------------------------------------------------------------------
template <typename T>
bool NeighborList<T>::isAllocated() const
{
  return true;
}

/**
 * @brief Gives this array a human readable name
 * @param name The name of this array
 */
// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::setInitValue(T initValue)
{
  m_InitValue = initValue;
}

/**
 * @brief Sets all the values to value.
 */
// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::initializeWithValue(T initValue, size_t offset)
{
}

/**
 * @brief getTypeName Returns a string representation of the type of data that is stored by this class. This
 * can be a primitive like char, float, int or the name of a class.
 * @return
 */
// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::getXdmfTypeAndSize(QString& xdmfTypeName, int& precision) const
{
  T value = 0x00;
  xdmfTypeName = "UNKNOWN";
  precision = 0;
  if(typeid(value) == typeid(int8_t))
  {
    xdmfTypeName = "Char";
    precision = 1;
  }
  if(typeid(value) == typeid(uint8_t))
  {
    xdmfTypeName = "UChar";
    precision = 1;
  }

  if(typeid(value) == typeid(int16_t))
  {
    xdmfTypeName = "16 BIT NOT SUPPORTED BY XDMF";
    precision = 0;
  }
  if(typeid(value) == typeid(uint16_t))
  {
    xdmfTypeName = "16 BIT NOT SUPPORTED BY XDMF";
    precision = 0;
  }

  if(typeid(value) == typeid(int32_t))
  {
    xdmfTypeName = "Int";
    precision = 4;
  }
  if(typeid(value) == typeid(uint32_t))
  {
    xdmfTypeName = "UInt";
    precision = 4;
  }

  if(typeid(value) == typeid(int64_t))
  {
    xdmfTypeName = "Int";
    precision = 8;
  }
  if(typeid(value) == typeid(uint64_t))
  {
    xdmfTypeName = "UInt";
    precision = 8;
  }

  if(typeid(value) == typeid(float))
  {
    xdmfTypeName = "Float";
    precision = 4;
  }
  if(typeid(value) == typeid(double))
  {
    xdmfTypeName = "Float";
    precision = 8;
  }

  if(typeid(value) == typeid(bool))
  {
    xdmfTypeName = "uchar";
    precision = 1;
  }
}

// -----------------------------------------------------------------------------
template <typename T>
QString NeighborList<T>::getFullNameOfClass() const
{
  QString theType = getTypeAsString();
  theType = "NeighborList<" + theType + ">";
  return theType;
}

// -----------------------------------------------------------------------------
template <typename T>
QString NeighborList<T>::getTypeAsString() const
{
  if constexpr(std::is_same_v<T, int8_t>)
  {
    return "int8_t";
  }
  else if constexpr(std::is_same_v<T, uint8_t>)
  {
    return "uint8_t";
  }
  else if constexpr(std::is_same_v<T, int16_t>)
  {
    return "int16_t";
  }
  else if constexpr(std::is_same_v<T, uint16_t>)
  {
    return "uint16_t";
  }
  else if constexpr(std::is_same_v<T, int32_t>)
  {
    return "int32_t";
  }
  else if constexpr(std::is_same_v<T, uint32_t>)
  {
    return "uint32_t";
  }
  else if constexpr(std::is_same_v<T, int64_t>)
  {
    return "int64_t";
  }
  else if constexpr(std::is_same_v<T, uint64_t>)
  {
    return "uint64_t";
  }
  else if constexpr(std::is_same_v<T, float>)
  {
    return "float";
  }
  else if constexpr(std::is_same_v<T, double>)
  {
    return "double";
  }
  else if constexpr(std::is_same_v<T, bool>)
  {
    return "bool";
  }

  return "UnknownType";
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::takeOwnership()
{
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::releaseOwnership()
{
}

// -----------------------------------------------------------------------------
template <typename T>
void* NeighborList<T>::getVoidPointer(size_t i)
{
  return nullptr;
}

// -----------------------------------------------------------------------------
template <typename T>
int NeighborList<T>::eraseTuples(const std::vector<size_t>& idxs)
{
  int err = 0;
  // If nothing is to be erased just return
  if(idxs.empty())
  {
    return 0;
  }

  size_t idxsSize = static_cast<size_t>(idxs.size());
  if(idxsSize >= getNumberOfTuples())
  {
    resizeTuples(0);
    return 0;
  }

  size_t arraySize = m_Array.size();
  // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
  // off the end of the array and return an error code.
  for(std::vector<size_t>::size_type i = 0; i < idxs.size(); ++i)
  {
    if(idxs[i] >= arraySize)
    {
      return -100;
    }
  }

  std::vector<SharedVectorType> replacement(arraySize - idxsSize);

  size_t idxsIndex = 0;
  size_t rIdx = 0;
  for(size_t dIdx = 0; dIdx < arraySize; ++dIdx)
  {
    if(dIdx != idxs[idxsIndex])
    {
      replacement[rIdx] = m_Array[dIdx];
      ++rIdx;
    }
    else
    {
      ++idxsIndex;
      if(idxsIndex == idxsSize)
      {
        idxsIndex--;
      }
    }
  }
  m_Array = replacement;
  m_NumTuples = m_Array.size();
  return err;
}

// -----------------------------------------------------------------------------
template <typename T>
int NeighborList<T>::copyTuple(size_t currentPos, size_t newPos)
{
  m_Array[newPos] = m_Array[currentPos];
  return 0;
}

// -----------------------------------------------------------------------------
template <typename T>
bool NeighborList<T>::copyFromArray(size_t destTupleOffset, IDataArray::ConstPointer sourceArray, size_t srcTupleOffset, size_t totalSrcTuples)
{
  if(!m_IsAllocated)
  {
    return false;
  }
  if(destTupleOffset >= m_Array.size())
  {
    return false;
  }
  if(!sourceArray->isAllocated())
  {
    return false;
  }
  const Self* source = dynamic_cast<const Self*>(sourceArray.get());

  if(source == nullptr)
  {
    return false;
  }

  if(sourceArray->getNumberOfComponents() != getNumberOfComponents())
  {
    return false;
  }

  if(srcTupleOffset + totalSrcTuples > sourceArray->getNumberOfTuples())
  {
    return false;
  }

  if(totalSrcTuples * sourceArray->getNumberOfComponents() + destTupleOffset * getNumberOfComponents() > m_Array.size())
  {
    return false;
  }

  for(size_t i = srcTupleOffset; i < srcTupleOffset + totalSrcTuples; i++)
  {
    m_Array[destTupleOffset + i] = source->getList(i);
  }
  return true;

  //      if(!m_IsAllocated) { return false; }
  //      if(nullptr == m_Array) { return false; }
  //      if(destTupleOffset > m_MaxId) { return false; }
  //      if(!sourceArray->isAllocated()) { return false; }
  //      Self* source = dynamic_cast<Self*>(sourceArray.get());
  //      if(nullptr == source->getPointer(0)) { return false; }

  //      if(sourceArray->getNumberOfComponents() != getNumberOfComponents()) { return false; }

  //      if (srcTupleOffset + totalSrcTuples > sourceArray->getNumberOfTuples()) { return false; }

  //      if( totalSrcTuples*sourceArray->getNumberOfComponents() + destTupleOffset*getNumberOfComponents() > m_Size) { return false; }

  //      size_t elementStart = destTupleOffset*getNumberOfComponents();
  //      size_t totalBytes = (totalSrcTuples*sourceArray->getNumberOfComponents()) * sizeof(T);
  //      std::memcpy(m_Array + elementStart, source->getPointer(srcTupleOffset * sourceArray->getNumberOfComponents()), totalBytes);
  //      return true;
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::initializeTuple(size_t i, const void* p)
{
  Q_UNUSED(i);
  Q_UNUSED(p);
  Q_ASSERT(false);
}

// -----------------------------------------------------------------------------
template <typename T>
size_t NeighborList<T>::getNumberOfTuples() const
{
  return m_NumTuples;
}

// -----------------------------------------------------------------------------
template <typename T>
size_t NeighborList<T>::getSize() const
{
  size_t total = 0;
  for(size_t dIdx = 0; dIdx < m_Array.size(); ++dIdx)
  {
    total += m_Array[dIdx]->size();
  }
  return total;
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::setNumberOfComponents(int nc)
{
}

// -----------------------------------------------------------------------------
template <typename T>
int NeighborList<T>::getNumberOfComponents() const
{
  return 1;
}

// -----------------------------------------------------------------------------
template <typename T>
std::vector<size_t> NeighborList<T>::getComponentDimensions() const
{
  std::vector<size_t> dims = {1};
  return dims;
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::SetRank(int rnk)
{
}

// -----------------------------------------------------------------------------
template <typename T>
int NeighborList<T>::getRank() const
{
  return 1;
}

// -----------------------------------------------------------------------------
template <typename T>
size_t NeighborList<T>::getTypeSize() const
{
  return sizeof(SharedVectorType);
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::initializeWithZeros()
{
  m_Array.clear();
  m_IsAllocated = false;
}

// -----------------------------------------------------------------------------
template <typename T>
IDataArray::Pointer NeighborList<T>::deepCopy(bool forceNoAllocate) const
{
  bool allocate = m_IsAllocated;
  if(forceNoAllocate)
  {
    allocate = false;
  }

  typename NeighborList<T>::Pointer daCopyPtr = NeighborList<T>::CreateArray(getNumberOfTuples(), getName(), allocate);

  if(m_IsAllocated && !forceNoAllocate)
  {
    size_t count = (m_IsAllocated ? getNumberOfTuples() : 0);
    for(size_t i = 0; i < count; i++)
    {
      typename NeighborList<T>::SharedVectorType sharedNeiLst(new std::vector<T>(*(m_Array[i])));
      daCopyPtr->setList(static_cast<int>(i), sharedNeiLst);
    }
  }
  return daCopyPtr;
}

// -----------------------------------------------------------------------------
template <typename T>
int32_t NeighborList<T>::resizeTotalElements(size_t size)
{
  // std::cout << "NeighborList::resizeTotalElements(" << size << ")" << std::endl;
  size_t old = m_Array.size();
  m_Array.resize(size);
  m_NumTuples = size;
  if(size == 0)
  {
    m_IsAllocated = false;
  }
  else
  {
    m_IsAllocated = true;
  }
  // Initialize with zero length Vectors
  for(size_t i = old; i < m_Array.size(); ++i)
  {
    m_Array[i] = SharedVectorType(new VectorType);
  }
  return 1;
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::resizeTuples(size_t numTuples)
{
  resizeTotalElements(numTuples);
}

// FIXME: These need to be implemented
// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::printTuple(QTextStream& out, size_t i, char delimiter) const
{
  SharedVectorType sharedVec = m_Array[i];
  VectorType* vec = sharedVec.get();
  size_t size = vec->size();
  out << size;
  for(size_t j = 0; j < size; j++)
  {
    out << delimiter << vec->at(j);
  }
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::printComponent(QTextStream& out, size_t i, int j) const
{
  Q_ASSERT(false);
}

// -----------------------------------------------------------------------------
template <typename T>
int NeighborList<T>::writeH5Data(hid_t parentId, const std::vector<size_t>& tDims) const
{
  int err = 0;

  // Generate the NumNeighbors array and also compute the total number
  // of elements that would be needed to flatten the array so we
  // can compare this with what is written in the file. If they are
  // different we are going to overwrite what is in the file with what
  // we compute here.

  QString numNeighborsArrayName = m_NumNeighborsArrayName;

  if(numNeighborsArrayName.isEmpty())
  {
    numNeighborsArrayName = getName() + "_NumNeighbors";
  }

  Int32ArrayType::Pointer numNeighborsPtr = Int32ArrayType::CreateArray(m_Array.size(), numNeighborsArrayName, true);
  int32_t* numNeighbors = numNeighborsPtr->getPointer(0);
  size_t total = 0;
  for(size_t dIdx = 0; dIdx < m_Array.size(); ++dIdx)
  {
    numNeighbors[dIdx] = static_cast<int32_t>(m_Array[dIdx]->size());
    total += m_Array[dIdx]->size();
  }

  // Check to see if the NumNeighbors is already written to the file
  bool rewrite = false;
  if(!QH5Lite::datasetExists(parentId, numNeighborsArrayName))
  {
    // The NumNeighbors Array is NOT already in the file so write it to the file
    numNeighborsPtr->writeH5Data(parentId, tDims);
  }
  else
  {
    // The NumNeighbors array is in the dream3d file so read it up into memory and compare with what
    // we have in memory.
    std::vector<int32_t> fileNumNeigh(m_Array.size());
    err = H5Lite::readVectorDataset(parentId, numNeighborsArrayName.toStdString(), fileNumNeigh);
    if(err < 0)
    {
      return -602;
    }

    // Compare the 2 vectors to make sure they are exactly the same;
    if(fileNumNeigh.size() != numNeighborsPtr->getNumberOfTuples())
    {
      rewrite = true;
    }
    // The sizes are the same, now compare each value;
    int32_t* fileNumNeiPtr = &(fileNumNeigh.front());
    size_t nBytes = numNeighborsPtr->getNumberOfTuples() * sizeof(int32_t);
    if(::memcmp(numNeighbors, fileNumNeiPtr, nBytes) != 0)
    {
      rewrite = true;
    }
  }

  // Write out the NumNeighbors Array because something was different between what we computed at
  // the top of the function versus what is in memory
  if(rewrite)
  {
    numNeighborsPtr->writeH5Data(parentId, tDims);
  }

  // Allocate an array of the proper size so we can concatenate all the arrays together into a single array that
  // can be written to the HDF5 File. This operation can ballon the memory size temporarily until this operation
  // is complete.
  QVector<T> flat(total);
  size_t currentStart = 0;
  for(size_t dIdx = 0; dIdx < m_Array.size(); ++dIdx)
  {
    size_t nEle = m_Array[dIdx]->size();
    if(nEle == 0)
    {
      continue;
    }
    T* start = m_Array[dIdx]->data(); // get theNeighborList<T>::Pointer to the front of the array
    //    T* end = start + nEle; // get theNeighborList<T>::Pointer to the end of the array
    T* dst = flat.data() + currentStart;
    ::memcpy(dst, start, nEle * sizeof(T));

    currentStart += m_Array[dIdx]->size();
  }

  // Now we can actually write the actual array data.
  int32_t rank = 1;
  hsize_t dims[1] = {total};
  if(total > 0)
  {
    err = QH5Lite::writePointerDataset(parentId, getName(), rank, dims, flat.data());
    if(err < 0)
    {
      return -605;
    }

    err = QH5Lite::writeScalarAttribute(parentId, getName(), SIMPL::HDF5::DataArrayVersion, getClassVersion());
    if(err < 0)
    {
      return -604;
    }
    err = QH5Lite::writeStringAttribute(parentId, getName(), SIMPL::HDF5::ObjectType, getNameOfClass());
    if(err < 0)
    {
      return -607;
    }

    // Write the tuple dimensions as an attribute
    hsize_t size = tDims.size();
    err = QH5Lite::writePointerAttribute(parentId, getName(), SIMPL::HDF5::TupleDimensions, 1, &size, tDims.data());
    if(err < 0)
    {
      return -609;
    }

    std::vector<size_t> cDims = getComponentDimensions();
    // write the component dimensions as  an attribute
    size = cDims.size();
    err = QH5Lite::writePointerAttribute(parentId, getName(), SIMPL::HDF5::ComponentDimensions, 1, &size, cDims.data());
    if(err < 0)
    {
      return -610;
    }

    err = QH5Lite::writeStringAttribute(parentId, getName(), "Linked NumNeighbors Dataset", numNeighborsArrayName);
    if(err < 0)
    {
      return -608;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
template <typename T>
int NeighborList<T>::writeXdmfAttribute(QTextStream& out, const int64_t* volDims, const QString& hdfFileName, const QString& groupPath, const QString& label) const
{
  int precision = 0;
  QString xdmfTypeName;
  getXdmfTypeAndSize(xdmfTypeName, precision);

  out << "    <Attribute Name=\"" << getName() << label << "\" AttributeType=\"Scalar\" Center=\"Node\">";
  out << "      <DataItem Format=\"HDF\" Dimensions=\"" << volDims[0] << " " << volDims[1] << " " << volDims[2] << "\" ";
  out << "NumberType=\"" << xdmfTypeName << "\" "
      << "Precision=\"" << precision << "\" >";
  out << "        " << hdfFileName.toLatin1().data() << groupPath.toLatin1().data() << "/" << getName();
  out << "      </DataItem>";
  out << "    </Attribute>";
  return 1;
}

// -----------------------------------------------------------------------------
template <typename T>
ToolTipGenerator NeighborList<T>::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;
  QLocale usa(QLocale::English, QLocale::UnitedStates);

  toolTipGen.addTitle("Attribute Array Info");
  toolTipGen.addValue("Name", getName());
  toolTipGen.addValue("Type", getTypeAsString());
  toolTipGen.addValue("Number of Tuples", usa.toString(static_cast<qlonglong>(getNumberOfTuples())));
  toolTipGen.addValue("Number of Lists", usa.toString(static_cast<qlonglong>(getNumberOfLists())));

  return toolTipGen;
}

// -----------------------------------------------------------------------------
template <typename T>
QString NeighborList<T>::getInfoString(SIMPL::InfoStringFormat format) const
{
  QLocale usa(QLocale::English, QLocale::UnitedStates);

  QString info;
  QTextStream ss(&info);

  if(format == SIMPL::HtmlFormat)
  {
    ss << "<html><head></head>\n";
    ss << "<body>\n";
    ss << "<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\">\n";
    ss << "<tbody>\n";
    ss << "<tr bgcolor=\"#FFFCEA\"><th colspan=2>Attribute Array Info</th></tr>";

    ss << R"(<tr bgcolor="#E9E7D6"><th align="right">Name:</th><td>)" << getName() << "</td></tr>";

    ss << R"(<tr bgcolor="#FFFCEA"><th align="right">Type:</th><td> NeighborList&lt;)" << getTypeAsString() << "&gt;</td></tr>";
    QString numStr = usa.toString(static_cast<qlonglong>(getNumberOfTuples()));
    ss << R"(<tr bgcolor="#FFFCEA"><th align="right">Number of Tuples:</th><td>)" << numStr << "</td></tr>";

    ss << "</tbody></table>\n";
    ss << "</body></html>";
  }
  else if(format == SIMPL::MarkDown)
  {
    ss << "+ Name: " << getName() << "\n";
    ss << "+ Type: " << getTypeAsString() << "\n";
    ss << "+ Num. Tuple: " << getNumberOfTuples() << "\n";
    QString compDimStr = "(variable)";

    ss << "+ Comp. Dims: " << compDimStr << "\n";
    ss << "+ Total Elements:  " << m_Array.size() << "\n";
    ss << "+ Minimum Memory: " << (m_Array.size() * sizeof(T)) << "\n";
  }
  return info;
}

// -----------------------------------------------------------------------------
template <typename T>
int NeighborList<T>::readH5Data(hid_t parentId)
{
  int err = 0;

  std::vector<T> flat;
  err = QH5Lite::readVectorDataset(parentId, getName(), flat);
  if(err < 0)
  {
    return err;
  }

  QString numNeighborsArrayName = m_NumNeighborsArrayName;

  if(numNeighborsArrayName.isEmpty())
  {
    numNeighborsArrayName = getName() + "_NumNeighbors";
  }

  // Read the Attribute Off the data set to find the name of the array that holds all the sizes
  err = QH5Lite::readStringAttribute(parentId, getName(), "Linked NumNeighbors Dataset", numNeighborsArrayName);
  if(err < 0)
  {
    return err;
  }
  // Generate the number of neighbors array and also compute the total number
  // of elements that would be needed to flatten the array
  std::vector<int32_t> numNeighbors;

  // Check to see if the NumNeighbors exists in the file, which it must.
  if(QH5Lite::datasetExists(parentId, numNeighborsArrayName))
  {
    err = QH5Lite::readVectorDataset(parentId, numNeighborsArrayName, numNeighbors);
    if(err < 0)
    {
      return -702;
    }
  }
  else
  {
    return -703;
  }

  // int32_t totalElements = std::accumulate(numNeighbors.begin(), numNeighbors.end(), 0);

  // Loop over all the entries and make new Vectors to hold the incoming data
  m_Array.resize(numNeighbors.size());
  m_IsAllocated = true;
  size_t currentStart = 0;
  qint32 count = static_cast<qint32>(numNeighbors.size());
  for(QVector<int32_t>::size_type dIdx = 0; dIdx < count; ++dIdx)
  {
    size_t nEle = numNeighbors[dIdx];
    if(nEle > 0)
    {
      m_Array[dIdx] = SharedVectorType(new VectorType(numNeighbors[dIdx]));

      T* dst = m_Array[dIdx]->data(); // get theNeighborList<T>::Pointer to the front of the array
      //    T* end = start + nEle; // get theNeighborList<T>::Pointer to the end of the array
      T* start = flat.data() + currentStart;
      ::memcpy(dst, start, nEle * sizeof(T));
      currentStart += nEle;
    }
    else
    {
      m_Array[dIdx] = SharedVectorType(new VectorType(0));
    }
  }
  m_NumTuples = m_Array.size(); // Sync up the numTuples property with the size of the internal array
  return err;
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::addEntry(int grainId, T value)
{
  if(grainId >= static_cast<int>(m_Array.size()))
  {
    size_t old = m_Array.size();
    m_Array.resize(grainId + 1);
    m_IsAllocated = true;
    // Initialize with zero length Vectors
    for(size_t i = old; i < m_Array.size(); ++i)
    {
      m_Array[i] = SharedVectorType(new VectorType);
    }
  }
  m_Array[grainId]->push_back(value);
  m_NumTuples = m_Array.size();
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::clearAllLists()
{
  m_Array.clear();
  m_IsAllocated = false;
}

// -----------------------------------------------------------------------------
template <typename T>
void NeighborList<T>::setList(int grainId, SharedVectorType neighborList)
{
  if(grainId >= static_cast<int>(m_Array.size()))
  {
    size_t old = m_Array.size();
    m_Array.resize(grainId + 1);
    m_IsAllocated = true;
    // Initialize with zero length Vectors
    for(size_t i = old; i < m_Array.size(); ++i)
    {
      m_Array[i] = SharedVectorType(new VectorType);
    }
  }
  m_Array[grainId] = neighborList;
}

// -----------------------------------------------------------------------------
template <typename T>
T NeighborList<T>::getValue(int grainId, int index, bool& ok) const
{
#ifndef NDEBUG
  if(m_Array.size() > 0u)
  {
    Q_ASSERT(grainId < static_cast<int>(m_Array.size()));
  }
#endif
  SharedVectorType vec = m_Array[grainId];
  if(index < 0 || static_cast<size_t>(index) >= vec->size())
  {
    ok = false;
    return -1;
  }
  return (*vec)[index];
}

// -----------------------------------------------------------------------------
template <typename T>
int NeighborList<T>::getNumberOfLists() const
{
  return static_cast<int>(m_Array.size());
}

// -----------------------------------------------------------------------------
template <typename T>
int NeighborList<T>::getListSize(int grainId) const
{
#ifndef NDEBUG
  if(m_Array.size() > 0u)
  {
    Q_ASSERT(grainId < static_cast<int>(m_Array.size()));
  }
#endif
  return static_cast<int>(m_Array[grainId]->size());
}

// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::VectorType& NeighborList<T>::getListReference(int grainId) const
{
#ifndef NDEBUG
  if(m_Array.size() > 0u)
  {
    Q_ASSERT(grainId < static_cast<int>(m_Array.size()));
  }
#endif
  return *(m_Array[grainId]);
}

// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::SharedVectorType NeighborList<T>::getList(int grainId) const
{
#ifndef NDEBUG
  if(m_Array.size() > 0u)
  {
    Q_ASSERT(grainId < static_cast<int>(m_Array.size()));
  }
#endif
  return m_Array[grainId];
}

// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::VectorType NeighborList<T>::copyOfList(int grainId) const
{
#ifndef NDEBUG
  if(m_Array.size() > 0u)
  {
    Q_ASSERT(grainId < static_cast<int>(m_Array.size()));
  }
#endif

  VectorType copy(*(m_Array[grainId]));
  return copy;
}

// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::VectorType& NeighborList<T>::operator[](int grainId)
{
#ifndef NDEBUG
  if(m_Array.size() > 0u)
  {
    Q_ASSERT(grainId < static_cast<int>(m_Array.size()));
  }
#endif
  return *(m_Array[grainId]);
}

// -----------------------------------------------------------------------------
template <typename T>
typename NeighborList<T>::VectorType& NeighborList<T>::operator[](size_t grainId)
{
#ifndef NDEBUG
  if(m_Array.size() > 0ul)
  {
    Q_ASSERT(grainId < m_Array.size());
  }
#endif
  return *(m_Array[grainId]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#if !defined(__APPLE__) && !defined(_MSC_VER)
#undef SIMPLib_EXPORT
#define SIMPLib_EXPORT
#endif

template class SIMPLib_EXPORT NeighborList<char>;

template class SIMPLib_EXPORT NeighborList<int8_t>;
template class SIMPLib_EXPORT NeighborList<uint8_t>;

template class SIMPLib_EXPORT NeighborList<int16_t>;
template class SIMPLib_EXPORT NeighborList<uint16_t>;

template class SIMPLib_EXPORT NeighborList<int32_t>;
template class SIMPLib_EXPORT NeighborList<uint32_t>;

template class SIMPLib_EXPORT NeighborList<int64_t>;
template class SIMPLib_EXPORT NeighborList<uint64_t>;

template class SIMPLib_EXPORT NeighborList<float>;
template class SIMPLib_EXPORT NeighborList<double>;

#if defined(__APPLE__) || defined(_MSC_VER)
template class SIMPLib_EXPORT NeighborList<size_t>;
#endif

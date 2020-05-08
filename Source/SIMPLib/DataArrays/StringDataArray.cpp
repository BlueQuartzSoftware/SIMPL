/* ============================================================================
 * Copyright (c) 2009-2018 BlueQuartz Software, LLC
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
#include "StringDataArray.h"

#include <QtCore/QTextStream>

#include "SIMPLib/HDF5/H5DataArrayReader.h"
#include "SIMPLib/HDF5/H5DataArrayWriter.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StringDataArray::StringDataArray()
: _ownsData(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StringDataArray::StringDataArray(size_t numTuples, const QString& name, bool allocate)
: IDataArray(name)
, _ownsData(true)
{
  m_Array.resize(numTuples);
  setName(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StringDataArray::Pointer StringDataArray::CreateArray(size_t numTuples, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  StringDataArray* d = new StringDataArray(numTuples, name, allocate);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StringDataArray::Pointer StringDataArray::CreateArray(size_t numTuples, const std::vector<size_t>& compDims, const QString& name, bool allocate)
{
  if(name.isEmpty())
  {
    return NullPointer();
  }
  StringDataArray* d = new StringDataArray(numTuples, name, allocate);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer StringDataArray::createNewArray(size_t numElements, int rank, const size_t* dims, const QString& name, bool allocate) const
{
  IDataArray::Pointer p = StringDataArray::CreateArray(numElements, name, allocate);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer StringDataArray::createNewArray(size_t numElements, const std::vector<size_t>& dims, const QString& name, bool allocate) const
{
  IDataArray::Pointer p = StringDataArray::CreateArray(numElements, name, allocate);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// IDataArray::Pointer StringDataArray::createNewArray(size_t numElements, std::vector<size_t> dims, const QString& name, bool allocate)
//{
//  IDataArray::Pointer p = StringDataArray::CreateArray(numElements, name,
// allocate);
//  return p;
//}

StringDataArray::~StringDataArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StringDataArray::isAllocated() const
{
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::setInitValue(const std::string& initValue)
{
  m_InitValue = QString::fromStdString(initValue);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::setInitValue(const QString& initValue)
{
  m_InitValue = initValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::getXdmfTypeAndSize(QString& xdmfTypeName, int& precision) const
{
  xdmfTypeName = getNameOfClass();
  precision = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StringDataArray::getTypeAsString() const
{
  return "StringDataArray";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::takeOwnership()
{
  this->_ownsData = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::releaseOwnership()
{
  this->_ownsData = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* StringDataArray::getVoidPointer(size_t i)
{
  return static_cast<void*>(&(m_Array[i]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t StringDataArray::getNumberOfTuples() const
{
  return m_Array.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t StringDataArray::getSize() const
{
  return m_Array.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StringDataArray::getNumberOfComponents() const
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<size_t> StringDataArray::getComponentDimensions() const
{
  std::vector<size_t> dims = {1};
  return dims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::SetRank(int rnk)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StringDataArray::getRank() const
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t StringDataArray::getTypeSize() const
{
  return sizeof(QString);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StringDataArray::eraseTuples(const std::vector<size_t>& idxs)
{

  int err = 0;

  // If nothing is to be erased just return
  if(idxs.empty())
  {
    return 0;
  }
  size_t idxs_size = static_cast<size_t>(idxs.size());
  if(idxs_size >= getNumberOfTuples())
  {
    resizeTuples(0);
    return 0;
  }

  // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
  // off the end of the array and return an error code.
  // for(std::vector<size_t>::size_type i = 0; i < idxs.size(); ++i)
  for(auto& value : idxs)
  {
    if(value >= static_cast<size_t>(m_Array.size()))
    {
      return -100;
    }
  }

  // Create a new Array to copy into
  std::vector<QString> newArray;
  std::vector<size_t>::size_type start = 0;
  for(QVector<QString>::size_type i = 0; i < m_Array.size(); ++i)
  {
    bool keep = true;
    for(std::vector<size_t>::size_type j = start; j < idxs.size(); ++j)
    {
      if(static_cast<size_t>(i) == idxs[j])
      {
        keep = false;
        break;
      }
    }
    if(keep)
    {
      newArray.push_back(m_Array[i]);
    }
  }
  m_Array = newArray;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StringDataArray::copyTuple(size_t currentPos, size_t newPos)
{
  if(currentPos >= m_Array.size())
  {
    return -1;
  }
  if(newPos >= m_Array.size())
  {
    return -1;
  }
  // QString s = m_Array[currentPos];
  m_Array[newPos] = m_Array[currentPos];
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StringDataArray::copyFromArray(size_t destTupleOffset, IDataArray::ConstPointer sourceArray, size_t srcTupleOffset, size_t totalSrcTuples)
{
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

  if(srcTupleOffset + totalSrcTuples > sourceArray->getNumberOfTuples())
  {
    return false;
  }
  if(totalSrcTuples + destTupleOffset > m_Array.size())
  {
    return false;
  }

  for(size_t i = srcTupleOffset; i < srcTupleOffset + totalSrcTuples; i++)
  {
    m_Array[destTupleOffset + i] = source->getValue(i);
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::initializeTuple(size_t pos, const void* value)
{
  m_Array[pos] = *(reinterpret_cast<const QString*>(value));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::initializeWithZeros()
{
  m_Array.assign(m_Array.size(), QString(""));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::initializeWithValue(const QString& value)
{
  m_Array.assign(m_Array.size(), value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::initializeWithValue(const std::string& value)
{
  m_Array.assign(m_Array.size(), QString::fromStdString(value));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer StringDataArray::deepCopy(bool forceNoAllocate) const
{
  StringDataArray::Pointer daCopy = StringDataArray::CreateArray(getNumberOfTuples(), getName(), true);
  if(!forceNoAllocate)
  {
    for(std::vector<QString>::size_type i = 0; i < m_Array.size(); ++i)
    {
      daCopy->setValue(i, m_Array[i]);
    }
  }
  return daCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t StringDataArray::resizeTotalElements(size_t size)
{
  m_Array.resize(size);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::resizeTuples(size_t numTuples)
{
  m_Array.resize(numTuples);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::initialize()
{
  if(!m_Array.empty())
  {
    m_Array.clear();
    this->_ownsData = true;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::printTuple(QTextStream& out, size_t i, char delimiter) const
{
  out << m_Array[i];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::printComponent(QTextStream& out, size_t i, int j) const
{
  out << m_Array[i];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StringDataArray::getFullNameOfClass() const
{
  return "StringDataArray";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StringDataArray::writeH5Data(hid_t parentId, const std::vector<size_t>& tDims) const
{
  return H5DataArrayWriter::writeStringDataArray<StringDataArray>(parentId, this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StringDataArray::writeXdmfAttribute(QTextStream& out, const int64_t* volDims, const QString& hdfFileName, const QString& groupPath, const QString& labelb) const
{
  out << "<!-- Xdmf is not supported for " << getNameOfClass() << " with type " << getTypeAsString() << " --> ";
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StringDataArray::getInfoString(SIMPL::InfoStringFormat format) const
{
  if(format == SIMPL::HtmlFormat)
  {
    return getToolTipGenerator().generateHTML();
  }
  else
  {
  }
  return QString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ToolTipGenerator StringDataArray::getToolTipGenerator() const
{
  ToolTipGenerator toolTipGen;
  QLocale usa(QLocale::English, QLocale::UnitedStates);

  toolTipGen.addTitle("Attribute Array Info");
  toolTipGen.addValue("Name", getName());
  toolTipGen.addValue("Type", getTypeAsString());
  toolTipGen.addValue("Number of Tuples", usa.toString(static_cast<qlonglong>(getNumberOfTuples())));

  return toolTipGen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StringDataArray::readH5Data(hid_t parentId)
{
  int err = 0;
  this->resizeTuples(0);
  std::vector<std::string> strings;
  err = H5Lite::readVectorOfStringDataset(parentId, getName().toStdString(), strings);

  m_Array.resize(strings.size());
  for(std::vector<QString>::size_type i = 0; i < strings.size(); i++)
  {
    m_Array[i] = QString::fromStdString(strings[i]);
  }
#if 0
  IDataArray::Pointer p = H5DataArrayReader::ReadStringDataArray(parentId, getName());
  if (p.get() == nullptr)
  {
    return -1;
  }
  StringDataArray::Pointer srcPtr = std::dynamic_pointer_cast<StringDataArray>(p);
  size_t count = srcPtr->getNumberOfTuples();
  for (size_t i = 0; i < count; ++i)
  {
    m_Array.push_back( srcPtr->getValue(i) );
  }
#endif
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StringDataArray::setValue(size_t i, const QString& value)
{
  m_Array[i] = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StringDataArray::getValue(size_t i) const
{
  return m_Array.at(i);
}

// -----------------------------------------------------------------------------
StringDataArray::Pointer StringDataArray::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
StringDataArray::Pointer StringDataArray::New()
{
  Pointer sharedPtr(new(StringDataArray));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString StringDataArray::getNameOfClass() const
{
  return QString("StringDataArray");
}

// -----------------------------------------------------------------------------
QString StringDataArray::ClassName()
{
  return QString("StringDataArray");
}

// -----------------------------------------------------------------------------
int StringDataArray::getClassVersion() const
{
  return 2;
}

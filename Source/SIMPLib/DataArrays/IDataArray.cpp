///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, mjackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////

#include "IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::IDataArray(const QString& name)
: IDataStructureNode(name)
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::~IDataArray() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool IDataArray::copyFromArray(size_t destTupleOffset, IDataArray::Pointer sourceArray)
{
  return copyFromArray(destTupleOffset, sourceArray, 0, sourceArray->getNumberOfTuples());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath IDataArray::getDataArrayPath() const
{
  DataArrayPath path = getParentPath();
  path.setDataArrayName(getName());
  return path;
}

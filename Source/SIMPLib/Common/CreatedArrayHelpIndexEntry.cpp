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

#include "CreatedArrayHelpIndexEntry.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreatedArrayHelpIndexEntry::CreatedArrayHelpIndexEntry()
: m_ArrayNumComponents(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreatedArrayHelpIndexEntry::~CreatedArrayHelpIndexEntry() = default;

// -----------------------------------------------------------------------------
CreatedArrayHelpIndexEntry::Pointer CreatedArrayHelpIndexEntry::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
CreatedArrayHelpIndexEntry::Pointer CreatedArrayHelpIndexEntry::New()
{
  Pointer sharedPtr(new(CreatedArrayHelpIndexEntry));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString CreatedArrayHelpIndexEntry::getNameOfClass() const
{
  return QString("CreatedArrayHelpIndexEntry");
}

// -----------------------------------------------------------------------------
QString CreatedArrayHelpIndexEntry::ClassName()
{
  return QString("CreatedArrayHelpIndexEntry");
}

// -----------------------------------------------------------------------------
void CreatedArrayHelpIndexEntry::setFilterName(const QString& value)
{
  m_FilterName = value;
}

// -----------------------------------------------------------------------------
QString CreatedArrayHelpIndexEntry::getFilterName() const
{
  return m_FilterName;
}

// -----------------------------------------------------------------------------
void CreatedArrayHelpIndexEntry::setFilterHumanLabel(const QString& value)
{
  m_FilterHumanLabel = value;
}

// -----------------------------------------------------------------------------
QString CreatedArrayHelpIndexEntry::getFilterHumanLabel() const
{
  return m_FilterHumanLabel;
}

// -----------------------------------------------------------------------------
void CreatedArrayHelpIndexEntry::setFilterGroup(const QString& value)
{
  m_FilterGroup = value;
}

// -----------------------------------------------------------------------------
QString CreatedArrayHelpIndexEntry::getFilterGroup() const
{
  return m_FilterGroup;
}

// -----------------------------------------------------------------------------
void CreatedArrayHelpIndexEntry::setFilterSubGroup(const QString& value)
{
  m_FilterSubGroup = value;
}

// -----------------------------------------------------------------------------
QString CreatedArrayHelpIndexEntry::getFilterSubGroup() const
{
  return m_FilterSubGroup;
}

// -----------------------------------------------------------------------------
void CreatedArrayHelpIndexEntry::setArrayDefaultName(const QString& value)
{
  m_ArrayDefaultName = value;
}

// -----------------------------------------------------------------------------
QString CreatedArrayHelpIndexEntry::getArrayDefaultName() const
{
  return m_ArrayDefaultName;
}

// -----------------------------------------------------------------------------
void CreatedArrayHelpIndexEntry::setArrayGroup(const QString& value)
{
  m_ArrayGroup = value;
}

// -----------------------------------------------------------------------------
QString CreatedArrayHelpIndexEntry::getArrayGroup() const
{
  return m_ArrayGroup;
}

// -----------------------------------------------------------------------------
void CreatedArrayHelpIndexEntry::setArrayType(const QString& value)
{
  m_ArrayType = value;
}

// -----------------------------------------------------------------------------
QString CreatedArrayHelpIndexEntry::getArrayType() const
{
  return m_ArrayType;
}

// -----------------------------------------------------------------------------
void CreatedArrayHelpIndexEntry::setArrayNumComponents(int value)
{
  m_ArrayNumComponents = value;
}

// -----------------------------------------------------------------------------
int CreatedArrayHelpIndexEntry::getArrayNumComponents() const
{
  return m_ArrayNumComponents;
}

// -----------------------------------------------------------------------------
void CreatedArrayHelpIndexEntry::setArrayDescription(const QString& value)
{
  m_ArrayDescription = value;
}

// -----------------------------------------------------------------------------
QString CreatedArrayHelpIndexEntry::getArrayDescription() const
{
  return m_ArrayDescription;
}

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

#include "AppVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AppVersion::AppVersion() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AppVersion::AppVersion(int majorNum, int minorNum, int patchNum)
{
  m_MajorNum = majorNum;
  m_MinorNum = minorNum;
  m_PatchNum = patchNum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AppVersion::~AppVersion() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AppVersion::AppVersion(const AppVersion& rhs)
{
  m_MajorNum = rhs.m_MajorNum;
  m_MinorNum = rhs.m_MinorNum;
  m_PatchNum = rhs.m_PatchNum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AppVersion& AppVersion::operator=(const AppVersion& rhs) = default;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AppVersion::operator==(const AppVersion& rhs)
{
  return (m_MajorNum == rhs.m_MajorNum && m_MinorNum == rhs.m_MinorNum && m_PatchNum == rhs.m_PatchNum);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AppVersion::operator>(const AppVersion& rhs)
{
  if(m_MajorNum > rhs.m_MajorNum)
  {
    return true;
  }
  
  if(m_MajorNum == rhs.m_MajorNum)
  {
    if(m_MinorNum > rhs.m_MinorNum)
    {
      return true;
    }
    
    if(m_MinorNum == rhs.m_MinorNum)
    {
      return m_PatchNum > rhs.m_PatchNum;
    }
    return false;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AppVersion::operator<(const AppVersion& rhs)
{
  if(m_MajorNum < rhs.m_MajorNum)
  {
    return true;
  }
  
  if(m_MajorNum == rhs.m_MajorNum)
  {
    if(m_MinorNum < rhs.m_MinorNum)
    {
      return true;
    }
    
    if(m_MinorNum == rhs.m_MinorNum)
    {
      return m_PatchNum < rhs.m_PatchNum;
    }
    return false;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AppVersion::getMajorNum()
{
  return m_MajorNum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AppVersion::getMinorNum()
{
  return m_MinorNum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AppVersion::getPatchNum()
{
  return m_PatchNum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppVersion::setMajorNum(int major)
{
  m_MajorNum = major;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppVersion::setMinorNum(int minor)
{
  m_MinorNum = minor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppVersion::setPatchNum(int patch)
{
  m_PatchNum = patch;
}

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

#ifndef _changeglobalvalue_h_
#define _changeglobalvalue_h_

#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

class SIMPLib_EXPORT ChangeGlobalValue : public AbstractFilter
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(ChangeGlobalValue)
  SIMPL_STATIC_NEW_MACRO(ChangeGlobalValue)
  SIMPL_TYPE_MACRO_SUPER(ChangeGlobalValue, AbstractFilter)

  virtual ~ChangeGlobalValue();

  SIMPL_FILTER_PARAMETER(int, Value)
  Q_PROPERTY(int Value READ getValue WRITE setValue)

  SIMPL_FILTER_PARAMETER(int*, GlobalValue)
  Q_PROPERTY(int* GlobalValue READ getGlobalValue WRITE setGlobalValue)

  /**
  * @brief newFilterInstance Reimplemented from @see AbstractFilter class
  */
  virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

  /**
  * @brief execute Reimplemented from @see AbstractFilter class
  */
  virtual void execute();

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  virtual void preflight();

protected:
  ChangeGlobalValue();

private:
  ChangeGlobalValue(const ChangeGlobalValue&); // Copy Constructor Not Implemented
  void operator=(const ChangeGlobalValue&);    // Operator '=' Not Implemented
};

#endif /* _changeglobalvalue_h_ */
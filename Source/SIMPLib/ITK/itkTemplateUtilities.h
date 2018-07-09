/* ============================================================================
 * Copyright (c) 2014 William Lenthe
 * Copyright (c) 2014 DREAM3D Consortium
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
 * Neither the name of William Lenthe or any of the DREAM3D Consortium contributors
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
 *  This code was partially written under United States Air Force Contract number
 *                              FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#if 0

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

namespace TemplateConstants
{

  enum Types
  {
    UnknownType = 0,
    Bool,
    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float,
    Double
  };

}


class TemplateUtilities
{
  public:
    TemplateUtilities() {}
    virtual ~TemplateUtilities();

    static QString getTypeNameFromType(int type)
    {
      if(TemplateConstants::Bool == type)
      {
        return SIMPL::TypeNames::Bool;
      }
      else if(TemplateConstants::Int8 == type)
      {
        return SIMPL::TypeNames::Int8;
      }
      else if(TemplateConstants::UInt8 == type)
      {
        return SIMPL::TypeNames::UInt8;
      }
      else if(TemplateConstants::Int16 == type)
      {
        return SIMPL::TypeNames::Int16;
      }
      else if(TemplateConstants::UInt16 == type)
      {
        return SIMPL::TypeNames::UInt16;
      }
      else if(TemplateConstants::Int32 == type)
      {
        return SIMPL::TypeNames::Int32;
      }
      else if(TemplateConstants::UInt32 == type)
      {
        return SIMPL::TypeNames::UInt32;
      }
      else if(TemplateConstants::Int64 == type)
      {
        return SIMPL::TypeNames::Int64;
      }
      else if(TemplateConstants::UInt64 == type)
      {
        return SIMPL::TypeNames::UInt64;
      }
      else if(TemplateConstants::Float == type)
      {
        return SIMPL::TypeNames::Float;
      }
      else if(TemplateConstants::Double == type)
      {
        return SIMPL::TypeNames::Double;
      }
      else
      {
        return SIMPL::Defaults::Any;
      }
    }

    static int getTypeFromTypeName(QString type)
    {
      if(0 == type.compare(SIMPL::TypeNames::Bool))
      {
        return TemplateConstants::Bool;
      }
      else if(0 == type.compare(SIMPL::TypeNames::Int8))
      {
        return TemplateConstants::Int8;
      }
      else if(0 == type.compare(SIMPL::TypeNames::UInt8))
      {
        return TemplateConstants::UInt8;
      }
      else if(0 == type.compare(SIMPL::TypeNames::Int16))
      {
        return TemplateConstants::Int16;
      }
      else if(0 == type.compare(SIMPL::TypeNames::UInt16))
      {
        return TemplateConstants::UInt16;
      }
      else if(0 == type.compare(SIMPL::TypeNames::Int32))
      {
        return TemplateConstants::Int32;
      }
      else if(0 == type.compare(SIMPL::TypeNames::UInt32))
      {
        return TemplateConstants::UInt32;
      }
      else if(0 == type.compare(SIMPL::TypeNames::Int64))
      {
        return TemplateConstants::Int64;
      }
      else if(0 == type.compare(SIMPL::TypeNames::UInt64))
      {
        return TemplateConstants::UInt64;
      }
      else if(0 == type.compare(SIMPL::TypeNames::Float))
      {
        return TemplateConstants::Float;
      }
      else if(0 == type.compare(SIMPL::TypeNames::Double))
      {
        return TemplateConstants::Double;
      }
      else
      {
        return TemplateConstants::UnknownType;
      }
    }

  private:
    TemplateUtilities(const TemplateUtilities&) = delete; // Copy Constructor Not Implemented
    void operator=(const TemplateUtilities&) = delete; // Move assignment Not Implemented
};

#endif


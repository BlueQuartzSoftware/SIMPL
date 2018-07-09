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


#pragma once

#include <functional>


#include <QtCore/QVector>
#include <QtCore/QVariant>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"

class AbstractFilter;

/**
 * @class FilterParameter FilterParameter.h DREAM3DLib/FilterParameters/FilterParameter.h
 * @brief This class holds the various properties that an input parameter to a
 * filter needs to describe itself.
 *
 * @date Jan 17, 2012
 * @version 1.0
 */
class SIMPLib_EXPORT FilterParameter
{
  public:
    SIMPL_SHARED_POINTERS(FilterParameter)
    SIMPL_TYPE_MACRO(FilterParameter)

    enum Category
    {
      Parameter = 0,
      RequiredArray = 1,
      CreatedArray = 2,
      Uncategorized = 3
    };

    virtual ~FilterParameter();

    SIMPL_INSTANCE_STRING_PROPERTY(HumanLabel)
    SIMPL_INSTANCE_STRING_PROPERTY(PropertyName)

    /**
     * @brief getWidgetType This is a pure virtual function. All subclasses need
     * to implement this function.
     * @return
     */
    virtual QString getWidgetType() const = 0;

    SIMPL_VIRTUAL_INSTANCE_PROPERTY(QVariant, DefaultValue)
    SIMPL_INSTANCE_PROPERTY(Category, Category)
    SIMPL_INSTANCE_PROPERTY(bool, ReadOnly)
    SIMPL_INSTANCE_PROPERTY(int, GroupIndex)

    /**
     * @brief readJson
     * @return
     */
    virtual void readJson(const QJsonObject &json);

    /**
     * @brief writeJson
     * @return
     */
    virtual void writeJson(QJsonObject &json);

    /**
     * @brief Handle DataArrayPath changes if necessary
     * @param filter
     * @param renamePath
     */
    virtual void dataArrayPathRenamed(AbstractFilter* filter, DataArrayPath::RenameType renamePath);

  protected:
    FilterParameter();

  private:
    FilterParameter(const FilterParameter&) = delete; // Copy Constructor Not Implemented
    void operator=(const FilterParameter&) = delete;  // Move assignment Not Implemented
};

typedef QVector<FilterParameter::Pointer> FilterParameterVector;

// -----------------------------------------------------------------------------
// This section of Macros allows each FilterParameter subclass to create a macro
// or set of macros that can lessen the amout of code that needs to be written
// in order to create an instantiation of the subclass. The technique used here
// is the 'paired, sliding list' of macro parameters that also makes use of
// __VA__ARGS__
// -----------------------------------------------------------------------------

#define SIMPL_BIND_SETTER(CLASS, PTR, PROP)\
std::bind(&CLASS::set##PROP, PTR, std::placeholders::_1)

#define SIMPL_BIND_GETTER(CLASS, PTR, PROP)\
std::bind(&CLASS::get##PROP, PTR)

// Define overrides that can be used by the expansion of our main macro.
// Each subclass can define a macro that takes up to nine (9) arguments
// to the constructor. These macros support a minimum of 4 arguments.

#define SIMPL_NEW_FP_9(Class, Desc, Prop, Category, Filter, Index, A, B, C, D)\
  Class::New(Desc, #Prop, get##Prop(), Category,\
  SIMPL_BIND_SETTER(Filter, this, Prop),\
  SIMPL_BIND_GETTER(Filter, this, Prop),\
  Index, A, B, C, D)

#define SIMPL_NEW_FP_8(Class, Desc, Prop, Category, Filter, Index, A, B, C)\
  Class::New(Desc, #Prop, get##Prop(), Category,\
  SIMPL_BIND_SETTER(Filter, this, Prop),\
  SIMPL_BIND_GETTER(Filter, this, Prop),\
  Index, A, B, C)

#define SIMPL_NEW_FP_7(Class, Desc, Prop, Category, Filter, Index, A, B)\
  Class::New(Desc, #Prop, get##Prop(), Category,\
  SIMPL_BIND_SETTER(Filter, this, Prop),\
  SIMPL_BIND_GETTER(Filter, this, Prop),\
  Index, A, B)

#define SIMPL_NEW_FP_6(Class, Desc, Prop, Category, Filter, Index, A)\
  Class::New(Desc, #Prop, get##Prop(), Category,\
  SIMPL_BIND_SETTER(Filter, this, Prop),\
  SIMPL_BIND_GETTER(Filter, this, Prop),\
  Index, A)

#define SIMPL_NEW_FP_5(Class, Desc, Prop, Category, Filter, Index)\
  Class::New(Desc, #Prop, get##Prop(), Category,\
  SIMPL_BIND_SETTER(Filter, this, Prop),\
  SIMPL_BIND_GETTER(Filter, this, Prop),\
  Index)

#define SIMPL_NEW_FP_4(Class, Desc, Prop, Category, Filter)\
  Class::New(Desc, #Prop, get##Prop(), Category,\
  SIMPL_BIND_SETTER(Filter, this, Prop),\
  SIMPL_BIND_GETTER(Filter, this, Prop))

#define SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP_5(Class, Desc, Prop, Category, Filter, Index)\
  Class::New(Desc, #Prop, get##Prop(), Category,\
  SIMPL_BIND_GETTER(Filter, this, Prop),\
  Index)

#define SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP_4(Class, Desc, Prop, Category, Filter)\
  Class::New(Desc, #Prop, get##Prop(), Category,\
  SIMPL_BIND_GETTER(Filter, this, Prop))


/**
 * @brief This macro is needed for Visual Studio due to differences of VAR_ARGS when
 * passed to another macro that results in a new macro that needs expansion.
 */
#define SIMPL_EXPAND( x ) x

// -----------------------------------------------------------------------------
// Define a macro that uses the "paired, sliding arg list"
// technique to select the appropriate override.
#define _FP_GET_OVERRIDE(A, B, C, D, E, F, G, H, I, NAME, ...) NAME
#define _FP_GET_PREFLIGHTUPDATEDVALUE_OVERRIDE(A, B, C, D, E, NAME, ...) NAME



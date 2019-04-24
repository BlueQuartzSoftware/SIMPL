#pragma once

#include <QtCore/QString>

#define NEWLINE_SIMPL '\n'
#define TAB "  "

#define OVERWRITE_SOURCE_FILE 1

/* These are for replacements in the template files */
#define CLASS_NAME "@CLASS_NAME@"
#define HEADER_PATH "@HEADER_PATH@"
#define SUPERCLASS_NAME "@SUPERCLASS_NAME@"
#define LIB_NAME "@LIB_NAME@"
#define FULL_LIB_NAME "@FULL_LIB_NAME@"
#define MODULE_INIT_CODE "@MODULE_INIT_CODE@"
#define DATE_TIME_GENERATED "@DATE_TIME_GENERATED@"

namespace
{
const QString kRead("READ");
const QString kWrite("WRITE");
const QString kArgs("ARGS");
const QString kConst("CONST");
const QString kConstMethod("CONST_METHOD");
const QString kSuperClass("SUPERCLASS");
const QString kOverload("OVERLOAD");
const QString kReturnValuePolicy("RETURN_VALUE_POLICY");
const QString kConstGetOverload("CONST_GET_OVERLOAD");

/* These are for the macros that appear in the header files */
const QString kPYB11_CREATE_BINDINGS("PYB11_CREATE_BINDINGS");
const QString kSIMPL_STATIC_NEW_MACRO("SIMPL_STATIC_NEW_MACRO");
const QString kSIMPL_FILTER_NEW_MACRO("SIMPL_FILTER_NEW_MACRO");
const QString kSIMPL_SHARED_POINTERS("SIMPL_SHARED_POINTERS");
const QString kPYB11_PROPERTY("PYB11_PROPERTY");
const QString kPYB11_METHOD("PYB11_METHOD");
const QString kPYB11_STATIC_CREATION("PYB11_STATIC_CREATION");
const QString kPYB11_CREATION("PYB11_CREATION");
const QString kPYB11_ENUMERATION("PYB11_ENUMERATION");

/* */
const QString kSIMPL_FILTER_PARAMETER("SIMPL_FILTER_PARAMETER");
}

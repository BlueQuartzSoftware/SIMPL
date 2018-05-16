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
  static const QString kRead("READ");
  static const QString kWrite("WRITE");
  static const QString kArgs("ARGS");
  static const QString kConst("CONST");
  static const QString kConstMethod("CONST_METHOD");
  static const QString kSuperClass("SUPERCLASS");
  static const QString kOverload("OVERLOAD");

  /* These are for the macros that appear in the header files */
  static const QString kPYB11_CREATE_BINDINGS("PYB11_CREATE_BINDINGS");
  static const QString kSIMPL_STATIC_NEW_MACRO("SIMPL_STATIC_NEW_MACRO");
  static const QString kSIMPL_FILTER_NEW_MACRO("SIMPL_FILTER_NEW_MACRO");
  static const QString kSIMPL_SHARED_POINTERS("SIMPL_SHARED_POINTERS");
  static const QString kPYB11_PROPERTY("PYB11_PROPERTY");
  static const QString kPYB11_METHOD("PYB11_METHOD");
  static const QString kPYB11_STATIC_CREATION("PYB11_STATIC_CREATION");
  static const QString kPYB11_CREATION("PYB11_CREATION");
  static const QString kPYB11_ENUMERATION("PYB11_ENUMERATION");
}

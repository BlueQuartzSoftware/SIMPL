/**
  @file
  @author Stefan Frings
*/

#pragma once

#include <QtGlobal>

#if 0
// This is specific to Windows dll's
#if defined(Q_OS_WIN)
#if defined(QTWEBAPPLIB_EXPORT)
#define DECLSPEC Q_DECL_EXPORT
#elif defined(QTWEBAPPLIB_IMPORT)
#define DECLSPEC Q_DECL_IMPORT
#endif
#endif
#if !defined(DECLSPEC)
#define DECLSPEC
#endif
#endif

/* Cmake will define QtWebAppLib_EXPORTS on Windows when it
configures to build a shared library. If you are going to use
another build system on windows or create the visual studio
projects by hand you need to define QtWebAppLib_EXPORTS when
building the DLL on windows.
*/

#if defined(QtWebAppLib_EXPORTS) /* Compiling the DLL/Dylib */
#if defined(_MSC_VER)            /* MSVC Compiler Case */
#define QtWebAppLib_EXPORT __declspec(dllexport)
#define DECLSPEC __declspec(dllexport)
#define EXPIMP_TEMPLATE
#elif(__GNUC__ >= 4) /* GCC 4.x has support for visibility options */
#define QtWebAppLib_EXPORT __attribute__((visibility("default")))
#endif
#else                 /* Importing the DLL into another project */
#if defined(_MSC_VER) /* MSVC Compiler Case */
#define QtWebAppLib_EXPORT __declspec(dllimport)
#define DECLSPEC __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#elif(__GNUC__ >= 4) /* GCC 4.x has support for visibility options */
#define QtWebAppLib_EXPORT __attribute__((visibility("default")))
#endif
#endif


/* If QtWebAppLib_EXPORT was never defined, define it here */
#ifndef QtWebAppLib_EXPORT
#define QtWebAppLib_EXPORT
#endif

#ifndef EXPIMP_TEMPLATE
#define EXPIMP_TEMPLATE
#endif

#ifndef DECLSPEC
#define DECLSPEC
#endif



/** Get the library version number */
DECLSPEC const char* getQtWebAppLibVersion();


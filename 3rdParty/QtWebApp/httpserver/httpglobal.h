/**
  @file
  @author Stefan Frings
*/

#ifndef HTTPGLOBAL_H
#define HTTPGLOBAL_H

#include <QtGlobal>

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

/* Cmake will define QtWebAppLib_EXPORTS on Windows when it
configures to build a shared library. If you are going to use
another build system on windows or create the visual studio
projects by hand you need to define QtWebAppLib_EXPORTS when
building the MXADatModel DLL on windows.
*/

#if defined(QtWebAppLib_BUILT_AS_DYNAMIC_LIB)

#if defined(QtWebAppLib_EXPORTS) /* Compiling the MXA DLL/Dylib */
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
#endif

/* If QtWebAppLib_EXPORT was never defined, define it here */
#ifndef QtWebAppLib_EXPORT
#define QtWebAppLib_EXPORT
#define EXPIMP_TEMPLATE
#define DECLSPEC
#endif

/** Get the library version number */
DECLSPEC const char* getQtWebAppLibVersion();

#endif // HTTPGLOBAL_H

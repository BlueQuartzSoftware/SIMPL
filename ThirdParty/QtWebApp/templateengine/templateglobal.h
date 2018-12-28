/**
  @file
  @author Stefan Frings
*/

#pragma once

#include <QtGlobal>

// This is specific to Windows dll's
#if defined(Q_OS_WIN)
#if defined(QTWEBAPPLIB_EXPORT)
#define QtWebAppLib_EXPORT Q_DECL_EXPORT
#elif defined(QTWEBAPPLIB_IMPORT)
#define QtWebAppLib_EXPORT Q_DECL_IMPORT
#endif
#endif
#if !defined(QtWebAppLib_EXPORT)
#define QtWebAppLib_EXPORT
#endif


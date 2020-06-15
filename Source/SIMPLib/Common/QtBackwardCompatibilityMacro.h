

#pragma once

#include <QtCore/QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
#define QFONTMETRICS_WIDTH width
#else
#define QFONTMETRICS_WIDTH horizontalAdvance
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
#define QFOREGROUND QPalette::Foreground
#else
#define QFOREGROUND QPalette::WindowText

#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)

#ifndef QVECTOR_FROM_STD_VECTOR
#define QVECTOR_FROM_STD_VECTOR(def, qvector, other) def qvector = def::fromStdVector(other);
#endif

#ifndef QVECTOR_TO_STD_VECTOR
#define QVECTOR_TO_STD_VECTOR(def, qvector, other) def other = qvector.toStdVector();
#endif

#ifndef QLIST_FROM_QSET
#define QLIST_FROM_QSET(def, qlist, other) def qlist = other.toList();
#endif

#else

#ifndef QVECTOR_FROM_STD_VECTOR
#define QVECTOR_FROM_STD_VECTOR(def, qvecVarName, svector) def qvecVarName(svector.begin(), svector.end());
#endif

#ifndef QVECTOR_TO_STD_VECTOR
#define QVECTOR_TO_STD_VECTOR(def, qvecVarName, svector) def svector(qvecVarName.begin(), qvecVarName.end());
#endif

#ifndef QLIST_FROM_QSET
#define QLIST_FROM_QSET(def, qlist, other) def qlist = other.values();
#endif

#endif

#pragma once

#include <QtCore/qglobal.h>

#if defined(SHARELIB_LIBRARY)
#  define SHARELIB_EXPORT Q_DECL_EXPORT
#else
#  define SHARELIB_EXPORT Q_DECL_IMPORT
#endif

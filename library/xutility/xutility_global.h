#pragma once

#include <QtCore/qglobal.h>

#ifndef XUTILITY_LIB_STATIC
# if defined(XUTILITY_LIB)
#  define XUTILITY_EXPORT Q_DECL_EXPORT
#  define XFUNCTION_EXPORT Q_DECL_EXPORT
# else
#  define XFUNCTION_EXPORT
#  define XUTILITY_EXPORT Q_DECL_IMPORT
# endif
#else
# define XUTILITY_EXPORT
# define XFUNCTION_EXPORT
#endif


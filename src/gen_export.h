#ifndef GEN_EXPORT_H
#define GEN_EXPORT_H

#include <QtCore/qglobal.h>

#if defined(GEN_EXPORT_LIBRARY)
#define GENLIB_EXPORT Q_DECL_EXPORT
#else
#define GENLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // GEN_EXPORT_H

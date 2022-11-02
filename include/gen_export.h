#ifndef GEN_EXPORT_H
#define GEN_EXPORT_H

#include <QtCore/qglobal.h>

#if defined(GEN_EXPORTS)
#define GENLIB_EXPORT Q_DECL_EXPORT
#elif defined(Gen_IMPORTS)
#define GENLIB_EXPORT Q_DECL_IMPORT
#else
#define GENLIB_EXPORT /**/
#endif

#endif // GEN_EXPORT_H

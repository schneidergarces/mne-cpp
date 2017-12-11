// ?? needs work
#ifndef NEURONE_GLOBAL_H
#define NEURONE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(NEURONE_LIBRARY)
#  define NEURONESHARED_EXPORT Q_DECL_EXPORT
#else
#  define NEURONESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // NEURONE_GLOBAL_H

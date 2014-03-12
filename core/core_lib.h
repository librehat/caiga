/*
 * Every header files should include this header.
 * And defines classes with CORE_LIB
 */

#ifndef CORE_LIB_H
#define CORE_LIB_H

#if defined CORE
#define CORE_LIB  Q_DECL_EXPORT
#else
#define CORE_LIB Q_DECL_IMPORT
#endif

#endif // CORE_LIB_H

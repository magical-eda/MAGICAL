/*
 *     This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 *  Distribution,  Product Version 5.7, and is subject to the Cadence
 *  LEF/DEF Open Source License Agreement.   Your  continued  use  of
 *  this file constitutes your acceptance of the terms of the LEF/DEF
 *  Open Source License and an agreement to abide by its  terms.   If
 *  you  don't  agree  with  this, you must remove this and any other
 *  files which are part of the distribution and  destroy any  copies
 *  made.
 *
 *     For updates, support, or to become part of the LEF/DEF Community,
 *  check www.openeda.org for details.
 */
/**
 * @file lefiKRDefs.hpp
 */


/* Definitions header file for K&R related defines */

#if defined(__STDC__) || defined(__cplusplus) || defined(WIN32)

#ifndef PROTO_PARAMS
#define PROTO_PARAMS(params) params
#define DEFINED_PROTO_PARAMS
#endif

#ifndef VAR_ARG_LIST
#define VAR_ARG_LIST ...
#define DEFINED_VAR_ARG_LIST
#endif

#ifndef EXTERN
#ifdef  LEFI_CPP_ANSI
#define EXTERN extern "C"
#else
#define EXTERN
#endif
#define DEFINED_EXTERN
#endif

#else

#ifndef PROTO_PARAMS
#define PROTO_PARAMS(params) (/* nothing */)
#define DEFINED_PROTO_PARAMS
#endif

#ifndef VAR_ARG_LIST
#define VAR_ARG_LIST va_tdcl(char *)
#define DEFINED_VAR_ARG_LIST
#endif

#ifndef EXTERN
#define EXTERN extern
#define DEFINED_EXTERN
#ifndef DEFINED_CONST
#define const /* nothing */
#define DEFINED_CONST
#endif
#endif

#endif /* __STDC__ */


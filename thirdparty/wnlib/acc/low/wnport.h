/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wnportH
#define wnportH



#define WN_IHUGE (1000000000)

#define WN_FHUGE (1.0e+50)
#define WN_FTINY (1.0/WN_FHUGE)


#ifdef _LANGUAGE_C_PLUS_PLUS
#  ifndef __cplusplus
#    define __cplusplus
#  endif
#endif


/*
 * EXTERN is used to allow header files from ANSI C modules to be used by
 * modules compiled under C++
 */
#ifndef EXTERN
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif
#endif

/*
 * This is used to bracket code that may or may be in C or C++, forcing it
 * to have C linkage. For example, you may have a header file containing a
 * typedef for a pointer to a function. Put EXTERN_BEGIN and EXTERN_END
 * around the typedef to force it to be a pointer to a "C" function, whether
 * it is included in a C file or a C++ file.
 */
#ifdef __cplusplus
#define WN_EXTERN_BEGIN extern "C" {
#define WN_EXTERN_END   };
#else
#define WN_EXTERN_BEGIN
#define WN_EXTERN_END
#endif


#if defined(unix)  || defined(UNIX)      || defined(__unix__)   || \
    defined(linux) || defined(__linux__) || defined(__CYGWIN__) || \
    defined(hpux)  || defined(__hpux__)
# define WN_UNIX
#endif

#if defined(vms)
# define WN_VMS
#endif

#if defined(WIN32)
# define WN_WINDOWS
#endif

#if !defined(WN_UNIX) && !defined(WN_VMS) && !defined(WN_WINDOWS)
# error Unrecognized OS
#endif
#if defined(WN_UNIX) && defined(WN_VMS)
# error Unix and VMS
#endif
#if defined(WN_UNIX) && defined(WN_WINDOWS)
# error Unix and Windows
#endif
#if defined(WN_VMS) && defined(WN_WINDOWS)
# error VMS and Windows
#endif

#if defined(WN_WINDOWS)
# define WN_OSSLASH ("\\")
#else
# define WN_OSSLASH ("/")
#endif

/*   say WN_DEPRECATED after a routine extern declaration in the .h file, this
** will generate a compiler warning every time it is called. */
/*   note gcc on sparc doesn't understand this attribute */
#if defined(__GNUC__) && !defined(sparc) && !defined(__ia64__)
# define WN_DEPRECATED __attribute__ ((__deprecated__))
#else
# define WN_DEPRECATED
#endif

/*   tell the compiler that a routine doesn't return.  Used mostly for
** asserts. */
#if defined(__GNUC__)
# define WN_NORETURN   __attribute__ ((__noreturn__))
#else
# define WN_NORETURN
#endif

#endif /* wnportH */

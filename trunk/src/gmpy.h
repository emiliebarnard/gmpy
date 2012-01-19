/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * gmpy.h                                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Python interface to the GMP or MPIR, MPFR, and MPC multiple precision   *
 * libraries.                                                              *
 *      Copyright 2000 - 2009 Alex Martelli                                *
 *      Copyright 2008, 2009, 2010, 2011, 2012 Case Van Horsen             *
 *                                                                         *
 * This library is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation; either version 2.1 of the License, or  *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * Lesser General Public License for more details.                         *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public        *
 * License along with this library; if not, write to the Free Software     *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA           *
 * 02110-1301  USA                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
  gmpy C API extension header file.
  Part of Python's gmpy module since version 0.4

  Created by Pearu Peterson <pearu@cens.ioc.ee>, November 2000.
  Edited by A. Martelli <aleaxit@yahoo.com>, December 2000.
  Edited by Case Van Horsen <casevh@gmail.com>, 2009, 2010, 2011.

  Version 1.02, February 2007.
  Version 1.03, June 2008
  Version 1.04, June 2008 (no changes)
  Version 1.05, February 2009 (support MPIR)
  Version 1.20, January 2010 (remove obsolete MS hacks) casevh
  Version 2.00, April 2010 (change to gmpy2) casevh
                October 2010 (added Py_hash_t) casevh
                December 2010 (added mpfr, mpc) casevh
                January 2011 (add Pygmpy_context) casevh
                April 2011 (split into multiple files) casevh
 */

#ifndef Py_GMPYMODULE_H
#define Py_GMPYMODULE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Check for Python version requirements. */

#if PY_VERSION_HEX < 0x02060000
#  error "GMPY2 requires Python 2.6 or later."
#endif

#if PY_VERSION_HEX < 0x030200A4
typedef long Py_hash_t;
typedef unsigned long Py_uhash_t;
#  define _PyHASH_IMAG 1000003
#endif

/* Define various macros to deal with differences between Python 2 and 3. */

#if (PY_MAJOR_VERSION == 3)
#define PY3
#define Py2or3String_FromString     PyUnicode_FromString
#define Py2or3String_Check          PyUnicode_Check
#define Py2or3String_Format         PyUnicode_Format
#define Py2or3String_AsString       PyUnicode_AS_DATA
#define PyStrOrUnicode_Check(op)    (PyBytes_Check(op) || PyUnicode_Check(op))
#define PyIntOrLong_FromLong        PyLong_FromLong
#define PyIntOrLong_Check(op)       PyLong_Check(op)
#define PyIntOrLong_FromSize_t      PyLong_FromSize_t
#define PyIntOrLong_FromSsize_t     PyLong_FromSsize_t
#define PyIntOrLong_AsSsize_t       PyLong_AsSsize_t
#define PyIntOrLong_AsLong          PyLong_AsLong
#else
#define PY2
#define Py2or3String_FromString     PyString_FromString
#define Py2or3String_Check          PyString_Check
#define Py2or3String_Format         PyString_Format
#define Py2or3String_AsString       PyString_AsString
#define PyStrOrUnicode_Check(op)    (PyString_Check(op) || PyUnicode_Check(op))
#define PyIntOrLong_FromLong        PyInt_FromLong
#define PyIntOrLong_Check(op)       (PyInt_Check(op) || PyLong_Check(op))
#define PyIntOrLong_FromSize_t      PyInt_FromSize_t
#define PyIntOrLong_FromSsize_t     PyInt_FromSsize_t
#define PyIntOrLong_AsSsize_t       PyInt_AsSsize_t
#define PyIntOrLong_AsLong          PyInt_AsLong
#endif

/* Support MPIR, if requested. */

#ifdef MPIR
#  include "mpir.h"
#else
#  include "gmp.h"
#endif

#if defined(MS_WIN32) && defined(_MSC_VER)
   /* so one won't need to link explicitly to gmp.lib...: */
#  if defined(MPIR)
#    pragma comment(lib,"mpir.lib")
#  else
#    pragma comment(lib,"gmp.lib")
#  endif
#  define isnan _isnan
#  define isinf !_finite
#  define USE_ALLOCA 1
#  define inline __inline
#endif

#ifdef __GNUC__
#  define USE_ALLOCA 1
#endif

#ifndef alloca
# ifdef __GNUC__
#  define alloca __builtin_alloca
# else
#   ifdef _MSC_VER
#    include <malloc.h>
#    define alloca _alloca
#   else
#    if HAVE_ALLOCA_H
#     include <alloca.h>
#    else
       char *alloca ();
#    endif
#   endif
# endif
#endif

#define ALLOC_THRESHOLD 8192

#ifdef DEBUG
#  define TRACE(msg) if(options.debug) fprintf(stderr, msg)
#else
#  define TRACE(msg)
#endif

#define TYPE_ERROR(msg) PyErr_SetString(PyExc_TypeError, msg)
#define VALUE_ERROR(msg) PyErr_SetString(PyExc_ValueError, msg)
#define ZERO_ERROR(msg) PyErr_SetString(PyExc_ZeroDivisionError, msg)
#define SYSTEM_ERROR(msg) PyErr_SetString(PyExc_SystemError, msg)
#define OVERFLOW_ERROR(msg) PyErr_SetString(PyExc_OverflowError, msg)

#define GMPY_DEFAULT -1

/* Define the minimum memory amount allocated. 8 has historically been
 * used, but 16 might be better for some applications or 64-bit systems.
 */
#define GMPY_ALLOC_MIN (2 * (GMP_NUMB_BITS >> 3))

/* To prevent excessive memory usage, we don't want to save very large
 * numbers in the cache. The default value specified in the options
 * structure is 128 words (512 bytes on 32-bit platforms, 1024 bytes on
 * 64-bit platforms).
 */
#define MAX_CACHE_LIMBS 16384

/* The maximum number of objects that can be saved in a cache is specified
 * here. The default value is 100.*/
#define MAX_CACHE 1000

/* Choose which memory manager is used: Python or C.
 * NOTE: The use of PyMem is not compatible with Sage, therefore it is
 * disabled by default.
 */
 
#define USE_PYMEM 0
#ifdef USE_PYMEM
#  define GMPY_FREE(NAME) PyMem_Free(NAME)
#  define GMPY_MALLOC(NAME) PyMem_Malloc(NAME)
#  define GMPY_REALLOC(NAME, SIZE) PyMem_Realloc(NAME, SIZE)
#else
#  define GMPY_FREE(NAME) free(NAME)
#  define GMPY_MALLOC(NAME) malloc(NAME)
#  define GMPY_REALLOC(NAME, SIZE) realloc(NAME, SIZE)
#endif

#ifdef USE_ALLOCA
#  define TEMP_ALLOC(B, S) \
    if(S < ALLOC_THRESHOLD) { \
        B = alloca(S); \
    } else { \
        if(!(B = GMPY_MALLOC(S))) { \
            PyErr_NoMemory(); \
            return NULL; \
        } \
    }
#  define TEMP_FREE(B, S) if(S >= ALLOC_THRESHOLD) GMPY_FREE(B)
#else
#  define TEMP_ALLOC(B, S) \
    if(!(B = GMPY_MALLOC(S)))  { \
        PyErr_NoMemory(); \
        return NULL; \
    }
#  define TEMP_FREE(B, S) GMPY_FREE(B)
#endif

/* Various defs to mask differences between Python versions. */

#define Py_RETURN_NOTIMPLEMENTED\
    return Py_INCREF(Py_NotImplemented), Py_NotImplemented

#ifndef Py_SIZE
#define Py_SIZE(ob)     (((PyVarObject*)(ob))->ob_size)
#endif

#ifndef Py_TYPE
#define Py_TYPE(ob)     (((PyObject*)(ob))->ob_type)
#endif

/* The gmpy_args.h file includes macros that are used for argument
 * processing.
 */
#include "gmpy_args.h"

#include "gmpy_mpz.h"
#include "gmpy_mpz_conv.h"
#include "gmpy_mpz_divmod.h"
#include "gmpy_mpz_divmod2exp.h"
#include "gmpy_mpz_inplace.h"
#include "gmpy_xmpz_inplace.h"

#include "gmpy_mpq.h"
#include "gmpy_mpq_conv.h"

#ifdef WITHMPFR
#  include "mpfr.h"
#  include "gmpy_mpfr.h"
#  if MPFR_VERSION < 0x030100
#    error gmpy2 requires MPFR 3.1.0 or later
#  endif
#  include "gmpy_context.h"
#endif

#ifdef WITHMPC
#  include "mpc.h"
#  include "gmpy_mpc.h"
#  if MPC_VERSION < 0x000900
#    error gmpy2 requires MPC 0.9.0 or later
#  endif
#endif

/* Support object caching, creation, and deletion. */

#include "gmpy_cache.h"

/* Suport for miscellaneous functions (ie. version, license, etc.). */

#include "gmpy_misc.h"

/* Support basic arithmetic operations on mpz, mpq, mpfr, and mpc types. */

#include "gmpy_basic.h"

/* Support operations other than those in gmpy_basic. */

#include "gmpy_mpany.h"

/* Support conversion to/from binary format. */

#include "gmpy_binary.h"


#ifdef __cplusplus
}
#endif
#endif /* !defined(Py_GMPYMODULE_H */

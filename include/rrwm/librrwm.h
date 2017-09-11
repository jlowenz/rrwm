/*
 * MATLAB Compiler: 6.4 (R2017a)
 * Date: Sun Sep 10 21:33:57 2017
 * Arguments:
 * "-B""macro_default""-W""lib:librrwm""-T""link:lib""-d""outlib""cRRWM.m""make_
 * default_groups.m"
 */

#ifndef __librrwm_h
#define __librrwm_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_librrwm
#define PUBLIC_librrwm_C_API __global
#else
#define PUBLIC_librrwm_C_API /* No import statement needed. */
#endif

#define LIB_librrwm_C_API PUBLIC_librrwm_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_librrwm
#define PUBLIC_librrwm_C_API __declspec(dllexport)
#else
#define PUBLIC_librrwm_C_API __declspec(dllimport)
#endif

#define LIB_librrwm_C_API PUBLIC_librrwm_C_API


#else

#define LIB_librrwm_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_librrwm_C_API 
#define LIB_librrwm_C_API /* No special import/export declaration */
#endif

extern LIB_librrwm_C_API 
bool MW_CALL_CONV librrwmInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_librrwm_C_API 
bool MW_CALL_CONV librrwmInitialize(void);

extern LIB_librrwm_C_API 
void MW_CALL_CONV librrwmTerminate(void);



extern LIB_librrwm_C_API 
void MW_CALL_CONV librrwmPrintStackTrace(void);

extern LIB_librrwm_C_API 
bool MW_CALL_CONV mlxCRRWM(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_librrwm_C_API 
bool MW_CALL_CONV mlxMake_default_groups(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                         *prhs[]);



extern LIB_librrwm_C_API bool MW_CALL_CONV mlfCRRWM(int nargout, mxArray** X, mxArray** score, mxArray** time, mxArray* A, mxArray* group1, mxArray* group2, mxArray* prob_c, mxArray* amp_max, mxArray* iter_max, mxArray* conv_threshold, mxArray* tolC);

extern LIB_librrwm_C_API bool MW_CALL_CONV mlfMake_default_groups(int nargout, mxArray** group1, mxArray** group2, mxArray* g1_count, mxArray* g2_count);

#ifdef __cplusplus
}
#endif
#endif

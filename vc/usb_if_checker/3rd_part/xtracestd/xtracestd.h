//---------------------------------------------------------------------------
//
// MODULE NAME: xtracestd.h
//
// DESCRIPTION: Replaces the standard MFC TRACE macros by the according
//              XTRACE macros from xtrace.h.
//
// AUTHOR     : Written by Wolfgang Busch (wolfgang.busch@knuut.de) 
//
// COPYRIGHT  : You can use or modify this file in any way you like.
//
// HISTORY    :
// 2000-10-23 : Initial release
//
//---------------------------------------------------------------------------
#ifndef __XTRACE_STANDARD_H__
#define __XTRACE_STANDARD_H__

#ifdef TRACE
// Redefine TRACE macros to make XTRACE macros the default

#undef  TRACE
#define TRACE  XTRACE
#undef  TRACE0
#define TRACE0 XTRACE0
#undef  TRACE1
#define TRACE1 XTRACE1
#undef  TRACE2
#define TRACE2 XTRACE2
#undef  TRACE3
#define TRACE3 XTRACE3

#ifndef __XTRACE_H__  // Include "xtrace.h" if necessary
#include "xtrace.h"
#endif

#else  // <Afx.h> not included

#error Include <Afx.h> first -- TRACE not defined!

#endif  // ifdef TRACE

#endif

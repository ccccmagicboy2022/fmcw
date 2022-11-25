//---------------------------------------------------------------------------
//
// MODULE NAME: xtrace.h
//
// DESCRIPTION: A set of extended TRACE macros for easy source navigation.
//
// AUTHOR     : Written by Wolfgang Busch (wolfgang.busch@knuut.de) 
//
// COPYRIGHT  : You can use or modify this file in any way you like.
//
// HISTORY    :
// 2000-10-23 : Bugfixes and enhancements
//              - Evaluation of vargs in ExtTrace::Trace fixed
//              - Macros are expanded to a single statement (using brackets
//                and ',' operator)
//              - Macros renamed from TRACE to XTRACE
//              - Append newline if necessary
// 2000-10-18 : Initial release
//
//---------------------------------------------------------------------------
#ifndef __XTRACE_H__
#define __XTRACE_H__

struct ExtTrace
{
	static CString s_strFilename;
	static int     s_iLinenum;
	static void    Trace(LPCTSTR lpszFormat, ...);
};

#ifdef _DEBUG

// Helper to pass filename and line number of compilation unit


// The following set of XTRACE macros is defined according to
// the usual MFC TRACE macros PLUS the ability to print file name
// and line number of the trace point

#define XTRACE  \
   ExtTrace::s_strFilename = __FILE__, \
   ExtTrace::s_iLinenum = __LINE__, \
   ExtTrace::Trace

#define XTRACE0(sz) \
   { \
      ExtTrace::s_strFilename = __FILE__;  \
      ExtTrace::s_iLinenum = __LINE__;     \
      ExtTrace::Trace(_T("%s"), _T(sz));   \
   }

#define XTRACE1(sz, p1) \
   { \
      ExtTrace::s_strFilename = __FILE__;  \
      ExtTrace::s_iLinenum = __LINE__;     \
      ExtTrace::Trace(_T(sz), (p1));       \
   }

#define XTRACE2(sz, p1, p2) \
   { \
      ExtTrace::s_strFilename = __FILE__;  \
      ExtTrace::s_iLinenum = __LINE__;     \
      ExtTrace::Trace(_T(sz), (p1), (p2)); \
   }

#define XTRACE3(sz, p1, p2, p3) \
   { \
      ExtTrace::s_strFilename = __FILE__;        \
      ExtTrace::s_iLinenum = __LINE__;           \
      ExtTrace::Trace(_T(sz), (p1), (p2), (p3)); \
   }

#else // Release build: no traces

#define XTRACE    1 ? (void)0 : ::AfxTrace
#define XTRACE0(sz)
#define XTRACE1(sz, p1)
#define XTRACE2(sz, p1, p2)
#define XTRACE3(sz, p1, p2, p3)

#endif  // ifdef _DEBUG

#endif
//---------------------------------------------------------------------------
//
// MODULE NAME: xtrace.cpp
//
// DESCRIPTION: Implementation module to support a set of extended TRACE
//              macros for easy source navigation.
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
#include "stdafx.h"
#include "xtrace.h"

CString ExtTrace::s_strFilename; // name of current compilation unit (__FILE__)
int     ExtTrace::s_iLinenum;    // current line number (__LINE__)

void ExtTrace::Trace(LPCTSTR lpszFormat, ...)
{
   // Build extended format string with preceding file name and line number
   CString strExtFormat;
   strExtFormat.Format( _T("%s (%d): %s"),
                        (LPCTSTR)ExtTrace::s_strFilename,
                        ExtTrace::s_iLinenum,
                        lpszFormat);

   CString strMessage;  // The full trace message

   // Format trace message
   va_list args;
   va_start(args, lpszFormat);
   strMessage.FormatV(strExtFormat, args);
   va_end(lpszFormat);

   // Append newline if necessary
   if (strMessage[strMessage.GetLength()-1] != _T('\n'))
      strMessage += _T('\n');

   // Print trace message
   ::AfxTrace(strMessage);
}

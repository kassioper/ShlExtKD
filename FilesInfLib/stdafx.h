// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <string>
#include <tchar.h>
#include  <locale>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
#if defined UNICODE
	typedef std::wstring tstring; 
#else
	typedef std::string tstring;
#endif


//struct ci_char_traits : public char_traits<wchar_t> {
//    static bool eq(wchar_t c1, wchar_t c2) { return toupper(c1) == toupper(c2); }
//    static bool ne(wchar_t c1, wchar_t c2) { return toupper(c1) != toupper(c2); }
//    static bool lt(wchar_t c1, wchar_t c2) { return toupper(c1) <  toupper(c2); }
//    static int compare(const wchar_t* s1, const wchar_t* s2, size_t n) {
//        while( n-- != 0 ) {
//            if( toupper(*s1) < toupper(*s2) ) return -1;
//            if( toupper(*s1) > toupper(*s2) ) return 1;
//            ++s1; ++s2;
//        }
//        return 0;
//    }
//    static const char* find(const wchar_t* s, int n, wchar_t a) {
//        while( n-- > 0 && toupper(*s) != toupper(a) ) {
//            ++s;
//        }
//        return s;
//    }
//};
//
//typedef std::basic_string<wchar_t, ci_char_traits> tstring;
//
// TODO: reference additional headers your program requires here

#include "IntSafe.h"

#ifndef _QWORD_DEFINED
#define _QWORD_DEFINED
typedef __int64 QWORD, *LPQWORD;
#endif

#define MAKEQWORD(a, b)	\
	((QWORD)( ((QWORD) ((DWORD) (a))) << 32 | ((DWORD) (b))))

#ifndef _L2SZCH
#define _L2SZCH
#if UNICODE
	static const int L2SZCH = 1;
#else
	static const int L2SZCH = 0;
#endif
#endif

typedef tstring::size_type StrIdx;

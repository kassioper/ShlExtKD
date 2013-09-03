#include "StdAfx.h"
#include "FileInf.h"
#include "FilesUtils.h"
#include "Shlwapi.h"


  void CFileInf::DetailsRead(bool volatile * pisAbort)
	{
		::FileDetailsRead(
			Path->c_str()
			, Size
			, TimeCreate
			, CheckSum
			, pisAbort
			);
	}
/*
  size_t DWORD_ToString(TCHAR* buf, DWORD dw)
	{
			for (BYTE i = 8; i > 0; --i, dw <<= 4, ++buf)
			{
				BYTE b = (BYTE)(dw >> ((sizeof(DWORD) << 3) - 4));
				*buf = 9 >= b ? _T('0') + b : _T('A') + b - 10;
				//if (9 >= b)
				//	*buf = _T('0') + b ;
				// else
				//	*buf = _T('A') + b - 10;
			}
			return 8;
	}

	size_t CFileInf::ToString(TCHAR* buf, size_t bufSize)
	{
		const TCHAR space = _T(' ');
		const size_t c_afterNameMinLen = 64;
		if (!bufSize)
			return 0;
		else if (1 == bufSize)
			goto lbEnd;
		TCHAR *buf0 = buf;
		size_t len = Path->size();
		bool isOver;
		if (isOver = len + c_afterNameMinLen >= bufSize - 1)
			len = bufSize - 2;
		memcpy(buf, Path->data(), len << L2SZCH);
		buf += len;
		if (isOver)
		{
			*buf++ = _T('*');
			goto lbEnd;
		}
		*buf++ = space;
		*buf++ = space;

		StrFormatByteSize( Size,	buf, c_afterNameMinLen);
		while (*buf++ != NULL) ;
		--buf;
 		*buf++ = space;
		*buf++ = space;

		SYSTEMTIME stUTC, stLocal;
    FileTimeToSystemTime(&TimeCreate, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
		len = GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &stLocal, NULL, buf, c_afterNameMinLen);
		buf += len - 1;
		*buf++ = space;
		len = GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &stLocal, NULL, buf, c_afterNameMinLen);
		buf += len - 1;
		*buf++ = space;
		*buf++ = space;

		buf += DWORD_ToString(buf, CheckSum);
lbEnd:
		*buf = NULL; //?
		return buf - buf0;
	}
*/

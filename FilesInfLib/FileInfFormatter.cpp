#include "StdAfx.h"
#include "Shlwapi.h"
#include "FileInfFormatter.h"

#define space _T(' ');

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

	TCHAR* CFileInfFormatter::Format(CFileInf* pFileInf)
	{
		if (!pFileInf)
		{
			Len = 0;
			*Buf = NULL;
			return Buf;
		}
		TCHAR *buf = (TCHAR*)Buf;
		size_t len = pFileInf->Path->size();
		bool isOver;
		if (isOver = MAX_PATH < len)
			len = MAX_PATH;
		memcpy(buf, pFileInf->Path->data(), len << L2SZCH);
		buf += len;
		if (isOver)
			*buf++ = _T('*');

		*buf++ = space;
		*buf++ = space;

		StrFormatByteSize(pFileInf->Size,	buf, ARRAYSIZE(Buf)+  (buf - Buf));
		while (*buf++ != NULL) ;
		--buf;
 		*buf++ = space;
		*buf++ = space;

		SYSTEMTIME stUTC, stLocal;
    FileTimeToSystemTime(&pFileInf->TimeCreate, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
		len = GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &stLocal, NULL, buf, ARRAYSIZE(Buf)+  (buf - Buf));
		buf += len - 1;
		*buf++ = space;
		len = GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &stLocal, NULL, buf, buf - Buf);
		buf += len - 1;
		*buf++ = space;
		*buf++ = space;

		buf += DWORD_ToString(buf, pFileInf->CheckSum);
lbEnd:
		*buf = NULL; //need!!
		Len = buf - Buf;

		return Buf;
		// END Format
	}

	TCHAR* CFileInfFormatter::FormatLogFileName(CFilesInf* pFilesInf, TCHAR **pszFileApart)
	{
		//_tcscpy_s(Buf, ARRAYSIZE(Buf), _T("c:\\ShlExtKD.log"));
		//return Buf;
		if (pFilesInf->m_FilesInf.size())
		{
			tstring *path = pFilesInf->m_FilesInf[0]->Path;
			_tcscpy_s(Buf, ARRAYSIZE(Buf), path->c_str());
			//PathRemoveFileSpec(szDir);
			int nSlash = 2;
			for (int i = path->length() - 1; i >= 0; --i )
			{
				if ('\\' == Buf[i])
				{
					if (pszFileApart)
					{
						*pszFileApart = Buf + i + 1;
						Buf[i] = NULL;
					}
					_tcscpy_s(Buf + i + 1, ARRAYSIZE(Buf)-(i+1), _T("ShlExtKD.log"));
					break;
				}
			}
		}
		else
		{
			Buf[0] = NULL;
		}
		return Buf;
	}

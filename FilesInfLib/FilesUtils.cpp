#include "StdAfx.h"
#include "FilesUtils.h"

// Map a "view" size of 10MB (used in the filemap function)
#define MAX_VIEW_SIZE	10485760
// Read 4K of data at a time (used in the C++ streams, Win32 I/O, and assembly functions)
#define MAX_BUFFER_SIZE	4096
#define GetLastErrorEx 'L'
#define ExitEx 'E'

DWORD FileDetailsReadStd(const TCHAR* filePath, QWORD&  qwSize, FILETIME& fileTime, DWORD& dwCheckSum
		, HANDLE evAbort) // bool volatile * pisAbort
{
	_ASSERTE(filePath);
	_ASSERTE(lstrlen(filePath));

	DWORD dwErrorCode = NO_ERROR;
	HANDLE hFile = NULL;

	DWORD dwCrc32 = 0x0;
	LPBYTE pCrc = (LPBYTE)&dwCrc32; 
	QWORD qwFileSize;
	FILETIME fileTime0;

	try
	{
		// Open the file
		hFile = CreateFile(filePath,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_SEQUENTIAL_SCAN
			//| FILE_FLAG_NO_BUFFERING
			,
			NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			dwErrorCode = GetLastError();
		else
		{
			{
				DWORD dwLo = 0, dwHi = 0;
				dwLo = GetFileSize(hFile, &dwHi);
				if(dwLo == INVALID_FILE_SIZE 
					&& (dwErrorCode = GetLastError()) != NO_ERROR)
					throw ExitEx;
				qwFileSize = MAKEQWORD(dwHi, dwLo);
			}
			if (!GetFileTime(hFile, &fileTime0, NULL, NULL)) 
				throw GetLastErrorEx;

			BYTE buf[MAX_BUFFER_SIZE];
//BYTE buf_max[2 * MAX_BUFFER_SIZE - 1], *buf;
//
//  buf = (BYTE *) ((DWORD) (buf_max + MAX_BUFFER_SIZE - 1) & ~(MAX_BUFFER_SIZE - 1));

			DWORD dwBytesRead, dwLoop;
			while(true)
			{

				//if (*pisAbort)
				if (WaitForSingleObject(evAbort, 0) != WAIT_TIMEOUT)
				{
					dwErrorCode = ERROR_CANCELLED;
					break;
				}
			BOOL bSuccess = ReadFile(hFile, buf, MAX_BUFFER_SIZE, &dwBytesRead, NULL);
				if (!dwBytesRead)
					break;
				if (!bSuccess)
				{
					dwErrorCode = GetLastError();
					break;
				}
				BYTE checkSumOffset = 0;
				for(dwLoop = 0; dwLoop < dwBytesRead; dwLoop++)
				{
						*(pCrc + checkSumOffset) ^= buf[dwLoop];
						checkSumOffset++;
						if (4 <= checkSumOffset)
							checkSumOffset = 0;
					//CalcCrc32(buffer[dwLoop], dwCrc32);
				}
				//bSuccess = ReadFile(hFile, buffer, sizeof(buffer), &dwBytesRead, NULL);
			}
		}
	}
	catch(...)
	{
		// An unknown exception happened
		dwErrorCode = ERROR_CRC;
	}

	if(hFile != NULL) CloseHandle(hFile);


		if (NO_ERROR == dwErrorCode)
		{
			qwSize = qwFileSize;
			dwCheckSum = dwCrc32;
			fileTime = fileTime0;
		}	return dwErrorCode;
}

  DWORD FileDetailsReadMap(const TCHAR* filePath, QWORD&  qwSize, FILETIME& fileTime, DWORD& dwCheckSum
		, HANDLE evAbort) // bool volatile * pisAbort)
	{
		_ASSERTE(filePath);
		_ASSERTE(lstrlen(filePath));
		DWORD dwCrc32;
		DWORD dwErrorCode = NO_ERROR;
		HANDLE hFile = NULL, hFilemap = NULL;

		dwCrc32 = 0x0;
		LPBYTE pCrc = (LPBYTE)&dwCrc32; 
		FILETIME fileTime0;

		QWORD qwFileSize, qwFileSize0;
		try
		{
			// Open the file
			hFile = CreateFile(filePath,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_SEQUENTIAL_SCAN,				NULL);
			if(hFile == INVALID_HANDLE_VALUE)
				throw GetLastErrorEx;

		  QWORD qwFileOffset = 0;
			DWORD dwByteCount, dwViewSize;
			LPVOID dwBaseAddress;

			{
				DWORD dwLo = 0, dwHi = 0;
				dwLo = GetFileSize(hFile, &dwHi);
				if(dwLo == INVALID_FILE_SIZE 
					&& (dwErrorCode = GetLastError()) != NO_ERROR)
					throw ExitEx;
				qwFileSize = qwFileSize0 = MAKEQWORD(dwHi, dwLo);
			}

			if (!GetFileTime(hFile, &fileTime0, NULL, NULL)) 
				throw GetLastErrorEx;
			// Get the file size
			//if(!GetFileSizeQW(hFile, qwFileSize))
			//	dwErrorCode = ERROR_BAD_LENGTH;
			//else 
			if(0 != qwFileSize)	// We cannot CRC zero byte files
			{
				// Create the file mapping
				hFilemap = CreateFileMapping(hFile,
					NULL,
					PAGE_READONLY,
					0,
					0,
					NULL);

				if(NULL == hFilemap)
					throw GetLastErrorEx;

				LPBYTE pByte;
				//LPDWORD pDWord;

				// Loop while we map a section of the file and CRC it
				while(0 < qwFileSize)
				{
					//if (*pisAbort)
					if (WaitForSingleObject(evAbort, 0) != WAIT_TIMEOUT)
					{
						dwErrorCode = ERROR_CANCELLED;
						throw ExitEx;
					}
					if(qwFileSize < MAX_VIEW_SIZE)
						dwViewSize = LODWORD(qwFileSize);
					else
						dwViewSize = MAX_VIEW_SIZE;

					dwBaseAddress = MapViewOfFile(hFilemap,
						FILE_MAP_READ,
						HIDWORD(qwFileOffset),
						LODWORD(qwFileOffset),
						dwViewSize);

					dwByteCount = dwViewSize;
					pByte = (LPBYTE)dwBaseAddress;
					BYTE checkSumOffset = 0;
					while(0 < dwByteCount--)
					{
						*(pCrc + checkSumOffset) ^= *pByte;
						pByte++;
						checkSumOffset++;
						if (4 <= checkSumOffset)
							checkSumOffset = 0;
					}

					UnmapViewOfFile((LPVOID)dwBaseAddress);
					qwFileOffset += dwViewSize;
					qwFileSize -= dwViewSize;
				}  // while(qwFileSize > 0)
			} // if(qwFileSize != 0)
		}
		catch(char c)
		{
			if (GetLastErrorEx == c)
				dwErrorCode = GetLastError();
		}
		catch(...)
		{
			// An unknown exception happened
			dwErrorCode = ERROR_CRC;
		}

		if(hFile != NULL) CloseHandle(hFile);
		if(hFilemap != NULL) CloseHandle(hFilemap);

		if (NO_ERROR == dwErrorCode)
		{
			qwSize = qwFileSize0;
			dwCheckSum = dwCrc32;
			fileTime = fileTime0;
		}
		return dwErrorCode;
	}

	bool FileNameCompare(const tstring *strL, const tstring *strR) {
		const size_t npos = std::basic_string<TCHAR>::npos;

		StrIdx nExtL = strL->find_last_of(_T("."));
		StrIdx nExtR = strR->find_last_of(_T("."));

		int nRez = strL->compare(0, nExtL, *strR, 0, nExtR);   
		if (0 == nRez)
		{
			if (npos == nExtL || strL->size() < ++nExtL)
				return true;
			else if (npos == nExtR || strR->size() < ++nExtR)
				return false;
			else
				nRez = strL->compare(nExtL, npos, *strR, nExtR, npos);   
		}
		return nRez <= 0;
  }

 // DWORD FileDetailsRead(const TCHAR* filePath, QWORD&  qwSize, FILETIME& fileTime, DWORD& dwCheckSum
	//	, bool volatile * pisAbort)
	//{
	//	return 
	//		//FileDetailsMap
	//		FileDetailsReadStd
	//		(filePath, qwSize, fileTime, dwCheckSum, pisAbort);
	//}

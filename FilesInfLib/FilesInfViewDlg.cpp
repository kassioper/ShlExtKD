#include "stdafx.h"
#include "FilesInfViewDlg.h"
#include "FilesInfViewDlg.rc.h"
#include "FileInfFormatter.h"
#include <windows.h> 
#include <windowsx.h> 
#include "Shlobj.h"
#include "Shlwapi.h"
#include "Commdlg.h"

struct FilesInfDlgData
{
	CFilesInf* filesInf;
	int itemHeight;
	HWND hwndList;
};


inline FilesInfDlgData* FilesInfDlgDataGet(HWND hWnd)
{
	return (FilesInfDlgData*) ::GetWindowLongPtr (hWnd, GWLP_USERDATA);
}

void DlgInit(HWND hwndDlg)
{
	HWND hwndOwner; 
	RECT rc, rcDlg, rcOwner;

 if ((hwndOwner = GetParent(hwndDlg)) == NULL) 
    {
        hwndOwner = GetDesktopWindow(); 
    }

    GetWindowRect(hwndOwner, &rcOwner); 
    GetWindowRect(hwndDlg, &rcDlg); 
    CopyRect(&rc, &rcOwner); 

    // Offset the owner and dialog box rectangles so that right and bottom 
    // values represent the width and height, and then offset the owner again 
    // to discard space taken up by the dialog box. 

    OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
    OffsetRect(&rc, -rc.left, -rc.top); 
    OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 

    // The new position is the sum of half the remaining space and the owner's 
    // original position. 

    SetWindowPos(hwndDlg, 
                 HWND_TOP, 
                 rcOwner.left + (rc.right / 2), 
                 rcOwner.top + (rc.bottom / 2), 
                 0, 0,          // Ignores size arguments. 
                 SWP_NOSIZE); 
}

BOOL CALLBACK FileInfViewDlgProc(HWND hWndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
			{
			  ::SetWindowLongPtr (hWndDlg, GWLP_USERDATA, lParam);
				DlgInit(hWndDlg);
				HWND hwndList = GetDlgItem(hWndDlg, IDC_LST_FILES);
				
				FilesInfDlgData* dlgData = reinterpret_cast<FilesInfDlgData*>(lParam);
				dlgData->hwndList = hwndList;
				LRESULT itemHeight = ::SendMessage(hwndList, LB_GETITEMHEIGHT, 0, 0);
				dlgData->itemHeight = itemHeight;
				CFilesInf* filesInf = dlgData->filesInf; //FilesInfGet(hWnd);
				
				CFilesInf::Index count = filesInf->m_FilesInf.size();
				SendMessage(hwndList, LB_SETCOUNT, count, 0);
				SendMessage(hwndList, LB_SETHORIZONTALEXTENT , 1024, 0L);
				TCHAR buf[64];
				_sntprintf_s(buf, ARRAYSIZE(buf), _TRUNCATE
					,_T("ShlExtKD files count = %d"), count);
				SetWindowText(hWndDlg, buf);
				::SendDlgItemMessage(hWndDlg,IDC_STATUS, WM_SETTEXT, 0, (LPARAM)_T("processing..."));
				filesInf->FilesDetailsRead(hWndDlg);

				//DataRefresh(hWndDlg);
				//if (GetDlgCtrlID((HWND) wParam) != ID_ITEMNAME) 
				//{ 
				//		SetFocus(GetDlgItem(hwndDlg, ID_ITEMNAME)); 
				//		return FALSE; 
				//} 
        return TRUE;
			}
    case WM_COMMAND:
			{
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
					{
						FilesInfDlgData* dlgData = FilesInfDlgDataGet(hWndDlg);
						dlgData->filesInf->FilesDetailsReadStop();
            EndDialog(hWndDlg, LOWORD(wParam));
            return TRUE;
					}
        case IDSAVE:
					{
						TCHAR szFileName[MAX_PATH+1];
						TCHAR szDir[MAX_PATH+1];
						TCHAR szSaveFileName[MAX_PATH+1]= _T("");
						OPENFILENAME ofn;
						ZeroMemory(&ofn, sizeof(ofn));
						ofn.lStructSize = sizeof(ofn); 
						ofn.hwndOwner = hWndDlg;
						ofn.lpstrFilter = _T("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
						ofn.lpstrFile = (LPWSTR)szSaveFileName;
						ofn.nMaxFile = MAX_PATH;
						ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
						ofn.lpstrDefExt = _T("txt");

						FilesInfDlgData* dlgData = FilesInfDlgDataGet(hWndDlg);
				/*		if (dlgData->filesInf->m_FilesInf.size())
						{
							tstring *path = dlgData->filesInf->m_FilesInf[0]->Path;
							_tcscpy_s(szDir, ARRAYSIZE(szDir), path->c_str());
							//PathRemoveFileSpec(szDir);
							int nSlash = 2;
							for (int i = path->length() - 1; i >= 0; --i )
							{
								if ('\\' == szDir[i])
								{
									if (0 == --nSlash)
									{
										ofn.lpstrFile = szDir + i + 1;
										break;
									}
									else
									{
										szDir[i] = NULL;
									}
								}
							}
							if (!ofn.lpstrFile )
								ofn.lpstrFile = szDir;
							ofn.lpstrInitialDir = szDir;
						}*/
						CFileInfFormatter fmtr;
						ofn.lpstrInitialDir = fmtr.FormatLogFileName(dlgData->filesInf, &ofn.lpstrFile);

						if (GetSaveFileName(&ofn))
						{
							SaveToFile(*dlgData->filesInf, ofn.lpstrFile);
						}
					}
        }
        return FALSE;
			}
		case UM_FILE_INF_ITEM_REFRESH:
			{
						FilesInfDlgData* dlgData = FilesInfDlgDataGet(hWndDlg);
						HWND hwndList = dlgData->hwndList;
						RECT client_rectangle;
						LRESULT top = ::SendMessage(hwndList, LB_GETTOPINDEX, 0, 0);
						GetClientRect(hwndList, &client_rectangle);
						if (0 == dlgData->itemHeight || 
							(top <= lParam && lParam <= top 
							+ (client_rectangle.bottom - client_rectangle.top)/dlgData->itemHeight + 1 ))
						{
							RedrawWindow(hwndList, NULL, NULL, RDW_INVALIDATE);
						}
						return TRUE;
			}
		case WM_MEASUREITEM:
        {
            return true;
        }
		case UM_FILE_INF_STATUS:
			{
				::SendDlgItemMessage(hWndDlg,IDC_STATUS, WM_SETTEXT, 0, lParam);
            return true;
			}
        case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT draw_item = (LPDRAWITEMSTRUCT)lParam;
						if (draw_item)
						{
							UINT itemID = draw_item->itemID;
							if (0 <= itemID)
							{
								
								FilesInfDlgData* dlgData = FilesInfDlgDataGet(hWndDlg);
								CFilesInf* filesInf = dlgData->filesInf;
								
								if (filesInf->m_FilesInf.size() > itemID)
								{
									CFileInf *pFi = filesInf->m_FilesInf[draw_item->itemID];
									if (pFi)
									{
										TCHAR *pszItemRez;
										tstring *pPath = pFi->Path;
										StrIdx len;

										if ( pFi->IsNotRead() )
										{
											pszItemRez = (TCHAR*)pPath->c_str();
										  len = pPath->length();
										} else {
											CFileInfFormatter formatter(pFi);
											pszItemRez = formatter.Buf;
											len = formatter.Len;

											//TCHAR pszItem[64+MAX_PATH];
											//pszItemRez = pszItem;
											//len = pFi->ToString(pszItem, ARRAYSIZE(pszItem));
										}

										DrawText(draw_item->hDC, pszItemRez, 
													 static_cast<int>(len), &draw_item->rcItem, 
													 DT_LEFT);
									} //pfi
								} // filesInfData->size() > itemID
							} // 0 <= itemID
						} // draw_item
            return TRUE;
        }
    }
    return FALSE;
}

INT_PTR FileInfViewDlgShow(HINSTANCE hInst, HWND hwndOwner, CFilesInf* pFilesInf)
{
	 HRSRC r = FindResource(hInst, MAKEINTRESOURCE(IDD_DLG_FILES_INF), RT_DIALOG);
	 HGLOBAL g = LoadResource(hInst, r);

	 IMalloc *pIMalloc;
	 HRESULT hr = SHGetMalloc(&pIMalloc);
	 if (FAILED(hr))
		 return 0;

	 FilesInfDlgData* dlgData = (FilesInfDlgData*) pIMalloc->Alloc(sizeof(FilesInfDlgData));
	 dlgData->filesInf = pFilesInf;

   INT_PTR ret = DialogBoxIndirectParam(hInst, 
       (LPDLGTEMPLATE)LockResource(g), 
       hwndOwner, 
       (DLGPROC)FileInfViewDlgProc,
			 (LPARAM)dlgData); 

   pIMalloc->Free(dlgData);
	 pIMalloc->Release();
    return ret; 
}

// WonDrag.c --- Remove shell32 dependencies by using fake Drag* functions
// Author: katahiromz
// License: MIT

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include "WonDrag.h"

UINT WONAPI WonDragQueryFileA(HDROP hDrop, UINT iFile, LPSTR pszFile, UINT cchFile)
{
    LPSTR lpDrop;
    UINT i = 0;
    DROPFILES *lpDropFileStruct = GlobalLock(hDrop);

    if (!lpDropFileStruct)
        goto end;

    lpDrop = (LPSTR)lpDropFileStruct + lpDropFileStruct->pFiles;

    if (lpDropFileStruct->fWide)
    {
        LPWSTR lpszFileW = NULL;
        if (pszFile && iFile != 0xFFFFFFFF)
        {
            lpszFileW = HeapAlloc(GetProcessHeap(), 0, cchFile*sizeof(WCHAR));
            if (!lpszFileW)
                goto end;
        }

        i = WonDragQueryFileW(hDrop, iFile, lpszFileW, cchFile);

        if (lpszFileW)
        {
            WideCharToMultiByte(CP_ACP, 0, lpszFileW, -1, pszFile, cchFile, 0, NULL);
            HeapFree(GetProcessHeap(), 0, lpszFileW);
        }
        goto end;
    }

    while (i++ < iFile)
    {
        while (*lpDrop++); /* skip filename */

        if (!*lpDrop)
        {
            i = (iFile == 0xFFFFFFFF) ? i : 0;
            goto end;
        }
    }

    i = lstrlenA(lpDrop);
    if (!pszFile)
        goto end;
    lstrcpynA(pszFile, lpDrop, cchFile);
end:
    GlobalUnlock(hDrop);
    return i;
}

UINT WONAPI WonDragQueryFileW(HDROP hDrop, UINT iFile, LPWSTR pszFile, UINT cchFile)
{
    LPWSTR lpwDrop;
    UINT i = 0;
    DROPFILES *lpDropFileStruct = GlobalLock(hDrop);

    if (!lpDropFileStruct)
        goto end;

    lpwDrop = (LPWSTR)((LPSTR)lpDropFileStruct + lpDropFileStruct->pFiles);

    if (!lpDropFileStruct->fWide)
    {
        LPSTR lpszFileA = NULL;

        if (pszFile && iFile != 0xFFFFFFFF)
        {
            lpszFileA = HeapAlloc(GetProcessHeap(), 0, cchFile);
            if (!lpszFileA)
                goto end;
        }

        i = WonDragQueryFileA(hDrop, iFile, lpszFileA, cchFile);

        if (lpszFileA)
        {
            MultiByteToWideChar(CP_ACP, 0, lpszFileA, -1, pszFile, cchFile);
            HeapFree(GetProcessHeap(), 0, lpszFileA);
        }
        goto end;
    }

    i = 0;
    while (i++ < iFile)
    {
        while (*lpwDrop++); /* skip filename */

        if (!*lpwDrop)
        {
            i = (iFile == 0xFFFFFFFF) ? i : 0;
            goto end;
        }
    }

    i = lstrlenW(lpwDrop);
    if (!pszFile)
        goto end;
    lstrcpynW(pszFile, lpwDrop, cchFile);

end:
    GlobalUnlock(hDrop);
    return i;
}

VOID WONAPI WonDragAcceptFiles(HWND hWnd, BOOL fAccept)
{
    if (!IsWindow(hWnd))
        return;

    LONG_PTR exstyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (fAccept)
        exstyle |= WS_EX_ACCEPTFILES;
    else
        exstyle &= ~WS_EX_ACCEPTFILES;

    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exstyle);
}

BOOL WONAPI WonDragQueryPoint(HDROP hDrop, POINT *ppt)
{
    DROPFILES *lpDropFileStruct = GlobalLock(hDrop);
    *ppt = lpDropFileStruct->pt;
    BOOL bRet = lpDropFileStruct->fNC;
    GlobalUnlock(hDrop);
    return bRet;
}

HGLOBAL WONAPI WonCreateHDROP(LPITEMIDLIST pidlRoot, LPITEMIDLIST * apidl, UINT cidl)
{
    UINT i;
    INT size = 0;
    WCHAR wszFileName[MAX_PATH];
    HGLOBAL hGlobal = NULL;
    DROPFILES *pDropFiles;
    INT offset;

    LPITEMIDLIST *pidls = (LPITEMIDLIST *)HeapAlloc(GetProcessHeap(), 0, cidl * sizeof(*pidls));
    if (!pidls)
        goto cleanup;

    /* get the size needed */
    size = sizeof(DROPFILES);

    for (i = 0; i < cidl; ++i)
    {
        pidls[i] = ILCombine(pidlRoot, apidl[i]);
        SHGetPathFromIDListW(pidls[i], wszFileName);
        size += (wcslen(wszFileName) + 1) * sizeof(WCHAR);
    }

    size += sizeof(WCHAR);

    /* Fill the structure */
    hGlobal = GlobalAlloc(GHND|GMEM_SHARE, size);
    if(!hGlobal)
        goto cleanup;

    pDropFiles = GlobalLock(hGlobal);
    offset = (sizeof(DROPFILES) + sizeof(WCHAR) - 1) / sizeof(WCHAR);
    pDropFiles->pFiles = offset * sizeof(WCHAR);
    pDropFiles->fWide = TRUE;

    for (i = 0; i < cidl; ++i)
    {
        SHGetPathFromIDListW(pidls[i], wszFileName);
        wcscpy(((WCHAR*)pDropFiles)+offset, wszFileName);
        offset += wcslen(wszFileName) + 1;
        ILFree(pidls[i]);
    }

    ((WCHAR*)pDropFiles)[offset] = 0;
    GlobalUnlock(hGlobal);

cleanup:
    if (pidls)
        HeapFree(GetProcessHeap(), 0, pidls);

    return hGlobal;
}

VOID WONAPI WonDragFinish(HDROP hDrop)
{
    GlobalFree(hDrop);
}

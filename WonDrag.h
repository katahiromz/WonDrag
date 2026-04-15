// WonDrag.h --- Remove shell32 dependencies by using fake Drag* functions
// Author: katahiromz
// License: MIT

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WONAPI
    #define WONAPI WINAPI
#endif

UINT WONAPI WonDragQueryFileA(HDROP hDrop, UINT iFile, LPSTR pszFile, UINT cchFile);
UINT WONAPI WonDragQueryFileW(HDROP hDrop, UINT iFile, LPWSTR pszFile, UINT cchFile);
VOID WONAPI WonDragAcceptFiles(HWND hWnd, BOOL fAccept);
BOOL WONAPI WonDragQueryPoint(HDROP hDrop, POINT *ppt);
VOID WONAPI WonDragFinish(HDROP hDrop);

HGLOBAL WONAPI WonCreateHDROP(LPITEMIDLIST pidlRoot, LPITEMIDLIST * apidl, UINT cidl);

#ifdef __cplusplus
} // extern "C"
#endif

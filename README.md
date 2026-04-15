# WonDrag

Remove shell32 dependencies by using fake Drag* functions.

```c
#include "WonDrag.h"

UINT WonDragQueryFileA(HDROP hDrop, UINT iFile, LPSTR pszFile, UINT cchFile);
UINT WonDragQueryFileW(HDROP hDrop, UINT iFile, LPWSTR pszFile, UINT cchFile);
VOID WonDragAcceptFiles(HWND hWnd, BOOL fAccept);
BOOL WonDragQueryPoint(HDROP hDrop, POINT *ppt);
VOID WonDragFinish(HDROP hDrop);

HGLOBAL WonCreateHDROP(LPITEMIDLIST pidlRoot, LPITEMIDLIST *apidl, UINT cidl);
```

License: MIT

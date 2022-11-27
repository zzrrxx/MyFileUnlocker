#include "MyContextMenuExt.h"

#include <new>
#include <Shlwapi.h>
#include <strsafe.h>

#include "Global.h"

#define MAX_QUERY_FILE_INDEX 0xFFFFFFFF

#define IDM_DISPLAY          0 

MyContextMenuExt::MyContextMenuExt() :
    m_Ref(1),
    m_MenuText{0},
    m_VerbW{0},
    m_VerbHelpTextW{0},
    m_VerbCanonicalNameW{0},
    m_VerbA{0},
    m_VerbHelpTextA{0},
    m_VerbCanonicalNameA{0},
    m_MenuBmp(NULL)
{
    Global* g = Global::Instance();
    g->IncreDllRef();

    // TODO: load bmp resource

    wcscpy_s(m_MenuText, L"File Unlocker");
    wcscpy_s(m_VerbW, L"File Unlocker");
    wcscpy_s(m_VerbCanonicalNameW, L"File Unlocker");
    wcscpy_s(m_VerbHelpTextW, L"Find the process locking the file");

    strcpy_s(m_VerbA, "File Unlocker");
    strcpy_s(m_VerbHelpTextA, "Find the process locking the file");
    strcpy_s(m_VerbCanonicalNameA, "File Unlocker");

}
MyContextMenuExt::~MyContextMenuExt() {
    Global::Instance()->DecreDllRef();
}

HRESULT MyContextMenuExt::ComponentCreator(const IID& iid, void** ppv) {
    HRESULT hr = E_OUTOFMEMORY;

    MyContextMenuExt* ext = new (std::nothrow) MyContextMenuExt();
    if (ext) {
        hr = ext->QueryInterface(iid, ppv);
        ext->Release();
    }

    return hr;
}

void MyContextMenuExt::PerformAction() {
    OutputDebugStringA("-----PerformAction");
}


#pragma region IUnknown
IFACEMETHODIMP MyContextMenuExt::QueryInterface(REFIID riid, void **ppv) {
    static const QITAB qit[] = {
        QITABENT(MyContextMenuExt, IContextMenu),
        QITABENT(MyContextMenuExt, IShellExtInit),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}
IFACEMETHODIMP_(ULONG) MyContextMenuExt::AddRef() {
    return InterlockedIncrement(&m_Ref);
}
IFACEMETHODIMP_(ULONG) MyContextMenuExt::Release() {
    ULONG ref = InterlockedDecrement(&m_Ref);
    if (ref == 0) {
        delete this;
    }
    return ref;
}
#pragma endregion

#pragma region IShellExtInit
IFACEMETHODIMP MyContextMenuExt::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID) {
    if (pDataObj == NULL) {
        return E_INVALIDARG;
    }

    char buf[1024];
    OutputDebugStringA("-----Initialize");

    HRESULT hr = E_FAIL;
    Global* g = Global::Instance();

    FORMATETC fe = { (CLIPFORMAT)g->ShellIdList(), NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm;

    sprintf_s(buf, "-----Initialize ShellIdList %d", g->ShellIdList());
    OutputDebugStringA(buf);

    if (SUCCEEDED(pDataObj->GetData(&fe, &stm))) {
        OutputDebugStringA("----- Initialize 1");
        FORMATETC etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM stg = { TYMED_HGLOBAL };

        if (SUCCEEDED(pDataObj->GetData(&etc, &stg))) {
            OutputDebugStringA("----- Initialize 2");
            HDROP hDrop = static_cast<HDROP>(GlobalLock(stg.hGlobal));
            if (hDrop != NULL) {
                OutputDebugStringA("----- Initialize 3");

                UINT filesCount = DragQueryFileW(hDrop, MAX_QUERY_FILE_INDEX, NULL, 0);
                for (int i = 0; i < filesCount; i++) {
                    wchar_t fullFileName[MAX_PATH];
                    DragQueryFileW(hDrop, i, fullFileName, ARRAYSIZE(fullFileName));

                    wchar_t wbuf[4096];
                    wsprintf(wbuf, L"-----Initialize 4 %s", fullFileName);
                    OutputDebugStringW(wbuf);
                }
                hr = S_OK;

                GlobalUnlock(stg.hGlobal);
                ReleaseStgMedium(&stg);
            }
        }

        ReleaseStgMedium(&stm);
    }

    return hr;
}
#pragma endregion

#pragma region IContextMenu
IFACEMETHODIMP MyContextMenuExt::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
    OutputDebugStringA("-----QueryContextMenu");
    if (CMF_DEFAULTONLY & uFlags || CMF_NOVERBS & uFlags || CMF_VERBSONLY & uFlags) {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
    }
    OutputDebugStringA("-----QueryContextMenu 1");

    MENUITEMINFO item = { sizeof(MENUITEMINFO) };
    item.fMask = MIIM_BITMAP | MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE;
    item.wID = idCmdFirst + IDM_DISPLAY;
    item.fType = MFT_STRING;
    item.fState = MFS_ENABLED;
    item.dwTypeData = m_MenuText;
    //item.hbmpItem = static_cast<HBITMAP>(m_MenuBmp);

   
    if (!InsertMenuItem(hMenu, indexMenu, TRUE, &item)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
     OutputDebugStringA("-----QueryContextMenu 2");

    MENUITEMINFO sep = { sizeof(sep) };
    sep.fMask = MIIM_TYPE;
    sep.fType = MFT_SEPARATOR;
    if (!InsertMenuItem(hMenu, indexMenu + 1, TRUE, &sep)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
     OutputDebugStringA("-----QueryContextMenu 3");

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(IDM_DISPLAY + 1));
}
IFACEMETHODIMP MyContextMenuExt::InvokeCommand(LPCMINVOKECOMMANDINFO pici) {
    BOOL fUnicode = FALSE;
    int commandID = -1;

    if (pici->cbSize == sizeof(CMINVOKECOMMANDINFOEX)) {
        if (pici->fMask & CMIC_MASK_UNICODE) {
            fUnicode = TRUE;
        }
    }

    if (!fUnicode && HIWORD(pici->lpVerb)) {

        if (StrCmpIA(pici->lpVerb, m_VerbA) == 0) {
            PerformAction();
        } else {
            return E_FAIL;
        }
        
    } else if (fUnicode && HIWORD(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW)) {

        if (StrCmpIW(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW, m_VerbW) == 0) {
            PerformAction();
        } else {
            return E_FAIL;
        }

    } else {
        if (LOWORD(pici->lpVerb) == IDM_DISPLAY) {
            PerformAction();
        } else {
            return E_FAIL;
        }
    }
    return S_OK;
}
IFACEMETHODIMP MyContextMenuExt::GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax) {
    switch (uFlags) {

    case GCS_HELPTEXTW:
        StringCchCopy(reinterpret_cast<PWSTR>(pszName), cchMax, m_VerbHelpTextW);
        break;

    case GCS_VERBW:
        StringCchCopy(reinterpret_cast<PWSTR>(pszName), cchMax, m_VerbCanonicalNameW);
        break;

    default:
        return S_OK;
    }

    return S_OK;
}
#pragma endregion


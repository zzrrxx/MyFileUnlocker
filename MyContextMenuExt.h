#ifndef _MY_CONTEXT_MENU_EXT_H_
#define _MY_CONTEXT_MENU_EXT_H_

#include <Windows.h>
#include <ShlObj.h>

class MyContextMenuExt : public IShellExtInit, public IContextMenu {
public:
    MyContextMenuExt();
    ~MyContextMenuExt();

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);

    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
    IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax);

    static HRESULT ComponentCreator(const IID& iid, void** ppv);

protected:
    virtual void PerformAction();

protected:
    long    m_Ref;
    wchar_t m_MenuText[64];
    wchar_t m_VerbW[64];
    char    m_VerbA[128];
    wchar_t m_VerbHelpTextW[64];
    char    m_VerbHelpTextA[128];
    wchar_t m_VerbCanonicalNameW[64];
    char    m_VerbCanonicalNameA[128];
    HANDLE  m_MenuBmp;

};

#endif // _MY_CONTEXT_MENU_EXT_H_


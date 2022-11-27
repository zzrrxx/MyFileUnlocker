// dllmain.cpp : Defines the entry point for the DLL application.

#include "Global.h"
#include "ClassFactory.h"
#include "Regist.h"
#include "MyContextMenuExt.h"

#pragma comment(lib, "Shlwapi")

#define EXTENSION_NAME L"FileUnlocker"

// {76CA20EC-2C23-4094-BEB3-8EB80862D7E8}
const CLSID CLSID_FILE_UNLOCKER = {
    0x76ca20ec, 0x2c23, 0x4094, 0xbe, 0xb3, 0x8e, 0xb8, 0x8, 0x62, 0xd7, 0xe8
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    Global* g = Global::Instance();
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        g->Reset();
        g->SetDllInst(hModule);

        // add ClassFactory to Global instance
        g->AddFactory(CLSID_FILE_UNLOCKER, MyContextMenuExt::ComponentCreator, EXTENSION_NAME, EXT_TYPE_CONTEXT_MENU);

        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;

    default:
        break;
    }

    return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
  HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

  FactoryInfo* factories = NULL;
  int factoriesCount = 0;
  Global::Instance()->GetFactories(&factories, &factoriesCount);
  for (int i = 0; i < factoriesCount; i++) {
    if (IsEqualCLSID(factories[i].CLSID, rclsid)) {
      ClassFactory* classFactory = new ClassFactory(factories[i].Func);
      if (classFactory == NULL) return E_OUTOFMEMORY;
      hr = classFactory->QueryInterface(riid, ppv);
      classFactory->Release();
      break;
    }
  }

  return hr;

}

STDAPI DllCanUnloadNow() {
  return Global::Instance()->DllRef() > 0 ? S_FALSE : S_OK;
}

STDAPI DllRegisterServer() {
    HRESULT hr;
    int counter = 1;
    wchar_t szModule[MAX_PATH];

    Global* g = Global::Instance();

    if (GetModuleFileName(g->DllInst(), szModule, ARRAYSIZE(szModule)) == 0) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    FactoryInfo* factories = NULL;
    int factoriesCount = 0;
    g->GetFactories(&factories, &factoriesCount);
    for (int i = 0; i < factoriesCount; i++) {
        FactoryInfo& fact = factories[i];
        hr = Regist::RegisterInprocServer(szModule, fact.CLSID, EXTENSION_NAME, L"Apartment");
    
        if (SUCCEEDED(hr)) {
            if (fact.ExtType == EXT_TYPE_CONTEXT_MENU) {

                hr = Regist::RegisterShellExtContextMenuHandler(L"*", fact.CLSID, fact.FriendlyName);
                if (SUCCEEDED(hr)) {
                    hr = Regist::RegisterShellExtContextMenuHandler(L"Directory", fact.CLSID, fact.FriendlyName);
                }

            } else if (fact.ExtType == EXT_TYPE_OVERLAY_ICON_HANDLER) {

                wchar_t tmpBuf[1024];
                wsprintf(tmpBuf, L"    %d%s", counter, fact.FriendlyName);
                hr = Regist::RegisterShellExtOverlayIconHandler(tmpBuf, fact.CLSID);

            }
        }

        counter++;
    }

    return hr;

}

STDAPI DllUnregisterServer() {
    HRESULT hr = S_OK;
    int counter = 1;
    wchar_t szModule[MAX_PATH];

    Global* g = Global::Instance();

    if (GetModuleFileName(g->DllInst(), szModule, ARRAYSIZE(szModule)) == 0) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    } 

    FactoryInfo* factories = NULL;
    int factoriesCount = 0;
    g->GetFactories(&factories, &factoriesCount);
    for (int i = 0; i < factoriesCount; i++) {
        FactoryInfo& fact = factories[i];
        hr = Regist::UnregisterInprocServer(fact.CLSID);
    
        if (SUCCEEDED(hr)) {
            if (fact.ExtType == EXT_TYPE_CONTEXT_MENU) {

                hr = Regist::UnregisterShellExtContextMenuHandler(L"*", fact.CLSID, fact.FriendlyName);

            } else if (fact.ExtType == EXT_TYPE_OVERLAY_ICON_HANDLER) {

                wchar_t tmpBuf[1024];
                wsprintf(tmpBuf, L"    %d%s", counter, fact.FriendlyName);
                hr = Regist::UnregisterShellExtOverlayIconHandler(tmpBuf, fact.CLSID);

            }
        }

        counter++;
    }

    return hr;
}




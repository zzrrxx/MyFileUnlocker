
#include "Regist.h"

#include <strsafe.h>
#include <Shlwapi.h>

HRESULT Regist::SetHKCRRegistryKeyAndValue(PCWSTR pszSubKey, PCWSTR pszValueName, PCWSTR pszData, HKEY key) {
  HRESULT hr;
  HKEY hKey = NULL;

  hr = HRESULT_FROM_WIN32(RegCreateKeyEx(key, pszSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));

  if (SUCCEEDED(hr)) {
    if (pszData != NULL) {
        DWORD cbData = lstrlen(pszData) * sizeof(*pszData);
        hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, pszValueName, 0, REG_SZ, reinterpret_cast<const BYTE *>(pszData), cbData));
    }

    RegCloseKey(hKey);
  }

  return hr;
}

HRESULT Regist::GetHKCRRegistryKeyAndValue(PCWSTR pszSubKey, PCWSTR pszValueName, PWSTR pszData, DWORD cbData) {
  HRESULT hr;
  HKEY hKey = NULL;

  hr = HRESULT_FROM_WIN32(RegOpenKeyEx(HKEY_CLASSES_ROOT, pszSubKey, 0, KEY_READ, &hKey));

  if (SUCCEEDED(hr)) {
    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, pszValueName, NULL, NULL, reinterpret_cast<LPBYTE>(pszData), &cbData));

    RegCloseKey(hKey);
  }

  return hr;
}

HRESULT Regist::RegisterInprocServer(PCWSTR pszModule, const CLSID& clsid, PCWSTR pszFriendlyName, PCWSTR pszThreadModel) {
  if (pszModule == NULL || pszThreadModel == NULL) {
    return E_INVALIDARG;
  }

  HRESULT hr;

  wchar_t szCLSID[MAX_PATH];
  StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

  wchar_t szSubkey[MAX_PATH];

  hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
  if (SUCCEEDED(hr)) {
    hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszFriendlyName, HKEY_CLASSES_ROOT);

    if (SUCCEEDED(hr)) {
      hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s\\InprocServer32", szCLSID);
      if (SUCCEEDED(hr)) {
        hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszModule, HKEY_CLASSES_ROOT);
        if (SUCCEEDED(hr)) {
          hr = SetHKCRRegistryKeyAndValue(szSubkey, L"ThreadingModel", pszThreadModel, HKEY_CLASSES_ROOT);
        }
      }
    }
  }

  return hr;
}

HRESULT Regist::UnregisterInprocServer(const CLSID& clsid){
  HRESULT hr = S_OK;

  wchar_t szCLSID[MAX_PATH] = {0};
  StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

  wchar_t szSubkey[MAX_PATH] = {0};

  hr = StringCchPrintfW(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
  if (SUCCEEDED(hr)) {
    hr = HRESULT_FROM_WIN32(SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubkey));
  }

  return hr;
}

HRESULT Regist::RegisterShellExtOverlayIconHandler(PCWSTR pszName, const CLSID& clsid) {
  if (pszName == NULL) {
    return E_INVALIDARG;
  }

  HRESULT hr = S_OK;

  wchar_t szCLSID[MAX_PATH];
  StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

  wchar_t fullPath[1024];
  wsprintf(fullPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\%s", pszName);

  if (SUCCEEDED(hr)) {
    hr = SetHKCRRegistryKeyAndValue(fullPath, NULL, szCLSID, HKEY_LOCAL_MACHINE);
  }

  return hr;

}

HRESULT Regist::UnregisterShellExtOverlayIconHandler(PCWSTR pszName, const CLSID& clsid) {
  if (pszName == NULL) {
    return E_INVALIDARG;
  }

  HRESULT hr = S_OK;

  wchar_t szCLSID[MAX_PATH];
  StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

  wchar_t fullPath[1024];
  wsprintf(fullPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\%s", pszName);

  if (SUCCEEDED(hr)) {
    hr = HRESULT_FROM_WIN32(SHDeleteKey(HKEY_LOCAL_MACHINE, fullPath));
  }

  return hr;
}

HRESULT Regist::RegisterShellExtContextMenuHandler(PCWSTR pszFileType, const CLSID& clsid, PCWSTR pszFriendlyName) {
  if (pszFileType == NULL) {
    return E_INVALIDARG;
  }

  HRESULT hr = S_OK;

  wchar_t szCLSID[MAX_PATH];
  StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

  if (*pszFileType == L'.') {
      wchar_t szDefaultVal[260];
      hr = GetHKCRRegistryKeyAndValue(pszFileType, NULL, szDefaultVal, 
          sizeof(szDefaultVal));

      if (SUCCEEDED(hr) && szDefaultVal[0] != L'\0')
      {
          pszFileType = szDefaultVal;
      }
  }

  wchar_t fullPath[1024];
  wsprintf(fullPath, L"%s\\shellex\\ContextMenuHandlers\\%s", pszFileType, pszFriendlyName);

  if (SUCCEEDED(hr)) {
    hr = SetHKCRRegistryKeyAndValue(fullPath, NULL, szCLSID, HKEY_CLASSES_ROOT);
  }

  return hr;
}

HRESULT Regist::UnregisterShellExtContextMenuHandler(PCWSTR pszFileType, const CLSID& clsid, PCWSTR pszFriendlyName) {
  if (pszFileType == NULL) {
    return E_INVALIDARG;
  }

  HRESULT hr = S_OK;
  wchar_t szCLSID[MAX_PATH] = {0};
  StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

  if (*pszFileType == L'.') {
    wchar_t szDefaultVal[260];
    hr = GetHKCRRegistryKeyAndValue(pszFileType, NULL, szDefaultVal, sizeof(szDefaultVal));

    if (SUCCEEDED(hr) && szDefaultVal[0] != L'\0') {
      pszFileType = szDefaultVal;
    }
  }

  wchar_t fullPath[1024];
  wsprintf(fullPath, L"%s\\shellex\\ContextMenuHandlers\\%s", pszFileType, pszFriendlyName);

  if (SUCCEEDED(hr)) {
    hr = HRESULT_FROM_WIN32(SHDeleteKey(HKEY_CLASSES_ROOT, fullPath));
  }

  return hr;
}
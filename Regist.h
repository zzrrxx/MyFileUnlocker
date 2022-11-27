#pragma once
class Regist {
public:
  static HRESULT RegisterInprocServer(PCWSTR pszModule, const CLSID& clsid, PCWSTR pszFriendlyName, PCWSTR pszThreadModel);
  static HRESULT UnregisterInprocServer(const CLSID& clsid);

  static HRESULT RegisterShellExtOverlayIconHandler(PCWSTR pszName, const CLSID& clsid);
  static HRESULT UnregisterShellExtOverlayIconHandler(PCWSTR pszName, const CLSID& clsid);

  static HRESULT RegisterShellExtContextMenuHandler(PCWSTR pszFileType, const CLSID& clsid, PCWSTR pszFriendlyName);
  static HRESULT UnregisterShellExtContextMenuHandler(PCWSTR pszFileType, const CLSID& clsid, PCWSTR pszFriendlyName);


  static HRESULT SetHKCRRegistryKeyAndValue(PCWSTR pszSubKey, PCWSTR pszValueName, PCWSTR pszData, HKEY key);
  static HRESULT GetHKCRRegistryKeyAndValue(PCWSTR pszSubKey, PCWSTR pszValueName, PWSTR pszData, DWORD cbData);
};


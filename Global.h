#pragma once

#include <Windows.h>

typedef HRESULT (*FPCOMPCREATOR) (const IID&, void**);

#define EXT_TYPE_CONTEXT_MENU         1
#define EXT_TYPE_OVERLAY_ICON_HANDLER 2

struct FactoryInfo {
    CLSID         CLSID;
    FPCOMPCREATOR Func;
    wchar_t       FriendlyName[1024];
    int           ExtType;
};

class Global {
public:

  static Global* Instance();

  ~Global();

  HINSTANCE DllInst() const;
  void SetDllInst(HINSTANCE val);

  long DllRef() const;
  void IncreDllRef();
  void DecreDllRef();

  void AddFactory(const CLSID id, FPCOMPCREATOR func, const wchar_t* friendlyName, int extType);
  void GetFactories(FactoryInfo** retFactories, int* count) const;

  void Reset();

private:
  Global();
  static Global* m_Instance;


  FactoryInfo* m_FactoryData;
  int          m_FactoryDataLen;
  HINSTANCE    m_hInst;
  long         m_DllRef;
};


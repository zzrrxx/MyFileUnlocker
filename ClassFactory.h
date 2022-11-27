#pragma once

#include <Windows.h>
#include <Unknwn.h>

#include "Global.h"

class ClassFactory: public IClassFactory {
public:
  // IUnknown
  IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);
  IFACEMETHODIMP_(ULONG) AddRef();
  IFACEMETHODIMP_(ULONG) Release();

  // IClassFactory
  IFACEMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv);
  IFACEMETHODIMP LockServer(BOOL fLock);

  explicit ClassFactory(FPCOMPCREATOR pFuncTemp);
  ~ClassFactory();

private:
  FPCOMPCREATOR m_Creator;
  long          m_Ref;

};


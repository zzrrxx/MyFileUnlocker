
#include "ClassFactory.h"

#include <new>
#include <Shlwapi.h>

ClassFactory::ClassFactory(FPCOMPCREATOR pFuncTemp) : m_Ref(1), m_Creator(pFuncTemp) {
  Global::Instance()->IncreDllRef();
}

ClassFactory::~ClassFactory() {
  Global::Instance()->DecreDllRef();
}

IFACEMETHODIMP ClassFactory::QueryInterface(REFIID riid, void **ppv) {
  static const QITAB qit[] = {
    QITABENT(ClassFactory, IClassFactory),
    { 0 },
  };
  return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) ClassFactory::AddRef() {
    return InterlockedIncrement(&m_Ref);
}

IFACEMETHODIMP_(ULONG) ClassFactory::Release() {
  ULONG cRef = InterlockedDecrement(&m_Ref);
  if (0 == cRef) {
    delete this;
  }
  return cRef;
}

IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
  HRESULT hr = CLASS_E_NOAGGREGATION;

  if (pUnkOuter == NULL) {
    hr = E_OUTOFMEMORY;

    hr = (*m_Creator)(riid, ppv);
  }

  return hr;
}

IFACEMETHODIMP ClassFactory::LockServer(BOOL fLock) {
  if (fLock) {
    Global::Instance()->IncreDllRef();
  } else {
    Global::Instance()->DecreDllRef();
  }
  return S_OK;
}
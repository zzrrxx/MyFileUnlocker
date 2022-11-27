#include "pch.h"
#include "Global.h"

#include <stdlib.h>

Global* Global::m_Instance = NULL;

Global* Global::Instance() {
    if (m_Instance == NULL) {
        m_Instance = new Global();
    }
    return m_Instance;
}


Global::Global() : m_hInst(NULL), m_DllRef(0), m_FactoryData(NULL), m_FactoryDataLen(0) {
    m_FactoryData = (FactoryInfo*) malloc(sizeof(FactoryInfo) * 8);
    //assert(m_FactoryData);
    memset(&m_FactoryData, 0, sizeof(FactoryInfo) * 8);

}

Global::~Global() {
    if (m_FactoryData) {
        free(m_FactoryData);
        m_FactoryData = NULL;
    }
}

void Global::SetDllInst(HINSTANCE val) {
    m_hInst = val;
}
HINSTANCE Global::DllInst() const {
    return m_hInst;
}

long Global::DllRef() const {
    return m_DllRef;
}
void Global::IncreDllRef() {
    InterlockedIncrement(&m_DllRef);
}
void Global::DecreDllRef() {
    InterlockedDecrement(&m_DllRef);
}

void Global::AddFactory(const CLSID id, FPCOMPCREATOR func, const wchar_t* friendlyName, int extType) {

    FactoryInfo* p = m_FactoryData + m_FactoryDataLen;
    memcpy((void*)(&p->CLSID), &id, sizeof(CLSID));
    p->Func = func;
    wcscpy_s(p->FriendlyName, friendlyName);
    p->ExtType = extType;
    m_FactoryDataLen++;

}
void Global::GetFactories(FactoryInfo** retFactories, int* count) const {
    *count = m_FactoryDataLen;
    *retFactories = m_FactoryData;
}

void Global::Reset() {
    m_DllRef = 0;
    m_hInst = NULL;
    if (m_FactoryData) free(m_FactoryData);
    m_FactoryData = NULL;
    m_FactoryDataLen = 0;
}





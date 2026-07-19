#include "cbpp/CML.h"

#include <math.h>
#include <ctype.h>

#include "engine/datafile/object.h"

// CObject

namespace cbpp::cdf {
    static const char* g_sNullString = "(null)";

    CObject::CObject(int_t iValue) {
        *this = CreateObject(EObjectClass::Integer);
        m_pObj->SetValue(iValue);
    }

    CObject::CObject(float_t fValue) {
        *this = CreateObject(EObjectClass::Float);
        m_pObj->SetValue(fValue);
    }

    CObject::CObject(const char* sValue) {
        *this = CreateObject(EObjectClass::String);
        m_pObj->SetValue(sValue);
    }

    CObject::CObject(IObject* pData) : m_pObj(pData) {}
    CObject::operator IObject*() const { return m_pObj; }
    IObject* CObject::GetPointer() const { return m_pObj; }

    bool CObject::operator==(const CObject& pOther) const {
        return m_pObj == pOther.m_pObj;
    }

    bool CObject::operator!=(const CObject& pOther) const {
        return m_pObj != pOther.m_pObj;
    }

    EObjectClass CObject::Class() const { return m_pObj->Class(); }
    size_t CObject::Length() const { return m_pObj->Length(); }
    size_t CObject::Size() const { return m_pObj->Size(); }

    CObject& CObject::operator=(int_t iValue) { m_pObj->SetValue(iValue); return *this; }
    CObject& CObject::operator=(float_t fValue) { m_pObj->SetValue(fValue); return *this; }
    CObject& CObject::operator=(const char* sValue) { m_pObj->SetValue(sValue); return *this; }
    void CObject::SetBinaryData(const uint8_t* pData, size_t iLength) { m_pObj->SetValue(pData, iLength); }

    CObject CObject::operator[](size_t iIndex) { return CObject( m_pObj->At(iIndex) ); }
    CObject CObject::operator[](const char* sName) { return CObject( m_pObj->At(sName) ); }

    void CObject::Push(CObject pObj) { m_pObj->PushValue(pObj.GetPointer()); }
    void CObject::Push(const char* sName, CObject pObj) { m_pObj->PushValue(sName, pObj.GetPointer()); }

    const char* CObject::IndexName(size_t iIndex) const { return m_pObj->IndexName(iIndex); }
    CObject CObject::Index(size_t iIndex) { return CObject( m_pObj->At(iIndex) ); }

    CObject::operator int_t() const { return m_pObj->AsInt(); }
    CObject::operator float_t() const { return m_pObj->AsFloat(); }
    CObject::operator const char*() const { return m_pObj->AsString(); }
    CObject::operator uint8_t*() { return m_pObj->AsBinary(); }
    CObject::operator bool() const { return (*this) != cdf::NIL; }

    void CObject::Serialize(char* pBuffer) const { m_pObj->Serialize(pBuffer); }

    int_t CObject::AsInt() const { return m_pObj->AsInt(); }
    float_t CObject::AsFloat() const { return m_pObj->AsFloat(); }
    const char* CObject::AsString() const { return m_pObj->AsString(); }
    uint8_t* CObject::AsBinary() { return m_pObj->AsBinary(); }

    void CObject::Merge(CObject pSource) { m_pObj->Merge(pSource); }
}

namespace cbpp::cdf {
    int_t CNullObject::AsInt() const {
        return 0;
    }

    float_t CNullObject::AsFloat() const {
        return 0.0f;
    }

    const char* CNullObject::AsString() const {
        return s_sName;
    }

    uint8_t* CNullObject::AsBinary() {
        return NULL;
    }

    void CNullObject::SetValue(int_t iData) {}

    void CNullObject::SetValue(float_t fData) {}

    void CNullObject::SetValue(const char* sData) {}

    void CNullObject::SetValue(const uint8_t* pData, size_t iLength) {}

    IObject* CNullObject::At(const char* sName) { return this; }

    IObject* CNullObject::At(size_t iIndex) { return this; }

    const char* CNullObject::IndexName(size_t iIndex) { return g_sNullString; }

    void CNullObject::PushValue(IObject* pObj) { }

    void CNullObject::PushValue(const char* sName, IObject* pObj) { }

    EObjectClass CNullObject::Class() const { return EObjectClass::Nil; }

    size_t CNullObject::Length() const { return 0; }

    size_t CNullObject::Size() const { return 1; }

    void CNullObject::Serialize(char* pBuffer) const { *pBuffer = 0; }

    void CNullObject::Merge(CObject pSource) { }

    CNullObject g_pNullObject;
    const CObject NIL(&g_pNullObject);
}

// CIntObject

namespace cbpp::cdf {
    int_t CIntObject::AsInt() const {
        return m_iData;
    }

    float_t CIntObject::AsFloat() const {
        return (float_t)(m_iData);
    }

    const char* CIntObject::AsString() const {
        return s_sName;
    }

    uint8_t* CIntObject::AsBinary() {
        return NULL;
    }

    void CIntObject::SetValue(int_t iData) {
        m_iData = iData;
    }

    void CIntObject::SetValue(float_t fData) {
        m_iData = (int_t)(roundf(fData));
    }

    void CIntObject::SetValue(const char* sData) {
        m_iData = 0;
    }

    void CIntObject::SetValue(const uint8_t* pData, size_t iLength) {
        m_iData = 0;
    }

    IObject* CIntObject::At(const char*) {
        return &g_pNullObject;
    }

    IObject* CIntObject::At(size_t) {
        return &g_pNullObject;
    }

    const char* CIntObject::IndexName(size_t iIndex) { return g_sNullString; }

    void CIntObject::PushValue(IObject* pObj) {}
    void CIntObject::PushValue(const char* sName, IObject* pObj) {}

    EObjectClass CIntObject::Class() const {
        return EObjectClass::Integer;
    }

    size_t CIntObject::Length() const {
        return 0;
    }

    size_t CIntObject::Size() const {
        return sizeof(m_iData);
    }

    void CIntObject::Serialize(char* pBuffer) const {
        memcpy( pBuffer, &m_iData, sizeof(m_iData) );
    }

    void CIntObject::Merge(CObject pSource) {
        m_iData = pSource.AsInt();
    }
}

// CBinaryObject

namespace cbpp::cdf {
    int_t CBinaryObject::AsInt() const {
        return 0;
    }

    float_t CBinaryObject::AsFloat() const {
        return 0.0f;
    }

    const char* CBinaryObject::AsString() const {
        return s_sName;
    }

    uint8_t* CBinaryObject::AsBinary() {
        return m_aData.Data();
    }

    void CBinaryObject::SetValue(int_t iData) {}

    void CBinaryObject::SetValue(float_t fData) {}

    void CBinaryObject::SetValue(const char* sData) {}

    void CBinaryObject::SetValue(const uint8_t* pData, size_t iLength) {
        m_aData.SetArray(pData, iLength);
    }

    IObject* CBinaryObject::At(const char*) {
        return &g_pNullObject;
    }

    IObject* CBinaryObject::At(size_t) {
        return &g_pNullObject;
    }

    const char* CBinaryObject::IndexName(size_t iIndex) { return g_sNullString; }

    void CBinaryObject::PushValue(IObject* pObj) {}
    void CBinaryObject::PushValue(const char* sName, IObject* pObj) {}

    EObjectClass CBinaryObject::Class() const {
        return EObjectClass::Binary;
    }

    size_t CBinaryObject::Length() const {
        return m_aData.Length();
    }

    size_t CBinaryObject::Size() const {
        return m_aData.Length();
    }

    void CBinaryObject::Serialize(char* pBuffer) const {
        memcpy( pBuffer, m_aData.Data(), m_aData.Length() );
    }

    void CBinaryObject::Merge(CObject pSource) {
        m_aData.SetArray( pSource.AsBinary(), pSource.Length() );
    }
}

// CFloatObject

namespace cbpp::cdf {
    int_t CFloatObject::AsInt() const {
        return (int_t)(m_fData);
    }

    float_t CFloatObject::AsFloat() const {
        return m_fData;
    }

    const char* CFloatObject::AsString() const {
        return s_sName;
    }

    uint8_t* CFloatObject::AsBinary() {
        return NULL;
    }

    void CFloatObject::SetValue(int_t iData) {
        m_fData = (float_t)(iData);
    }

    void CFloatObject::SetValue(float_t fData) {
        m_fData = fData;
    }

    void CFloatObject::SetValue(const char* sData) {
        m_fData = 0.0f;
    }

    void CFloatObject::SetValue(const uint8_t* pData, size_t iLength) {
        m_fData = 0.0f;
    }

    IObject* CFloatObject::At(const char*) {
        return &g_pNullObject;
    }

    IObject* CFloatObject::At(size_t) {
        return &g_pNullObject;
    }

    const char* CFloatObject::IndexName(size_t iIndex) { return g_sNullString; }

    void CFloatObject::PushValue(IObject* pObj) {}
    void CFloatObject::PushValue(const char* sName, IObject* pObj) {}

    EObjectClass CFloatObject::Class() const {
        return EObjectClass::Float;
    }

    size_t CFloatObject::Length() const {
        return 0;
    }

    size_t CFloatObject::Size() const {
        return sizeof(m_fData);
    }

    void CFloatObject::Serialize(char* pBuffer) const {
        memcpy( pBuffer, &m_fData, sizeof(m_fData) );
    }

    void CFloatObject::Merge(CObject pSource) {
        m_fData = pSource.AsFloat();
    }
}

// CStringObject

namespace cbpp::cdf {
    int_t CStringObject::AsInt() const {
        return 0;
    }

    float_t CStringObject::AsFloat() const {
        return 0.0f;
    }

    const char* CStringObject::AsString() const {
        return m_sData.String();
    }

    uint8_t* CStringObject::AsBinary() {
        return (uint8_t*)(m_sData.Pointer());
    }

    void CStringObject::SetValue(int_t iData) {}

    void CStringObject::SetValue(float_t fData) {}

    void CStringObject::SetValue(const char* sData) {
        m_sData.Set(sData);
    }

    void CStringObject::SetValue(const uint8_t* pData, size_t iLength) {}

    IObject* CStringObject::At(const char*) {
        return &g_pNullObject;
    }

    IObject* CStringObject::At(size_t) {
        return &g_pNullObject;
    }
    
    const char* CStringObject::IndexName(size_t iIndex) { return g_sNullString; }

    void CStringObject::PushValue(IObject* pObj) {}
    void CStringObject::PushValue(const char* sName, IObject* pObj) {}

    EObjectClass CStringObject::Class() const {
        return EObjectClass::String;
    }

    size_t CStringObject::Length() const {
        return m_sData.Length();
    }

    size_t CStringObject::Size() const {
        return m_sData.Length() + 1;
    }

    void CStringObject::Serialize(char* pBuffer) const {
        memcpy( pBuffer, m_sData.String(), m_sData.Length() + 1 );
    }

    void CStringObject::Merge(CObject pSource) {
        m_sData.Set( pSource.AsString() );
    }
}

// CArrayObject

namespace cbpp::cdf {
    int_t CArrayObject::AsInt() const {
        return 0;
    }

    float_t CArrayObject::AsFloat() const {
        return 0.0f;
    }

    const char* CArrayObject::AsString() const {
        return s_sName;
    }

    uint8_t* CArrayObject::AsBinary() {
        return NULL;
    }

    void CArrayObject::SetValue(int_t iData) {}

    void CArrayObject::SetValue(float_t fData) {}

    void CArrayObject::SetValue(const char* sData) {}

    void CArrayObject::SetValue(const uint8_t* pData, size_t iLength) {}

    IObject* CArrayObject::At(const char*) {
        return &g_pNullObject;
    }

    IObject* CArrayObject::At(size_t iIndex) {
        if(iIndex >= m_pData.Length()) { return &g_pNullObject; }
        return m_pData[iIndex];
    }

    const char* CArrayObject::IndexName(size_t iIndex) { return g_sNullString; }

    void CArrayObject::PushValue(IObject* pObj) {
        m_pData.PushBack(pObj);
    }

    void CArrayObject::PushValue(const char* sName, IObject* pObj) {
        m_pData.PushBack(pObj);
    }

    EObjectClass CArrayObject::Class() const {
        return EObjectClass::Array;
    }

    size_t CArrayObject::Length() const {
        return m_pData.Length();
    }

    size_t CArrayObject::Size() const {
        if(m_pData.Length() == 0) { return 0; }

        const size_t iUnit = m_pData[0]->Size();
        return m_pData.Length() * iUnit;
    }

    CArrayObject::~CArrayObject() {
        for(size_t i = 0; i < m_pData.Length(); i++) {
            DeleteObject(m_pData[i]);
        }
    }

    void CArrayObject::Serialize(char* pBuffer) const {
        for(size_t i = 0; i < m_pData.Length(); i++) {
            IObject* pCurrent = m_pData[i];

            pCurrent->Serialize(pBuffer);
            pBuffer += pCurrent->Size();
        }
    }

    void CArrayObject::Merge(CObject pSource) {
        for(size_t i = 0; i < pSource.Length(); i++) {
            m_pData.PushBack( CopyObject(pSource[i]) );
        }
    }
}

// CDictObject

namespace cbpp::cdf {
    int_t CDictObject::AsInt() const {
        return 0;
    }

    float_t CDictObject::AsFloat() const {
        return 0.0f;
    }

    const char* CDictObject::AsString() const {
        return s_sName;
    }

    uint8_t* CDictObject::AsBinary() {
        return NULL;
    }

    void CDictObject::SetValue(int_t iData) {}

    void CDictObject::SetValue(float_t fData) {}

    void CDictObject::SetValue(const char* sData) {}

    void CDictObject::SetValue(const uint8_t* pData, size_t iLength) {}

    IObject* CDictObject::At(const char* sName) {
        IObject** pObj = m_dTable.At(sName);
        if(pObj == NULL) { return &g_pNullObject; }
        return *pObj;
    }

    IObject* CDictObject::At(size_t iIndex) {
        if(iIndex >= m_dTable.Length()) { return &g_pNullObject; }
        return m_dTable.Index(iIndex);
    }

    const char* CDictObject::IndexName(size_t iIndex) {
        if(iIndex >= m_dTable.Length()) { return g_sNullString; }
        return m_dTable.IndexKey(iIndex);
    }

    void CDictObject::PushValue(IObject* pObj) { }

    void CDictObject::PushValue(const char* sName, IObject* pObj) {
        m_dTable.Insert(sName, pObj);
    }

    EObjectClass CDictObject::Class() const {
        return EObjectClass::Object;
    }

    size_t CDictObject::Length() const {
        return m_dTable.Length();
    }

    size_t CDictObject::Size() const {
        size_t iLen = 0;

        for( size_t i = 0; i < m_dTable.Length(); i++ ) {
            iLen += m_dTable.Index(i)->Size();
        }

        return iLen;
    }

    CDictObject::~CDictObject() {
        for(size_t i = 0; i < m_dTable.Length(); i++) {
            DeleteObject(m_dTable.Index(i));
        }
    }

    void CDictObject::Serialize(char* pBuffer) const {
        for(size_t i = 0; i < m_dTable.Length(); i++) {
            IObject* pCurrent = m_dTable.Index(i);

            pCurrent->Serialize(pBuffer);
            pBuffer += pCurrent->Size();
        }
    }

    void CDictObject::Merge(CObject pSource) {
        for(size_t i = 0; i < pSource.Length(); i++) {
            const char* sCurrentName = pSource.IndexName(i);

            IObject** pTest = m_dTable.At(sCurrentName);

            if(pTest == NULL) {
                m_dTable.Insert( sCurrentName, CopyObject(pSource[i]) );
            } else {
                (*pTest)->Merge(pSource[i]);
            }
        }
    }
}

namespace cbpp::cdf {
    CObject CreateObject(EObjectClass iClass) {
        IObject* pObj = NULL;

        switch(iClass) {
            case EObjectClass::Array:
                pObj = cbpp::New<CArrayObject>(); break;

            case EObjectClass::Binary:
                pObj = cbpp::New<CBinaryObject>(); break;

            case EObjectClass::Float:
                pObj = cbpp::New<CFloatObject>(); break;

            case EObjectClass::Integer:
                pObj = cbpp::New<CIntObject>(); break;

            case EObjectClass::Object:
                pObj = cbpp::New<CDictObject>(); break;

            case EObjectClass::String:
                pObj = cbpp::New<CStringObject>(); break;

            case EObjectClass::Nil:
                return NIL;

            default:
                CbAssertf(true, "Unknown object type (%i), update le enum", iClass);
        }

        return CObject(pObj);
    }

    void DeleteObject(CObject pObj) {
        if( pObj == cdf::NIL ) {
            return;
        }

        cbpp::Delete(pObj.GetPointer());
    }

    void PrintTabs(size_t iNum) {
        for(size_t i = 0; i < iNum; i++) {
            printf("\t");
        }
    }

    void PrintObject(CObject pObj, size_t iDepth) {
        if(pObj == cdf::NIL) {
            PrintTabs(iDepth);
            puts("NIL");
        }

        switch(pObj.Class()) {
            case EObjectClass::Array: {
                for(size_t i = 0; i < pObj.Length(); i++) {
                    PrintTabs(iDepth);
                    printf("[%li] ", i);
                    if(pObj[i].Class() == EObjectClass::Object || pObj[i].Class() == EObjectClass::Array) {
                        putc('\n', stdout);
                    }
                    PrintObject( pObj[i], iDepth+1 );
                }
                break;
            }

            case EObjectClass::Object: {
                for(size_t i = 0; i < pObj.Length(); i++) {
                    PrintTabs(iDepth);
                    printf("%s : ", pObj.IndexName(i));
                    if(pObj[i].Class() == EObjectClass::Object || pObj[i].Class() == EObjectClass::Array) {
                        putc('\n', stdout);
                    }
                    PrintObject( pObj[i], iDepth+1 );
                }
                break;
            }

            case EObjectClass::Integer: {
                printf("%d\n", (int_t)(pObj));
                break;
            }

            case EObjectClass::String: {
                printf("%s\n", (const char*)(pObj));
                break;
            }

            case EObjectClass::Float: {
                printf("%f\n", (float_t)pObj);
                break;
            }

            case EObjectClass::Binary: {
                printf("Binary [%li]\n", pObj.Length());
                break;
            }
        }
    }

    CObject CopyObject(CObject pObj) {
        if(pObj == cdf::NIL) { return cdf::NIL; }

        switch( pObj.Class() ) {
            case EObjectClass::Integer: {
                CObject pNew = CreateObject(EObjectClass::Integer);
                pNew = pObj.AsInt();
                return pNew;
            }

            case EObjectClass::Float: {
                CObject pNew = CreateObject(EObjectClass::Float);
                pNew = pObj.AsFloat();
                return pNew;
            }

            case EObjectClass::String: {
                CObject pNew = CreateObject(EObjectClass::String);
                pNew = pObj.AsString();
                return pNew;
            }

            case EObjectClass::Binary: {
                CObject pNew = CreateObject(EObjectClass::Binary);
                pNew.SetBinaryData( pObj.AsBinary(), pObj.Length() );
                return pNew;
            }

            case EObjectClass::Array: {
                CObject pNew = CreateObject(EObjectClass::Array);

                for(size_t i = 0; i < pObj.Length(); i++) {
                    pNew.Push( CopyObject(pObj[i]) );
                }

                return pNew;
            }

            case EObjectClass::Object: {
                CObject pNew = CreateObject(EObjectClass::Object);

                for(size_t i = 0; i < pObj.Length(); i++) {
                    const char* sName = pObj.IndexName(i);
                    CObject pValue = pObj[i];

                    pNew.Push( sName, CopyObject(pValue) );
                }

                return pNew;
            }
        }

        return cdf::NIL;
    }

    CPathAccess::CPathAccess(CObject pObj, EPathError iStatus) : m_pPathObj(pObj), m_iStatus(iStatus) { }

    CPathAccess::operator CObject() const {
        return m_pPathObj;
    }

    CObject CPathAccess::Object() const {
        return m_pPathObj;
    }

    EPathError CPathAccess::Status() const {
        return m_iStatus;
    }

    CPathAccess AccessObject(CObject pObj, const char* sPath) {
        CbAssert(true, "FEATURE NOT IMPLEMENTED");
        return CPathAccess( cdf::NIL, EPathError::NotFound );
    }
}

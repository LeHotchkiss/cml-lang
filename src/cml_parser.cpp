#include "CML.h"

#include <ctype.h>
#include <stdio.h>

#include "src/settings.h"

namespace cml {
    static const char* g_aErrorTexts[] = {
        "Ok",

        "Stray identifier",
        "Stray number",
        "Stray keyword",
        "Stray string",
        "Stray table",
        "Stray array",
        "Braces mismatch",

        "Undefined file reference",
        "A non-string value is marked as reference",
        "Concat not in a table, or not from a table",
        "Can`t find concat source",
        "Can`t override value of different type",

        "Not a number",
        "Source file not found",
        "Unexpected EOF",
        "Source file version is incompatible",
        "Bad version value",
        "Undefined escape character",
        "Undefined number type suffix",

        "Stack overflow",
        "Stack underflow",
        "Referenced file size exceeds the limit"
    };

    static const char* g_aPathErrorTexts[] = {
        "Ok",
        "Entry not found",
        "Indexing a table",
        "Accessing an array with '.'",
        "Bad array index",
        "Bad name separator"
    };

    static const char* g_aClassNames[] = {
        "Table",
        "Array",
        "Binary",
        "Integer",
        "Float",
        "String"
    };

    const char* StringError(ETextError iType) {
        return g_aErrorTexts[(size_t)iType];
    }

    const char* StringError(EPathError iType) {
        return g_aPathErrorTexts[(size_t)iType];
    }

    const char* ClassString(EObjectClass iClass) {
        return g_aClassNames[(size_t)iClass];
    }

    CTextParser::IncludeNode::~IncludeNode() {
        if(pFile != NULL) {
            s_callbacksInfo.fileClose(pFile);
            pFile = NULL;
        }
    }

    bool CTextParser::IsValidNameStart(int iChar) {
        return isalpha(iChar) || (iChar == '_');
    }

    CTextParser::EKeyword CTextParser::IsKeyword(const char* sName) {
        if( strcmp(sName, "include") == 0 ) {
            return EKeyword::Include;
        } else if( strcmp(sName, "version") == 0 ) {
            return EKeyword::Version;
        } else if( strcmp(sName, "true") == 0 ) {
            return EKeyword::True;
        } else if( strcmp(sName, "false") == 0 ) {
            return EKeyword::False;
        } else if( strcmp(sName, "concat") == 0 ) {
            return EKeyword::Inherit;
        } else {   
            return EKeyword::Name;
        }
    }

    ETextError CTextParser::AddFile(const char* sPath) {
        void* pFile = s_callbacksInfo.fileOpen(sPath);
        if(pFile == NULL) {
            return ETextError::NoFile;
        }

        m_aFilesStack.Push( {sPath, pFile, m_iLine} );

        m_iCol = 1;
        m_iLine = 1;

        return ETextError::Ok;
    }

    int CTextParser::Peek() {
        if(m_aFilesStack.Length() == 0) {
            return 0;
        }

        void* pFile = m_aFilesStack.Head().pFile;

        int iChar = s_callbacksInfo.fileReadByte(pFile);

        if( iChar < 0 ) {          // this file is over, step out and resume    
            m_iCol = 1;
            m_iLine = m_aFilesStack.Head().iLine;  

            m_aFilesStack.Pop();  

            if(m_aFilesStack.Length() > 0) {                
                return Peek();          // peek again
            } else {
                return 0;               // include stack is empty, we are basically done
            }
        }
        
        m_iCol++;
        if(iChar == '\n') {
            m_iCol = 1;
            m_iLine++;
        }

        return iChar;
    }

    ETextError CTextParser::ProcessName() {
        EKeyword iKW = this->IsKeyword(m_sLexemBuffer.Data());

        switch (iKW) {
            case EKeyword::Name: {
                if(m_iPromise == EPromise::Value || m_bInsideArray) {
                    return ETextError::StrayIdentifier;
                }

                m_iPromise = EPromise::Value;

                m_sCurrentName.Set( m_sLexemBuffer.Data() );
                break;
            }
            
            case EKeyword::Version: {
                if(m_iPromise != EPromise::None) {
                    return ETextError::StrayKeyword;
                }

                m_iPromise = EPromise::Version;

                break;
            }
            
            case EKeyword::Include: {
                if(m_iPromise != EPromise::None) {
                    return ETextError::StrayKeyword;
                }

                m_iPromise = EPromise::IncludePath;

                break;
            }

            case EKeyword::True: {
                if(m_iPromise != EPromise::Value && !m_bInsideArray) {
                    return ETextError::StrayKeyword;
                }

                m_iPromise = EPromise::None;

                const char* sName = m_bInsideArray ? NULL : m_sCurrentName.String();
                this->PushInt( sName, 1 );
                break;
            }

            case EKeyword::False: {
                if(m_iPromise != EPromise::Value && !m_bInsideArray) {
                    return ETextError::StrayKeyword;
                }

                m_iPromise = EPromise::None;

                const char* sName = m_bInsideArray ? NULL : m_sCurrentName.String();
                this->PushInt( sName, 0 );
                break;
            }

            case EKeyword::Inherit: {
                if(m_iPromise != EPromise::None) {
                    return ETextError::StrayKeyword;
                }

                if( m_aStack.Head().Class() != EObjectClass::Object ) {
                    return ETextError::BadInheritance;
                }

                m_iPromise = EPromise::ParentPath;

                break;
            }
        }

        return ETextError::Ok;
    }
    
    ETextError CTextParser::ParseName(int iChar) {
        m_sLexemBuffer.Clear();

        m_sLexemBuffer.PushBack(iChar);

        volatile int iCurrent = 1;
        while(iCurrent != 0) {
            iCurrent = this->Peek();

            if( isalnum(iCurrent) || (iCurrent == '_') ) {
                m_sLexemBuffer.PushBack(iCurrent);
            } else {    // name ends
                m_sLexemBuffer.PushBack('\0');

                ETextError iRet = this->ProcessName();
                if(iRet != ETextError::Ok) { return iRet; }

                iRet = this->ProcessChar(iCurrent);
                if(iRet != ETextError::Ok) { return iRet; }

                return ETextError::Ok;
            }
        }

        return ETextError::UnexpectedEOF;
    }
    
    ETextError CTextParser::ParseString(int iChar) {
        m_sLexemBuffer.Clear();

        bool bExpectEscape = false;
        
        volatile int iCurrent = 1;
        while(iCurrent != 0) {
            iCurrent = this->Peek();

            if(bExpectEscape == true) {
                bExpectEscape = false;

                switch (iCurrent) {
                    case 'n': {
                        m_sLexemBuffer.PushBack('\n');
                        break;
                    }

                    case 't': {
                        m_sLexemBuffer.PushBack('\t');
                        break;
                    }

                    case '\\': {
                        m_sLexemBuffer.PushBack('\\');
                        break;
                    }

                    case '"': {
                        m_sLexemBuffer.PushBack('"');
                        break;
                    }

                    default: {
                        m_sLexemBuffer.PushBack('\0');
                        return ETextError::BadEscapeChar;
                    }
                }
                
            } else {
                if(iCurrent == '\\') {
                    bExpectEscape = true;
                    continue;
                }

                if(iCurrent == '"') {
                    m_sLexemBuffer.PushBack('\0');
                    return ETextError::Ok;
                }

                m_sLexemBuffer.PushBack(iCurrent);
            }
        }

        return ETextError::UnexpectedEOF;
    }

    ETextError CTextParser::ProcessNumber() {
        if(!m_bInsideArray && m_iPromise == EPromise::None) {
            return ETextError::StrayNumber;
        }

        if(m_iRefType != ERefType::NoLink) {
            return ETextError::BadReference;
        }

        int iNumberTest = hlib::IsNumber(m_sLexemBuffer.Data());

        float_t fData = 0.0f;
        int_t iData = 0;
        EObjectClass iClass = EObjectClass::Integer;

        switch (iNumberTest) {
            case 1: {
                iClass = EObjectClass::Float;
                fData = (float_t)strtof(m_sLexemBuffer.Data(), NULL);
                break;
            }
                
            case 0: {
                iClass = EObjectClass::Integer;
                iData = (int_t)atoi(m_sLexemBuffer.Data());
                break;
            }

            case -1: {
                return ETextError::BadNumber;
            }
        }
        
        switch (m_iForceNumberType) {
            case EForceNumberType::ForceInteger: {
                iClass = EObjectClass::Integer;
                iData = (fData != 0.0f) ? (int_t)fData : iData;         // apply the forced number type
                break;
            }

            case EForceNumberType::ForceFloat: {
                iClass = EObjectClass::Float;
                fData = (iData != 0) ? (float_t)iData : fData;
                break;
            }
        }
        
        if(m_iPromise == EPromise::Version) {
            if(iNumberTest != 0 || iData <= 0) {
                return ETextError::BadVersion;
            }

            if(iData > CML_VERSION || iData < CML_VERSION_LEAST) {
                return ETextError::VersionMismatch;
            }
            
        } else {
            if(!m_bInsideArray && m_iPromise == EPromise::None) {
                return ETextError::StrayNumber;
            }

            const char* sName = m_bInsideArray ? NULL : m_sCurrentName.String();

            if(iClass == EObjectClass::Integer) {
                this->PushInt(sName, iData);
            } else {
                this->PushFloat(sName, fData);
            }
        }

        m_iPromise = EPromise::None;

        return ETextError::Ok;
    }

    ETextError CTextParser::ParseNumber(int iChar) {
        m_sLexemBuffer.Clear();
        m_sLexemBuffer.PushBack(iChar);

        volatile int iCurrent = 1;
        while(iCurrent != 0) {
            iCurrent = this->Peek();

            if(isdigit(iCurrent) || (iCurrent == '.') || (iCurrent == 'e') || (iCurrent == 'E') || (iCurrent == '-')) {
                m_sLexemBuffer.PushBack(iCurrent);
            } else {
                m_sLexemBuffer.PushBack('\0');

                if(isalpha(iCurrent)) { // we have a type suffix
                    if(iCurrent == 'i') {
                        m_iForceNumberType = EForceNumberType::ForceInteger;
                    } else if(iCurrent == 'f') {
                        m_iForceNumberType = EForceNumberType::ForceFloat;
                    } else {
                        return ETextError::BadNumberSuffix;
                    }
                }
                
                ETextError iRet = this->ProcessNumber();
                if(iRet != ETextError::Ok) { return iRet; }

                if( m_iForceNumberType == EForceNumberType::None ) {
                    iRet = this->ProcessChar(iCurrent);
                } else {
                    m_iForceNumberType = EForceNumberType::None;
                }

                if(iRet != ETextError::Ok) { return iRet; }

                return ETextError::Ok;
            }
        }
        
        return ETextError::UnexpectedEOF;
    }

    int GetOppositeScobe(int iScobe) {
        switch(iScobe) {
            case '{' : return '}';
            case '[' : return ']';
            case '}' : return '{';
            case ']' : return '[';
        }

        return '\0';
    }

    ETextError CTextParser::CheckScobes(int iCurrent) {
        if(m_aScobesStack.Length() == 0) {
            return ETextError::ScobeMiss;
        }

        ScobeNode Node = m_aScobesStack.Head();
        if( Node.iLevel != m_aStack.Length() || Node.iScobe != GetOppositeScobe(iCurrent) ) {
            return ETextError::ScobeMiss;
        }
        m_aScobesStack.Pop();

        return ETextError::Ok;
    }

    ETextError CTextParser::ProcessChar(int iCurrent) {
        if(this->IsValidNameStart(iCurrent)) {                                      // NAME
            ETextError iRet = this->ParseName(iCurrent);
            if(iRet != ETextError::Ok) { return iRet; }
            
        } else if(iCurrent == '"') {                                                // STRING
            if(!m_bInsideArray && m_iPromise == EPromise::None) {
                return ETextError::StrayString;
            }
            
            ETextError iRet = this->ParseString(iCurrent);
            if(iRet != ETextError::Ok) { return iRet; }

            switch(m_iPromise) {
                case EPromise::ParentPath: {
                    CObject pHead = m_aStack.Head();
                    CPathAccess access = AccessObject(m_pRootObject, m_sLexemBuffer.Data());//m_pRootObject.Access(m_sLexemBuffer.Data());

                    CObject pParent = access.Object();

                    if(pParent == cml::NIL) {
                        return ETextError::ParentNotFound;
                    }
                    
                    for(size_t i = 0; i < pParent.Length(); i++) {
                        const char* sName = pParent.IndexName(i);
                        CObject pParents = pParent[i];
                        CObject pOurs = pHead[sName];

                        if(pOurs == cml::NIL) {
                            pHead.Push(sName, CopyObject(pParents));            // new value
                            
                        } else {
                            if(pOurs.Class() != pParents.Class()) {
                                return ETextError::OverrideTypeMix;
                            }
                            
                            if(pOurs.Class() == EObjectClass::Array) {          // arrays are concatenated
                                for(size_t k = 0; k < pParents.Length(); k++) {
                                    pOurs.Push( CopyObject(pParents[k]) );
                                }

                            } else {
                                DeleteObject(pOurs);                            // other values are overriden
                                pHead.Push(sName, CopyObject(pParents)); 
                            }
                        }
                    }
                    
                    break;
                }
                
                case EPromise::IncludePath: {
                    if(m_bAllowIncludes) {
                        iRet = AddFile(m_sLexemBuffer.Data());  // including
                        if(iRet != ETextError::Ok) {
                            return iRet;
                        }
                    }

                    break;
                }
                
                default: {
                    if(!m_bInsideArray && m_iPromise == EPromise::None) {
                        return ETextError::StrayString;
                    }

                    CObject pStringObj;

                    CObject pHead = m_aStack.Head();
                    CObject pTest = pHead[m_sCurrentName.String()];

                    if(!m_bInsideArray && pTest != cml::NIL) { // overriding
                        DeleteObject(pTest);
                    }
                    
                    if(m_iRefType == ERefType::NoLink) {
                        pStringObj = CreateObject(EObjectClass::String);
                        pStringObj = m_sLexemBuffer.Data();
                    } else {
                        pStringObj = this->ResolveFileRef(iRet);

                        if(pStringObj == cml::NIL) {
                            return iRet;
                        }

                        m_iRefType = ERefType::NoLink;
                    }

                    if(!m_bInsideArray) {
                        pHead.Push(m_sCurrentName.String(), pStringObj);
                    } else {
                        pHead.Push(pStringObj);
                    }
                }                    
            }
            
            m_iPromise = EPromise::None;
            
        } else if(isdigit(iCurrent) || (iCurrent == '-') || (iCurrent == '.')) {    // NUMBER
            ETextError iRet = this->ParseNumber(iCurrent);
            if(iRet != ETextError::Ok) { return iRet; }
            
        } else if(iCurrent == '{') {                                                // BLOCK OPENING
            if(!m_bInsideArray && m_iPromise == EPromise::None) {
                return ETextError::StrayBlock;
            }

            if(m_iRefType != ERefType::NoLink) {
                return ETextError::BadReference;
            }

            if(m_aStack.Length() == CML_STACK_LIMIT) {
                return ETextError::StackOverflow;
            }
            
            CObject pHead = m_aStack.Head();
            CObject pDict = pHead[m_sCurrentName.String()];

            if(pDict == cml::NIL) {
                pDict = CreateObject(EObjectClass::Object);
            }

            if(!m_bInsideArray) {
                pHead.Push(m_sCurrentName.String(), pDict);
            } else {
                pHead.Push(pDict);
            }

            m_aStack.Push(pDict);
            m_aScobesStack.Push( {'{', m_aStack.Length()} );
            
            m_iPromise = EPromise::None;
            
        } else if(iCurrent == '}' || iCurrent == ']') {                             // SCOPE CLOSING
            if(m_aStack.Length() <= 1) {
                return ETextError::StackUnderflow;
            }

            ETextError iRet = this->CheckScobes(iCurrent);
            if(iRet != ETextError::Ok) { return iRet; }
            
            m_aStack.Pop();
            
        } else if(iCurrent == '[') {                                                // ARRAY OPENING
            if(m_iPromise == EPromise::None && !m_bInsideArray) {
                return ETextError::StrayArray;
            }

            if(m_iRefType != ERefType::NoLink) {
                return ETextError::BadReference;
            }

            if(m_aStack.Length() == CML_STACK_LIMIT) {
                return ETextError::StackOverflow;
            }

            CObject pHead = m_aStack.Head();
            CObject pArr = pHead[m_sCurrentName.String()];
            
            if(pArr == cml::NIL) {
                pArr = CreateObject(EObjectClass::Array);
            }

            if(!m_bInsideArray) {
                pHead.Push(m_sCurrentName.String(), pArr);
            } else {
                pHead.Push(pArr);
            }
            
            m_aStack.Push(pArr);
            m_aScobesStack.Push( {'[', m_aStack.Length()} );

            m_iPromise = EPromise::None;

        } else if(iCurrent == '#') {                                                // COMMENTARY
            int iCommChar = 1;
            while(iCommChar != 0) {
                iCommChar = this->Peek();
                if(iCommChar == '\n' || iCommChar == '#') {
                    break;
                }
            }
            
        } else if(iCurrent == '@') {
            m_iRefType = ERefType::Text;                                            // REFERENCES

        } else if(iCurrent == '&') {
            m_iRefType = ERefType::Binary;
            
        } else if(iCurrent == '\n') {
            if(m_iPromise != EPromise::None) {
                m_iLine -= 2;
                return ETextError::StrayKeyword;
            }
        }
        
        return ETextError::Ok;
    }

    CObject CTextParser::ResolveFileRef(ETextError& iCode) {
        void* pFile = s_callbacksInfo.fileOpen(m_sLexemBuffer.Data());

        if(pFile == NULL) {
            iCode = ETextError::BadFileRef;
            return cml::NIL;
        }

        char* pBuffer = NULL;
        const size_t iFileLen = s_callbacksInfo.fileLength(pFile);

        if(iFileLen > CML_MAX_REFFILE) {
            iCode = ETextError::RefHugeFile;
            return cml::NIL;
        }

        CObject pRet;

        if(m_iRefType == ERefType::Text) {
            pBuffer = cml_alloc_t<char>::Malloc(iFileLen+1);
            pBuffer[iFileLen] = '\0';
            s_callbacksInfo.fileReadFull(pFile, pBuffer);

            pRet = CreateObject(EObjectClass::String);
            pRet = (const char*)(pBuffer);

        } else if(m_iRefType == ERefType::Binary) {
            pBuffer = cml_alloc_t<char>::Malloc(iFileLen);
            s_callbacksInfo.fileReadFull(pFile, pBuffer);

            pRet = CreateObject(EObjectClass::Binary);
            pRet.SetBinaryData((const uint8_t*)pBuffer, iFileLen);

        }

        cml_alloc_t<char>::Free(pBuffer);
        return pRet;
    }

    void CTextParser::Reset() {
        m_iPromise = EPromise::None;
        m_bInsideArray = false;

        m_aFilesStack.Clear();
        m_aStack.Clear();
        m_iRefType = ERefType::NoLink;
        m_iForceNumberType = EForceNumberType::None;
        
        m_iCol = 1; m_iLine = 1;

        DeleteObject(m_pRootObject);
        m_pRootObject = cml::NIL;
    }

    ETextError CTextParser::PushFloat(const char* sName, float_t fValue) {
        if(m_aStack.Length() == 0) {
            return ETextError::StackUnderflow;
        }

        CObject pHead = m_aStack.Head();
        CObject pTest = pHead[sName];

        if(pTest != cml::NIL) {
            if( pTest.Class() == EObjectClass::Float ) {
                pTest = fValue;
            } else {
                return ETextError::OverrideTypeMix;
            }
        } else {
            if( sName == NULL ) {
                pHead.Push(fValue);
            } else {
                pHead.Push(sName, fValue);
            }
        }

        return ETextError::Ok;
    }

    ETextError CTextParser::PushInt(const char* sName, int_t iValue) {
        if(m_aStack.Length() == 0) {
            return ETextError::StackUnderflow;
        }

        CObject pHead = m_aStack.Head();
        CObject pTest = pHead[sName];

        if(pTest != cml::NIL) {
            if( pTest.Class() == EObjectClass::Integer ) {
                pTest = iValue;
            } else {
                return ETextError::OverrideTypeMix;
            }
        } else {
            if( sName == NULL ) {
                pHead.Push(iValue);
            } else {
                pHead.Push(sName, iValue);
            }
        }

        return ETextError::Ok;
    }

    ETextError CTextParser::Parse(const char* sPath, bool bAllowIncludes) {
        this->Reset();

        m_bAllowIncludes = bAllowIncludes;

        ETextError iRet = AddFile(sPath);

        if(iRet != ETextError::Ok) { return iRet; }

        m_pRootObject = CreateObject(EObjectClass::Object);

        m_aStack.Push(m_pRootObject);

        int iCurrent = 1;
        while(iCurrent != 0) {
            iCurrent = this->Peek();

            m_bInsideArray = m_aStack.Head().Class() == EObjectClass::Array;

            iRet = this->ProcessChar(iCurrent);
            if(iRet != ETextError::Ok) { return iRet; }
        }
        
        if( m_aScobesStack.Length() != 0 ) {
            return ETextError::ScobeMiss;
        }
        
        return ETextError::Ok;
    }
    
    size_t CTextParser::FormatError(ETextError iCode, char* sBuffer, size_t iBufferLn) {
        const char* sError = StringError(iCode);
        const char* sFileSrc;
        
        if(m_aFilesStack.Length() > 0) {
            sFileSrc = m_aFilesStack.Head().sPath.String();
        } else {
            sFileSrc = "(none)";
        }

        const char* pSlash = strrchr(sFileSrc, '/') ;
        const char* sFile = (pSlash == NULL) ? sFileSrc : pSlash + 1;

        return snprintf(sBuffer, iBufferLn, "%s:%i.%i %s (near '%s')", sFile, m_iLine, m_iCol, sError, m_sLexemBuffer.Data());
    }

    CObject CTextParser::Root() {
        return m_pRootObject;
    }

    CPathAccess CTextParser::operator[](const char* sPath) {
        if(m_pRootObject == cml::NIL) { return CPathAccess(cml::NIL, EPathError::NotFound); }
        return AccessObject(m_pRootObject, sPath);
    }

    CTextParser& CTextParser::operator=(const CTextParser& Other) {
        if(m_pRootObject != cml::NIL) {
            DeleteObject(m_pRootObject);
        }

        m_pRootObject = CopyObject(Other.m_pRootObject);

        return *this;
    }

    CTextParser::~CTextParser() {
        DeleteObject(m_pRootObject);
    }
}

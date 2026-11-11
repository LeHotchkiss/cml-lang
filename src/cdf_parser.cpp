#include "CML.h"

namespace cml {
    EBinaryError CBinaryParser::ReadNametable() {
        if(m_nametable.Length() == 0) {
            return EBinaryError::Ok;
        }

        array_t<char> buff;

        while(true) {
            int c = m_file->GetChar();

            if(c == -1) {
                return EBinaryError::UnexpectedEOF;
            }

            buff.PushBack(c);

            if(c == '\0') {
                m_nametable.PushBack(buff.Data());

                if(m_nametable.Length() >= m_header.nametableLen) {
                    return EBinaryError::Ok;
                }
            }
        }

        return EBinaryError::Ok;
    }

    const char* CBinaryParser::LookupName(uint32_t idx) {
        if(idx >= m_nametable.Length()) {
            return NULL;
        }

        return m_nametable[idx].String();
    }

    CObject CBinaryParser::ReadObject(EBinaryError& code, SObjectInfo& info) {
        size_t read = m_file->Read(&info, sizeof(info));

        if(read != sizeof(info)) {
            code = EBinaryError::UnexpectedEOF;
            return NIL;
        }

        if(info.type >= (int32_t)EObjectClass::LAST) {
            code = EBinaryError::BadType;
            return NIL;
        }
        
        EObjectClass type = (EObjectClass)info.type; // i despise this enum casting behaviour
        
        CObject obj = CreateObject(type);
        
        if(obj == NIL && type != EObjectClass::Nil) {
            code = EBinaryError::AllocFailure;
            return NIL;
        }
        
        switch(type) {
            case EObjectClass::Object : {
                SObjectInfo childInfo;
                for(size_t i = 0; i < info.length; i++) {
                    CObject current = this->ReadObject(code, childInfo);

                    if(code == EBinaryError::Ok) {
                        const char* objName = this->LookupName(childInfo.nameID);

                        if(objName == NULL) {
                            DeleteObject(obj);
                            DeleteObject(current);
                            code = EBinaryError::BadNameID;
                            return NIL;
                        } else {
                            obj.Push(objName, current);
                        }
                    } else {
                        DeleteObject(obj);
                        return NIL;
                    }
                }
                return obj;
            }
            
            case EObjectClass::Array : {
                for(size_t i = 0; i < info.length; i++) {
                    SObjectInfo childInfo;
                    CObject current = this->ReadObject(code, childInfo);

                    if(code == EBinaryError::Ok) {
                        obj.Push(current);
                    } else {
                        DeleteObject(obj);
                        return NIL;
                    }
                }
                return obj;
            }

            case EObjectClass::Nil : {
                return NIL;
            }
            
            default: {
                array_t<unsigned char> data(info.length);

                read = m_file->Read(data.Data(), info.length);

                if(read != info.length) {
                    code = EBinaryError::UnexpectedEOF;
                    DeleteObject(obj);
                    return NIL;
                }
                
                switch(type) {
                    case EObjectClass::Binary : {
                        obj.SetBinaryData(data.Data(), data.Length());
                        break;
                    }

                    case EObjectClass::String : {       // strings don`t have their null written in the file, we append it here
                        data.PushBack('\0');
                        obj = (const char*)data.Data();
                        break;
                    }
                    
                    case EObjectClass::Float : {
                        float_t fltData;
                        char fltBuffer[sizeof(fltData)];
                        memset(fltBuffer, 0, sizeof(fltBuffer));

                        size_t dataLen = (data.Length() > sizeof(float_t)) ? sizeof(float_t) : data.Length();
                        memcpy(fltBuffer, data.Data(), dataLen);

                        fltData = *((float_t*)(fltBuffer));
                        obj = fltData;
                        break;
                    }

                    case EObjectClass::Integer : {
                        int_t intData;
                        char intBuffer[sizeof(intData)];
                        memset(intBuffer, 0, sizeof(intBuffer));

                        size_t dataLen = (data.Length() > sizeof(int_t)) ? sizeof(int_t) : data.Length();
                        memcpy(intBuffer, data.Data(), dataLen);

                        intData = *((int_t*)(intBuffer));
                        obj = intData;
                        break;
                    }
                }

                return obj;
            }
        }
    }

    void CBinaryParser::Reset() {
        m_nametable.Clear();

        if(m_file != NULL) {
            hlib::FileClose(m_file);
            m_file = NULL;
        }

        if(m_root != NIL) {
            DeleteObject(m_root);
            m_root = NIL;
        }
    }

    EBinaryError CBinaryParser::Parse(const char* sPath) {
        using namespace hlib;

        this->Reset();

        m_file = FileOpen(sPath, EFileMode::Read);

        if(m_file == NULL) {
            return EBinaryError::NoFile;
        }

        size_t read = m_file->Read(&m_header, sizeof(m_header));

        if(read != sizeof(m_header)) {
            FileClose(m_file);
            m_file = NULL;
            return EBinaryError::UnexpectedEOF;
        }

        if(m_header.version.major != CDF_VERSION_MAJOR) {
            FileClose(m_file);
            m_file = NULL;
            return EBinaryError::VersionMismatch;
        }

        EBinaryError code = this->ReadNametable();

        if(code != EBinaryError::Ok) {
            FileClose(m_file);
            m_file = NULL;
            return code;
        }

        SObjectInfo info;
        CObject root = this->ReadObject(code, info);

        if(code != EBinaryError::Ok) {
            FileClose(m_file);
            m_file = NULL;
            DeleteObject(root);
            return code;
        }

        m_root = root;

        FileClose(m_file);
        m_file = NULL;

        return EBinaryError::Ok;
    }

    CBinaryParser::~CBinaryParser() {
        this->Reset();
    }
}

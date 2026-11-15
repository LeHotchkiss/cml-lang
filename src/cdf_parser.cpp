#include "CML.h"

#include "hlib/New.h"

namespace cml {
    void CBinaryParser::Reset() {
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
        
    }

    CBinaryParser::~CBinaryParser() {
        this->Reset();
    }
}

#include "compress_reader.h"

using namespace hlib;

namespace cml {
    IFile* OpenZipped(const char* sPath, int compression) {
        return hlib::FileOpen(sPath, hlib::EFileMode::Write);

        CZippedFile* file = hlib::New<CZippedFile, cml_alloc_t, &cml::g_defaultAlloc>(compression);

        if(!file->Open(sPath, hlib::EFileMode::Write)) {
            hlib::Delete<CZippedFile, cml_alloc_t, &cml::g_defaultAlloc>(file);
            return NULL;
        }

        return (hlib::IFile*)file;
    }

    EBinaryError WriteBinary(const char* sPath, CObject pObj, int compression) {
        IFile* fp = OpenZipped(sPath, compression);

        SCDFHead header;
            header.formatNum        = CDF_MAGIC_NUMBER;
            header.version.major    = CDF_VERSION_MAJOR;
            header.version.minor    = CDF_VERSION_MINOR;

        size_t bytes = fp->Write(&header, sizeof(header));
        if(bytes != sizeof(header)) {
            FileClose(fp);
            return EBinaryError::WriteError;
        }

        EBinaryError code = pObj.Serialize(fp, NULL);

        if(code != EBinaryError::Ok) {
            FileClose(fp);
            return code;
        }

        return EBinaryError::Ok;
    }
}

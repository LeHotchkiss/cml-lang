#include "CML.h"

namespace cml {
    EBinaryError CBinaryParser::ReadNametable() {
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
    
    EBinaryError CBinaryParser::Parse(const char* sPath) {
        using namespace hlib;

        m_file = FileOpen(sPath, EFileMode::Read);

        if(m_file == NULL) {
            return EBinaryError::NoFile;
        }

        size_t read = m_file->Read(&m_header, sizeof(m_header));

        if(read != sizeof(m_header)) {
            FileClose(m_file);
            return EBinaryError::UnexpectedEOF;
        }

        if(m_header.version.major != CDF_VERSION_MAJOR) {
            FileClose(m_file);
            return EBinaryError::VersionMismatch;
        }

        EBinaryError code = this->ReadNametable();

        if(code != EBinaryError::Ok) {
            FileClose(m_file);
            return code;
        }

        return EBinaryError::Ok;
    }
}

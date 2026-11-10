#ifndef CML_COMPRESSED_READER_H
#define CML_COMPRESSED_READER_H

#include "hlib/Filesystem.h"

namespace cml {
    class CZippedFile : public hlib::IFile {
        virtual bool Open(const char*, int) = 0;
        virtual void Close() = 0;

        virtual int GetChar() const = 0;
        virtual void PutChar(int) = 0;

        virtual void SetPos(hlib::EFilePos, int64_t) = 0;
        virtual size_t GetPos() const = 0;
        virtual void Rewind() = 0;

        virtual size_t Write(void*, size_t) = 0;
        virtual size_t Read(void*, size_t) = 0;

        virtual hlib::SFileInfo GetInfo() const = 0;
    };
}

#endif

#ifndef CML_COMPRESSED_READER_H
#define CML_COMPRESSED_READER_H

#include "hlib/Filesystem.h"

/*
    File IO class to shadow zlib compression reading API
*/

namespace cml {
    #ifndef CDF_NO_COMPRESSION
    class CZippedFile final : public hlib::IFile {
        void* m_fd = NULL;
        public:
            bool Open(const char*, int);
            void Close();

            int GetChar() const;
            void PutChar(int);

            void SetPos(hlib::EFilePos, int64_t);
            size_t GetPos() const;
            void Rewind();

            size_t Write(void*, size_t);
            size_t Read(void*, size_t);

            hlib::SFileInfo GetInfo() const;
    };
    #endif
}

#endif

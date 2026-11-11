#include "compress_reader.h"

#include <string.h>

#include "hlib/Bit.h"

#ifndef CDF_NO_COMPRESSION
    #include "zlib/zlib.h"
#endif

#define GZ_FD ((gzFile)m_fd)

int gzFilePos(hlib::EFilePos pos) {
    switch (pos) {
        case hlib::EFilePos::Start:   return SEEK_SET;
        case hlib::EFilePos::Current: return SEEK_CUR;
        case hlib::EFilePos::End:     return SEEK_END;
    }

    return -1;
}

namespace cml {
    bool CZippedFile::Open(const char* path, int modes) {
        char c_modes[8];
        memset(c_modes, 0, sizeof(c_modes));

        if(hlib::TestBitmask<int>(modes, hlib::EFileMode::Write)) {
            strcat(c_modes, "w");
        } else if(hlib::TestBitmask<int>(modes, hlib::EFileMode::Read)) {
            strcat(c_modes, "r");
        }

        if(hlib::TestBitmask<int>(modes, hlib::EFileMode::Append)) {
            strcat(c_modes, "a");
        }

        m_fd = gzopen(path, c_modes);

        return m_fd != NULL;
    }

    void CZippedFile::Close() {
        if(m_fd != NULL) {
            gzclose(GZ_FD);
            m_fd = NULL;
        }
    }

    int CZippedFile::GetChar() const {
        return gzgetc(GZ_FD);
    }

    void CZippedFile::PutChar(int ch) {
        gzputc(GZ_FD, ch);
    }

    void CZippedFile::SetPos(hlib::EFilePos pos, int64_t offset) {
        const int whence = gzFilePos(pos);

        gzseek(GZ_FD, (z_off_t)offset, whence);
    }

    size_t CZippedFile::GetPos() const {
        const z_off_t pos = gztell(GZ_FD);
        return pos < 0 ? 0 : (size_t)pos;
    }

    void CZippedFile::Rewind() {
        gzrewind(GZ_FD);
    }

    size_t CZippedFile::Write(void* data, size_t length) {
        return gzwrite(GZ_FD, data, length);
    }

    size_t CZippedFile::Read(void* data, size_t length) {
        return gzread(GZ_FD, data, length);
    }

    hlib::SFileInfo CZippedFile::GetInfo() const {
        return {};
    }
}

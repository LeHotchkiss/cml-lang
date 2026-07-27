#include "CML.h"

#include <stdio.h>

namespace cml {
    void* OpenFileDefault(const char* sName) {
        return (void*)fopen(sName, "rb");
    }
    cb_openfile_t g_fpOpenFile = OpenFileDefault;

    int GetCharDefault(void* pFile) {
        return fgetc((FILE*)pFile);
    }
    cb_getchar_t g_fpGetChar = GetCharDefault;

    void CloseFileDefault(void* pFile) {
        fclose((FILE*)pFile);
    }
    cb_closefile_t g_fpCloseFile = CloseFileDefault;

    extern cb_filelen_t g_fpFileLen;
    extern cb_readfile_t g_fpReadFile;
}

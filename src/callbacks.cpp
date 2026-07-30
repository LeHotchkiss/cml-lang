#include "CML.h"

#include <stdio.h>

namespace cml {
    void* OpenFileDefault(const char* sName) {
        return (void*)fopen(sName, "rb");
    }

    int GetCharDefault(void* pFile) {
        int c = fgetc((FILE*)pFile);
        if(feof((FILE*)pFile)) {
            return -1;
        }
        return c;
    }

    void CloseFileDefault(void* pFile) {
        fclose((FILE*)pFile);
    }

    size_t FileLengthDefault(void* pFile) {
        FILE* fp = (FILE*)pFile;
        size_t out = 0, oldPos = ftell(fp);

        fseek(fp, 0, SEEK_END);
        out = (size_t)(ftell(fp));
        fseek(fp, oldPos, SEEK_SET);

        return out;
    }

    void ReadFileDefault(void* pFile, char* pBuffer) {
        size_t iLen = FileLengthDefault(pFile);

        fread(pBuffer, iLen, 1, (FILE*)pFile);
    }
}

namespace cml {
    SCallbackInfo s_callbacksInfo = {
        OpenFileDefault,
        CloseFileDefault,
        FileLengthDefault,
        ReadFileDefault,
        GetCharDefault
    };

    void SetFileCallbacks(SCallbackInfo info) {
        s_callbacksInfo = info;
    }
}

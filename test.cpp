#include <stdio.h>

#include "CML.h"

int main(int argc, char** argv) {
    using namespace cml;

    char* sPath = (char*)"./complete_example.cml";

    if(argc > 1) {
        sPath = (char*)argv[1];
    }

    printf("Working with file: %s\n", sPath);

    CTextParser parser;
    ETextError code = parser.Parse(sPath, true);

    if(code == ETextError::Ok) {
        puts("Parsing OK");
        PrintObject( parser.Root() );

    } else {
        char buff[128];
        parser.FormatError(code, buff, sizeof(buff));
        puts(buff);
        return 1;
    }

    char sBuff[512];

    while(1) {
        printf("> "); scanf("%s", sBuff);

        CPathAccess access = AccessObject(parser.Root(), sBuff);

        if(access.Status() != EPathError::Ok) {
            puts(StringError(access.Status()));
            return 1;
        }

        PrintObject(access.Object());
    }

    return 0;
}

#include "CML.h"

#include <stdio.h>

int main() {
    using namespace cml;

    const char* cmlPath = "./cdf_test_source.cml";

    CTextParser parser;

    ETextError code = parser.Parse(cmlPath, true);

    if(code == ETextError::Ok) {
        puts("Parsing OK");
        PrintObject( parser.Root() );

    } else {
        char buff[128];
        parser.FormatError(code, buff, sizeof(buff));
        puts(buff);
        return 1;
    }

    CObject root = parser.Root();

    if(!root) {
        puts("null object");
        return 2;
    }

    EBinaryError code2 = WriteBinary("cdftest.cdf", root, 0);
    printf("write code %d\n", code2);

    if(code2 != EBinaryError::Ok) {
        return 3;
    }

    CBinaryParser parser2;

    code2 = parser2.Parse("cdftest.cdf");
    printf("read code %d\n", code2);

    if(code2 != EBinaryError::Ok) {
        return 4;
    }

    PrintObject(parser2.Root());

    return 0;
}

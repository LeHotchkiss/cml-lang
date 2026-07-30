#ifndef CML_SETTINGS_H
#define CML_SETTINGS_H

#include "hlib/String.h"
#include "hlib/Array.h"
#include "hlib/Table.h"
#include "hlib/Stack.h"

namespace cml {
    /*
        Redefine this to use your custom allocator if needed
    */
    template <typename T>
    using cml_alloc_t = hlib::CDefaultAllocator<T>;

    // Integer type to use
    typedef int32_t int_t;

    // Floating-point type to use
    typedef float float_t;
}

namespace cml {
    using string_t = hlib::CString<cml_alloc_t<char>>;

    template <typename value_t> 
    using array_t = hlib::CArray<value_t, cml_alloc_t<value_t>>;

    template <typename key_t, typename value_t> 
    using table_t = hlib::CBinTable<key_t, value_t, cml_alloc_t<hlib::CPair<key_t, value_t>>>;

    template <typename value_t>
    using stack_t = hlib::CStack<value_t, cml_alloc_t<value_t>>;

    typedef void* (*cml_openfile_t)(const char*);
    typedef int (*cml_getchar_t)(void*);
    typedef void (*cml_closefile_t)(void*);
    typedef size_t (*cml_filelen_t)(void*);
    typedef void (*cml_readfile_t)(void*, char*);

    struct SCallbackInfo {
        cml_openfile_t fileOpen;
        cml_closefile_t fileClose;
        cml_filelen_t fileLength;
        cml_readfile_t fileReadFull;
        cml_getchar_t fileReadByte;
    };

    extern SCallbackInfo s_callbacksInfo;
}

#endif

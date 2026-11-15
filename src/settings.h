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
    using cml_alloc_t = hlib::CDefaultAllocator;
    extern cml_alloc_t g_defaultAlloc;

    /*
        NOTE:
        Changing int_t anf float_t will likely break compatibility with CDF
        files written with other settings than yours. Keep this in mind.
    */

    // Integer type to use
    typedef int32_t int_t;

    // Floating-point type to use
    typedef float float_t;
}

namespace cml {
    using string_t = hlib::CString<cml_alloc_t, &g_defaultAlloc>;

    template <typename value_t> 
    using array_t = hlib::CArray<value_t, cml_alloc_t, &g_defaultAlloc>;

    template <typename key_t, typename value_t> 
    using table_t = hlib::CBinTable<key_t, value_t, cml_alloc_t, &g_defaultAlloc>;

    template <typename value_t>
    using stack_t = hlib::CStack<value_t, cml_alloc_t, &g_defaultAlloc>;
}

namespace cml {
    enum class ETextError : uint32_t {
        Ok,                     // We`re cool

        StrayIdentifier,        // Random identifier that does not connect to anything
        StrayNumber,            // Random out-of-context number
        StrayKeyword,           // Random out-of-context keyword
        StrayString,            // Random out-of-context string
        StrayBlock,             // Nameless block
        StrayArray,             // Nameless array
        ScobeMiss,              // Badly formatted block (curvy braces mismatch)

        BadFileRef,             // Can`t open said file path
        BadReference,           // Anything except string is marked as reference
        BadInheritance,         // Inheriting is used on non-table objects
        ParentNotFound,         // Parent object not found
        OverrideTypeMix,        // The child value we are about to overwrite is of different type than parent`s one

        BadNumber,              // Badly formatted number
        NoFile,                 // Source file not found
        UnexpectedEOF,          // EOF jumpscare in the middle of something
        VersionMismatch,        // Source file`s CML version is incompatible
        BadVersion,             // Version is negative or is a float
        BadEscapeChar,          // Wrong escape character is detected inside a string
        BadNumberSuffix,        // Wrong number type suffix

        StackOverflow,          // Stack depth exceeded
        StackUnderflow,         // Attempt to pop an empty stack
        RefHugeFile,            // The referenced file exceeds le limit
        AllocFailure            // Failed to allocate a new object
    };

    enum class EBinaryError : uint32_t {
        Ok,

        MagicNumberMismatch,    // File providen is not a CDF one
        NoFile,                 // Source file not found
        UnexpectedEOF,          // EOF jumpscare
        VersionMismatch,        // Major CDF version differs from file`s one
        AllocFailure,           // Failed to allocate a new object
        BadNameID,              // Invalid object name ID
        BadType,                // Invalid object type
        TypeMismatch,           // Type mismatch
        WriteError,             // Something went wrong with writing data
        NumberTooBig,           // Number in the binary is bigger than the runtime
    };
}

#endif

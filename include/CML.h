#ifndef CBPP_CML_API_H
#define CBPP_CML_API_H

#include <stdint.h>
#include <stddef.h>

#include "cbpp/Array.h"
#include "cbpp/Stack.h"
#include "cbpp/String.h"

#include "engine/datafile/object_model.h"

#define CBPP_CML_VERSION 100                        // Actual language version
#define CBPP_CML_VERSION_LEAST 100                  // Any versions below this are considered deprecated
#define CBPP_CML_MAX_REFFILE (64 * (1 << 20))       // File reference size limit, 64 MB by default

#define CBPP_CML_STACK_LIMIT 128                    // Nesting depth limit

namespace cbpp {
    class IFile;
}

namespace cbpp::cdf {
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
        RefHugeFile             // The referenced file exceeds le limit
    };

    const char* StringError(ETextError);
    const char* StringError(EPathError);

    /*
        Parse an entire CML file to the CObject tree
    */
    class CTextParser {
        struct IncludeNode {
            cbpp::CString sPath;
            cbpp::IFile* pFile = NULL;

            size_t iLine = 1;

            ~IncludeNode();
        };

        struct ScobeNode {
            char iScobe = '\0';
            size_t iLevel = 0;
        };

        enum class EKeyword : uint8_t {
            Name,
            Include,
            Version,
            True,
            False,
            Inherit
        };

        enum class ERefType : uint8_t {
            NoLink,
            Text,
            Binary
        };

        enum class EForceNumberType : uint8_t {
            None,
            ForceInteger,
            ForceFloat
        };

        enum class EPromise : uint8_t {
            None,
            Value,
            Version,
            IncludePath,
            ParentPath
        };
        
        cbpp::CArray<char> m_sLexemBuffer;
        cbpp::CString m_sCurrentName;

        cbpp::CStack<IncludeNode> m_aFilesStack;
        cbpp::CStack<CObject> m_aStack;
        cbpp::CStack<ScobeNode> m_aScobesStack;

        CObject m_pRootObject = cdf::NIL;

        size_t m_iLine = 0, m_iCol = 0;

        bool m_bAllowIncludes = true;
        bool m_bInsideArray = false;
        
        EPromise m_iPromise = EPromise::None; // remember our promise

        EForceNumberType m_iForceNumberType = EForceNumberType::None;

        ERefType m_iRefType = ERefType::NoLink;

        int Peek();

        bool IsValidNameStart(int);
        bool CheckRedef(const char*);
        EKeyword IsKeyword(const char*);

        ETextError ParseName(int);
        ETextError ParseString(int);
        ETextError ParseNumber(int);

        ETextError PushFloat(const char* sName, float_t fValue);
        ETextError PushInt(const char* sName, int_t iValue);

        ETextError AddFile(const char*);

        CObject ResolveFileRef(ETextError&);

        ETextError ProcessChar(int iChar);
        ETextError ProcessNumber();
        ETextError ProcessName();

        ETextError CheckScobes(int iChar);
        
        public:
            CTextParser() = default;

            CTextParser& operator=(const CTextParser& Other);

            /*
                Resets the parser. This is called automatically in
                the Parse call.
            */
            void Reset();

            /*
                Parse the said CML file
            */
            ETextError Parse(const char* sPath, bool bAllowIncludes = true);

            CObject Root();
            
            /*
                Path format is
                "obj.sub_obj.target",
                "obj.sub_array[2]", etc
            */
            CPathAccess operator[](const char* sPath);

            EPathError GetPathError() const;
            
            size_t FormatError(ETextError iCode, char* sBuffer, size_t iBufferLn);

            ~CTextParser();
    };

    size_t WriteObject(CObject pObj, char* sBuffer, size_t iBufferLen, bool bPretty = true);
    size_t WriteObject(CObject pObj, const char* sPath, bool bPretty = true);
}

#endif

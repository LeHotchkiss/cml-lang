#ifndef CML_OBJMODEL_H
#define CML_OBJMODEL_H

/*
    Object model interface, used both by CML and CDF
*/

#include <stdint.h>
#include <stddef.h>

#include "hlib/Filesystem.h"

#include "src/settings.h"

namespace cml {
    enum class EObjectClass {
        Nil,
        Object,
        Array,
        Binary,
        Integer,
        Float,
        String,

        LAST        // obviously, this entry must remain the last one
    };

    const char* ClassString(EObjectClass);

    enum class EPathError : uint32_t {
        Ok,                     // OK

        NotFound,               // Subobject does not exist
        ObjIndex,               // Indexing an object
        ArrayAccess,            // Accessing an array with '.'
        BadIndex,               // NaN or float is used as index
        BadSeparator,           // Something except '.' is used as a separator
        BadPath
    };
    
    struct SObjectInfo {
        struct {
            uint64_t type       : 8;
            uint64_t nameLength : 56;
        } data;

        uint64_t length;
    };

    class IObject;
    class CObject {
        /*
            CDF objects don`t behave like RAII (eek!) and only get deleted manually
            or when the parent object gets deleted.
        */

        IObject* m_pObj = NULL;

        public:
            CObject() = default;

            CObject(int_t iValue);
            CObject(float_t fValue);
            CObject(const char* sValue);

            CObject(IObject* pData);
            operator IObject*() const;
            IObject* GetPointer() const;

            EObjectClass Class() const;

            /*
                'Length' of the object:
                Number of entries of an array or table, or
                actual length of bytearray or string (excluding the null-terminator)
            */
            size_t Length() const;

            // Size of the object in bytes, if serialized
            size_t Size() const;

            bool operator==(const CObject& pOther) const;
            bool operator!=(const CObject& pOther) const;

            CObject& operator=(int_t iValue);
            CObject& operator=(float_t fValue);
            CObject& operator=(const char* sValue);
            void SetBinaryData(const uint8_t* pData, size_t iLength);

            CObject operator[](size_t iIndex);
            CObject operator[](const char* sName);

            EBinaryError Serialize(hlib::IFile*, string_t* = NULL) const;
            EBinaryError Deserialize(hlib::IFile*, const SObjectInfo&);

            // Index the N-th key in the table, if table, else NULL
            const char* IndexName(size_t iIndex) const;

            // Index the N-th value in the table or array, else NIL
            CObject Index(size_t iIndex);

            void Push(CObject pObj);
            void Push(const char* sName, CObject pObj);

            explicit operator int_t() const;
            explicit operator float_t() const;
            explicit operator const char*() const;
            explicit operator uint8_t*();

            int_t AsInt() const;
            float_t AsFloat() const;
            const char* AsString() const;
            uint8_t* AsBinary();

            void Merge(CObject);

            operator bool() const;
    };

    /*
        Null value to signal errors
        It is safe to call methods on it, they just do nothing
    */
    extern const CObject NIL;

    class CPathAccess {
        EPathError m_iStatus = EPathError::Ok;
        CObject m_pPathObj = cml::NIL;

        public:
            CPathAccess( CObject, EPathError );

            operator CObject() const;
            CObject Object() const;
            EPathError Status() const;
    };

    /*
        Access a subobject using the path string
    */
    CPathAccess AccessObject(CObject pObj, const char* sPath);

    /*
        For debugging usage
        Prints the object recursively to the stdout
    */
    void PrintObject(CObject pObj, size_t iDepth = 0);

    /*
        Create a new object of the given type
    */
    CObject CreateObject(EObjectClass iClass);

    /*
        Deallocate the given object and all of it`s children
    */
    void DeleteObject(CObject pObj);

    /*
        Get an independent copy of the given object
    */
    CObject CopyObject(CObject pObj);
}

#endif

#ifndef CBPP_DATAFILES_OBJMODEL_H
#define CBPP_DATAFILES_OBJMODEL_H

/*
    Object model interface, used both by CML and CDF
*/

#include <stdint.h>
#include <stddef.h>

#include "cbpp/Constants.h"

namespace cbpp::cdf {
    // Integer type to use
    typedef int32_t int_t;

    // Floating-point type to use
    typedef float float_t;

    enum class EObjectClass {
        Nil,
        Object,
        Array,
        Binary,
        Integer,
        Float,
        String
    };

    const char* ClassString(EObjectClass);

    enum class EPathError : uint32_t {
        Ok,                     // OK

        NotFound,               // Subobject does not exist
        ObjIndex,               // Indexing an object
        ArrayAccess,            // Accessing an array with '.'
        BadIndex,               // NaN or float is used as index
        BadSeparator            // Something except '.' is used as a separator
    };

    class IObject;
    class CObject {
        /*
            CDF objects don`t behave like RAII (eek!) and only get deleted manually
            or when the parent object gets deleted.
        */

        friend EPathError GetPathAccessError();

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

            // Dump object`s data to a buffer
            void Serialize(char* pBuffer) const;

            bool operator==(const CObject& pOther) const;
            bool operator!=(const CObject& pOther) const;

            CObject& operator=(int_t iValue);
            CObject& operator=(float_t fValue);
            CObject& operator=(const char* sValue);
            void SetBinaryData(const uint8_t* pData, size_t iLength);

            CObject operator[](size_t iIndex);
            CObject operator[](const char* sName);

            const char* IndexName(size_t iIndex) const;
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

    // Null value to signal errors
    extern const CObject NIL;

    class CPathAccess {
        EPathError m_iStatus = EPathError::Ok;
        CObject m_pPathObj = cdf::NIL;

        public:
            CPathAccess( CObject, EPathError );

            operator CObject() const;
            CObject Object() const;
            EPathError Status() const;
    };

    // Access a subobject using the path string
    CPathAccess AccessObject(CObject pObj, const char* sPath);

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

    CObject BinaryToObject(char* pBuffer, size_t iBufferLn);
}

#endif

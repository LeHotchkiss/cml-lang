#ifndef CBPP_DATAFILE_OBJECTS_H
#define CBPP_DATAFILE_OBJECTS_H

#include "cbpp/CML.h"
#include "cbpp/String.h"
#include "cbpp/Array.h"
#include "cbpp/Table.h"

namespace cbpp::cdf {
    class IObject {
        public:
            virtual IObject* At(const char*) = 0;
            virtual IObject* At(size_t) = 0;
            virtual const char* IndexName(size_t) = 0;

            virtual EObjectClass Class() const = 0;

            virtual int_t AsInt() const = 0;
            virtual float_t AsFloat() const = 0;
            virtual const char* AsString() const = 0;
            virtual uint8_t* AsBinary() = 0;

            virtual void Serialize(char*) const = 0;

            virtual void SetValue(int_t) = 0;
            virtual void SetValue(float_t) = 0;
            virtual void SetValue(const char*) = 0;
            virtual void SetValue(const uint8_t*, size_t) = 0;

            virtual void PushValue(IObject*) = 0;
            virtual void PushValue(const char*, IObject*) = 0;

            virtual size_t Length() const = 0;
            virtual size_t Size() const = 0;

            virtual void Merge(CObject) = 0;

            virtual ~IObject() = default;
    };

    class CBinaryObject final : public IObject {
        constexpr static const char* s_sName = "<binary>";

        cbpp::CArray<uint8_t> m_aData;

        public:
            int_t AsInt() const;
            float_t AsFloat() const;
            const char* AsString() const;
            uint8_t* AsBinary();

            void SetValue(int_t);
            void SetValue(float_t);
            void SetValue(const char*);
            void SetValue(const uint8_t*, size_t);

            void Serialize(char*) const;
            
            IObject* At(const char*);
            IObject* At(size_t);
            const char* IndexName(size_t);

            void PushValue(IObject*);
            void PushValue(const char*, IObject*);

            EObjectClass Class() const;

            size_t Length() const;
            size_t Size() const;

            void Merge(CObject);

            virtual ~CBinaryObject() override = default;
    };

    class CIntObject final : public IObject {
        constexpr static const char* s_sName = "<integer>";

        int_t m_iData = 0;

        public:
            int_t AsInt() const;
            float_t AsFloat() const;
            const char* AsString() const;
            uint8_t* AsBinary();

            void SetValue(int_t);
            void SetValue(float_t);
            void SetValue(const char*);
            void SetValue(const uint8_t*, size_t);

            void Serialize(char*) const;

            IObject* At(const char*);
            IObject* At(size_t);
            const char* IndexName(size_t);

            void PushValue(IObject*);
            void PushValue(const char*, IObject*);

            EObjectClass Class() const;

            size_t Length() const;
            size_t Size() const;

            void Merge(CObject);

            virtual ~CIntObject() override = default;
    };

    class CFloatObject final : public IObject {
        constexpr static const char* s_sName = "<float>";

        float_t m_fData = 0;

        public:
            int_t AsInt() const;
            float_t AsFloat() const;
            const char* AsString() const;
            uint8_t* AsBinary();

            void SetValue(int_t);
            void SetValue(float_t);
            void SetValue(const char*);
            void SetValue(const uint8_t*, size_t);

            void Serialize(char*) const;

            IObject* At(const char*);
            IObject* At(size_t);
            const char* IndexName(size_t);

            void PushValue(IObject*);
            void PushValue(const char*, IObject*);

            EObjectClass Class() const;

            size_t Length() const;
            size_t Size() const;

            void Merge(CObject);

            virtual ~CFloatObject() override = default;
    };

    class CStringObject final : public IObject {
        cbpp::CString m_sData;

        public:
            int_t AsInt() const;
            float_t AsFloat() const;
            const char* AsString() const;
            uint8_t* AsBinary();

            void SetValue(int_t);
            void SetValue(float_t);
            void SetValue(const char*);
            void SetValue(const uint8_t*, size_t);

            void Serialize(char*) const;

            IObject* At(const char*);
            IObject* At(size_t);
            const char* IndexName(size_t);

            void PushValue(IObject*);
            void PushValue(const char*, IObject*);

            EObjectClass Class() const;

            size_t Length() const;
            size_t Size() const;

            void Merge(CObject);

            virtual ~CStringObject() override = default;
    };

    class CArrayObject final : public IObject {
        constexpr static const char* s_sName = "<array>";

        cbpp::CArray<IObject*> m_pData;

        public:
            int_t AsInt() const;
            float_t AsFloat() const;
            const char* AsString() const;
            uint8_t* AsBinary();

            void SetValue(int_t);
            void SetValue(float_t);
            void SetValue(const char*);
            void SetValue(const uint8_t*, size_t);

            void Serialize(char*) const;

            IObject* At(const char*);
            IObject* At(size_t);
            const char* IndexName(size_t);

            void PushValue(IObject*);
            void PushValue(const char*, IObject*);

            EObjectClass Class() const;

            size_t Length() const;
            size_t Size() const;

            void Merge(CObject);

            virtual ~CArrayObject() override;
    };

    class CDictObject final : public IObject {
        constexpr static const char* s_sName = "<table>";

        cbpp::CBinTable<cbpp::CString, IObject*> m_dTable;

        public:
            int_t AsInt() const;
            float_t AsFloat() const;
            const char* AsString() const;
            uint8_t* AsBinary();

            void SetValue(int_t);
            void SetValue(float_t);
            void SetValue(const char*);
            void SetValue(const uint8_t*, size_t);

            void Serialize(char*) const;

            IObject* At(const char*);
            IObject* At(size_t);
            const char* IndexName(size_t);

            void PushValue(IObject*);
            void PushValue(const char*, IObject*);

            EObjectClass Class() const;

            size_t Length() const;
            size_t Size() const;

            void Merge(CObject);

            virtual ~CDictObject() override;
    };
    
    class CNullObject final : public IObject {
        constexpr static const char* s_sName = "<NIL>";

        public:
            int_t AsInt() const;
            float_t AsFloat() const;
            const char* AsString() const;
            uint8_t* AsBinary();

            void SetValue(int_t);
            void SetValue(float_t);
            void SetValue(const char*);
            void SetValue(const uint8_t*, size_t);

            void Serialize(char*) const;

            IObject* At(const char*);
            IObject* At(size_t);
            const char* IndexName(size_t);

            void PushValue(IObject*);
            void PushValue(const char*, IObject*);

            EObjectClass Class() const;

            size_t Length() const;
            size_t Size() const;

            void Merge(CObject);

            virtual ~CNullObject() override = default;
    };
}

#endif

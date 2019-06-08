//
// Created by Andrew Bregger on 2019-06-07.
//

#ifndef MU_TYPE_HPP
#define MU_TYPE_HPP

#include "common.hpp"

namespace mu {
    enum TypeKind {
        Primative_I8,
        Primative_I16,
        Primative_I32,
        Primative_I64,
        Primative_U8,
        Primative_U16,
        Primative_U32,
        Primative_U64,
        Primative_Float32,
        Primative_Float64,
        Primative_String,

        Structure,
        SType,
        Function,
        TraitAttribute,

        Array,
        DynArray,
        Tuple,
        Ptr,
        Reference,

        Mutable,

        Primative_Char = Primative_U8,
        Primative_Bool = Primative_U8,
    };


    class Type {
    public:
        explicit Type(TypeKind k, u64 sz);

        inline TypeKind kind() { return k; }
        inline u64 size() { return sz; }

        virtual std::string str() = 0;

        virtual bool is_primative() { return false; }
        virtual bool is_integer() { return false; }
        virtual bool is_float() { return false; }
        virtual bool is_string() { return false; }
        virtual bool is_char() { return false; }
        virtual bool is_bool() { return false; }
        virtual bool is_signed() { return false; }
        virtual bool is_unsigned() { return false; }
        virtual bool is_arithmetic() { return false; }


        virtual bool is_struct() { return false; }
        virtual bool is_stype() { return false; }
        virtual bool is_function() { return false; }
        virtual bool is_trait() { return false; }

        virtual bool is_ptr() { return false; }
        virtual bool is_ref() { return false; }
        virtual bool is_tuple() { return false; }
        virtual bool is_array() { return false; }
        virtual bool is_dyn() { return false; }

        virtual bool is_mutable() { return false; }

        virtual bool base_type() = 0;

    private:
        TypeKind k;
        u64 sz;
    };

    class PrimativeInt : public Type {
    public:
        PrimativeInt(TypeKind k, u64 sz);

        virtual std::string str();

        virtual bool is_primative() override { return true; }
        virtual bool is_integer() override { return true; }
        virtual bool is_arithmetic() override { return true; }
        virtual bool is_signed() override;
        virtual bool is_unsigned() override;
    };

    class PrimativeFloat : public Type {
    public:
        PrimativeFloat(TypeKind k, u64 sz);

        virtual std::string str();
        virtual bool is_primative() override { return true; }
        virtual bool is_arithmetic() override { return true; }
        virtual bool is_signed() override { return true; }
        virtual bool is_float() override { return true; }
    };

    class PrimativeString : public Type {
    public:
        PrimativeString(u64 sz);

        virtual std::string str();

        virtual bool is_string() override { return true; }
        virtual bool is_struct() override { return true; }
    };

//    class PrimativeBool : public Type {
//    public:
//        PrimativeBool(u64 sz);
//
//        virtual bool is_primative() { return false; }
//        virtual bool is_bool() { return false; }
//    };

    typedef mem::Pr<Type> TypePtr;
}



#endif //MU_TYPE_HPP

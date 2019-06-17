//
// Created by Andrew Bregger on 2019-06-07.
//

#ifndef MU_TYPE_HPP
#define MU_TYPE_HPP

//#include <llvm/IR/Type.h>

#include "common.hpp"
#include "parser/ast/ast_common.hpp"
#include "analysis/scope.hpp"
#include <unordered_map>
#include <unordered_set>

namespace mu {
    class Entity;

    class Local;

    class Function;

    class TypeMember;

    namespace types {
        enum TypeKind {
            Primitive_I8,
            Primitive_I16,
            Primitive_I32,
            Primitive_I64,
            Primitive_U8,
            Primitive_U16,
            Primitive_U32,
            Primitive_U64,
            Primitive_Float32,
            Primitive_Float64,
            Primitive_String,
            Unit_Type,

            // special alias of u8
            Primitive_Char,
            Primitive_Bool,

            StructureType,
            SType,
            FunctType,
            TraitAttributeType,

            // polymorphic variations
                    PolyStructureType,
            PolySType,
            PolyFunctionType,
            PolyTraitAttributeType,

            ArrayType,
            DynArrayType,
            TupleType,
            PtrType,
            ReferenceType,

            MutableType,

            PolyType,

            ModType,
        };

        /*
            Base class for all types in Mu.
            -   Integers are represented by PrimitiveInt. The signing and size are determined by the kind given to it.
                I thought this was a good way of representing it because most of the type when an integer is used it doesnt
                matter whether it is signed or not, that will be handled within the handling of the primitive types.
            -   Floats are handled in a similar manner, but since there are only two variations this is a simpler
                problem.
            -   Characters and booleans will be special alias of u8s.
            -   Structures are represented by a mapping of member names and their entity
                struct Foo {
                    {x -> u32,
                    y -> i32
                    ...}
                }
            -   STypes or Sum types are represented by a mapping of constructor names and type lists.
                enum Bar {
                    {
                        Foo -> [f32, i32],
                        Boo -> [char, bool, string],
                        ...
                    }
                }
            -   Functions/Procedures are represented a mapping of parameter names to local entities (to facility default values and
                out of order parameters) and return type.
                foo: [
                        x -> f32,
                        y -> bool = true
                     ]
            -   Type Traits
            -   Polymorphic Types, these types have constant blocks which contain the generic types associated with
                with this type.

            @note: It seems I am represented each entity and type in two different ways. Maybe it would be best
                    to consolidate the representations into the entity.
         */
        class Type {
        public:
            explicit Type(TypeKind k, u64 sz);

            virtual ~Type();

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

            virtual bool is_polymophic() { return false; }

            virtual bool is_polymorphic_param() { return false; }

            virtual bool is_module() { return false; }

            virtual bool is_unit() { return false; }

            virtual Type *base_type() = 0;

        private:
            TypeKind k;
            u64 sz;
        };

        class PolymorphicType;

        class TraitType;

        // represents bounding
        struct TypeBounds {
            PolymorphicType *bounded;
            std::vector<TraitType *> bounds;

            TypeBounds(PolymorphicType *bounded, const std::vector<TraitType *> &bounds);
        };

        typedef mem::Pr<Type> TypePtr;

        class PrimitiveInt : public Type {
        public:
            PrimitiveInt(TypeKind k, u64 sz);

            virtual ~PrimitiveInt();

            std::string str() override;

            bool is_primative() override { return true; }

            bool is_integer() override { return true; }

            bool is_arithmetic() override { return true; }

            bool is_signed() override;

            bool is_unsigned() override;

            Type *base_type() override;
        };


        class PrimitiveFloat : public Type {
        public:
            PrimitiveFloat(TypeKind k, u64 sz);

            virtual ~PrimitiveFloat();

            std::string str() override;

            bool is_primative() override { return true; }

            bool is_arithmetic() override { return true; }

            bool is_signed() override { return true; }

            bool is_float() override { return true; }

            Type *base_type() override;
        };

        class PrimitiveString : public Type {
        public:
            PrimitiveString(u64 sz);

            virtual ~PrimitiveString();

            std::string str() override;

            bool is_string() override { return true; }

            bool is_struct() override { return true; }

            Type *base_type() override;
        };

//    class PrimitiveBool : public Type {
//    public:
//        PrimitiveBool(u64 sz);
//
//        virtual bool is_primative() { return false; }
//        virtual bool is_bool() { return false; }
//    };

        class UnitType : public Type {
        public:
            UnitType();
            virtual ~UnitType();
            Type* base_type() override;

            std::string str() override;

            bool is_unit() override { return true; }
        };

        class Tuple : public Type {
        public:
            Tuple(std::vector<Type*> &types, u64 sz);

            virtual ~Tuple();

            std::string str() override;

            Type *base_type() override;

            bool is_tuple() override { return true; }

            template<typename Ty>
            Ty *get(u64 i) {
                if (i < types.size())
                    return CAST_PTR(Ty, types[i]);
                else
                    return nullptr;
            }

        private:
            std::vector<Type*> types;
        };

        class Pointer : public Type {
        public:
            Pointer(Type* &base);

            virtual ~Pointer();

            std::string str() override;

            Type *base_type() override;

            bool is_ptr() override { return true; }
            bool is_arithmetic() override { return true; }

        private:
            Type* base;
        };

        class Reference : public Type {
        public:
            Reference(Type* &base);

            virtual ~Reference();

            std::string str() override;

            Type *base_type() override;

            bool is_ref() override { return true; }

        private:
            Type* base;
        };

        class Array : public Type {
        public:
            Array(Type* &type, u64 count);

            virtual ~Array();

            bool is_array() override { return true; }

            std::string str() override;

            Type *base_type() override;

        private:
            Type* type;
            u64 count;
        };

        class DynArray : public Type {
        public:
            DynArray(Type* &type);

            virtual ~DynArray();

            std::string str() override;

            Type *base_type() override;

        private:
            Type* type;
        };

        class Mutable : public Type {
        public:
            Mutable(Type* &type);

            virtual ~Mutable();

            std::string str() override;

            Type *base_type() override;

        private:
            Type* type;
        };

        struct StructType : public Type {
        public:
            StructType(ast::Ident *name, const std::unordered_map<ast::Ident *, Entity *> &members,
                       mu::ScopePtr member_scope_ptr, u64 sz);

            virtual ~StructType();

            bool is_struct() override;

            std::string str() override;

        private:
            ast::Ident *name;
            // this includes methods and locals
            std::unordered_map<ast::Ident *, Entity *> members;

            // both elements point to the same scope.
            MemberScope *member_scope{nullptr};
            ScopePtr member_scope_ptr;
        };


        struct SumType : public Type {
        public:
            SumType(ast::Ident *name, const std::unordered_map<ast::Ident *, mu::TypeMember *> &members,
                    mu::ScopePtr member_scope_ptr, u64 sz);

            virtual ~SumType();

            bool is_stype() override;

            std::string str() override;

        private:
            ast::Ident *name;
            std::unordered_map<ast::Ident *, TypeMember *> members;

            // both elements point to the same scope.
            MemberScope *member_scope{nullptr};
            ScopePtr member_scope_ptr;
        };

        struct FunctionType : public Type {
        public:
            FunctionType(const std::vector<Type *> &params, Type *ret);

            virtual ~FunctionType();

            bool is_function() override;

            std::string str() override;

            Type* base_type() override { return this; }

        private:
            std::vector<Type *> params;
            Type *ret;
        };

        struct TypeField {
            ast::Ident *name;
            types::Type *default_type;

            TypeField(ast::Ident *name, types::Type *default_type);

            friend bool operator==(const TypeField &t1, const TypeField &t2);
        };

        struct TypeFieldHasher {
            u64 operator()(const TypeField &field) const;
        };

        class TraitType : public Type {
        public:
            TraitType(ast::Ident *name, const std::unordered_set<TypeField, TypeFieldHasher> &type_field,
                      const std::unordered_map<ast::Ident *, Function *> &members, ScopePtr member_scope_ptr);

            virtual ~TraitType();

            bool is_trait() override;

            std::string str() override;

        private:
            ast::Ident *name;
            std::unordered_set<TypeField, TypeFieldHasher> type_fields;
            std::unordered_map<ast::Ident *, Function *> members;

            MemberScope *member_scope{nullptr};
            ScopePtr member_scope_ptr;
        };

        class PolyStructType : public Type {
        public:
            PolyStructType(ast::Ident *name, const std::unordered_map<ast::Ident *, Local *> &members,
                           ScopePtr member_scope_ptr, ScopePtr const_block_ptr);

            virtual ~PolyStructType();

            bool is_struct() override;

            bool is_polymophic() override;

            std::string str() override;

        private:
            ast::Ident *name;
            std::unordered_map<ast::Ident *, Local *> members;

            // both elements point to the same scope.
            MemberScope *member_scope{nullptr};
            ScopePtr member_scope_ptr;

            ConstBlockScope *const_block{nullptr};
            ScopePtr const_block_ptr;
        };

        class PolySumType : public Type {
        public:

            PolySumType(ast::Ident *name, const std::unordered_map<ast::Ident *, TypeMember *> &members,
                        ScopePtr member_scope_ptr, ScopePtr const_block_potr);

            virtual ~PolySumType();

            bool is_stype() override;

            bool is_polymophic() override;

            std::string str() override;

        private:
            ast::Ident *name;
            std::unordered_map<ast::Ident *, TypeMember *> members;

            // both elements point to the same scope.
            MemberScope *member_scope{nullptr};
            ScopePtr member_scope_ptr;

            ConstBlockScope *const_block{nullptr};
            ScopePtr const_block_ptr;
        };

        class PolyFunction : public Type {
        public:

            PolyFunction(const std::vector<Type *> &params, Type *ret,
                         ScopePtr const_block_ptr);

            virtual ~PolyFunction();

            bool is_function() override;

            bool is_polymophic() override;

            std::string str() override;

        private:
            std::vector<Type *> params;
            Type *ret;

            ConstBlockScope *const_block{nullptr};
            ScopePtr const_block_ptr;
        };

        class PolyTraitType : public Type {
        public:

            PolyTraitType(ast::Ident *name, const std::unordered_set<TypeField, TypeFieldHasher> &type_fields,
                          const std::unordered_map<ast::Ident *, Function *> &members, ScopePtr member_scope_ptr,
                          ScopePtr &const_block_ptr);

            virtual ~PolyTraitType();

            bool is_trait() override;

            bool is_polymophic() override;

            std::string str() override;

        private:
            ast::Ident *name;
            std::unordered_set<TypeField, TypeFieldHasher> type_fields;
            std::unordered_map<ast::Ident *, Function *> members;

            MemberScope *member_scope{nullptr};
            ScopePtr member_scope_ptr;

            ConstBlockScope *const_block{nullptr};
            ScopePtr const_block_ptr;
        };


        class PolymorphicType : public Type {
        public:
            PolymorphicType(ast::Ident *name, std::vector<TraitType *> &bounds);

            virtual ~PolymorphicType();

            bool is_polymorphic_param() override { return true; }

            Type* base_type() override;

        private:
            ast::Ident *name;
            TypeBounds bounds;
        };

        class ModuleType : public Type {
        public:
            ModuleType(ast::Ident* name, ScopePtr module_scope_ptr, ast::AstNode* module_node);
            virtual ~ModuleType();

            bool is_module() override { return true; }

            Type* base_type() override;

        private:
            ast::Ident* name;
            ast::AstNode* module_node; // determines how the Module is to be resolved.

            ModuleScope* module_scope;
            ScopePtr module_scope_ptr; // contains all entities of the module
        };
    }
}

#endif //MU_TYPE_HPP

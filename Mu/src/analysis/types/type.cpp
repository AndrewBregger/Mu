//
// Created by Andrew Bregger on 2019-06-07.
//

#include "type.hpp"
#include "analysis/entity.hpp"
#include "interpreter.hpp"
#include <utility>
#include <algorithm>

mu::types::Type::Type(mu::types::TypeKind k, u64 sz, u64 align) : k(k), sz(sz), align(align) {

}

mu::types::Type::~Type() = default;

mu::types::PrimitiveInt::PrimitiveInt(mu::types::TypeKind k, u64 sz, u64 align) : Type(k, sz, align) {
}

mu::types::PrimitiveInt::~PrimitiveInt() = default;

std::string mu::types::PrimitiveInt::str() {
    switch(kind()) {
        case Primitive_I8:
            return "i8";
        case Primitive_I16:
            return "i16";
        case Primitive_I32:
            return "i32";
        case Primitive_I64:
            return "i64";
        case Primitive_U8:
            return "u8";
        case Primitive_U16:
            return "u16";
        case Primitive_U32:
            return "u32";
        case Primitive_U64:
            return "u64";
        case Primitive_Bool:
            return "bool";
        case Primitive_Char:
            return "char";
        default:
            return "Invalid Integer Type";
    }
}

bool mu::types::PrimitiveInt::is_signed() {
    switch(kind()) {
        case Primitive_I8:
        case Primitive_I16:
        case Primitive_I32:
        case Primitive_I64:
            return true;
        case Primitive_U8:
        case Primitive_U16:
        case Primitive_U32:
        case Primitive_U64:
            return false;
        default:
            return false;
    }
}

bool mu::types::PrimitiveInt::is_unsigned() {
    return !this->is_signed();
}


mu::types::Type *mu::types::PrimitiveInt::base_type() {
    return this;
}

mu::types::PrimitiveString::PrimitiveString(Entity* declaration, u64 sz, u64 align) : Type(Primitive_String, sz, align),
    declaration(declaration)  {
}

mu::types::PrimitiveString::~PrimitiveString() = default;

std::string mu::types::PrimitiveString::str() {
    return "string";
}


mu::types::Type *mu::types::PrimitiveString::base_type() {
    return this;
}

mu::types::PrimitiveFloat::PrimitiveFloat(mu::types::TypeKind k, u64 sz, u64 align) : Type(k, sz, align) {
}

mu::types::PrimitiveFloat::~PrimitiveFloat() = default;

std::string mu::types::PrimitiveFloat::str() {
    switch(kind()) {
        case Primitive_Float32:
            return "f32";
        case Primitive_Float64:
            return "f64";
        default:
            return "Invalid Float Type";
    }
}

mu::types::Type *mu::types::PrimitiveFloat::base_type() {
    return this;
}

mu::types::UnitType::UnitType() : Type(Unit_Type, 0, 0) {
}

mu::types::UnitType::~UnitType() = default;

mu::types::Type *mu::types::UnitType::base_type() {
    return this;
}

std::string mu::types::UnitType::str() {
    return "Unit";
}

// Get Pointer size from platform.
mu::types::Pointer::Pointer(mu::types::Type* &base) : Type(PtrType, (u64) 8, (u64) 8), base(base) {
}

mu::types::Pointer::~Pointer() = default;

std::string mu::types::Pointer::str() {
    return "*" + base->str();
}

mu::types::Type *mu::types::Pointer::base_type() {
    return base;
}

bool mu::types::Pointer::is_mutable() {
    return base->is_mutable();
}

mu::types::Reference::Reference(mu::types::Type* &base) : Type(ReferenceType, 8, 8), base(base) {
}

mu::types::Reference::~Reference() = default;

std::string mu::types::Reference::str() {
    return "&" + base->str();
}

mu::types::Type *mu::types::Reference::base_type() {
    return base->base_type();
}

bool mu::types::Reference::is_mutable() {
    return base->is_mutable();
}

mu::types::Array::Array(mu::types::Type* &type, u64 count) : Type(ArrayType, count * type->size(), type->alignment()),
    type(type), count(count) {
}


mu::types::Array::~Array() = default;

std::string mu::types::Array::str() {
    return "[" + type->str() + ";" + std::to_string(count) + "]";
}

mu::types::Type *mu::types::Array::base_type() {
    return this;
}

mu::types::Mutable::Mutable(mu::types::Type* &type) : Type(MutableType, type->size(), type->alignment()),
    type(type) {
}

mu::types::Mutable::~Mutable() = default;

std::string mu::types::Mutable::str() {
    return "mut " + type->str();
}

mu::types::Type *mu::types::Mutable::base_type() {
    return type->base_type();
}

mu::types::Tuple::Tuple(std::vector<mu::types::Type*> &types, u64 sz, u64 align) :
    Type(TupleType, sz, align), types(types) {
}

mu::types::Tuple::~Tuple() = default;

std::string mu::types::Tuple::str() {
    std::string s = "(";
    for(auto& t : types) {
         s += t->str();
         s += ", ";
    }
    s = s.substr(0, s.size() - 2);
    return s + ")";
}

mu::types::Type *mu::types::Tuple::base_type() {
    return this;
}

mu::types::Type* mu::types::Tuple::get_element_type(u64 i) {
    if (i < num_elements())
        return types[i];
    else
        return nullptr;
}

// compute the size from the structure representing a dynamic array.
// *T + u32 + u32
mu::types::DynArray::DynArray(mu::types::Type* &type) : Type(DynArrayType, 8 + 4 + 4, type->alignment()),
    type(type) {
}

mu::types::DynArray::~DynArray() = default;

std::string mu::types::DynArray::str() {
    return "[" + type->str() + "]";
}

mu::types::Type *mu::types::DynArray::base_type() {
    return this;
}

mu::types::StructType::StructType(ast::Ident *name, const std::vector<Entity *> &members,
                                  mu::ScopePtr member_scope_ptr, u64 sz, Entity *declaration, u64 align) :
                                  Type(StructureType, sz, align), name(name), members(members), member_scope_ptr(member_scope_ptr),
                                  declaration(declaration) {

   member_scope = member_scope_ptr->as<MemberScope>();
   if(!member_scope) {
       auto interp = Interpreter::get();
       interp->fatal("Compiler Error: invalid member scope given to struct type");
   }
}

mu::types::StructType::~StructType() = default;

bool mu::types::StructType::is_struct() {
    return true;
}

mu::types::Type* mu::types::StructType::base_type() {
    return this;
}

std::string mu::types::StructType::str() {
//    std::string s = name->value + " { ";
//
//    for(auto& m : members) {
//        s += m.second->get_type()->str() + ", ";
//    }
//
//    s = s.substr(0, s.size() - 2);
//    s += "}";

    return name->value();
}

u64 mu::types::StructType::get_index_of_member(mu::Entity *entity) {
    for(u64 i = 0; i < members.size(); ++i)
        if(members[i] == entity)
            return i;
    return (u64) -1;
}

mu::types::FunctionType::FunctionType(const std::vector<Type*>& params,
                                      mu::types::Type *ret) : Type(FunctType, 8, 8),
                                      params(params), ret(ret) {
}

mu::types::FunctionType::~FunctionType() = default;

bool mu::types::FunctionType::is_function() {
    return true;
}

std::string mu::types::FunctionType::str() {
    std::string s = "(";
    for(auto t : params) {
        s += t->str();
        s += ", ";
    }
    s = s.substr(0, s.size() - 2);
    s += ") " + ret->str();
    return s;
}

u64 mu::types::FunctionType::num_params() {
    return params.size();
}

mu::types::Type* mu::types::FunctionType::get_ret() {
    return ret;
}

mu::types::Type* mu::types::FunctionType::get_param(u64 i) {
    assert(i < num_params());

    return params[i];
}

mu::types::TypeField::TypeField(ast::Ident *name, mu::types::Type *default_type) :
    name(name), default_type(default_type) {
}

bool mu::types::operator==(const mu::types::TypeField& t1, const mu::types::TypeField & t2) {
    return t1.name == t2.name;
}

u64 mu::types::TypeFieldHasher::operator()(const mu::types::TypeField &field) const {
    std::hash<ast::Ident*> hasher;
    return hasher(field.name);
}

mu::types::SumType::SumType(ast::Ident *name, const std::vector<mu::TypeMember *> &members,
                            mu::ScopePtr member_scope_ptr, u64 sz, Entity *declaration, u64 align) : Type(SType, sz, align),
                        name(name), members(members), member_scope_ptr(member_scope_ptr),
                        declaration(declaration) {
    member_scope = member_scope_ptr->as<MemberScope>();
    if(!member_scope) {
        auto interp = Interpreter::get();
        interp->fatal("Compiler Error invalid member scope of sum type");
    }
}

mu::types::SumType::~SumType() = default;

bool mu::types::SumType::is_stype() {
    return true;
}

std::string mu::types::SumType::str() {
    return name->value();
}

mu::types::TraitType::TraitType(ast::Ident *name, const std::unordered_set<TypeField, TypeFieldHasher> &type_field,
                                const std::vector<Function *> &members, ScopePtr member_scope_ptr,
                                Entity *declaration) : Type(TraitAttributeType, 0, 0), name(name),
                                type_fields(type_field), members(members), member_scope_ptr(member_scope_ptr),
                                declaration(declaration) {
    member_scope = member_scope_ptr->as<MemberScope>();
    if(!member_scope) {
        auto interp = Interpreter::get();
        interp->fatal("Compiler Error: invalid member scope of trait type");
    }
}

mu::types::TraitType::~TraitType() = default;

bool mu::types::TraitType::is_trait() {
    return true;
}

std::string mu::types::TraitType::str() {
    return name->value();
}

mu::types::PolyStructType::PolyStructType(ast::Ident *name, const std::vector<Entity *> &members,
                                          ScopePtr member_scope_ptr, ScopePtr const_block_ptr, Entity *declaration) : Type(PolyStructureType, 0, 0),
                                          name(name), members(members), member_scope_ptr(member_scope_ptr),
                                          const_block_ptr(const_block_ptr), declaration(declaration) {
    member_scope = member_scope_ptr->as<MemberScope>();
    const_block = const_block_ptr->as<ConstBlockScope>();

    if(!member_scope) {
        auto interp = Interpreter::get();
        interp->fatal("Compiler Error: invalid member scope of polymophic structure");
    }

    if(!const_block) {
        auto interp = Interpreter::get();
        interp->fatal("Compiler Error: invalid const block scope of polymophic structure");
    }
}

mu::types::PolyStructType::~PolyStructType() = default;

bool mu::types::PolyStructType::is_struct() {
    return true;
}

bool mu::types::PolyStructType::is_polymophic() {
    return true;
}

std::string mu::types::PolyStructType::str() {
    std::string s = name->value();
    s += "[";
    for(auto m : *const_block)  {
        s += m.first->value;
        s += ", ";
    }
    s = s.substr(0, s.size() - 2) + "]";
    return s;
}

u64 mu::types::PolyStructType::get_index_of_member(mu::Entity *entity) {
    auto iter = std::find(members.begin(), members.end(), entity);
    if(iter == members.end())
        return (u64) -1;
    else
        return std::distance(iter, members.begin());
}

mu::types::PolySumType::PolySumType(ast::Ident *name, const std::vector<TypeMember *> &members,
                                    ScopePtr member_scope_ptr, ScopePtr const_block_potr, Entity *declaration) :
                                Type(PolySType, 0, 0), name(name), members(members), member_scope_ptr(member_scope_ptr),
                                const_block_ptr(const_block_ptr), declaration(declaration) {

    member_scope = member_scope_ptr->as<MemberScope>();
    const_block = const_block_ptr->as<ConstBlockScope>();

    if(!member_scope) {
        auto interp = Interpreter::get();
        interp->fatal("Compiler Error: invalid member scope of polymophic sum type");
    }

    if(!const_block) {
        auto interp = Interpreter::get();
        interp->fatal("Compiler Error: invalid const block scope of polymophic sum type");
    }
}

mu::types::PolySumType::~PolySumType() = default;

bool mu::types::PolySumType::is_stype() {
    return true;
}

bool mu::types::PolySumType::is_polymophic() {
    return true;
}

std::string mu::types::PolySumType::str() {
    std::string s = name->value();
    s += "[";
    for(auto m : *const_block)  {
        s += m.first->value;
        s += ", ";
    }
    s = s.substr(0, s.size() - 2) + "]";
    return s;
}

mu::types::PolyFunction::PolyFunction(const std::vector<Type *> &params, Type *ret, ScopePtr const_block_ptr,
                                      Entity *declaration) : Type(PolyFunctionType, 0, 0),
                                                             params(params), ret(ret), const_block_ptr(const_block_ptr),
                                                             declaration(declaration) {
    const_block = const_block_ptr->as<ConstBlockScope>();
    if(!const_block) {
        auto interp = Interpreter::get();
        interp->fatal("Compiler Error: invalid const member scope given to polymorphic function");
    }
}

mu::types::PolyFunction::~PolyFunction() = default;

bool mu::types::PolyFunction::is_function() {
    return true;
}

bool mu::types::PolyFunction::is_polymophic() {
    return false;
}

std::string mu::types::PolyFunction::str() {
    std::string s;
    s += "[";
    for(auto m : *const_block)  {
        s += m.first->value;
        s += ", ";
    }
    s = s.substr(0, s.size() - 2) + "]";
    s += "(";
    for(auto t : params) {
        s += t->str();
        s += ", ";
    }
    s = s.substr(0, s.size() - 2);
    s += ") " + ret->str();
    return s;
}

mu::types::PolyTraitType::PolyTraitType(ast::Ident *name,
                                        const std::unordered_set<TypeField, TypeFieldHasher> &type_fields,
                                        const std::vector<Function *> &members,
                                        ScopePtr member_scope_ptr,
                                        ScopePtr &const_block_ptr, Entity *declaration) :
                                        Type(PolyTraitAttributeType, 0, 0), name(name),
                                        type_fields(type_fields), members(members), member_scope_ptr(member_scope_ptr),
                                        const_block_ptr(const_block_ptr), declaration(declaration) {
    member_scope = member_scope_ptr->as<MemberScope>();
    const_block = const_block_ptr->as<ConstBlockScope>();

    if(!member_scope) {
        auto interp = Interpreter::get();
        interp->fatal("Compiler Error: invalid member scope of polymophic triat");
    }

    if(!const_block) {
        auto interp = Interpreter::get();
        interp->fatal("Compiler Error: invalid const block scope of polymophic triat");
    }
}

mu::types::PolyTraitType::~PolyTraitType() = default;

bool mu::types::PolyTraitType::is_trait() {
    return true;
}

bool mu::types::PolyTraitType::is_polymophic() {
    return true;
}

std::string mu::types::PolyTraitType::str() {
    std::string s = name->value() + "[";
    for(auto m : *const_block)  {
        s += m.first->value;
        s += ", ";
    }
    s = s.substr(0, s.size() - 2) + "]";
    return s;
}

mu::types::TypeBounds::TypeBounds(mu::types::PolymorphicType *bounded,
                                  const std::vector<mu::types::TraitType *> &bounds):
                                  bounded(bounded), bounds(bounds){
}

mu::types::PolymorphicType::PolymorphicType(ast::Ident *name, std::vector<mu::types::TraitType *> &bounds,
                                            Entity* declaration) :
    Type(PolyType, 0, 0), name(name), bounds(this, bounds), declaration(declaration) {
}

mu::types::PolymorphicType::~PolymorphicType() = default;

mu::types::Type *mu::types::PolymorphicType::base_type() {
    return this;
}

mu::types::ModuleType::ModuleType(ast::Ident *name, mu::ScopePtr module_scope_ptr, ast::AstNode *module_node) :
    Type(ModType, 0, 0), name(name), module_node(module_node), module_scope_ptr(module_scope_ptr) {
}

mu::types::ModuleType::~ModuleType() = default;

mu::types::Type *mu::types::ModuleType::base_type() {
    return this;
}



//
// Created by Andrew Bregger on 2019-06-08.
//

#include "entity.hpp"
#include "interpreter.hpp"
#include "typer.hpp"

namespace mu {
    Entity::Entity(ast::Ident *name, Scope* p, EntityKind k, ast::DeclPtr decl) :
        k(k), type(nullptr), parent(p), name(name), decl(decl) {
    }

    Scope *Entity::scope() {
        return parent;
    }

    ast::AstNode *Entity::node() {
        return decl.get();
    }

    ast::Ident *Entity::get_name() {
        return name;
    }


    void Entity::resolve(Typer *typer) {
        typer->resolve_entity(this);
    }

    Global::Global(ast::Ident *name, types::Type *type, AddressType addr_type, Scope* p, ast::DeclPtr decl) :
        Entity(name, p, GlobalEntity, decl), addr_type(addr_type), mut(decl->kind == ast::ast_global_mut) {
        this->type = type;
    }

    Global::Global(ast::Ident *name, Scope* p, ast::DeclPtr decl) : Global(name, nullptr, Unknown, p, decl) {
    }

    std::string Global::str() {
        auto s = "global" + (mut ? std::string(" mut ") : std::string(" "));
        return s + name->value();
    }

    void Global::resolve(Typer *typer) {
        typer->resolve(this);
    }

    Local::Local(ast::Ident *name, types::Type *type, AddressType addr_type, Scope* p, ast::DeclPtr decl) :
        Entity(name, p, LocalEntity, decl), addr_type(addr_type), mut(decl->kind == ast::ast_mut) {
        this->type = type;

    }

    void Local::resolve(Typer *typer) {
        typer->resolve(this);
    }

    std::string Local::str() {
        auto s = "local" + (mut ? std::string(" mut ") : std::string(" "));
        return s + name->value();
    }

    Constant::Constant(ast::Ident *name, types::Type *type, mu::Val val, Scope* p, ast::DeclPtr decl) : Entity(name, p, ConstantEntity, decl),
        val(val) {
    }

    void Constant::resolve(Typer *typer) {
        typer->resolve(this);
    }


    std::string Constant::str() {
        return "const " + name->value();
    }

    Alias::Alias(ast::Ident *name, types::Type *type, Scope* p, ast::DeclPtr decl) : Entity(name, p, AliasEntity, decl){
        this->type = type;
    }

    void Alias::resolve(Typer *typer) {
        typer->resolve(this);
    }

    std::string Alias::str() {
        return "alias " + name->value();
    }

    Function::Function(ast::Ident *name, const std::unordered_map<ast::Ident *, Local *> &params,
                       ScopePtr params_scope_ptr, types::Type *type, Scope* p, ast::DeclPtr decl) :
                       Entity(name, p, FunctionEntity, decl), params(params), param_scope_ptr(param_scope_ptr) {
        param_scope = params_scope_ptr.as<ParameterScope>();
        if(!param_scope) {
            auto interp = Interpreter::get();
            interp->fatal("Compiler Error: invalid parameter given to function entity");
        }
    }

    Function::Function(ast::Ident *name, Scope* p, ast::DeclPtr decl) :
        Function(name, {}, nullptr, nullptr, p, decl){
    }

    void Function::resolve(Typer *typer) {
       typer->resolve(this);
    }

    std::string Function::str() {
        return "fn " + name->value();
    }

    Type::Type(ast::Ident *name, types::Type *type, Scope* p, ast::DeclPtr decl) :
        Entity(name, p, TypeEntity, decl) {
        this->type = type;
    }

    Type::Type(ast::Ident *name, Scope* p, ast::DeclPtr decl) : Type(name, nullptr, p, decl) {
    }

    void Type::resolve(Typer *typer) {
        typer->resolve(this);
    }

    std::string Type::str() {
        std::string s;
        switch(decl->kind) {
            case ast::ast_structure:
                s = "struct ";
                break;
            case ast::ast_type:
                s = "type ";
                break;
            case ast::ast_type_class:
                s = "trait ";
                break;
            default:
                Interpreter::get()->fatal("Compiler Error: Type entity build with invalid decl type");
                break;
        }
        return s + name->value();
    }

    bool Type::is_struct() {
        if(type)
            return type->is_struct();
        else
            return decl->kind == ast::ast_structure;
    }

    bool Type::is_sumtype() {
        if(type)
            return type->is_stype();
        else
            return decl->kind == ast::ast_type;
    }

    bool Type::is_trait() {
        if(type)
            return type->is_trait();
        else
            return decl->kind == ast::ast_type_class;
    }

    bool Type::is_polymorphic() {
        if(type)
            return type->is_polymophic();
        else
            // we do not know yet
            return false;
    }


    Module::Module(ast::Ident *name, types::Type *type, ScopePtr exproted_scope_ptr, Scope *p, ast::DeclPtr decl) :
        Entity(name, p, ModuleEntity, decl), exported_scope_ptr(exproted_scope_ptr){
        this->type = type;
    }

    Module::Module(ast::Ident *name, Scope* p, ast::DeclPtr decl) :
        Entity(name, p, ModuleEntity, decl) {
    }

    std::string Module::str() {
        return "mod " + name->value();
    }
}

//
// Created by Andrew Bregger on 2019-06-08.
//

#include "entity.hpp"
#include "interpreter.hpp"
#include "typer.hpp"
#include <stack>

namespace mu {

    TypePath::TypePath() {}

    TypePath::TypePath(const std::vector<Scope *> &scopes) : scopes(scopes) {
    }

    void TypePath::push_scope(Scope *scope) {
        if(scope->get_name() != nullptr)
            scopes.push_back(scope);
        else {
            Interpreter::get()->fatal("Compiler Error: Adding a scope without a name to a type path");
        }
    }

    // path to entity, doesnt include the entity itself.
    // if method, returns the scope of the struct and parents.
    // if method is in global scope of main? return emtpy Path?
    TypePath::PathName TypePath::path() {
        PathName p;

        for(auto s : scopes)
            p.push_back(s->get_name()->val);

        return p;
    }

    Scope *TypePath::scope_by_name(Atom *name) {
        auto iter = std::search_n(scopes.begin(), scopes.end(), scopes.size(), name,
            [](Scope* s, Atom* n){
                auto name = s->get_name();
                // this shouldn't happen
                if(name) {
                    return name->val == n;
                }
                else return false;
            }
        );

        if(iter == scopes.end())
            return nullptr;
        else
            return *iter;
    }

    std::string TypePath::str() {
        std::string s;

        auto p = path();

        for(auto iter = p.begin(); iter < p.end();) {
            s += (*iter)->value;
            // maybe add
            if(++iter < p.end())
                s += ".";
        }

        return s;
    }

    Entity::Entity(ast::Ident *name, Scope* p, EntityKind k, ast::DeclPtr decl) :
        k(k), type(nullptr), parent(p), name(name), decl(decl) {
    }

//    int Entity::str() {
//        return 0;
//    }

    Entity::~Entity() = default;

    TypePath Entity::path() {
        TypePath p;
        std::stack<Scope*> scopes;
        auto curr = parent;
        // collect the scopes
        while(curr) {
            // if the entity was declared in a function, the
            // we want to skip any block or defer scopes that are parent.
            if(curr->get_name())
                scopes.push(curr);

            curr = curr->get_parent();
        }

        // since the stack above is going from the lower to higher scope
        // when popped off the stack they will be in the correct order.
        while(!scopes.empty()) {
            p.push_scope(scopes.top());
            scopes.pop();
        }

        return p;
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

    Global::~Global() = default;

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

    Local::~Local() = default;

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

    Constant::~Constant() = default;

    void Constant::resolve(Typer *typer) {
        typer->resolve(this);
    }


    std::string Constant::str() {
        return "const " + name->value();
    }

    Alias::Alias(ast::Ident *name, types::Type *type, Scope* p, ast::DeclPtr decl) : Entity(name, p, AliasEntity, decl){
        this->type = type;
    }

    Alias::~Alias() = default;

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

    Function::~Function() = default;

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

    Type::~Type() = default;

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

    Module::~Module() = default;

    std::string Module::str() {
        return "mod " + name->value();
    }
}

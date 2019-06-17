//
// Created by Andrew Bregger on 2019-06-08.
//

#ifndef MU_ENTITY_HPP
#define MU_ENTITY_HPP

#include "common.hpp"
#include "parser/ast/ast_common.hpp"
#include "types/type.hpp"
#include "scope.hpp"
#include "value.hpp"

namespace mu {
    class Typer;

    enum EntityKind {
        LocalEntity,
        GlobalEntity,
        ConstantEntity,
        FunctionEntity,
        AliasEntity,
        TypeEntity,
        ModuleEntity,
//        TypeMemberEntity,

        // This is only used in ConstBlock
//        GenericEntity,    this was originally representing the type paramters in polymorphic entities
//                          but because of how it was restructured, it can be a type entitiy instead of
//                          needed a new entity.
    };

    enum AddressType {
        Value,
        Reference,
        Unknown,
    };

    enum EntityStatus {
       Incomplete,
       Resolved,
       Resolving,
    };

    class Entity {
    public:
        explicit Entity(ast::Ident* name, Scope* p, EntityKind k, ast::DeclPtr decl);

        inline EntityKind kind() { return k; }
        Scope* scope();
        ast::AstNode* node();
        inline types::Type* get_type() { return type; }

//        template<typename Ty>
//        Ty* as() {
//            return dynamic_cast<Ty*>(this);
//        }

        ast::Ident* get_name();

        inline bool is_resolved() { return resolved == Resolved; }

        inline EntityStatus status() { return resolved; }

        inline void update_status(EntityStatus st) { resolved = st; }

        virtual bool validate() { return true; }

        virtual std::string str() = 0;

        // I am not sure what this should return.
        // Maybe the type
        // or nothing...
        virtual void resolve(Typer* typer);

    protected:
        EntityKind k;               // the entity kind.
        types::Type* type;          // the type of this entity.
        Scope* parent;            // the scope this entity belongs.
        ast::Ident* name{nullptr};  // name of the entity
        ast::DeclPtr decl;          // the source declaration where this entity is created.
        EntityStatus resolved{Incomplete};       // flag for whether the entity has been resolved by the typer.
    };

    typedef mem::Pr<Entity> EntityPtr;

    class Local : public Entity {
    public:
        Local(ast::Ident* name, types::Type* type, AddressType addr_type, Scope* p, ast::DeclPtr decl);
        inline AddressType address() { return addr_type; }

        void resolve(Typer* typer) override;

        std::string str() override;
    private:
        AddressType addr_type{Unknown};
        bool mut{false};
    };

    class Global : public Entity {
    public:
        Global(ast::Ident* name, types::Type* type, AddressType addr_type, Scope* p, ast::DeclPtr decl);

        // pre-declare constructor. This is used to declare the entity when in global scope.
        // At this point, the entities name, entity type, and parent scope are known but it might
        // be used by some other entity before it is declared. For this to work, it must be in the scope
        // event though it is not declared. This is only on entities that can be declared in global scope.
        Global(ast::Ident* name, Scope* p, ast::DeclPtr decl);

        inline AddressType address() { return addr_type; }
        std::string str() override;
        void resolve(Typer* typer) override;


    private:
        ast::Ident* name{nullptr};
        AddressType addr_type{Unknown};
        bool mut{false};
    };

    // this doesnt get an AddressType because it is always by value.
    class Constant : public Entity {
    public:
        Constant(ast::Ident* name, types::Type* type, mu::Val val, Scope* p, ast::DeclPtr decl);
        std::string str() override;
        void resolve(Typer* typer) override;

    private:
        mu::Val val;
    };

    class Type : public Entity {
    public:
       Type(ast::Ident* name, types::Type* type, Scope* p, ast::DeclPtr decl);

       Type(ast::Ident* name, Scope* p, ast::DeclPtr decl);

        std::string str() override;

       bool is_struct();

       bool is_sumtype();

       bool is_trait();

       bool is_polymorphic();

       void resolve(Typer* typer) override;

    };

    class Function : public Entity {
    public:
        Function(ast::Ident* name, const std::unordered_map<ast::Ident*, Local*>& params,
            ScopePtr params_scope_ptr, types::Type* type, Scope* p, ast::DeclPtr decl);

        Function(ast::Ident *name, Scope* p, ast::DeclPtr decl);

        std::string str() override;

        void resolve(Typer* typer) override;

    private:
        std::unordered_map<ast::Ident*, Local*> params;
        ParameterScope* param_scope{nullptr};
        ScopePtr        param_scope_ptr;
    };

    class Alias : public Entity {
    public:
        Alias(ast::Ident* name, types::Type* type, Scope* p, ast::DeclPtr decl);
        std::string str() override;
        void resolve(Typer* typer) override;

    private:
        ast::Ident* name;
    };

    class Module : public Entity {
    public:
        Module(ast::Ident* name, types::Type* type, ScopePtr exproted_scope_ptr, Scope* p, ast::DeclPtr decl);
        Module(ast::Ident* name, Scope* p, ast::DeclPtr decl);

        std::string str() override;

    private:
        ModuleScope* exported_scope{nullptr};
        ScopePtr exported_scope_ptr;

    };
}


#endif //MU_ENTITY_HPP

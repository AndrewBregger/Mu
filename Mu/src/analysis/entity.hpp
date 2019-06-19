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

    class Entity;

    // This is how I am going to specify a specific type when compairing
    // named types (structure, functions, typeclasses, sumtypes). Because
    // the above types are compared by name, we need to know the entire name
    // inorder to compare accuratly. struct x.y is not the same a struct w.y.
    //
    // this is the entity path inorder to reach a specified entity.
    // For example, there is a structure name 'X' in file module 'Y'
    // the path would be Y.X.

    // lets say module 'Y' is more complex. Now, 'X' is exported from a submodule
    // of 'Y' call 'W' resulting in a path Y.W.X
    //
    // Now more complex, let say we have a generic static method, 'foo[]' of generic
    // struct 'X[]' contained in sub module 'W' from the previous example.
    // the path would be Y.W.X[...].foo[...] .... I guess if foo is static on X then the
    // type parameters are not necessary to know what to call...right?
    // the spuare brackets are needed because because each instance of a generic
    // entity must be specified to know which on is being used.

    class TypePath {
    public:
        typedef std::vector<Atom*> PathName;

        // to iteratively build the TypePath
        TypePath();

        // builds the entire list all at once.
        TypePath(const std::vector<Scope*>& scopes);

        // pushes a new scope onto the path
        void push_scope(Scope* scope);

        // returns it a list of names (Atoms).
        PathName path();

        // returns the scope accociated to name
        Scope* scope_by_name(Atom* name);

        // builds a string of debugging and maybe error messges.
        std::string str();

    private:
        std::vector<Scope*> scopes;
    };

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
        virtual ~Entity();

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

        TypePath path();

        inline ast::Decl* get_decl() { return decl.get(); }

        template <typename Ty>
        Ty* get_decl_as() { return decl.template as<Ty>(); }


        virtual bool is_global() { return false; }
        virtual bool is_local() { return false; }
        virtual bool is_constant() { return false; }
        virtual bool is_type() { return false; }
        virtual bool is_module() { return false; }
        virtual bool is_function() { return false; }
        virtual bool is_alias() { return false; }

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
        virtual ~Local();

        inline AddressType address() { return addr_type; }

        void resolve(Typer* typer) override;
        
        bool is_local() override { return true; }

        std::string str() override;

        inline bool is_mutable() { return mut; }
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

        virtual ~Global();

        inline AddressType address() { return addr_type; }
        std::string str() override;

        bool is_global() override { return true; }

        void resolve(Typer* typer) override;

        inline bool is_mutable() { return mut; }


    private:
        ast::Ident* name{nullptr};
        AddressType addr_type{Unknown};
        bool mut{false};
    };

    // this doesnt get an AddressType because it is always by value.
    class Constant : public Entity {
    public:
        Constant(ast::Ident* name, types::Type* type, mu::Val val, Scope* p, ast::DeclPtr decl);
        virtual ~Constant();

        std::string str() override;

        bool is_constant() override { return true; }

        void resolve(Typer* typer) override;

    private:
        mu::Val val;
    };

    class Type : public Entity {
    public:
        Type(ast::Ident* name, types::Type* type, Scope* p, ast::DeclPtr decl);

        Type(ast::Ident* name, Scope* p, ast::DeclPtr decl);

        virtual ~Type();

        std::string str() override;

        bool is_type() override { return true; }

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

        virtual ~Function();

        std::string str() override;

        void resolve(Typer* typer) override;

        bool is_function() override { return true; }

    private:
        std::unordered_map<ast::Ident*, Local*> params;
        ParameterScope* param_scope{nullptr};
        ScopePtr        param_scope_ptr;
    };

    class Alias : public Entity {
    public:
        Alias(ast::Ident* name, types::Type* type, Scope* p, ast::DeclPtr decl);

        virtual ~Alias();

        std::string str() override;
        void resolve(Typer* typer) override;

        bool is_alias() override { return true; }

    private:
        ast::Ident* name;
    };

    class Module : public Entity {
    public:
        Module(ast::Ident* name, types::Type* type, ScopePtr exproted_scope_ptr, Scope* p, ast::DeclPtr decl);
        Module(ast::Ident* name, Scope* p, ast::DeclPtr decl);

        virtual ~Module();

        std::string str() override;

        bool is_module() override { return true; }

    private:
        ModuleScope* exported_scope{nullptr};
        ScopePtr exported_scope_ptr;

    };
}


#endif //MU_ENTITY_HPP

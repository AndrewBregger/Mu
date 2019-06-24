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
        TypePath(const std::vector<ScopePtr>& scopes);

        // pushes a new scope onto the path
        void push_scope(ScopePtr scope);

        // returns it a list of names (Atoms).
        PathName path();

        // returns the scope accociated to name
        ScopePtr scope_by_name(Atom* name);

        // builds a string of debugging and maybe error messges.
        std::string str();

    private:
        std::vector<ScopePtr> scopes;
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

    // @NOTE: Maybe when the constructor with the type is used, it should automatically be resloved.

    class Entity {
    public:
        explicit Entity(ast::Ident* name, ScopePtr p, EntityKind k, ast::DeclPtr decl);
        virtual ~Entity();

        inline EntityKind kind() { return k; }
        ScopePtr scope();
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

        inline ast::DeclPtr get_decl() { return decl; }

        template <typename Ty>
        Ty* get_decl_as() { return decl->template as<Ty>(); }


        template <typename Ty>
        const Ty* as() const {
            return dynamic_cast<const Ty*>(this);
        }

        template <typename Ty>
        Ty* as() {
            return const_cast<Ty*>(const_cast<const Entity&>(*this).as<Ty>());
        }

        void resolve_to(types::Type* type);

        inline ast::DeclPtr decl_ptr() { return decl; }


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
        virtual Entity* resolve(Typer* typer);

        virtual void debug_print(std::ostream& out);

    protected:
        EntityKind k;               // the entity kind.
        types::Type* type;          // the type of this entity.
        ScopePtr parent;            // the scope this entity belongs.
        ast::Ident* name{nullptr};  // name of the entity
        ast::DeclPtr decl;          // the source declaration where this entity is created.
        EntityStatus resolved{Incomplete};       // flag for whether the entity has been resolved by the typer.
    };

    typedef std::unique_ptr<Entity> EntityPtr;

    enum LocalFlags : u16 {
        Initialized = 1,
        Mutable = 2,
        Visible = 4,
        IsMember = 8,
        IsParameter = 16,
        InheritVisibility = 32,
        CVariadic = 64,
        TypeVariadic = 128,
        PolyVariadic = 256,
    };

    class Local : public Entity {
    public:
        Local(ast::Ident* name, types::Type* type, AddressType addr_type, ScopePtr p, ast::DeclPtr decl);

        virtual ~Local();

        inline AddressType address() { return addr_type; }

        Entity* resolve(Typer* typer) override;
        
        bool is_local() override { return true; }

        std::string str() override;

        inline bool is_mutable() { return flags & Mutable; }
        inline bool is_initialized() { return flags & Initialized; }
        inline bool is_visable() { return flags & Visible; }
        inline bool is_member() { return flags & IsMember; }
        inline bool is_parameter() { return flags & IsParameter; }
        inline bool is_visibility_inherited() { return flags & InheritVisibility; }
        inline bool is_cvariadic() { return flags & CVariadic; }
        inline bool is_typevariadic() { return flags & TypeVariadic; }
        inline bool is_polyvariadic() { return flags & PolyVariadic; }

        inline void set_mutable() { flags |= Mutable; }
        inline void set_initialized() { flags |= Initialized; }
        inline void set_visable() { flags |= Visible; }
        inline void set_parameter() { flags |= IsParameter; }

        inline void set_cvariadic()    { flags |= CVariadic; }
        inline void set_typevariadic() { flags |= TypeVariadic; }
        inline void set_polyvariadic() { flags |= PolyVariadic; }

        inline void set_inherit_visibility() { flags |- InheritVisibility; }

        inline void set_member(u32 offset) {
            this->offset = offset;
            flags |= IsMember;
        }

        inline u32 get_offset() {
            return offset;
        }


        void set_addressing(AddressType t);

        void debug_print(std::ostream& out) override;
        
    private:
        AddressType addr_type{Unknown};
        u16 flags{0};

        // member variables;
        u32 offset{0};
//        Function* function;
    };

    class Global : public Entity {
    public:
        Global(ast::Ident* name, types::Type* type, AddressType addr_type, ScopePtr p, ast::DeclPtr decl);

        // pre-declare constructor. This is used to declare the entity when in global scope.
        // At this point, the entities name, entity type, and parent scope are known but it might
        // be used by some other entity before it is declared. For this to work, it must be in the scope
        // event though it is not declared. This is only on entities that can be declared in global scope.
        Global(ast::Ident* name, ScopePtr p, ast::DeclPtr decl);

        virtual ~Global();

        inline AddressType address() { return addr_type; }
        std::string str() override;

        bool is_global() override { return true; }

        Entity* resolve(Typer* typer) override;

        inline bool is_mutable() { return mut; }
        inline bool is_initialized() { return initialized; }
        inline void set_initialized() { initialized = true; }

        void set_addressing(AddressType t);

        void debug_print(std::ostream& out) override;

    private:
        // ast::Ident* name{nullptr};
        AddressType addr_type{Unknown};
        bool initialized{false};
        bool mut{false};
    };

    // this doesnt get an AddressType because it is always by value.
    class Constant : public Entity {
    public:
        Constant(ast::Ident* name, types::Type* type, mu::Val val, ScopePtr p, ast::DeclPtr decl);
        virtual ~Constant();

        std::string str() override;

        bool is_constant() override { return true; }

        Entity* resolve(Typer* typer) override;

        void debug_print(std::ostream& out) override;

    private:
        mu::Val val;
    };

    class Type : public Entity {
    public:
        Type(ast::Ident* name, types::Type* type, ScopePtr p, ast::DeclPtr decl);

        Type(ast::Ident* name, ScopePtr p, ast::DeclPtr decl);

        virtual ~Type();

        std::string str() override;

        bool is_type() override { return true; }

        bool is_struct();

        bool is_sumtype();

        bool is_trait();

        bool is_polymorphic();

        Entity* resolve(Typer* typer) override;

        void debug_print(std::ostream& out) override;

    };

    enum FunctionFlags : u32 {
        ForeignFunction = 1,
        InlineFunction = 2,
        NoBody = 4,
    };

    class Function : public Entity {
    public:
        Function(ast::Ident *name, const std::vector<Local *> &params,
                 ScopePtr params_scope_ptr, types::Type *type, ScopePtr p, ast::DeclPtr decl);

        Function(ast::Ident *name, ScopePtr p, ast::DeclPtr decl);

        virtual ~Function();

        std::string str() override;

        Entity *resolve(Typer *typer) override;

        bool is_function() override { return true; }

        void debug_print(std::ostream &out) override;

        inline bool is_foreign() { return flags & ForeignFunction; }
        inline bool is_inline() { return flags & InlineFunction; }
        inline bool no_body() { return flags & NoBody; }
        inline void set_foreign(const std::string &name) {
            foreign_name = name;
            flags |= ForeignFunction;
        }

        inline void set_inline() { flags |= InlineFunction; }
        inline void set_no_body() { flags |= NoBody; }
        const std::string& get_foreign_name() { return foreign_name; }

        void set_param_info(const std::vector<Local*>& params, ScopePtr scope);


    private:
        std::vector<Local*> params;
        ParameterScope* param_scope{nullptr};
        ScopePtr        param_scope_ptr;

        u32 flags{0};
        std::string foreign_name;
    };

    class Alias : public Entity {
    public:
        Alias(ast::Ident* name, types::Type* type, ScopePtr p, ast::DeclPtr decl);

        virtual ~Alias();

        std::string str() override;
        Entity* resolve(Typer* typer) override;

        bool is_alias() override { return true; }

        void debug_print(std::ostream& out) override;


    private:
        // ast::Ident* name;
    };

    class Module : public Entity {
    public:
        Module(ast::Ident* name, types::Type* type, ScopePtr exproted_scope_ptr, ScopePtr p, ast::DeclPtr decl);
        Module(ast::Ident* name, ScopePtr p, ast::DeclPtr decl);

        virtual ~Module();

        std::string str() override;

        bool is_module() override { return true; }

        void debug_print(std::ostream& out) override;

    private:
        ModuleScope* exported_scope{nullptr};
        ScopePtr exported_scope_ptr;

    };
}


#endif //MU_ENTITY_HPP

//
// Created by Andrew Bregger on 2019-06-07.
//

#ifndef MU_SCOPE_HPP
#define MU_SCOPE_HPP

#include "common.hpp"

#include "parser/ast/ast_common.hpp"
#include <unordered_map>
#include <unordered_set>

namespace mu {
    enum ScopeKind {
        Block_Scope,
        ConstBlock_Scope,
        Parameter_Scope,
        Member_Scope,
        Module_Scope,
        Defer_Scope,
    };

    class Entity;

    class Scope {
    public:
        Scope(ast::AstNode* r, ScopeKind k, Scope* parent);

        std::pair<Entity*, bool> find(ast::Ident* name);

        bool insert(ast::Ident* name, Entity* entity);

        void add_child(Scope* scope);

        inline ScopeKind kind() { return k; }

        inline ast::AstNode* node() { return r; }

        inline std::unordered_map<Atom*, Entity*>::iterator begin() { return elements.begin(); }
        inline std::unordered_map<Atom*, Entity*>::iterator end() { return elements.end(); }

        virtual ast::Ident* get_name() { return nullptr; }

        inline Scope* get_parent() { return parent; }

    private:
        Scope* parent;
        std::unordered_set<Scope*> children;

        std::unordered_map<Atom*, Entity*> elements;
        ast::AstNode* r;
        ScopeKind k;
    };

    class BlockScope : public Scope {
    public:
        BlockScope(ast::AstNode* r,  Scope* parent);
    };

    class ConstBlockScope : public Scope {
    public:
        ConstBlockScope(ast::Ident* name, ast::AstNode* r,  Scope* parent);
         ast::Ident* get_name() override { return name; }
    private:
        ast::Ident* name{nullptr};
    };

    class ParameterScope : public Scope {
    public:
        ParameterScope(ast::AstNode* r, Scope* parent);
    };

    class MemberScope : public Scope {
    public:
        MemberScope(ast::Ident* name, ast::AstNode* r, Scope* parent);
         ast::Ident* get_name() override { return name; }
    private:
        ast::Ident* name{nullptr};
    };

    class ModuleScope : public Scope {
    public:
        ModuleScope(ast::Ident* name, ast::AstNode* r, Scope* parent);
        ast::Ident* get_name() override { return name; }

    private:
        ast::Ident* name{nullptr};
    };

    class DeferScope : public Scope {
    public:
        DeferScope(ast::AstNode* r, Scope* parent);
    };

    typedef mem::Pr<Scope> ScopePtr;

    template <typename Ty, typename... Args>
    ScopePtr make_scope(Args... args) {
        static_assert(std::is_base_of<Scope, Ty>::value, "attempting to construct a non-scope type as a scope");
        return ScopePtr(new Ty(args...));
    }
}


#endif //MU_SCOPE_HPP

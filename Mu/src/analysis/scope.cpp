//
// Created by Andrew Bregger on 2019-06-07.
//

#include "scope.hpp"

mu::Scope::Scope(ast::AstNode *r, mu::ScopeKind k, mu::ScopePtr parent) : r(r), k(k), parent(parent) {
}

void mu::Scope::replace_name(ast::Ident* name, Entity* entity) {
    auto iter = elements.find(name->val);
    if(iter == elements.end())
        return;
    else {
        elements[name->val] = entity;
    }
}

std::pair<mu::Entity*, bool> mu::Scope::find(ast::Ident *name) {
    auto iter = elements.find(name->val);
    if(iter == elements.end())
        return std::make_pair(nullptr, false);
    else
        return std::make_pair(iter->second, true);
}

bool mu::Scope::insert(ast::Ident *name, mu::Entity *entity) {
    auto iter = elements.emplace(name->val, entity);
    return iter.second;
}

void mu::Scope::add_child(mu::ScopePtr scope) {
    children.emplace(scope);
}

mu::BlockScope::BlockScope(ast::AstNode *r, mu::ScopePtr parent) : Scope(r, Block_Scope, parent) {
}

mu::MemberScope::MemberScope(ast::Ident *name, ast::AstNode *r, mu::ScopePtr parent) : Scope(r, Member_Scope, parent), name(name)  {
}

mu::ConstBlockScope::ConstBlockScope(ast::Ident *name, ast::AstNode *r, mu::ScopePtr parent) : Scope(r, ConstBlock_Scope, parent), name(name) {
}

mu::ParameterScope::ParameterScope(ast::AstNode *r, mu::ScopePtr parent) : Scope(r, Parameter_Scope, parent) {
}

mu::ModuleScope::ModuleScope(ast::Ident *name, ast::AstNode *r, mu::ScopePtr parent) : Scope(r, Module_Scope, parent), name(name) {
}

mu::DeferScope::DeferScope(ast::AstNode *r, mu::ScopePtr parent) : Scope(r, Defer_Scope, parent) {
}

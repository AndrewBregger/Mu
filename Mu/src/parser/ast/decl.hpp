//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_DECL_HPP
#define MU_DECL_HPP

#include "ast_common.hpp"

namespace ast {

    enum Visibility {
        Public,
        Private,
    };

    struct Decl : public AstNode {
        Decl(AstKind kind, const mu::Pos& pos) : AstNode(kind, pos) {}
    };

    struct Local : public Decl {
        PatternPtr names;
        SpecPtr type;
        ExprPtr init;

        Local(PatternPtr &names, SpecPtr &type, ExprPtr &init, const mu::Pos& pos)
                : Decl(ast_local, pos), names(std::move(names)), type(std::move(type)), init(std::move(init)) {}
    };

    struct Mutable : public Decl {
        PatternPtr names;
        SpecPtr type;
        ExprPtr init;

        Mutable(PatternPtr &names, SpecPtr &type, ExprPtr &init, mu::Pos& pos)
                : Decl(ast_local, pos), names(std::move(names)), type(std::move(type)), init(std::move(init)) {}
    };

    struct Global : public Decl {
        Ident* name{nullptr};
        SpecPtr type{nullptr};
        ExprPtr init{nullptr};

        Global(Ident *name, SpecPtr &type, ExprPtr &init, const mu::Pos& pos) : Decl(ast_global, pos),
              name(name),
              type(std::move(type)),
              init(std::move(init)) {}
    };

    struct GlobalMut : public Decl{
        Ident* name{nullptr};
        SpecPtr type{nullptr};
        ExprPtr init{nullptr};

        GlobalMut(Ident *name, SpecPtr &type, ExprPtr &init, const mu::Pos& pos) : Decl(ast_global_mut, pos),
            name(name),
            type(std::move(type)),
            init(std::move(init)) {}
    };

    struct Procedure : public Decl {

        DeclPtr generics;
    };

    struct Structure : public Decl {
        Ident* name{nullptr};
        std::vector<SpecPtr> bounds;
        std::vector<DeclPtr> members;
        DeclPtr generics;

        Structure(Ident *name, std::vector<SpecPtr> &bounds, std::vector<DeclPtr> &members, DeclPtr &generics,
                  const mu::Pos &pos) : Decl(ast_structure, pos),
                    name(name), bounds(std::move(bounds)), members(std::move(members)), generics(std::move(generics)) {}
    };

    struct Type : public Decl {
        DeclPtr generics;
    };

    struct TypeClass : public Decl {
        DeclPtr generics;

    };

    struct Use : public Decl {

    };

    struct Alias : public Decl {

    };


    struct Generic : public Decl {
        Ident* name;

        Generic(Ident* name, const mu::Pos& pos) : Decl(ast_generic, pos), name(name) {}
    };

    struct BoundedGeneric;

    struct GenericBounds {
        std::vector<ast::SpecPtr> type_bounds;
        BoundedGeneric* parent;

        GenericBounds(std::vector<ast::SpecPtr>& type_bounds, BoundedGeneric* parent) :
                type_bounds(std::move(type_bounds)), parent(parent) {}
    };
    struct BoundedGeneric : public Decl {
        Ident* name;
        GenericBounds bounds;

        BoundedGeneric(Ident* name, GenericBounds& bounds, const mu::Pos& pos) : Decl(ast_bounded_generic, pos),
            name(name), bounds(std::move(bounds)) {}
    };

    struct GenericGroup : public Decl {
        std::vector<DeclPtr> generics;

        GenericGroup(std::vector<DeclPtr>& generics, const mu::Pos& pos) : Decl(ast_generics_group, pos),
            generics(std::move(generics)) {}
    };

    struct MemberVariable : public Decl {
        std::vector<ast::Ident*> names;
        SpecPtr type;
        std::vector<ast::ExprPtr> init;
        Visibility vis{Visibility::Private};

        MemberVariable(std::vector<Ident *> &names, SpecPtr &type,
                       std::vector<ExprPtr> &init, Visibility vis, const mu::Pos& pos) :
                       Decl(ast_member_variable, pos), names(std::move(names)), type(std::move(type)),
                       init(std::move(init)), vis(vis) {}
    };
}


#endif //MU_DECL_HPP

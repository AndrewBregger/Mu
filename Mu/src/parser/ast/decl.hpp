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

    struct Attribute {
        Ident* attr;
        std::string value;

        Attribute(Ident* attr, const std::string& val): attr(attr), value(val) {}
    };

    struct AttributeList {
        std::vector<Attribute> attributes;

        AttributeList(const std::vector<Attribute>& attributes) : attributes(attributes) {}
    };

    enum Modifier {
        Mod_Inline,
        Mod_Public,
        Mod_Private
    };

    struct ProcedureSigniture {
        std::vector<DeclPtr> parameters;
        SpecPtr ret;
        DeclPtr generics;

        ProcedureSigniture(std::vector<DeclPtr>& parameters, SpecPtr& ret, DeclPtr& generics) :
                parameters(std::move(parameters)), ret(std::move(ret)), generics(std::move(generics)) {
        }
    };

    struct Procedure : public Decl {
        Ident* name;
        std::shared_ptr<ProcedureSigniture> signiture;
        ExprPtr body;
        AttributeList attributeList;
        std::vector<Modifier> modifiers;

        Procedure(Ident* name, std::shared_ptr<ProcedureSigniture>& signiture, ExprPtr& body, AttributeList& attributeList, const std::vector<Modifier>& modifiers,
                const mu::Pos& pos) : Decl(ast_procedure, pos), name(name), signiture(std::move(signiture)),
                body(std::move(body)), attributeList(std::move(attributeList)), modifiers(modifiers) {
        }
    };

    struct ProcedureParameter : public Decl {
        PatternPtr pattern;
        SpecPtr type;
        ExprPtr init;

        ProcedureParameter(PatternPtr& pattern, SpecPtr& type, ExprPtr& init, const mu::Pos& pos) :
            Decl(ast_procedure_parameter, pos), pattern(std::move(pattern)), type(std::move(type)), init(std::move(init)) {
        }
    };

    struct SelfParameter : public Decl {
        SelfParameter(const mu::Pos& pos) : Decl(ast_self_parameter, pos) {}
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
        Ident* name;
        std::vector<SpecPtr> bounds;
        std::vector<DeclPtr> members;
        DeclPtr generics;

        Type(Ident *name, std::vector<SpecPtr> &bounds, std::vector<DeclPtr> &members, DeclPtr &generics,
            const mu::Pos &pos) : Decl(ast_type, pos),
            name(name), bounds(std::move(bounds)), members(std::move(members)), generics(std::move(generics)) {}
    };

    struct TypeClass : public Decl {
        Ident* name;
        std::vector<DeclPtr> members;
        DeclPtr generics;

        TypeClass(Ident* name, std::vector<DeclPtr>& members, DeclPtr& generics, const mu::Pos& pos) :
            Decl(ast_type_class, pos), name(name), members(std::move(members)), generics(std::move(generics)) {}
    };

    struct Use : public Decl {

    };

    struct Alias : public Decl {
        Ident* name;
        SpecPtr type;

        Alias(Ident* name, SpecPtr& type, const mu::Pos& pos): Decl(ast_alias, pos),
            name(name), type(std::move(type)) {}
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

    struct Impl : public Decl {
        Ident* name;
        DeclPtr generics;
        std::vector<DeclPtr> methods;
        Impl(Ident* name, std::vector<DeclPtr>& methods, DeclPtr& generics, const mu::Pos& pos) :
            Decl(ast_impl, pos), name(name), methods(std::move(methods)), generics(std::move(generics)) {}
    };

    struct TypeMember : public Decl {
        Ident* name;
        std::vector<SpecPtr> types;

        TypeMember(Ident* name, std::vector<SpecPtr>& types, const mu::Pos& pos) :
            Decl(ast_type_member, pos), name(name), types(std::move(types)) {}
    };
}


#endif //MU_DECL_HPP

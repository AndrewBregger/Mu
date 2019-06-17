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

    struct Local : public Decl {
        PatternPtr names;
        SpecPtr type;
        ExprPtr init;

        Local(PatternPtr &names, SpecPtr &type, ExprPtr &init, const mu::Pos& pos);
    };

    struct Mutable : public Decl {
        PatternPtr names;
        SpecPtr type;
        ExprPtr init;

        Mutable(PatternPtr &names, SpecPtr &type, ExprPtr &init, mu::Pos& pos);
    };

    struct Global : public Decl {
        Ident* name{nullptr};
        SpecPtr type{nullptr};
        ExprPtr init{nullptr};
        Visibility vis;

        Global(Ident *name, SpecPtr &type, ExprPtr &init, ast::Visibility vis, const mu::Pos& pos);
    };

    struct GlobalMut : public Decl{
        Ident* name{nullptr};
        SpecPtr type{nullptr};
        ExprPtr init{nullptr};
        Visibility vis;

        GlobalMut(Ident *name, SpecPtr &type, ExprPtr &init, Visibility vis, const mu::Pos& pos);
    };

    struct Attribute {
        Ident* attr;
        std::string value;

        Attribute(Ident* attr, const std::string& val);
    };

    struct AttributeList {
        std::vector<Attribute> attributes;
        AttributeList(const std::vector<Attribute>& attributes);
    };

    enum Modifier {
        Mod_Inline,
//        Mod_Public,
//        Mod_Private
    };

    struct ProcedureSigniture {
        std::vector<DeclPtr> parameters;
        SpecPtr ret;
        DeclPtr generics;

        ProcedureSigniture(std::vector<DeclPtr>& parameters, SpecPtr& ret, DeclPtr& generics);
    };

    struct Procedure : public Decl {
        Ident* name;
        std::shared_ptr<ProcedureSigniture> signiture;
        ExprPtr body;
        AttributeList attributeList;
        Visibility vis;
        std::vector<Modifier> modifiers;

        Procedure(Ident* name, std::shared_ptr<ProcedureSigniture>& signiture, ExprPtr& body, AttributeList& attributeList, const std::vector<Modifier>& modifiers,
                Visibility vis, const mu::Pos& pos);
    };

    struct ProcedureParameter : public Decl {
        PatternPtr pattern;
        SpecPtr type;
        ExprPtr init;

        ProcedureParameter(PatternPtr& pattern, SpecPtr& type, ExprPtr& init, const mu::Pos& pos);
    };

    struct SelfParameter : public Decl {
        SelfParameter(const mu::Pos& pos);
    };

    struct Structure : public Decl {
        Ident* name{nullptr};
        std::vector<SpecPtr> bounds;
        std::vector<DeclPtr> members;
        DeclPtr generics;
        Visibility vis;

        Structure(Ident *name, std::vector<SpecPtr> &bounds, std::vector<DeclPtr> &members, DeclPtr &generics,
                  Visibility vis, const mu::Pos &pos);
    };

    struct Type : public Decl {
        Ident* name;
        std::vector<SpecPtr> bounds;
        std::vector<DeclPtr> members;
        DeclPtr generics;
        Visibility vis;

        Type(Ident *name, std::vector<SpecPtr> &bounds, std::vector<DeclPtr> &members, DeclPtr &generics,
            Visibility vis, const mu::Pos &pos);
    };

    struct TypeClass : public Decl {
        Ident* name;
        std::vector<DeclPtr> members;
        DeclPtr generics;
        Visibility vis;

        TypeClass(Ident* name, std::vector<DeclPtr>& members, DeclPtr& generics, Visibility vis, const mu::Pos& pos);
    };

    struct UsePath : public Decl {
        SPath path;
        bool all_names{false};

        UsePath(const SPath& path, bool all_names, const mu::Pos& pos);
    };

    struct UsePathList : public Decl {
        SPath base;
        std::vector<DeclPtr> subpaths;

        UsePathList(const SPath& path, std::vector<DeclPtr>& subpaths, const mu::Pos& pos);
    };

    struct UsePathAlias : public Decl {
        SPath path;
        ast::Ident* alias;

        UsePathAlias(const SPath& path, Ident* alias, const mu::Pos& pos);
    };

    struct Use : public Decl {
        DeclPtr use_path;
        Visibility vis;

        Use(DeclPtr& use_path, Visibility vis, const mu::Pos& pos);
    };

    struct Alias : public Decl {
        Ident* name;
        SpecPtr type;
        Visibility vis;

        Alias(Ident* name, SpecPtr& type, Visibility vis, const mu::Pos& pos);
    };


    struct Generic : public Decl {
        Ident* name;

        Generic(Ident* name, const mu::Pos& pos);
    };

    struct BoundedGeneric;

    struct GenericBounds {
        std::vector<ast::SpecPtr> type_bounds;
        BoundedGeneric* parent;

        GenericBounds(std::vector<ast::SpecPtr>& type_bounds, BoundedGeneric* parent);
    };
    struct BoundedGeneric : public Decl {
        Ident* name;
        GenericBounds bounds;

        BoundedGeneric(Ident* name, GenericBounds& bounds, const mu::Pos& pos);
    };

    struct GenericGroup : public Decl {
        std::vector<DeclPtr> generics;

        GenericGroup(std::vector<DeclPtr>& generics, const mu::Pos& pos);
    };

    struct MemberVariable : public Decl {
        std::vector<ast::Ident*> names;
        SpecPtr type;
        std::vector<ast::ExprPtr> init;
        Visibility vis{Visibility::Private};

        MemberVariable(std::vector<Ident *> &names, SpecPtr &type,
                       std::vector<ExprPtr> &init, Visibility vis, const mu::Pos& pos);
    };

    struct Impl : public Decl {
        Ident* name;
        DeclPtr generics;
        std::vector<DeclPtr> methods;
        Impl(Ident* name, std::vector<DeclPtr>& methods, DeclPtr& generics, const mu::Pos& pos);
    };

    struct TypeMember : public Decl {
        Ident* name;
        std::vector<SpecPtr> types;

        TypeMember(Ident* name, std::vector<SpecPtr>& types, const mu::Pos& pos);
    };

    struct TraitElementType : public Decl {
        Ident* name;
        SpecPtr init; // default type if one is not provided

        TraitElementType(Ident* name, SpecPtr& init, const mu::Pos& pos);
    };
}


#endif //MU_DECL_HPP

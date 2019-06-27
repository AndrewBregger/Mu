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
        virtual ~Local() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct Mutable : public Decl {
        PatternPtr names;
        SpecPtr type;
        ExprPtr init;

        Mutable(PatternPtr &names, SpecPtr &type, ExprPtr &init, mu::Pos& pos);
        virtual ~Mutable() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct Global : public Decl {
        Ident* name{nullptr};
        SpecPtr type{nullptr};
        ExprPtr init{nullptr};
        Visibility vis;

        Global(Ident *name, SpecPtr &type, ExprPtr &init, ast::Visibility vis, const mu::Pos& pos);
        virtual ~Global() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct GlobalMut : public Decl{
        Ident* name{nullptr};
        SpecPtr type{nullptr};
        ExprPtr init{nullptr};
        Visibility vis;

        GlobalMut(Ident *name, SpecPtr &type, ExprPtr &init, Visibility vis, const mu::Pos& pos);
        virtual ~GlobalMut() = default;
        void renderer(AstRenderer* renderer) override;
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
        
        virtual ~Procedure() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct ProcedureParameter : public Decl {
        PatternPtr pattern;
        SpecPtr type;
        ExprPtr init;

        ProcedureParameter(PatternPtr& pattern, SpecPtr& type, ExprPtr& init, const mu::Pos& pos);

        virtual ~ProcedureParameter() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct SelfParameter : public Decl {
        SelfParameter(const mu::Pos& pos);

        virtual ~SelfParameter() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct CVariadicParameter : public Decl {
        PatternPtr pattern;
        CVariadicParameter(PatternPtr pattern, const mu::Pos& pos);

        virtual ~CVariadicParameter() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct VariadicParameter : public Decl {
        PatternPtr pattern;
        SpecPtr type;

        VariadicParameter(PatternPtr pattern, SpecPtr type, const mu::Pos& pos);

        virtual ~VariadicParameter() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct Structure : public Decl {
        Ident* name{nullptr};
        std::vector<SpecPtr> bounds;
        std::vector<DeclPtr> members;
        DeclPtr generics;
        Visibility vis;

        Structure(Ident *name, std::vector<SpecPtr> &bounds, std::vector<DeclPtr> &members, DeclPtr &generics,
                  Visibility vis, const mu::Pos &pos);
            
        virtual ~Structure() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct Type : public Decl {
        Ident* name;
        std::vector<SpecPtr> bounds;
        std::vector<DeclPtr> members;
        DeclPtr generics;
        Visibility vis;

        Type(Ident *name, std::vector<SpecPtr> &bounds, std::vector<DeclPtr> &members, DeclPtr &generics,
            Visibility vis, const mu::Pos &pos);
        
        virtual ~Type() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct TypeClass : public Decl {
        Ident* name;
        std::vector<DeclPtr> members;
        DeclPtr generics;
        Visibility vis;

        TypeClass(Ident* name, std::vector<DeclPtr>& members, DeclPtr& generics, Visibility vis, const mu::Pos& pos);

        virtual ~TypeClass() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct UsePath : public Decl {
        SPath path;
        bool all_names{false};

        UsePath(const SPath& path, bool all_names, const mu::Pos& pos);

        virtual ~UsePath() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct UsePathList : public Decl {
        SPath base;
        std::vector<DeclPtr> subpaths;

        UsePathList(const SPath& path, std::vector<DeclPtr>& subpaths, const mu::Pos& pos);

        virtual ~UsePathList() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct UsePathAlias : public Decl {
        SPath path;
        ast::Ident* alias;

        UsePathAlias(const SPath& path, Ident* alias, const mu::Pos& pos);

        virtual ~UsePathAlias() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct Use : public Decl {
        DeclPtr use_path;
        Visibility vis;

        Use(DeclPtr& use_path, Visibility vis, const mu::Pos& pos);

        virtual ~Use() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct Alias : public Decl {
        Ident* name;
        SpecPtr type;
        Visibility vis;

        Alias(Ident* name, SpecPtr& type, Visibility vis, const mu::Pos& pos);

        virtual ~Alias() = default;
        void renderer(AstRenderer* renderer) override;
    };


    struct Generic : public Decl {
        Ident* name;

        Generic(Ident* name, const mu::Pos& pos);

        virtual ~Generic() = default;
        void renderer(AstRenderer* renderer) override;
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

        virtual ~BoundedGeneric() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct GenericGroup : public Decl {
        std::vector<DeclPtr> generics;

        GenericGroup(std::vector<DeclPtr>& generics, const mu::Pos& pos);

        virtual ~GenericGroup() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct MemberVariable : public Decl {
        std::vector<ast::Ident*> names;
        SpecPtr type;
        std::vector<ast::ExprPtr> init;
        Visibility vis{Visibility::Private};

        MemberVariable(std::vector<Ident *> &names, SpecPtr &type,
                       std::vector<ExprPtr> &init, Visibility vis, const mu::Pos& pos);
        
        virtual ~MemberVariable() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct Impl : public Decl {
        Ident* name;
        DeclPtr generics;
        std::vector<DeclPtr> methods;
        Impl(Ident* name, std::vector<DeclPtr>& methods, DeclPtr& generics, const mu::Pos& pos);

        virtual ~Impl() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct TypeMember : public Decl {
        Ident* name;
        std::vector<SpecPtr> types;

        TypeMember(Ident* name, std::vector<SpecPtr>& types, const mu::Pos& pos);

        virtual ~TypeMember() = default;
        void renderer(AstRenderer* renderer) override;
    };

    struct TraitElementType : public Decl {
        Ident* name;
        SpecPtr init; // default type if one is not provided

        TraitElementType(Ident* name, SpecPtr& init, const mu::Pos& pos);

        virtual ~TraitElementType() = default;
        void renderer(AstRenderer* renderer) override;
    };
}


#endif //MU_DECL_HPP

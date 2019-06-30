#include "decl.hpp"
#include "renderer.hpp"

namespace ast {
    Local::Local(PatternPtr &names, SpecPtr &type, ExprPtr &init, const mu::Pos &pos)
            : Decl(ast_local, pos), names(std::move(names)), type(std::move(type)), init(std::move(init)) {}

    Mutable::Mutable(PatternPtr &names, SpecPtr &type, ExprPtr &init, mu::Pos &pos)
            : Decl(ast_local, pos), names(std::move(names)), type(std::move(type)), init(std::move(init)) {}

    Global::Global(Ident *name, SpecPtr &type, ExprPtr &init, ast::Visibility vis, const mu::Pos &pos) : Decl(
            ast_global, pos),
                                                                                                         name(name),
                                                                                                         type(std::move(
                                                                                                                 type)),
                                                                                                         init(std::move(
                                                                                                                 init)),
                                                                                                         vis(vis) {}

    GlobalMut::GlobalMut(Ident *name, SpecPtr &type, ExprPtr &init, Visibility vis, const mu::Pos &pos) : Decl(
            ast_global_mut, pos),
                                                                                                          name(name),
                                                                                                          type(std::move(
                                                                                                                  type)),
                                                                                                          init(std::move(
                                                                                                                  init)),
                                                                                                          vis(vis) {}

    Attribute::Attribute(Ident *attr, const std::string &val) : attr(attr), value(val) {}

    AttributeList::AttributeList(const std::vector<Attribute> &attributes) : attributes(attributes) {}

    ProcedureSigniture::ProcedureSigniture(std::vector<DeclPtr> &parameters, SpecPtr &ret, DeclPtr &generics) :
            parameters(std::move(parameters)), ret(std::move(ret)), generics(std::move(generics)) {
    }

    Procedure::Procedure(Ident *name, std::shared_ptr<ProcedureSigniture> &signiture, ExprPtr &body,
                         AttributeList &attributeList, const std::vector<Modifier> &modifiers,
                         Visibility vis, const mu::Pos &pos) : Decl(ast_procedure, pos), name(name),
                                                               signiture(std::move(signiture)),
                                                               body(std::move(body)),
                                                               attributeList(std::move(attributeList)),
                                                               vis(vis), modifiers(modifiers)  {
    }

    ProcedureParameter::ProcedureParameter(PatternPtr &pattern, SpecPtr &type, ExprPtr &init, const mu::Pos &pos) :
            Decl(ast_procedure_parameter, pos), pattern(std::move(pattern)), type(std::move(type)),
            init(std::move(init)) {
    }

    SelfParameter::SelfParameter(bool mut, const mu::Pos &pos) : Decl(ast_self_parameter, pos), mut(mut) {}

    CVariadicParameter::CVariadicParameter(PatternPtr pattern, const mu::Pos &pos) : Decl(ast_c_variadic, pos),
        pattern(std::move(pattern)) {
    }

    VariadicParameter::VariadicParameter(PatternPtr pattern, SpecPtr type, const mu::Pos &pos) : Decl(ast_variadic, pos),
        pattern(std::move(pattern)), type(std::move(type)) {
    }

    Structure::Structure(Ident *name, std::vector<SpecPtr> &bounds, std::vector<DeclPtr> &members, DeclPtr &generics,
                         Visibility vis, const mu::Pos &pos) : Decl(ast_structure, pos),
                                                               name(name), bounds(std::move(bounds)),
                                                               members(std::move(members)),
                                                               generics(std::move(generics)),
                                                               vis(vis) {}

    Type::Type(Ident *name, std::vector<SpecPtr> &bounds, std::vector<DeclPtr> &members, DeclPtr &generics,
               Visibility vis, const mu::Pos &pos) : Decl(ast_type, pos),
                                                     name(name), bounds(std::move(bounds)), members(std::move(members)),
                                                     generics(std::move(generics)), vis(vis) {}

    TypeClass::TypeClass(Ident *name, std::vector<DeclPtr> &members, DeclPtr &generics, Visibility vis,
                         const mu::Pos &pos) :
            Decl(ast_type_class, pos), name(name), members(std::move(members)), generics(std::move(generics)),
            vis(vis) {}

    UsePath::UsePath(const SPath &path, bool all_names, const mu::Pos &pos) : Decl(ast_use_path, pos),
                                                                              path(path), all_names(all_names) {}

    UsePathList::UsePathList(const SPath &path, std::vector<DeclPtr> &subpaths, const mu::Pos &pos) : Decl(
            ast_use_path_list, pos),
                                                                                                      base(path),
                                                                                                      subpaths(
                                                                                                              std::move(
                                                                                                                      subpaths)) {}

    UsePathAlias::UsePathAlias(const SPath &path, Ident *alias, const mu::Pos &pos) : Decl(ast_use_path_alias, pos),
                                                                                      path(path), alias(alias) {}

    Use::Use(DeclPtr &use_path, Visibility vis, const mu::Pos &pos) : Decl(ast_use, pos), use_path(std::move(use_path)),
                                                                      vis(vis) {}

    Alias::Alias(Ident *name, SpecPtr &type, Visibility vis, const mu::Pos &pos) : Decl(ast_alias, pos),
                                                                                   name(name), type(std::move(type)), vis(vis) {}

    Generic::Generic(Ident *name, const mu::Pos &pos) : Decl(ast_generic, pos), name(name) {}

    GenericBounds::GenericBounds(std::vector<ast::SpecPtr> &type_bounds, BoundedGeneric *parent) :
            type_bounds(std::move(type_bounds)), parent(parent) {}

    BoundedGeneric::BoundedGeneric(Ident *name, GenericBounds &bounds, const mu::Pos &pos) : Decl(ast_bounded_generic,
                                                                                                  pos),
                                                                                             name(name),
                                                                                             bounds(std::move(
                                                                                                     bounds)) {}

    GenericGroup::GenericGroup(std::vector<DeclPtr> &generics, const mu::Pos &pos) : Decl(ast_generics_group, pos),
                                                                                     generics(std::move(generics)) {}

    MemberVariable::MemberVariable(std::vector<Ident *> &names, SpecPtr &type,
                                   std::vector<ExprPtr> &init, Visibility vis, const mu::Pos &pos) :
            Decl(ast_member_variable, pos), names(std::move(names)), type(std::move(type)),
            init(std::move(init)), vis(vis) {}

    Impl::Impl(Ident *name, std::vector<DeclPtr> &methods, DeclPtr &generics, const mu::Pos &pos) :
        Decl(ast_impl, pos), name(name), generics(std::move(generics)), methods(std::move(methods))  {}

    TypeMember::TypeMember(Ident *name, std::vector<SpecPtr> &types, const mu::Pos &pos) :
            Decl(ast_type_member, pos), name(name), types(std::move(types)) {}

    TraitElementType::TraitElementType(Ident *name, SpecPtr &init, const mu::Pos &pos) : Decl(ast_trait_element_type,
                                                                                              pos),
                                                                                         name(name), init(std::move(init)) {}
    void Local::renderer(AstRenderer* renderer) { renderer->render(this); }
    void Mutable::renderer(AstRenderer* renderer) { renderer->render(this); }
    void Global::renderer(AstRenderer* renderer) { renderer->render(this); }
    void GlobalMut::renderer(AstRenderer* renderer) { renderer->render(this); }
    void Procedure::renderer(AstRenderer* renderer) { renderer->render(this); }
    void ProcedureParameter::renderer(AstRenderer* renderer) { renderer->render(this); }
    void SelfParameter::renderer(AstRenderer* renderer) { renderer->render(this); }
    void CVariadicParameter::renderer(AstRenderer* renderer) { renderer->render(this); }
    void VariadicParameter::renderer(AstRenderer* renderer) { renderer->render(this); }
    void Structure::renderer(AstRenderer* renderer) { renderer->render(this); }
    void Type::renderer(AstRenderer* renderer) { renderer->render(this); }
    void TypeClass::renderer(AstRenderer* renderer) { renderer->render(this); }
    void UsePath::renderer(AstRenderer* renderer) { renderer->render(this); }
    void UsePathList::renderer(AstRenderer* renderer) { renderer->render(this); }
    void UsePathAlias::renderer(AstRenderer* renderer) { renderer->render(this); }
    void Use::renderer(AstRenderer* renderer) { renderer->render(this); }
    void Alias::renderer(AstRenderer* renderer) { renderer->render(this); }
    void Generic::renderer(AstRenderer* renderer) { renderer->render(this); }
    void BoundedGeneric::renderer(AstRenderer* renderer) { renderer->render(this); }
    void GenericGroup::renderer(AstRenderer* renderer) { renderer->render(this); }
    void MemberVariable::renderer(AstRenderer* renderer) { renderer->render(this); }
    void Impl::renderer(AstRenderer* renderer) { renderer->render(this); }
    void TypeMember::renderer(AstRenderer* renderer) { renderer->render(this); }
    void TraitElementType::renderer(AstRenderer* renderer) { renderer->render(this); }
}
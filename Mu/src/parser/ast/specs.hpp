//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_SPECS_HPP
#define MU_SPECS_HPP

#include "ast_common.hpp"

namespace ast {
    struct Spec : public AstNode {
        Spec(AstKind kind, const mu::Pos& pos) : AstNode(kind, pos) {}
    };

    struct Named : public Spec {
        Ident* name;
        Named(Ident* name, const mu::Pos &pos) : Spec(ast_named, pos), name(name) {}
    };

    struct NamedGeneric : public Spec {
        Ident* name;
        std::vector<SpecPtr> params;

        NamedGeneric(Ident* name, std::vector<SpecPtr>& params, const mu::Pos& pos) : Spec(ast_named_generic, pos),
            name(name), params(std::move(params)) {}
    };

    struct TupleSpec : public Spec {
        std::vector<SpecPtr> elements;

        TupleSpec(std::vector<SpecPtr> &elements, const mu::Pos& pos) : Spec(ast_tuple, pos),
            elements(std::move(elements)) {}
    };

    struct ListSpec : public Spec {
        SpecPtr type;
        ExprPtr size;

        ListSpec(SpecPtr &type, const ExprPtr &size, const mu::Pos& pos) : Spec(ast_list_spec, pos),
            type(std::move(type)), size(size) {}
    };

    struct DynListSpec : public Spec {
        SpecPtr type;

        DynListSpec(SpecPtr& type, const mu::Pos& pos) : Spec(ast_list_spec_dyn, pos),
            type(std::move(type)) {}
    };

    struct ProcedureSpec : public Spec {
        std::vector<SpecPtr> params;
        SpecPtr ret;

        ProcedureSpec(std::vector<SpecPtr>& params, SpecPtr& ret, const mu::Pos& pos) : Spec(ast_procedure_spec, pos),
            params(std::move(params)), ret(std::move(ret)) {}
    };

    struct PtrSpec : public Spec {
        SpecPtr type;

        PtrSpec(SpecPtr &type, const mu::Pos& pos) : Spec(ast_ptr, pos), type(std::move(type)) {}
    };

    struct RefSpec : public Spec {
        SpecPtr type;

        RefSpec(SpecPtr &type, const mu::Pos& pos) : Spec(ast_ref, pos), type(std::move(type)) {}
    };

    struct MutSpec : public Spec {
        SpecPtr type;

        MutSpec(SpecPtr &type, const mu::Pos& pos) : Spec(ast_mut, pos), type(std::move(type)) {}
    };

    struct SelfSpec : public Spec {
        SelfSpec(const mu::Pos& pos) : Spec(ast_self_type, pos) {}
    };

    // maps to an empty type spec and _
    struct InferSpec : public Spec {
        InferSpec(const mu::Pos& pos) : Spec(ast_infer_type, pos) {}
    };
}

#endif //MU_SPECS_HPP

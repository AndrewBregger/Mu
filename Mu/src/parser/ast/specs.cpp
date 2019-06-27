#include "specs.hpp"
#include "renderer.hpp"

namespace ast {
    ExprSpec::ExprSpec(ExprPtr& type, const mu::Pos& pos) : Spec(ast_expr_type, pos), type(std::move(type)) {
    }

    TupleSpec::TupleSpec(std::vector<SpecPtr> &elements, const mu::Pos& pos) : Spec(ast_tuple, pos),
    elements(std::move(elements)) {}

    ListSpec::ListSpec(SpecPtr &type, const ExprPtr &size, const mu::Pos& pos) : Spec(ast_list_spec, pos),
    type(std::move(type)), size(size) {}

    DynListSpec::DynListSpec(SpecPtr& type, const mu::Pos& pos) : Spec(ast_list_spec_dyn, pos),
    type(std::move(type)) {}

    ProcedureSpec::ProcedureSpec(std::vector<SpecPtr>& params, SpecPtr& ret, const mu::Pos& pos) : Spec(ast_procedure_spec, pos),
    params(std::move(params)), ret(std::move(ret)) {}

    PtrSpec::PtrSpec(SpecPtr &type, const mu::Pos& pos) : Spec(ast_ptr, pos), type(std::move(type)) {}

    RefSpec::RefSpec(SpecPtr &type, const mu::Pos& pos) : Spec(ast_ref, pos), type(std::move(type)) {}

    MutSpec::MutSpec(SpecPtr &type, const mu::Pos& pos) : Spec(ast_mut, pos), type(std::move(type)) {}

    SelfSpec::SelfSpec(const mu::Pos& pos) : Spec(ast_self_type, pos) {}

    InferSpec::InferSpec(const mu::Pos& pos) : Spec(ast_infer_type, pos) {}

    TypeLitSpec::TypeLitSpec(const mu::Pos& pos) : Spec(ast_type_lit, pos) {}

    UnitSpec::UnitSpec(const mu::Pos& pos) : Spec(ast_unit_type, pos) {}
    void ExprSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
    void TupleSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
    void ListSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
    void DynListSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
    void ProcedureSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
    void PtrSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
    void RefSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
    void MutSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
    void SelfSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
    void InferSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
    void TypeLitSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
    void UnitSpec::renderer(AstRenderer* renderer) { renderer->render(this); }
}